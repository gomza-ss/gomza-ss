#include <NNClient/core/CommandFront.h>
#include <NNClient/core/CommandProcessorFront.h>
#include <NNClient/core/Registry.h>

#include <NNClient/registry/commands/Front.h>

#include <NNClient/json/json.h>

#include <QThread>

namespace
{

std::string makeErrorJson(const std::string& errorMessage)
{
    nlohmann::json json;

    json["errorMsg"] = errorMessage;
    json["status"] = 1;

    const int indent = 4;
    return json.dump(indent);
}

class ContextHolder
{
public:
    ContextHolder(QTimer & timer, std::vector<std::string>& commands, nnlab::NNClientLogger* logger):
        m_timer(timer), m_commands(commands), m_releaseOnExit(true), m_logger(logger)
    {
    }

    ~ContextHolder()
    {
        if (m_releaseOnExit)
        {
            release();
        }
    }

    void release()
    {
        m_logger->logTrace("Stop watchdog timer");
        m_timer.stop();
        m_commands.clear();
        setReleaseOnExit(false);
    }

    void setReleaseOnExit(bool releaseOnExit)
    {
        m_releaseOnExit = releaseOnExit;
    }

private:
    QTimer& m_timer;
    std::vector<std::string>& m_commands;
    bool m_releaseOnExit;
    nnlab::NNClientLogger* m_logger;
};

} // namespace

namespace nnlab
{

CommandProcessorFront::CommandProcessorFront
(
    NNClientLogger* logger,
    NNClientMap<int, cv::Mat>& imageOutputBuffer,
    QObject* parent
):
    QObject(parent),
    m_watchdogTimeoutMS(0),
    m_commandProcessorBack(logger),
    m_logger(new NNClientLogger(std::string("CPF") + logger->loggerId(), this)),
    m_imageOutputBuffer(imageOutputBuffer)
{

    m_logger->logTrace("Creating CommandProcessorFront");

    QObject::connect
    (
        &m_commandProcessorBack,
        &CommandProcessorBack::responseReady,
        this,
        &CommandProcessorFront::responseReady
    );

    QObject::connect
    (
        &m_commandProcessorBack,
        &CommandProcessorBack::readyStdErrSignal,
        this,
        &CommandProcessorFront::readyStdErr
    );

    QObject::connect
    (
        &m_commandProcessorBack,
        &CommandProcessorBack::readyStdOutSignal,
        this,
        &CommandProcessorFront::readyStdOut
    );

    QObject::connect
    (
        &m_commandProcessorBack,
        &CommandProcessorBack::wroteToProcessSignal,
        this,
        &CommandProcessorFront::wroteToProcess
    );

    QObject::connect
    (
        &m_commandProcessorBack,
        &CommandProcessorBack::serverUnexpectedlyTerminated,
        this,
        &CommandProcessorFront::serverUnexpectedlyTerminated
    );

    QObject::connect
    (
        &m_watchdogTimer,
        &QTimer::timeout,
        this,
        &CommandProcessorFront::commandTimeoutCallback
    );

    m_watchdogTimer.setSingleShot(true);

    m_logger->logTrace("CommandProcessorFront created");
}

CommandProcessorFront::~CommandProcessorFront()
{
    m_logger->logTrace("CommandProcessorFront destructor");
}

bool CommandProcessorFront::processCommand(const std::string& command, const std::string& inputJson, const cv::Mat& frame)
{
    m_logger->logTrace(QString("Processing front command %0").arg(command.c_str()));
    ContextHolder holder(m_watchdogTimer, m_commands, m_logger);

    if (!m_commands.empty())
    {
        return false;
    }

    m_logger->logTrace(QString("Watchdog timeout is %0 ms").arg(m_watchdogTimeoutMS));
    if (m_watchdogTimeoutMS > 0)
    {
        m_watchdogTimer.start(m_watchdogTimeoutMS);
    }
    const auto* entry = CommandFrontRegistry::pCommandFront((command == std::string("onFrameCommand")) ? m_onFrameCommand : command);

    std::string errorMessage;
    if (entry == nullptr)
    {
        holder.release();
        errorMessage = "Command " + command + " not found in registry";
        emit commandReady(command, makeErrorJson(errorMessage));
        return true;
    }

    const bool ignoreExtra = (command != std::string("setup"));

    Config inputConfig = entry->inputConfig;
    if (!inputConfig.initialize(inputJson, errorMessage, ignoreExtra))
    {
        holder.release();
        emit commandReady(command, makeErrorJson(errorMessage));
        return true;
    }

    bool dynamicInputSize = false;
    if (inputConfig.get<bool>("dynamic_input_size", dynamicInputSize) && dynamicInputSize && !frame.empty())
    {
        inputConfig.Arg<int>("input_image_width", [w = frame.cols] () {return w; });
        inputConfig.Arg<int>("input_image_height", [h = frame.rows] () {return h; });
    }

    Config outputConfig = entry->outputConfig;

    std::vector<std::string> commandSequence = entry->commandSequence;

    if (entry->preFn)
    {
        if (!entry->preFn(m_commandProcessorBack, inputConfig, commandSequence, m_onFrameCommand, errorMessage))
        {
            holder.release();
            emit commandReady(command, makeErrorJson(errorMessage));
            return true;
        }
    }

    if (!frame.empty())
    {
        if (!entry->tensorRequired)
        {
            holder.release();
            errorMessage = "Command " + command + " doesn't require image";
            emit commandReady(command, makeErrorJson(errorMessage));
            return true;
        }

        if (!m_commandProcessorBack.fillSM(frame, errorMessage))
        {
            holder.release();
            errorMessage = std::string("Can't write image to shared memory: ") + errorMessage;
            emit commandReady(command, makeErrorJson(errorMessage));
            return true;
        }
    }
    else if (entry->tensorRequired)
    {
        holder.release();
        errorMessage = "Command " + command + " requires image";
        emit commandReady(command, makeErrorJson(errorMessage));
        return true;
    }

    m_commands = std::move(commandSequence);
    m_currentCommand = command;
    m_inputConfig = inputConfig;
    m_outputConfig = outputConfig;

    m_commandProcessorBack.processCommand(m_commands.front(), m_inputConfig);

    holder.setReleaseOnExit(false);

    return true;
}

void CommandProcessorFront::clearCommands()
{
    m_commands.clear();
}

cv::Size CommandProcessorFront::imageToSMSize() const
{
    return m_commandProcessorBack.imageToSMSize();
}

bool CommandProcessorFront::terminateProcess(bool force)
{
    return m_commandProcessorBack.terminateProcess(force);
}

void CommandProcessorFront::setWatchdogTimeoutMS(const int watchdogTimeoutMS)
{
    m_logger->logInfo(QString("Setting watchdog timeout to %0 ms").arg(watchdogTimeoutMS));
    m_watchdogTimeoutMS = watchdogTimeoutMS;
}

void CommandProcessorFront::responseReady(const std::string& /* command */, bool ok, const Config& outputConfig, std::string errorMessage)
{
    ContextHolder holder(m_watchdogTimer, m_commands, m_logger);

    if (!ok)
    {
        holder.release();
        emit commandReady(m_currentCommand, makeErrorJson(errorMessage));
        return;
    }

    if (!m_outputConfig.initialize(outputConfig, errorMessage))
    {
        holder.release();
        emit commandReady(m_currentCommand, makeErrorJson(errorMessage));
        return;
    }

    if (m_commands.empty())
    {
        m_logger->logError("Command sequence is empty in CommandProcessorFront. Stopping process commands");
        return;
    }

    m_commands.erase(m_commands.begin());
    if (!m_commands.empty())
    {
        m_commandProcessorBack.processCommand(m_commands.front(), m_inputConfig);
        holder.setReleaseOnExit(false);
        return;
    }

    if (m_currentCommand == std::string("setup"))
    {
        m_commandProcessorBack.clearNonImageSMs();
    }

    std::string notInitializedElemName;
    if (!m_outputConfig.initialized(notInitializedElemName))
    {
        holder.release();
        errorMessage = std::string("Input argument ") +
            notInitializedElemName +
            std::string(" was not initialized");
        emit commandReady(m_currentCommand, makeErrorJson(errorMessage));
        return;
    }

    const auto* entry = CommandFrontRegistry::pCommandFront(m_onFrameCommand);
    if (m_currentCommand != std::string("setup") && entry->tensorOutputRequired)
    {
        int imageId;
        if (!m_outputConfig.get<int>("image_id", imageId))
        {
            emit commandReady(m_currentCommand, makeErrorJson("Failed to get image_id in output config"));
            return;
        }

        cv::Mat outputImage;
        if (!m_commandProcessorBack.getFromSM(outputImage, errorMessage))
        {
            emit commandReady(m_currentCommand, makeErrorJson(errorMessage));
            return;
        }

        m_imageOutputBuffer[imageId] = outputImage;
    }

    std::string outputJson;
    if (!m_outputConfig.desc(outputJson, errorMessage))
    {
        holder.release();
        emit commandReady(m_currentCommand, makeErrorJson(errorMessage));
        return;
    }

    m_watchdogTimer.stop();
    holder.setReleaseOnExit(false);
    emit commandReady(m_currentCommand, outputJson);
}

void CommandProcessorFront::commandTimeoutCallback()
{
    m_watchdogTimer.stop();
    m_commands.clear();
    emit commandReady(m_currentCommand, makeErrorJson(std::string("Output response timeout")));
    return;
}

} // namespace nnlab
