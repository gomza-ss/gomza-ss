#pragma once

#include <string>

#include <QObject>
#include <QString>

#include <log4cxx/logger.h>

#define NN_CLIENT_LOGGER_NAME "nnclient"

namespace nnlab
{

class NNClientLogger : public QObject
{
    Q_OBJECT

public:

    NNClientLogger(const std::string& loggerId, QObject* parent = nullptr);
    ~NNClientLogger();

    std::string loggerId()
    {
        return m_loggerId;
    }

public slots:

    void logTrace(const QString& str);
    void logDebug(const QString& str);
    void logInfo(const QString& str);
    void logWarning(const QString& str);
    void logError(const QString& str);
    void logFatal(const QString& str);

private:

    std::string m_loggerId;
    log4cxx::LoggerPtr m_logger;
    bool m_canLog;

};

} // namespace nnlab
