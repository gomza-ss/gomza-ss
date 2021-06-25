#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/CommandProcessorBack.h>
#include <NNClient/core/NNClientLogger.h>

#include <opencv2/core/core.hpp>

#include <QString>
#include <QTimer>

#include <unordered_map>

namespace nnlab
{

class CommandProcessorFront: public QObject
{
    Q_OBJECT

public:

    CommandProcessorFront
    (
        NNClientLogger* logger,
        NNClientMap<int, cv::Mat>& imageOutputBuffer,
        QObject* parent = nullptr
    );
    ~CommandProcessorFront();

    bool processCommand(const std::string& command, const std::string& inputJson, const cv::Mat& frame = cv::Mat());
    void clearCommands();

    cv::Size imageToSMSize() const;

    bool terminateProcess(bool force = false);

    void setWatchdogTimeoutMS(const int watchdogTimeoutMS);

public slots:

    void responseReady(const std::string& command, bool ok, const Config& outputConfig, std::string errorMessage);

private slots:

    void commandTimeoutCallback();

signals:

    void commandReady(const std::string& command, std::string jsonResponse);

    void readyStdErr(QString str);
    void readyStdOut(QString str);

    void wroteToProcess(QString str);

    void serverUnexpectedlyTerminated();

private:

    QTimer m_watchdogTimer;
    int m_watchdogTimeoutMS;

    CommandProcessorBack m_commandProcessorBack;
    std::vector<std::string> m_commands;
    std::string m_currentCommand;
    std::string m_onFrameCommand;
    Config m_inputConfig;
    Config m_outputConfig;
    NNClientLogger *m_logger;

    NNClientMap<int, cv::Mat>& m_imageOutputBuffer;
};

} // namespace nnlab
