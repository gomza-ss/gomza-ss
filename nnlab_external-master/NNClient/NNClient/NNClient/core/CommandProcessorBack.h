#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/NNClientLogger.h>
#include <NNClient/core/ProcessManager.h>

#include <opencv2/core/core.hpp>

#include <QSharedMemory>

namespace nnlab
{

class CommandProcessorBack : public QObject
{
    Q_OBJECT

public:

    CommandProcessorBack(NNClientLogger* logger);
    ~CommandProcessorBack();

    void processCommand(
        const std::string& command,
        const Config& inputConfig);

    bool createSM(Config & inputConfig, std::string & errorMessage);

    bool createProcess(const Config& inputConfig);

    bool terminateProcess(bool force = false);

    bool setSize(const Config & config);

    bool setChannels(const Config & config);

    bool loadModel(const std::string& name, Config & config);

    bool fillSM(const cv::Mat& image, std::string & errorMessage);

    bool getFromSM(cv::Mat& image, std::string& errorMessage);

    void clearNonImageSMs();

    cv::Size imageToSMSize() const;

public slots:

    void readyStdOutSlot(QString str);

signals:

    void responseReady(const std::string& command, bool ok, const Config& outputConfig, std::string errorMessage);

    void readyStdErrSignal(QString str);
    void readyStdOutSignal(QString str);

    void wroteToProcessSignal(QString str);

    void serverUnexpectedlyTerminated();

private:
    bool fillSMFromFile
    (
        const std::string& path,
        const std::string& keyPrefix,
        Config& config
    );

    bool loadModelMeta(const std::string& path, Config& config) const;

    ProcessManager m_processManager;
    NNClientMap<std::string, QSharedMemory> m_sharedMemoryMap;
    int m_width{ 0 };
    int m_height{ 0 };
    int m_channels{ 0 };

    const std::string m_imageSMKey{"image"};

    NNClientLogger* m_logger;
};

} // namespace nnlab
