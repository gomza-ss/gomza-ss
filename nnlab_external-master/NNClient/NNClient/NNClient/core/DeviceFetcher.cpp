#include "DeviceFetcher.h"

#include <NNClient/core/Config.h>
#include <NNClient/core/DeviceContextManager.h>

#include <NNClient/registry/NNClientRegistries.h>

#include <QCoreApplication>
#include <QFile>

#include <type_traits>
#include <atomic>
#include <chrono>

namespace
{

bool readFileContents(const QString &fileName, QByteArray &output)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly))
    {
        return false;
    }

    output = f.readAll();
    return true;
}

bool fillSM(QSharedMemory& mem, const QByteArray& content)
{
    if (!mem.create(content.size()))
    {
        return false;
    }

    if (!mem.lock())
    {
        return false;
    }

    if (!mem.isAttached())
    {
        if (!mem.attach())
        {
            return false;
        }
    }

    char *to = (char*)mem.data();
    const char *from = content.data();
    int sendSize = content.size();
    memcpy(to, from, sendSize);

    if (!mem.unlock())
    {
        return false;
    }

    return true;
}

struct OpenVINOTestData
{
    QByteArray m_xmlContent;
    QByteArray m_binContent;
    QString m_imagePath;
    QString m_referencePath;
};

class UniqueIdGeneratorDeviceSearch
{
public:
    static QString generate()
    {
        return
            m_processPID
            + "_device_search_"
            + QString::number(std::chrono::high_resolution_clock::now().time_since_epoch().count())
            + "_"
            + QString::number(m_state++);
    }

private:
    static QString m_processPID;
    static std::atomic<int> m_state;
};

QString UniqueIdGeneratorDeviceSearch::m_processPID = QString::number(QCoreApplication::applicationPid());
std::atomic<int> UniqueIdGeneratorDeviceSearch::m_state{};

} // namespace

// this macros collect code used in many places in the file
// in order to reduce probability of copy-paste bugs
#define EMIT_FATAL_ERROR(message) \
    emit fatalError(message);     \
    return;

#define SEARCH_FOR_OTHER_DEVICES(deviceVendor)                             \
    m_deviceList[deviceVendor] = std::vector<std::vector<std::string> >(); \
    emit searchForTaskSetuppersSignal();                                   \
    return;                                                                \

namespace nnlab
{

DeviceFetcher::DeviceFetcher(NNClientLogger* logger, QObject* parent): QObject(parent), m_processManager(logger)
{
    QObject::connect
    (
        this,
        &DeviceFetcher::searchForTaskSetuppersSignal,
        this,
        &DeviceFetcher::searchForTaskSetuppers
    );

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::readyStdOut,
        this,
        &DeviceFetcher::readyStdOutCallback
    );

    QObject::connect
    (
        &m_processManager,
        &ProcessManager::readyStdErr,
        this,
        &DeviceFetcher::readyStdErrCallback
    );

    // server crash leads to fatal error during device search
    QObject::connect
    (
        &m_processManager,
        &ProcessManager::processUnexpectedlyTerminated,
        [this]()
        {
            emit fatalError(QString("Server unexpectedly terminated"));
        }
    );

    // clear contents on finish, be it success or failure
    QObject::connect
    (
        this,
        &DeviceFetcher::deviceListReady,
        [this](QString /* unused */)
        {
            clearDeviceSearchState();
        }
    );

    QObject::connect
    (
        this,
        &DeviceFetcher::fatalError,
        [this](QString /* unused */)
        {
            clearDeviceSearchState();
        }
    );
}

bool DeviceFetcher::getDeviceList(const ::nnlab::json& taskConfig, std::string& errorMessage)
{
    Config config;

    const std::string moduleKey("module");
    const std::string taskKey("task");

    config.Arg<std::string>(moduleKey);
    config.Arg<std::string>(taskKey);

    if (!config.initialize(taskConfig.dump(), errorMessage))
    {
        return false;
    }

    std::string moduleName;
    if (!config.get<std::string>(moduleKey, moduleName))
    {
        errorMessage = std::string("Parameter module was not found");
        return false;
    }

    std::string taskName;
    if (!config.get<std::string>(taskKey, taskName))
    {
        errorMessage = std::string("Parameter task was not found");
        return false;
    }

    clearDeviceSearchState();
    prepareTaskSetuppersToSearch(moduleName, taskName);
    searchForTaskSetuppers();

    return true;
}

void DeviceFetcher::clearDeviceSearchState()
{
    if (m_processManager.isStarted())
    {
        m_processManager.terminateProcess();
    }

    m_deviceList.clear();
    m_taskSetuppersToSearch.clear();
    m_searchedDeviceVendor.clear();
    m_openVINOSearchSMs.clear();
}

void DeviceFetcher::prepareTaskSetuppersToSearch(const std::string& module, const std::string& task)
{
    static const std::array<std::string, 2> deviceVendors =
    {
        NN_CLIENT_NVIDIA,
        NN_CLIENT_INTEL
    };

    static const NNClientMap<std::string, std::string> vendorToServer =
    {
        { NN_CLIENT_NVIDIA, std::string("Caffe2") },
        { NN_CLIENT_INTEL, std::string("OpenVINO") }
    };

    for (const std::string& currentDeviceVendor : deviceVendors)
    {
        const std::string taskStringName(module + "_" + vendorToServer.at(currentDeviceVendor) + "_" + task);

        if (!TaskSetuperRegistry::pTaskSetuper(taskStringName))
        {
            m_deviceList[currentDeviceVendor] = std::vector<std::vector<std::string> >();
        }
        else
        {
            m_taskSetuppersToSearch.push_back(taskStringName);
        }
    }
}

void DeviceFetcher::searchForTaskSetuppers()
{
    if (m_taskSetuppersToSearch.empty())
    {
        const QString& serializedOutput = serializeOutputToJson();

        // calling deviceListReady only on success
        if (!serializedOutput.isEmpty())
        {
            emit deviceListReady(serializedOutput);
        }
        return;
    }

    const std::string currTaskSetupperToSearch = m_taskSetuppersToSearch.front();
    m_taskSetuppersToSearch.erase(m_taskSetuppersToSearch.begin());

    searchForTaskSetupper(currTaskSetupperToSearch);
}

void DeviceFetcher::searchForTaskSetupper(const std::string& taskSetupperToSearch)
{
    const TaskSetuper* ptaskSetupper = TaskSetuperRegistry::pTaskSetuper(taskSetupperToSearch);

    if (!ptaskSetupper)
    {
        // catch logic bug
        EMIT_FATAL_ERROR(QString("ptaskSetupper is null in DeviceFetcher::searchForTaskSetupper"));
    }

    std::string deviceVendor;
    std::string errorMessage;
    if (!ptaskSetupper->getTaskParam<std::string>(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, deviceVendor, errorMessage))
    {
        EMIT_FATAL_ERROR(QString::fromStdString(errorMessage));
    }

    std::string executablePath;
    if (!ptaskSetupper->getTaskParam<std::string>(NN_CLIENT_EXEC_PATH_PARAM_NAME, executablePath, errorMessage))
    {
        EMIT_FATAL_ERROR(QString::fromStdString(errorMessage));
    }

    if (deviceVendor == NN_CLIENT_NVIDIA)
    {
        // special case: make sure CUDA_VISIBLE_DEVICES is unset
        Config cvdConfig;
        cvdConfig.Arg<std::string>(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME);
        if (!cvdConfig.setParam<std::string>(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, NN_CLIENT_NVIDIA, errorMessage))
        {
            EMIT_FATAL_ERROR(QString::fromStdString(errorMessage));
        }
        cvdConfig.Arg<std::string>(NN_CLIENT_DEVICE_PARAM_NAME);
        if (!cvdConfig.setParam<std::string>(NN_CLIENT_DEVICE_PARAM_NAME, std::string(), errorMessage))
        {
            EMIT_FATAL_ERROR(QString::fromStdString(errorMessage));
        }

        DeviceContextManager ensureCudaVisibleDevicesEmpty(cvdConfig);
        if (!m_processManager.createProcess(executablePath))
        {
            // cannot create process
            // either server is not installed or cannot start
            // return empty list and move to the next device vendor
            SEARCH_FOR_OTHER_DEVICES(deviceVendor);
        }

    }
    else
    {
        if (!m_processManager.createProcess(executablePath))
        {
            // cannot create process
            // either server is not installed or cannot start
            // return empty list and move to the next device vendor
            SEARCH_FOR_OTHER_DEVICES(deviceVendor);
        }
    }

    if (!m_processManager.isStarted())
    {
        // catch logic bug
        EMIT_FATAL_ERROR(QString("DeviceFetcher: server process is expected to be started"));
    }

    m_searchedDeviceVendor = deviceVendor;
    if (m_searchedDeviceVendor == NN_CLIENT_NVIDIA)
    {
        writeToProcess(QString("getcudadevices"));
    }
    else if (m_searchedDeviceVendor == NN_CLIENT_INTEL)
    {
        std::string dataPrefix;
        if (!ptaskSetupper->getTaskParam<std::string>("data_prefix", dataPrefix, errorMessage))
        {
            EMIT_FATAL_ERROR(QString::fromStdString("Get dataPrefix failed: " + errorMessage));
        }
        std::replace(dataPrefix.begin(), dataPrefix.end(), '\\', '/');

        std::string model;
        if (!ptaskSetupper->getTaskParam<std::string>(NN_CLIENT_REFERENCE_PATH_PARAM_NAME, model, errorMessage))
        {
            EMIT_FATAL_ERROR(QString::fromStdString("Get model failed: " + errorMessage));
        }

        std::string modelName;
        if (!ptaskSetupper->getTaskParam<std::string>(model, modelName, errorMessage))
        {
            EMIT_FATAL_ERROR(QString::fromStdString("Get modelName failed: " + errorMessage));
        }
        if (modelName.empty())
        {
            EMIT_FATAL_ERROR("modelName for OpenVINO is empty");
        }

        std::string referencePath = dataPrefix + "/" + modelName + "/";

        const QString irXML = QString::fromStdString(referencePath) + QStringLiteral("model.xml");
        const QString irBIN = QString::fromStdString(referencePath) + QStringLiteral("model.bin");

        OpenVINOTestData testData;
        if (!readFileContents(irXML, testData.m_xmlContent) || !readFileContents(irBIN, testData.m_binContent))
        {
            SEARCH_FOR_OTHER_DEVICES(deviceVendor);
        }

        QString imagePath = QString::fromStdString(referencePath) + QStringLiteral("test.bmp");
        imagePath.replace(' ', '^');
        QString referenceTPath = QString::fromStdString(referencePath);
        referenceTPath.replace(' ', '^');

        testData.m_imagePath = imagePath.toLocal8Bit().constData();
        testData.m_referencePath = referenceTPath.toLocal8Bit().constData();

        // false is the default value for use_faster_rcnn
        bool useFasterRCNN = false;
        ptaskSetupper->getTaskParam<bool>("use_faster_rcnn", useFasterRCNN, errorMessage);

        // by default we expect algorithm to be a detector
        const int detectorType = 1;
        int algorithmType = detectorType;
        ptaskSetupper->getTaskParam<int>("openvino_check_algorithm_type", algorithmType, errorMessage);

        const QString key1 = UniqueIdGeneratorDeviceSearch::generate();
        const QString key2 = UniqueIdGeneratorDeviceSearch::generate();

        m_openVINOSearchSMs[key1.toStdString()].setKey(key1);
        m_openVINOSearchSMs[key2.toStdString()].setKey(key2);
        fillSM(m_openVINOSearchSMs[key1.toStdString()], testData.m_xmlContent);
        fillSM(m_openVINOSearchSMs[key2.toStdString()], testData.m_binContent);

        const int encrypted = 1;
        const QString cmd = QString("getopenvinodevices %1 %2 %3 %4 %5 %6 %7 %8 %9")
            .arg(testData.m_xmlContent.size())
            .arg(key1)
            .arg(testData.m_binContent.size())
            .arg(key2)
            .arg(encrypted)
            .arg(testData.m_imagePath)
            .arg(testData.m_referencePath)
            .arg(useFasterRCNN)
            .arg(algorithmType);

        writeToProcess(cmd);
    }
    else
    {
        EMIT_FATAL_ERROR(QString("DeviceFetcher: unexpected device vendor ") + QString::fromStdString(deviceVendor));
    }
}

void DeviceFetcher::serverOutputToDeviceList(QString str)
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
        SEARCH_FOR_OTHER_DEVICES(m_searchedDeviceVendor);
    }

    if (str.startsWith(QString("getcudadevicesok")))
    {
        if (m_searchedDeviceVendor != NN_CLIENT_NVIDIA)
        {
            // catch logic error
            EMIT_FATAL_ERROR(QString("Got getcudadevicesok for vendor: ") + QString::fromStdString(m_searchedDeviceVendor));
        }

        QString strToParse = str.mid(QString("getcudadevicesok").size());

        std::vector<std::vector<std::string> > parsedDeviceInfo;

        QStringList tokens = strToParse.split(QString("|_|_|*321SEPARATOR*123|_|_|"));

        std::string guid;
        std::string name;
        std::string memAvail;
        std::string memTotal;

        int tokenCnt = 0;
        for (const auto& token: tokens)
        {
            switch (tokenCnt)
            {
                case 1:
                    guid = token.toStdString();
                    break;
                case 2:
                    name = token.toStdString();
                    break;
                case 3:
                    memAvail = token.toStdString();
                    break;
                case 4:
                    memTotal = token.toStdString();
                    parsedDeviceInfo.push_back({ guid, name, memAvail, memTotal });
                    tokenCnt = 0;
                    break;
                default:
                    break;
            }
            ++tokenCnt;
        }

        m_deviceList[m_searchedDeviceVendor] = parsedDeviceInfo;
    }
    else if (str.startsWith(QString("getopenvinodevicesok")))
    {
        if (m_searchedDeviceVendor != NN_CLIENT_INTEL)
        {
            // catch logic error
            EMIT_FATAL_ERROR(QString("Got getopenvinodevicesok for vendor: ") + QString::fromStdString(m_searchedDeviceVendor));
        }

        #if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
            const auto emptySkipFlag = QString::SkipEmptyParts;
        #else
            const auto emptySkipFlag = Qt::SplitBehaviorFlags::SkipEmptyParts;
        #endif

        QStringList tokens = str.split(QRegExp("\\s+"), emptySkipFlag);
        if (tokens.empty())
        {
            SEARCH_FOR_OTHER_DEVICES(m_searchedDeviceVendor);
        }

        // remove getopenvinodevicesok
        tokens.erase(tokens.begin());

        m_deviceList[m_searchedDeviceVendor] = std::vector<std::vector<std::string> >();
        for (const QString& token: tokens)
        {
            m_deviceList[m_searchedDeviceVendor].push_back({ token.toStdString(), std::string("Intel ") + token.toStdString() });
        }
    }
    else
    {
        // error while getting devices for this vendor, treating as no devices available
        SEARCH_FOR_OTHER_DEVICES(m_searchedDeviceVendor);
    }
    emit searchForTaskSetuppersSignal();
}

QString DeviceFetcher::serializeOutputToJson()
{
    nnlab::json outputJson;
    
    const NNClientMap<int, std::string> idxToName = {
        { 0, static_cast<const char *>(NN_CLIENT_DEVICE_PARAM_NAME) },
        { 1, "name" },
        { 2, "availableMemory" },
        { 3, "totalMemory" }
    };

    for (const auto& pair: m_deviceList)
    {
        const auto& deviceVendor = pair.first;
        const auto& vendorOutputVec = pair.second;
        nnlab::json vendorJson;

        for (size_t i = 0; i < vendorOutputVec.size(); ++i)
        {
            if (vendorOutputVec[i].size() > idxToName.size())
            {
                // catch logic error
                emit fatalError(QString("DeviceFetcher::serializeOutputToJson: Number of tokens per device exceeds limit"));
                return QString();
            }

            nnlab::json deviceJson;
            for (size_t j = 0; j < vendorOutputVec[i].size(); ++j)
            {
                deviceJson[idxToName.at(static_cast<int>(j))] = vendorOutputVec[i][j];
            }
            vendorJson.push_back(deviceJson);
        }

        if (!vendorJson.empty())
        {
            outputJson[deviceVendor.c_str()] = vendorJson;
        }
    }

    std::string outputJsonStdString = outputJson.dump(4);
    if (outputJson.empty())
    {
        outputJsonStdString = "{}";
    }
    return QString::fromStdString(outputJsonStdString);
}

void DeviceFetcher::readyStdOutCallback(QString str)
{
    emit readyStdOut(str);
    serverOutputToDeviceList(str);
}

void DeviceFetcher::readyStdErrCallback(QString str)
{
    emit readyStdErr(str);
}

void DeviceFetcher::writeToProcess(QString str)
{
    if (!m_processManager.writeToProcess(str.toStdString()))
    {
        EMIT_FATAL_ERROR(QString("Failed to write to process"));
    }
    emit wroteToProcess(str);
}

} // namespace nnlab
