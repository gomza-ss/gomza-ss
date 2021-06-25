#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/Macro.h>
#include <NNClient/core/ProcessManager.h>
#include <NNClient/core/NNClientLogger.h>

#include <QObject>
#include <QSharedMemory>

namespace nnlab
{

class DeviceFetcher: public QObject
{

    Q_OBJECT

public:

    DeviceFetcher(NNClientLogger* logger, QObject* parent = nullptr);
    bool getDeviceList(const ::nnlab::json& taskConfig, std::string& errorMessage);

private:

    void clearDeviceSearchState();
    void prepareTaskSetuppersToSearch(const std::string& module, const std::string& task);

    void searchForTaskSetupper(const std::string& taskSetupperToSearch);

    void serverOutputToDeviceList(QString str);

    void writeToProcess(QString str);

    QString serializeOutputToJson();

private slots:

    void searchForTaskSetuppers();

    void readyStdOutCallback(QString str);
    void readyStdErrCallback(QString str);

signals:
       
    void readyStdOut(QString str);
    void readyStdErr(QString str);

    void wroteToProcess(QString str);

    void searchForTaskSetuppersSignal();
    void fatalError(QString str);

    void deviceListReady(QString deviceList);

private:

    ProcessManager m_processManager;

    NNClientMap<std::string, std::vector<std::vector<std::string> > > m_deviceList;
    NNClientMap<std::string, QSharedMemory> m_openVINOSearchSMs;

    std::vector<std::string> m_taskSetuppersToSearch;
    std::string m_searchedDeviceVendor;

private:

    NN_CLIENT_DISABLE_COPY_AND_MOVE(DeviceFetcher)

};

} // namespace nnlab
