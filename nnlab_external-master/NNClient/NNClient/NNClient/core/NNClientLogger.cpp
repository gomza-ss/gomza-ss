#include <NNClient/core/NNClientLogger.h>
#include <NNClient/core/NNClientTypes.h>

namespace
{

enum class LogLevel
{
    NNCLIENT_TRACE = 0,
    NNCLIENT_DEBUG = 1,
    NNCLIENT_INFO = 2,
    NNCLIENT_WARNING = 3,
    NNCLIENT_ERROR = 4,
    NNCLIENT_FATAL = 5
};

void logImpl(const log4cxx::LoggerPtr& logger, const LogLevel& level, const QString& str, bool canLog, const std::string& loggerId)
{
    const nnlab::NNClientMap<LogLevel, log4cxx::LevelPtr> logLevelMap = {
        { LogLevel::NNCLIENT_TRACE, log4cxx::Level::getTrace() },
        { LogLevel::NNCLIENT_DEBUG, log4cxx::Level::getDebug() },
        { LogLevel::NNCLIENT_INFO, log4cxx::Level::getInfo() },
        { LogLevel::NNCLIENT_WARNING, log4cxx::Level::getWarn() },
        { LogLevel::NNCLIENT_ERROR, log4cxx::Level::getError() },
        { LogLevel::NNCLIENT_FATAL, log4cxx::Level::getFatal() },
    };

    if (!logLevelMap.count(level))
    {
        if (canLog)
        {
            // this case signals a bug in logging logic
            LOG4CXX_WARN(logger, "Logging error in nnclient: unexpected logLevel");
            return;
        }
    }

    if (canLog)
    {
        LOG4CXX_LOG(logger, logLevelMap.at(level), str.toStdString() + std::string(" (") + loggerId + std::string(")"));
    }
}

} // namespace

namespace nnlab
{

NNClientLogger::NNClientLogger(const std::string& loggerId, QObject* parent): QObject(parent), m_canLog(false)
{
    m_logger = log4cxx::Logger::getLogger(NN_CLIENT_LOGGER_NAME);

    // we can log only in case of existing appenders
    m_canLog = !m_logger->getAllAppenders().empty();

    m_loggerId = loggerId;

    this->logTrace("Logger created");
}

NNClientLogger::~NNClientLogger()
{
    this->logTrace("Logger destructor");
}

void NNClientLogger::logTrace(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_TRACE, str, m_canLog, m_loggerId);
}

void NNClientLogger::logDebug(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_DEBUG, str, m_canLog, m_loggerId);
}

void NNClientLogger::logInfo(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_INFO, str, m_canLog, m_loggerId);
}

void NNClientLogger::logWarning(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_WARNING, str, m_canLog, m_loggerId);
}

void NNClientLogger::logError(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_ERROR, str, m_canLog, m_loggerId);
}

void NNClientLogger::logFatal(const QString& str)
{
    logImpl(m_logger, LogLevel::NNCLIENT_FATAL, str, m_canLog, m_loggerId);
}

} // namespace nnlab
