#include <NNClient/NNClient.h>

#include <NNClient/core/CommandProcessorFront.h>
#include <NNClient/core/DeviceFetcher.h>
#include <NNClient/core/NNClientLogger.h>

#include <QCoreApplication>

#include <set>

namespace nnlab
{

class NNClientIdGenerator
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

std::string NNClientIdGenerator::m_processPID = std::to_string(QCoreApplication::applicationPid());
std::atomic<int> NNClientIdGenerator::m_state{};

NNClient::NNClient(const QString& config, QObject* parent):
    QObject(parent),
    m_numFatals(0),
    m_initialized(false)
{
    m_taskConfig = nnlab::json::parse(config.toStdString());
    fillMaxBufferSize();

    m_logger = new NNClientLogger(NNClientIdGenerator::generate(), this);
    m_logger->logTrace("Creating NNClient");

    m_df = new DeviceFetcher(m_logger, this);
    m_cp = new CommandProcessorFront(m_logger, m_imageOutputBuffer, this);

    const int watchdogTimeoutMSDefault = 60000;
    const int watchdogTimeout = getIntFromJson(m_taskConfig, "on_frame_timeout_ms", watchdogTimeoutMSDefault);
    m_cp->setWatchdogTimeoutMS(watchdogTimeout);

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::commandReady,
        this,
        &NNClient::commandReady
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::serverUnexpectedlyTerminated,
        this,
        &NNClient::serverUnexpectedlyTerminatedCallback
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::readyStdOut,
        this,
        &NNClient::readyStdOut
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::readyStdErr,
        this,
        &NNClient::readyStdErr
    );


    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::wroteToProcess,
        this,
        &NNClient::wroteToProcess
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::readyStdOut,
        m_logger,
        &NNClientLogger::logTrace
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::readyStdErr,
        m_logger,
        &NNClientLogger::logWarning
    );

    QObject::connect
    (
        m_cp,
        &CommandProcessorFront::wroteToProcess,
        m_logger,
        &NNClientLogger::logTrace
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::deviceListReady,
        this,
        &NNClient::deviceListReady
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::deviceListReady,
        m_logger,
        &NNClientLogger::logInfo
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::readyStdOut,
        this,
        &NNClient::readyStdOut
    );
    
    QObject::connect
    (
        m_df,
        &DeviceFetcher::readyStdErr,
        this,
        &NNClient::readyStdErr
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::wroteToProcess,
        this,
        &NNClient::wroteToProcess
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::fatalError,
        this,
        &NNClient::fatalError
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::readyStdOut,
        m_logger,
        &NNClientLogger::logInfo
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::readyStdErr,
        m_logger,
        &NNClientLogger::logWarning
    );

    QObject::connect
    (
        m_df,
        &DeviceFetcher::wroteToProcess,
        m_logger,
        &NNClientLogger::logInfo
    );

    m_logger->logTrace("NNClient created");
}

NNClient::~NNClient()
{
    m_logger->logTrace("NNClient destructor");
}

void NNClient::commandReady(const std::string& commandName, std::string outputJson) //-V813
{
    if (!checkJson(outputJson))
    {
        return;
    }

    if (commandName == "setup")
    {
        initializeHandler();
        return;
    }
    else if (commandName == "onFrameCommand")
    {
        onFrameCommandHandler(outputJson);
        return;
    }
}

void NNClient::serverUnexpectedlyTerminatedCallback()
{
    calledOnError(std::string("Server unexpectedly terimnated"));
}

void NNClient::initializeServer()
{
    m_initialized = false;

    m_logger->logInfo(QString("\nInitializing server from config:\n") +
                      QString::fromStdString(m_taskConfig.dump(4)));

    m_cp->clearCommands();

    if (!m_cp->processCommand("setup", m_taskConfig.dump()))
    {
        // not happening, if we called clearCommands()
        calledOnError("Server is already analyzing other command");
        return;
    }
}

void NNClient::processImage(int imageId)
{
    if (!m_initialized)
    {
        return;
    }

    if (!hasImage(imageId))
    {
        calledOnError(std::string("Image with id ") + std::to_string(imageId) + std::string(" is not in buffer"));
        return;
    }

    m_logger->logTrace(QString("Processing image with id ") + QString::number(imageId));

    json inputJson = m_taskConfig;
    inputJson["image_id"] = imageId;
    std::string metadata = m_imageBuffer[imageId].second.toStdString();
    if (!metadata.empty())
    {
        json metadataJson;
        std::string errorMessage;
        if (!jsonSafeParse(metadata, metadataJson, errorMessage))
        {
            calledOnError(std::string("Failed to parse metadata: ") + errorMessage);
            return;
        }
        else
        {
            for (auto f : metadataJson.items())
            {
                inputJson[f.key()] = f.value();
            }
        }
    }
    m_cp->processCommand(std::string("onFrameCommand"), inputJson.dump(), m_imageBuffer[imageId].first);
}

bool NNClient::checkJson(const std::string& jsonStr)
{
    nnlab::json json = nnlab::json::parse(jsonStr);
    const auto statusJson = json["status"];
    const auto errorMessageJson = json["errorMsg"];
    std::string errorMessage = "Json does not contain errorMsg field";
    if (errorMessageJson.type() == nlohmann::json::value_t::string)
    {
        errorMessage = errorMessageJson.get<std::string>();
    }

    if (statusJson.type() != nnlab::json::value_t::number_integer && statusJson.type() != nnlab::json::value_t::number_unsigned)
    {
        calledOnError(errorMessage);
        return false;
    }

    if (statusJson.get<int>() != 0)
    {
        calledOnError(errorMessage);
        return false;
    }

    return true;
}

void NNClient::calledOnError(const std::string& errorMessage)
{
    m_initialized = false;
    ++m_numFatals;

    m_cp->terminateProcess(true);
    m_cp->clearCommands();

    if (!errorMessage.empty())
    {
        m_logger->logError(QString("Got fatalError: ") + QString::fromStdString(errorMessage));
    }

    const int defaultNumFatals = 3;
    const int fatalsLimit = getIntFromJson(m_taskConfig, "fatalsLimit", defaultNumFatals);
    m_logger->logInfo(QString("Current fatals: %0, fatals limit: %1").arg(m_numFatals).arg(fatalsLimit));
    if (fatalsLimit > 0 && static_cast<int>(m_numFatals) > fatalsLimit) 
    {
        emit fatalError(QString::fromStdString(errorMessage));
    }
    else
    {
        m_logger->logInfo(QString("Reinitializing server"));
        initializeServer();
    }
}

void NNClient::initializeHandler()
{
    m_logger->logInfo(QString("Server was successfully initialized"));
    m_logger->logInfo(QString("imageBuffer size: %0").arg(m_imageBuffer.size()));

    if (m_imageBuffer.empty())
    {
        if (m_numFatals <= 0)
        {
            // normal case
            // inform user about initialization finish
            setServerInitialized();
            emit serverInitialized();
        }

        m_logger->logInfo(QString("Reseting numFatals to 0"));
        m_numFatals = 0;
    }
    else
    {
        // this happens after fatal error or 
        // reinitialization of server after server crash

        const int defaultNireq = 0;
        const int nireq = getIntFromJson(m_taskConfig, std::string("nireq"), defaultNireq);

        if (nireq <= 0)
        {
            // catch logic bug
            if (m_imageBuffer.size() != 1)
            {
                emit fatalError(QString("Expceted imageBuffer size to be 1, got ") +
                                QString::number(m_imageBuffer.size()) +
                                QString(" instead"));
                return;
            }

            // sequintial processing, single image
            const int imageId = m_imageBuffer.cbegin()->first;
            setServerInitialized();
            processImage(imageId);

            return;
        }
        else
        {
            while (m_imageBuffer.size() < m_maxBufferSize - 1)
            {
                pushImage(genNewImageId(), m_imageBuffer.begin()->second.first, m_imageBuffer.begin()->second.second);
            }

            m_asyncIdsSentToRestart.clear();

            const int imageId = m_imageBuffer.cbegin()->first;
            m_asyncIdsSentToRestart.insert(imageId);
            setServerInitialized();
            processImage(imageId);

            return;
        }
    }

}

void NNClient::onFrameCommandHandler(const std::string& outputJson)
{
    const int defaultNireq = 0;
    const int nireq = getIntFromJson(m_taskConfig, std::string("nireq"), defaultNireq);

    if (nireq <= 0)
    {
        // sequintial processing
        // if frame was processed we assume setup was successfull
        // and set m_numFatals = 0
        m_logger->logTrace(QString("Sequential processing. Reseting numFatals to 0"));
        m_numFatals = 0;
    }
    else if (m_numFatals > 0 && !m_imageBuffer.empty())
    {
        // parallel case restart is handled separately
        m_logger->logInfo(QString("numFatals: %0").arg(m_numFatals));
        m_logger->logInfo(QString("Processing images in buffer"));

        const int defaultImageId = -1;
        const int imageId = getIntFromJson(nnlab::json::parse(outputJson), std::string("image_id"), defaultImageId);

        if (!hasImage(imageId))
        {
            for (auto it = m_imageBuffer.cbegin(); it != m_imageBuffer.end(); ++it)
            {
                if (m_asyncIdsSentToRestart.find(it->first) == m_asyncIdsSentToRestart.end())
                {
                    m_asyncIdsSentToRestart.insert(it->first);
                    processImage(it->first);
                    return;
                }
            }

            // catch logic bug
            emit fatalError(QString("Async restart logic error: images finished before positive id"));
            return;
        }

        // all images in buffer were processed
        m_logger->logInfo(QString("All images in buffer were processed. Reseting numFatals to 0"));
        m_numFatals = 0;
    }

    m_logger->logTrace(QString("Frame was processed"));
    emit frameResultReady(QString::fromStdString(outputJson));
}

int NNClient::genNewImageId() const
{
    std::set<int> usedIds;
    for (decltype(m_imageBuffer)::const_iterator it = m_imageBuffer.begin(); it != m_imageBuffer.end(); ++it)
    {
        usedIds.insert(usedIds.end(), it->first);
    }

    for (size_t i = 0; i < m_imageBuffer.size() + 1; ++i)
    {
        if (usedIds.find(static_cast<int>(i)) == usedIds.end())
        {
            return static_cast<int>(i);
        }
    }

    int newId = *(std::max_element(usedIds.begin(), usedIds.end())) + 1;
    return newId;
}

void NNClient::fillMaxBufferSize()
{
    // we need this copy to avoid adding null "nireq" field
    auto taskConfigCopy = m_taskConfig;

    const auto nireqJson = taskConfigCopy["nireq"];
    if (nireqJson.type() == nlohmann::json::value_t::null)
    {
        // standard mode
        m_maxBufferSize = 1;
        return;
    }

    int nireq = 0;
    try
    {
        nireq = nireqJson.get<decltype(nireq)>();
    }
    catch (const nnlab::json::exception& )
    {
        // standard mode
        m_maxBufferSize = 1;
        return;
    }

    if (nireq <= 0)
    {
        // standard mode
        m_maxBufferSize = 1;
        return;
    }

    // parallel mode
    m_maxBufferSize = nireq + 1;
}

cv::Size NNClient::imageToSMSize() const
{
    return m_cp->imageToSMSize();
}

bool NNClient::hasImage(int frameIdimageId) const
{
    return m_imageBuffer.count(frameIdimageId) > 0;
}

void NNClient::pushImage(int imageId, const cv::Mat& image, const QString& metadata)
{
    if (m_imageBuffer.size() >= m_maxBufferSize)
    {
        calledOnError(std::string("Image buffer is full"));
        return;
    }

    m_logger->logTrace(QString("Pushing new image with id ") + QString::number(imageId));

    // operator[] overrides element if such exists, don't use insert/emplace
    m_imageBuffer[imageId] = std::make_pair(image, metadata);
}

int NNClient::pushAndProcessImage(const cv::Mat& frame, const QString& metadata)
{
    int newImageId = genNewImageId();
    pushImage(newImageId, frame, metadata);
    processImage(newImageId);
    return newImageId;
}

cv::Mat NNClient::popImage(int imageId)
{
    if (!hasImage(imageId))
    {
        return cv::Mat();
    }

    cv::Mat result = m_imageBuffer[imageId].first;
    m_imageBuffer.erase(imageId);

    return result;
}

void NNClient::getDeviceList()
{
    m_logger->logInfo(QString("\nGetting device list from config:\n") +
                      QString::fromStdString(m_taskConfig.dump(4)));

    std::string errorMessage;
    if (!m_df->getDeviceList(m_taskConfig, errorMessage))
    {
        calledOnError(errorMessage);
        return;
    }
}

cv::Mat NNClient::popOutputImage(int imageId)
{
    if (m_imageOutputBuffer.count(imageId) == 0)
    {
        return cv::Mat();
    }

    cv::Mat result = m_imageOutputBuffer[imageId];
    m_imageOutputBuffer.erase(imageId);

    return result;
}

void NNClient::setServerInitialized()
{
    m_logger->logInfo("Setting m_initialized to true");

    m_initialized = true;
}

}
