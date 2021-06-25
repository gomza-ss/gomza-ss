//
// ВНИМАНИЕ!
// Данный код используется разработчиками NNClient в целях тестирования библиотеки.
// Мы не рекомендуем осуществлять поиск устройств NVIDIA приведенным здесь способом.
//

#include <NNClient/core/CommandProcessorBack.h>
#include <NNClient/core/NNClientLogger.h>
#include <NNClient/core/Macro.h>

#include <NNClient/registry/setup/core/SecurosSearchUtils.h>

#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

#include <iostream>

bool getCUDADevicesInfo(std::vector<nnlab::CudaDeviceInfo> &cudaDevicesInfo, std::string& errorMessage)
{
    errorMessage.clear();

    const std::string caffe2ServerPath = nnlab::getInstallPath() + std::string("/NN_servers/Caffe2Server/Caffe2Server.exe");

    nnlab::Config config;
    config.Arg<std::string>(NN_CLIENT_EXEC_PATH_PARAM_NAME);

    if (!config.setParam<std::string>(NN_CLIENT_EXEC_PATH_PARAM_NAME, caffe2ServerPath, errorMessage))
    {
        return false;
    }

    nnlab::NNClientLogger logger("0");
    nnlab::CommandProcessorBack commandProcessorBack(&logger);
    if (!commandProcessorBack.createProcess(config))
    {
        errorMessage = std::string("Couldn't create process from path: ") + caffe2ServerPath;
        return false;
    }

    QEventLoop eventLoop;

    QObject::connect(
        &commandProcessorBack,
        &nnlab::CommandProcessorBack::responseReady,
        [&](const std::string& /* command */, bool ok, nnlab::Config outputConfig, std::string errorMessageOut)
        {
            if (!ok)
            {
                errorMessage = errorMessageOut;
                return;
            }
            config = outputConfig;
            eventLoop.quit();
        }
    );

    commandProcessorBack.processCommand(std::string("getcudadevices"), config);
    eventLoop.exec();

    if (!errorMessage.empty())
    {
        // was set in lambda slot on failure
        return false;
    }

    // workaround to use std::optional in nnlab::Config::get
    std::vector<nnlab::CudaDeviceInfo> tmpCudaDeviceInfoOut;
    if (!config.get<decltype(tmpCudaDeviceInfoOut)>("devices", tmpCudaDeviceInfoOut))
    {
        errorMessage = std::string("Could not get output devices");
        return false;
    }
    cudaDevicesInfo = tmpCudaDeviceInfoOut;

    return true;
}

struct InternalMain: QObject
{
    Q_OBJECT

public:

    InternalMain() : QObject() {}

public slots:

    void internalMain()
    {
        std::vector<nnlab::CudaDeviceInfo> cudaDevicesInfo;
        std::string errorMessage;

        if (!getCUDADevicesInfo(cudaDevicesInfo, errorMessage)) {
            std::cout << errorMessage << std::endl;
            emit finish();
            return;
        }

        if (cudaDevicesInfo.empty()) {
            std::cout << "No CUDA devices detected" << std::endl;
        }

        for (nnlab::CudaDeviceInfo deviceInfo : cudaDevicesInfo) {
            std::cout << deviceInfo.name << " " << deviceInfo.GUID << " " << deviceInfo.availableMemory << "/" << deviceInfo.totalMemory << std::endl;;
        }

        emit finish();
    }

signals:

    void finish();

} g_internalMain;

#include "CUDADevices.moc"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QObject::connect(&g_internalMain, SIGNAL(finish()), &app, SLOT(quit()));
    QTimer::singleShot(0, &g_internalMain, SLOT(internalMain()));

    return app.exec();
}
