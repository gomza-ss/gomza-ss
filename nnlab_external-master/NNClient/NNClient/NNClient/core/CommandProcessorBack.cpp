#include <NNClient/core/CommandProcessorBack.h>
#include <NNClient/registry/NNClientRegistries.h>

#include <NNClient/registry/commands/Back.h>
#include <NNClient/core/CommandWithParams.h>

#include <NNClient/core/DeviceContextManager.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <QCoreApplication>
#include <QFile>

#include <atomic>
#include <chrono>

namespace nnlab
{

class UniqueIdGenerator
{
public:
    static std::string generate()
    {
        return
            m_processPID
            + "_"
            + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count())
            + "_"
            + std::to_string(m_state++);
    }

private:
    static std::string m_processPID;
    static std::atomic<int> m_state;
};

std::string UniqueIdGenerator::m_processPID = std::to_string(QCoreApplication::applicationPid());
std::atomic<int> UniqueIdGenerator::m_state{};

CommandProcessorBack::CommandProcessorBack(NNClientLogger* logger): m_processManager(logger),
                                                                   m_logger(new NNClientLogger(std::string("CPB") + logger->loggerId(), this))
{
    m_logger->logTrace("Creating CommandProcessorBack");

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::readyStdOut,
        this,
        &CommandProcessorBack::readyStdOutSlot
    );

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::readyStdOut,
        this,
        &CommandProcessorBack::readyStdOutSignal
    );

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::readyStdErr,
        this,
        &CommandProcessorBack::readyStdErrSignal
    );

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::processUnexpectedlyTerminated,
        this,
        &CommandProcessorBack::serverUnexpectedlyTerminated
    );

    m_logger->logTrace("CommandProcessorBack created");
}

CommandProcessorBack::~CommandProcessorBack()
{
    m_logger->logTrace("CommandProcessorBack destructor");
}

void CommandProcessorBack::processCommand
(
    const std::string & command,
    const Config & inputConfig
)
{
    m_logger->logTrace(QString("Processing back command %0").arg(command.c_str()));
    const bool status = false;
    std::string errorMessage;
    Config outputConfig;

    const auto* pCommandBack = CommandBackRegistry::pCommandBack(command);
    if (pCommandBack == nullptr)
    {
        errorMessage = "Command " + command + " not found in registry";
        emit responseReady(command, status, outputConfig, errorMessage);
        return;
    }
    Config config = pCommandBack->inputConfig();
    if (!config.initialize(inputConfig, errorMessage))
    {
        emit responseReady(command, status, outputConfig, errorMessage);
        return;
    }

    std::string notInitializedElemName;
    if (!config.initialized(notInitializedElemName))
    {
        errorMessage = std::string("Input argument ") +
            notInitializedElemName +
            std::string(" was not initialized");
        emit responseReady(command, status, outputConfig, errorMessage);
        return;
    }

    std::string commandToServer;
    if (!commandWithParams(command, config, commandToServer, errorMessage))
    {
        emit responseReady(command, status, outputConfig, errorMessage);
        return;
    }

    if (!m_processManager.writeToProcess(commandToServer))
    {
        errorMessage = "writeToProcess failed";
        if (m_processManager.isStarted()) {
            emit responseReady(command, status, outputConfig, errorMessage);
        }
        return;
    }

    emit wroteToProcessSignal(QString::fromStdString(commandToServer));
}

bool CommandProcessorBack::createSM(Config & inputConfig, std::string & errorMessage)
{
    m_sharedMemoryMap[m_imageSMKey].setKey(QString::fromStdString(UniqueIdGenerator::generate()));

    if (!m_sharedMemoryMap[m_imageSMKey].create(m_width * m_height * m_channels))
    {
        errorMessage = m_sharedMemoryMap[m_imageSMKey].errorString().toStdString();
        return false;
    }
    m_logger->logInfo(QString("Created image shared memory with size %1 %2 %3").arg(m_width).arg(m_height).arg(m_channels));

    inputConfig.Arg<std::string>("smkey", [this]() {return m_sharedMemoryMap[m_imageSMKey].key().toStdString(); });

    return true;
}

bool CommandProcessorBack::createProcess(const Config& inputConfig)
{
    // sets and restores back CUDA_VISIBLE_DEVICES for NVIDIA servers
    DeviceContextManager deviceContextManager(inputConfig);

    std::string path;
    if (!inputConfig.get<std::string>(NN_CLIENT_EXEC_PATH_PARAM_NAME, path))
    {
        return false;
    }

    return m_processManager.createProcess(path);
}

bool CommandProcessorBack::terminateProcess(bool force)
{
    return m_processManager.terminateProcess(force);
}

bool CommandProcessorBack::setSize(const Config & config)
{
    int width;
    if (!config.get<int>("input_image_width", width))
    {
        return false;
    }

    int height;
    if (!config.get<int>("input_image_height", height))
    {
        return false;
    }

    m_width = width;
    m_height = height;

    return true;
}

bool CommandProcessorBack::setChannels(const Config & config)
{
    int channels;
    if (!config.get<int>("channels", channels))
    {
        return false;
    }

    m_channels = channels;

    return true;
}

namespace
{

bool readFileContents(const std::string &filename, std::string &output)
{
    QFile f(QString::fromStdString(filename));
    if (!f.open(QFile::ReadOnly))
    {
        return false;
    }

    output = f.readAll().toStdString();
    return true;
}

}

bool CommandProcessorBack::fillSMFromFile
(
    const std::string& path,
    const std::string& keyPrefix,
    Config& config
)
{
    std::string modelFileContent;
    if (!readFileContents(path, modelFileContent))
    {
        return false;
    }

    const auto uniqueId = UniqueIdGenerator::generate();

    // if key does not exist it is guaranteed by standard to be inserted
    auto *pMem =  &m_sharedMemoryMap[keyPrefix];

    pMem->setKey(QString::fromStdString(uniqueId));

    if (!pMem->create(static_cast<int>(modelFileContent.size())))
    {
        return false;
    }

    if (!pMem->lock())
    {
        return false;
    }

    void *to = pMem->data();
    const auto *from = modelFileContent.data();
    size_t sendSize = modelFileContent.size();
    memcpy(to, from, sendSize);
    
    if (!pMem->unlock())
    {
        return false;
    }

    config.Arg<int>(keyPrefix + "_size", [&modelFileContent]() {return modelFileContent.size(); });
    config.Arg<std::string>(keyPrefix + "_key", [uniqueId]() {return uniqueId; });

    return true;
}

bool CommandProcessorBack::loadModelMeta(const std::string& path, Config& config) const
{
    std::string meta;
    if (!readFileContents(path, meta))
    {
        m_logger->logTrace(QString("No meta found for model %0.").arg(path.c_str()));
        return true;
    }
    m_logger->logTrace(QString("Found meta for model %0.").arg(path.c_str()));

    std::string errorMessage;

    return config.initialize(meta, errorMessage, false);
}

bool CommandProcessorBack::loadModel(const std::string& name, Config& config)
{
    std::string deviceVendor;
    if (!config.get(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, deviceVendor))
    {
        return false;
    }

    std::string dataPrefix;
    if (!config.get("data_prefix", dataPrefix))
    {
        return false;
    }

    std::string modelName;
    if (!config.get(name, modelName))
    {
        return false;
    }

    std::string archExt = deviceVendor == NN_CLIENT_NVIDIA ? "pbtxt" : "xml";
    std::string weightsExt = deviceVendor == NN_CLIENT_NVIDIA ? "pb" : "bin";

    std::string modelPathWoExt = dataPrefix + "/" + modelName + "/model.";
    std::string modelMetaPath = modelPathWoExt + "meta";
    std::string modelArchitecturePath = modelPathWoExt + archExt;
    std::string modelWeightsPath = modelPathWoExt + weightsExt;

    if (!loadModelMeta(modelMetaPath, config))
    {
        return false;
    }

    if (!fillSMFromFile(modelArchitecturePath, name + "_architecture", config))
    {
        return false;
    }

    if (!fillSMFromFile(modelWeightsPath, name + "_weights", config))
    {
        return false;
    }

    return true;
}

void CommandProcessorBack::readyStdOutSlot(QString str)
{
    // this prefix is a workaround to separate stdout output from libraries
    // which cannot be redirected, like OpenVINO HDDL plugin
    const QString nnServerPrefix("ISSNNServerStdOut ");
    if (str.contains(nnServerPrefix) && str.size() > nnServerPrefix.size())
    {
        str = str.mid(str.indexOf(nnServerPrefix) + nnServerPrefix.size());
    }
    else
    {
        // just ignore stdout, it was sent for user logging anyway
        return;
    }

    bool status = false;
    Config outputConfig;
    std::string message = str.trimmed().toStdString();
    std::string errorMessage;
    std::string commandName = message.substr(0, message.find(' '));
    if (commandName.size() < 2 || commandName.substr(commandName.size() - 2) != "ok")
    {
        errorMessage = "Got unexpected response: " + commandName;
        emit responseReady(commandName, status, outputConfig, errorMessage);
        return;
    }
    commandName = commandName.substr(0, commandName.size() - 2);

    const auto * pCommandBack = CommandBackRegistry::pCommandBack(commandName);
    if (pCommandBack == nullptr)
    {
        errorMessage = "Command " + commandName + " not found in registry";
        emit responseReady(commandName, status, outputConfig, errorMessage);
        return;
    }
    OutputParser outputParser = pCommandBack->outputParser();
    outputConfig = pCommandBack->outputConfig();

    if (!outputParser(message, outputConfig, errorMessage))
    {
        emit responseReady(commandName, status, outputConfig, errorMessage);
        return;
    }

    std::string notInitializedElemName;
    if (!outputConfig.initialized(notInitializedElemName))
    {
        errorMessage = std::string("Output argument ") +
            notInitializedElemName +
            std::string(" was not initialized");
        emit responseReady(commandName, status, outputConfig, errorMessage);
        return;
    }

    status = true;

    emit responseReady(commandName, status, outputConfig, errorMessage);
}

bool CommandProcessorBack::fillSM(const cv::Mat& image, std::string& errorMessage)
{
    if (!m_sharedMemoryMap[m_imageSMKey].isAttached())
    {
        if (!m_sharedMemoryMap[m_imageSMKey].attach())
        {
            return false;
        }
    }

    cv::Mat convertedInput = image;

    const auto nChannels = convertedInput.channels();
    const auto width = convertedInput.cols;
    const auto height = convertedInput.rows;

    const int grayChNum = 1;
    const int bgrChNum = 3;
    const int bgraChNum = 4;

#define NNCLIENT_CPB_SAFE_CV_CALL(stmt) \
try { \
    stmt; \
} \
catch(const cv::Exception& e) \
{ \
    errorMessage = e.what(); \
    return false; \
}

    if (m_channels != nChannels) {

        if (m_channels == grayChNum)
        {
            if (nChannels == bgrChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_BGR2GRAY));
            }
            else if (nChannels == bgraChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_BGRA2GRAY));
            }
            else
            {
                errorMessage = "Unsupported number of input channels";
                return false;
            }
        }
        else if (m_channels == bgrChNum)
        {
            if (nChannels == grayChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_GRAY2BGR));
            }
            else if (nChannels == bgraChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_BGRA2BGR));
            }
            else {
                errorMessage = "Unsupported number of input channels";
                return false;
            }
        }
        else if (m_channels == bgraChNum)
        {
            if (nChannels == grayChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_GRAY2BGRA));
            }
            else if (nChannels == bgrChNum)
            {
                NNCLIENT_CPB_SAFE_CV_CALL(cv::cvtColor(image, convertedInput, cv::COLOR_BGR2BGRA));
            }
            else
            {
                errorMessage = "Unsupported number of input channels";
                return false;
            }
        }
        else
        {
            errorMessage = "Unsupported number of target channels";
            return false;
        }
    }

    if (width != m_width || height != m_height)
    {
        NNCLIENT_CPB_SAFE_CV_CALL(cv::resize(convertedInput, convertedInput, cv::Size(m_width, m_height)));
    }

    if (!convertedInput.isContinuous())
    {
        convertedInput = convertedInput.clone();
    }

    if (!m_sharedMemoryMap[m_imageSMKey].lock())
    {
        errorMessage = "Failed to lock image shared memory";
        return false;
    }

    auto *to = m_sharedMemoryMap[m_imageSMKey].data();
    const auto *from = convertedInput.ptr();
    const int sendSize = m_width * m_height * m_channels;

    m_logger->logTrace(QString("Sending %1 bytes to image shared memory").arg(sendSize));
    memcpy(to, from, static_cast<size_t>(sendSize));

    if (!m_sharedMemoryMap[m_imageSMKey].unlock())
    {
        errorMessage = "Failed to unlock image shared memory";
        return false;
    }

    return true;
}

bool CommandProcessorBack::getFromSM(cv::Mat& image, std::string& errorMessage)
{
    auto& imageSM = m_sharedMemoryMap[m_imageSMKey];

    if (!imageSM.isAttached())
    {
        if (!imageSM.attach())
        {
            errorMessage = "getFromSM: Failed to attach image shared memory";
            return false;
        }
    }

    image = cv::Mat(m_height, m_width, CV_8UC1);

    if (!imageSM.lock())
    {
        errorMessage = "getFromSM: Failed to lock image shared memory";
        return false;
    }

    auto *to = image.ptr();
    const auto *from = imageSM.data();
    const int sendSize = m_width * m_height;
    memcpy(to, from, static_cast<size_t>(sendSize));

    if (!imageSM.unlock())
    {
        errorMessage = "getFromSM: Failed to unlock image shared memory";
        return false;
    }

    return true;
}


void CommandProcessorBack::clearNonImageSMs()
{
    for (decltype(m_sharedMemoryMap)::iterator it = m_sharedMemoryMap.begin(); it != m_sharedMemoryMap.end(); )
    {
        if (it->first != m_imageSMKey)
        {
            it = m_sharedMemoryMap.erase(it);
            continue;
        }
        ++it;
    }
}

cv::Size CommandProcessorBack::imageToSMSize() const
{
    return cv::Size(m_width, m_height);
}

} // namespace nnlab
