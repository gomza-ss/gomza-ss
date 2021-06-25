#include <NNClient/core/ProcessManager.h>

#include <QCoreApplication>

#include <iostream>

namespace
{
    const int g_processWaitMsecs = 10000;
}

namespace nnlab
{

ProcessManager::ProcessManager(NNClientLogger* logger): m_logger(new NNClientLogger(std::string("PM") + logger->loggerId(), this))
{
    m_logger->logTrace("ProcessManager created");
}

bool ProcessManager::createProcess(const std::string &executablePath)
{
    m_logger->logTrace("Creating process");

    // if process was running and stuck on previous command, say setup,
    // its response may lead to crash in CommandProcessorFront logic
    // therefore, we need to first disconnect the output
    disconnectStandardStreams();

    if (isStarted())
    {
        m_logger->logTrace("Process already started, terminating first");
        if (!terminateProcess())
        {
            return false;
        }
    }

    const QString& qPath = QString::fromLocal8Bit(executablePath.c_str());

    setCreateProcessFlags();

    m_process.setProgram(qPath);
    m_normalTerminated = false;
    m_process.start();

    if (!m_process.waitForStarted(g_processWaitMsecs))
    {
        return false;
    }

    ensureProcessTermination();

    connectStandardStreams();

    m_logger->logInfo(QString("Created process (pid %1)").arg(m_process.processId()));

    return true;
}

bool ProcessManager::terminateProcess(bool force)
{
    if (m_process.state() == QProcess::NotRunning)
    {
        return false;
    }

    const auto pid = m_process.processId();
    m_logger->logInfo(QString("Terminating process (pid %1)").arg(pid));

    disconnectStandardStreams();

    m_normalTerminated = true;

    if (!force)
    {
        writeToProcess("quit");
    }

    if (!force && m_process.waitForFinished(g_processWaitMsecs))
    {
        m_logger->logInfo(QString("Process terminated normally (pid %1)").arg(pid));
    }
    else
    {
        m_process.disconnect();
        m_process.kill();
        m_process.waitForFinished(g_processWaitMsecs);
        m_logger->logWarning(QString("Process terminated with TerminateProcess (pid %1)").arg(pid));
    }

    return true;
}

bool ProcessManager::writeToProcess(const std::string &data)
{
    if (m_process.state() == QProcess::NotRunning)
        return false;

    std::string dataWithBackslash = data;
    dataWithBackslash.push_back('\n');
    m_process.write(dataWithBackslash.c_str(), dataWithBackslash.size());

    return m_process.waitForBytesWritten();
}

bool ProcessManager::isStarted() const
{
    return (m_process.state() == QProcess::Running);
}

ProcessManager::~ProcessManager()
{
    m_logger->logTrace("ProcessManager destructor begin");
    terminateProcess();
    m_logger->logTrace("ProcessManager destructor finished");
}

void ProcessManager::readyReadStandardOutput()
{
    QByteArray bytes;
    bytes = m_process.readAllStandardOutput();
    QString string = QString::fromLocal8Bit(bytes);

    emit readyStdOut(std::move(string));
}

void ProcessManager::readyReadStandardError()
{
    QByteArray bytes;
    bytes = m_process.readAllStandardError();
    QString string = QString::fromLocal8Bit(bytes);

    emit readyStdErr(std::move(string));
}

void ProcessManager::connectStandardStreams()
{
    QObject::connect
    (
        &m_process,
        &QProcess::readyReadStandardOutput,
        this,
        &ProcessManager::readyReadStandardOutput,
        static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection)
    );
    QObject::connect
    (
        &m_process,
        &QProcess::readyReadStandardError,
        this,
        &ProcessManager::readyReadStandardError,
        static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection)
    );
    QObject::connect
    (
        &m_process,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this,
        &ProcessManager::emitProcessUnexpectedlyTerminated,
        static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection)
    );
}

void ProcessManager::disconnectStandardStreams()
{
    QObject::disconnect
    (
        &m_process,
        &QProcess::readyReadStandardOutput,
        this,
        &ProcessManager::readyReadStandardOutput
    );
    QObject::disconnect
    (
        &m_process,
        &QProcess::readyReadStandardError,
        this,
        &ProcessManager::readyReadStandardError
    );
    QObject::disconnect
    (
        &m_process,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this,
        &ProcessManager::emitProcessUnexpectedlyTerminated
    );
}

void ProcessManager::ensureProcessTermination()
{
#ifdef _WIN32
    assignProcessToJob();
#elif defined(__linux__)
    std::string NN_CLIENT_UNUSED_VAR explanation("In Linux server terminates itself if parent dies");
#else
#error "Implement ProcessManager::ensureProcessTermination() for your system"
#endif
}

void ProcessManager::emitProcessUnexpectedlyTerminated(int /* exitCode */, QProcess::ExitStatus exitStatus)
{
    if (!m_normalTerminated || (exitStatus != QProcess::NormalExit))
    {
        emit processUnexpectedlyTerminated();
    }
}

void ProcessManager::setCreateProcessFlags()
{

#ifdef _WIN32

    m_process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args) {
        args->flags |= CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED;
    });

    return;

#endif

}

#ifdef _WIN32

void ProcessManager::assignProcessToJob()
{
    auto abort = [this](const std::string & /* message */)
    {
        terminateProcess();
    };

// we need deprecated pid() here to access PROCESS_INFORMATION
#pragma warning( push )
#pragma warning( disable: 4996 )
    PROCESS_INFORMATION processInfo;
    memcpy(&processInfo, reinterpret_cast<void*>(m_process.pid()), sizeof(processInfo));
#pragma warning( pop )

    m_job = makeHandleOwner(::CreateJobObjectW(nullptr, nullptr));
    if (!m_job)
    {
        abort("CreateJobObjectW() failed, " + std::to_string(GetLastError()));
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_BREAKAWAY_OK;
    if (FALSE == ::SetInformationJobObject(m_job.get(), JobObjectExtendedLimitInformation, &info, sizeof(info)))
    {
        abort("SetInformationJobObject() failed, " + std::to_string(GetLastError()));
    }

    if (FALSE == ::AssignProcessToJobObject(m_job.get(), processInfo.hProcess))
    {
        abort("AssignProcessToJobObject() failed, " + std::to_string(GetLastError()));
    }

    if (FALSE == ::ResumeThread(processInfo.hThread))
    {
        abort("ResumeThread() failed, " + std::to_string(GetLastError()));
    }
}

HandleOwner ProcessManager::makeHandleOwner(HANDLE handle) noexcept
{
    return
    {
        handle,
        [](HANDLE handle)
        {
            if (handle != nullptr)
                ::CloseHandle(handle);
        }
    };
}

#endif // _WIN32

} // namespace nnlab
