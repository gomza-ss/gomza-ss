#include <NNClient/NNClient.h>

#include <chrono>
#include <iostream>

#include <DeviceDemo.h>
#include <Crush.h>
#include <InitLog4cxx.h>

#include <fstream>

namespace nnlab
{

DeviceDemo::DeviceDemo(
    const nnlab::json& taskConfig,
    QObject *parent) : QObject(parent)
{
    initLog4cxx();

    m_nnClient = new NNClient(QString::fromStdString(taskConfig.dump()), this);
    m_taskConfig = taskConfig;

    QObject::connect
    (
        m_nnClient,
        &NNClient::deviceListReady,
        this,
        &DeviceDemo::deviceListReadyCallback
    );

    QObject::connect
    (
        m_nnClient,
        &NNClient::fatalError,
        this,
        &DeviceDemo::fatalErrorCallback
    );

    QObject::connect
    (
        m_nnClient,
        &NNClient::readyStdOut,
        this,
        &DeviceDemo::stdOutCallBack
    );
    
    QObject::connect
    (
        m_nnClient,
        &NNClient::readyStdErr,
        this,
        &DeviceDemo::stdErrCallBack
    );
    
    QObject::connect
    (
        m_nnClient,
        &NNClient::wroteToProcess,
        this,
        &DeviceDemo::wroteToProcessCallback
    );

}

void DeviceDemo::wroteToProcessCallback(QString command)
{
    std::cout << "Wrote to Server: " << command.toStdString() << std::endl;
}

void DeviceDemo::stdOutCallBack(QString stdOut)
{
    std::cout << "Got Server stdout: " << stdOut.toStdString();
}

void DeviceDemo::stdErrCallBack(QString stdErr)
{
    std::cerr << "Got Server stderr: " << stdErr.toStdString();
}

void DeviceDemo::deviceListReadyCallback(QString resultJson)
{
    m_endPoint = std::chrono::high_resolution_clock::now();
    std::cout << resultJson.toStdString() << std::endl;
    std::cout << "Search time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(m_endPoint - m_startPoint).count() / 1000.0
              << " secs" << std::endl;
    std::ofstream("deviceDetectOutput.json") << resultJson.toStdString();
    emit finished();
}

void DeviceDemo::fatalErrorCallback(QString errorMessage)
{
    crush(errorMessage.toStdString());
}

void DeviceDemo::run()
{
    m_startPoint = std::chrono::high_resolution_clock::now();
    m_nnClient->getDeviceList();
}

} // namespace nnlab
