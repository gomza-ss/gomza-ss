#pragma once

#include <NNClient/NNClient.h>

#include <opencv2/videoio.hpp>

#include <fstream>

namespace nnlab
{

class NNClientDemo: public QObject
{
    Q_OBJECT

public:

    NNClientDemo(
        const nnlab::json& taskConfig,
        const nnlab::json& demoConfig,
        QObject *parent = 0);

public slots:

    void run();

    void wroteToProcessCallback(QString command);
    void stdOutCallBack(QString stdOut);
    void stdErrCallBack(QString stdErr);

    void frameResultReadyCallback(QString resultJson);

    void fatalErrorCallback(QString errorMessage);

signals:

    void finished();

private:

    void readAndProcessFrame();
    void updateFrame(const std::string& outputJson);

private:

    NNClient* m_nnClient;

    cv::VideoCapture m_cap;

    nnlab::json m_taskConfig, m_demoConfig;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_endPoint;

    std::ifstream m_metaStream;

};

}
