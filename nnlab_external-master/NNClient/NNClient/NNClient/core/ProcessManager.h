#pragma once

#include <NNClient/core/NNClientLogger.h>

#include <QProcess>

#include <memory>

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#elif defined(__linux__)
#include <NNClient/core/Macro.h>
#else
#error "Implement ProcessManager for your system"
#endif

namespace nnlab
{

#ifdef _WIN32
using HandleOwner = std::unique_ptr<std::remove_pointer_t<HANDLE>, void (*)(HANDLE)>;
#endif

class ProcessManager: public QObject
{
    Q_OBJECT
        
public:

    ProcessManager(NNClientLogger* logger);
    ~ProcessManager();

    // creates new process
    // if there is already process running it is terminated
    bool createProcess(const std::string &executablePath);

    bool terminateProcess(bool force = false);

    bool writeToProcess(const std::string &data);

    bool isStarted() const;

signals:

    void readyStdOut(QString str);
    void readyStdErr(QString str);

    void processUnexpectedlyTerminated();

public slots:

    void readyReadStandardOutput();
    void readyReadStandardError();

private:

    void connectStandardStreams();
    void disconnectStandardStreams();

    void setCreateProcessFlags();
    void ensureProcessTermination();

    void emitProcessUnexpectedlyTerminated(int /* exitCode */, QProcess::ExitStatus exitStatus);

#ifdef _WIN32

    HandleOwner makeHandleOwner(HANDLE handle = nullptr) noexcept;
    void assignProcessToJob();

#endif

private:

    QProcess m_process;
    bool m_normalTerminated = false;
    
#ifdef _WIN32
    HandleOwner m_job = makeHandleOwner();
#endif

    NNClientLogger* m_logger;

};

} // namespace nnlab
