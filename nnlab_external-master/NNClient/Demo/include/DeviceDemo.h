#pragma once

#include <NNClient/NNClient.h>

namespace nnlab
{

class DeviceDemo: public QObject
{
    Q_OBJECT

public:

    DeviceDemo(
        const nnlab::json& taskConfig,
        QObject *parent = 0);

public slots:

    void run();

    void wroteToProcessCallback(QString command);
    void stdOutCallBack(QString stdOut);
    void stdErrCallBack(QString stdErr);

    void deviceListReadyCallback(QString resultJson);

    void fatalErrorCallback(QString errorMessage);

signals:

    void finished();
    
private:

    NNClient* m_nnClient;

    nnlab::json m_taskConfig;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_endPoint;

};

} // namespace nnlab
