#include <NNClient/NNClient.h>

#include <chrono>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <NNClientDemo.h>
#include <Crush.h>
#include <InitLog4cxx.h>
#include <OutputRepresenters/OutputRepresenterRegistry.h>

namespace
{

int frameCounter = 0;

} // namespace

namespace nnlab
{

NNClientDemo::NNClientDemo(
    const nnlab::json& taskConfig,
    const nnlab::json& demoConfig,
    QObject *parent) : QObject(parent)
{

    initLog4cxx();

    m_nnClient = new NNClient(QString::fromStdString(taskConfig.dump()), this);
    m_taskConfig = taskConfig;
    m_demoConfig = demoConfig;

    QObject::connect
    (
        m_nnClient,
        &NNClient::serverInitialized,
        this,
        &NNClientDemo::readAndProcessFrame
    );

    QObject::connect
    (
        m_nnClient,
        &NNClient::frameResultReady,
        this,
        &NNClientDemo::frameResultReadyCallback
    );

    QObject::connect
    (
        m_nnClient,
        &NNClient::fatalError,
        this,
        &NNClientDemo::fatalErrorCallback
    );

    if (bool(demoConfig["connectStandardStreams"]))
    {
        QObject::connect
        (
            m_nnClient,
            &NNClient::readyStdOut,
            this,
            &NNClientDemo::stdOutCallBack
        );


        QObject::connect
        (
            m_nnClient,
            &NNClient::readyStdErr,
            this,
            &NNClientDemo::stdErrCallBack
        );


        QObject::connect
        (
            m_nnClient,
            &NNClient::wroteToProcess,
            this,
            &NNClientDemo::wroteToProcessCallback
        );

    }

}

void NNClientDemo::wroteToProcessCallback(QString command)
{
    std::cout << "Wrote to Server: " << command.toStdString() << std::endl;
}

void NNClientDemo::stdOutCallBack(QString stdOut)
{
    std::cout << "Got Server stdout: " << stdOut.toStdString();
}

void NNClientDemo::stdErrCallBack(QString stdErr)
{
    std::cerr << "Got Server stderr: " << stdErr.toStdString();
}

void NNClientDemo::frameResultReadyCallback(QString resultJson)
{
    updateFrame(resultJson.toStdString());
}

void NNClientDemo::fatalErrorCallback(QString errorMessage)
{
    crush(errorMessage.toStdString());
}

void NNClientDemo::run()
{
    m_cap = cv::VideoCapture(std::string(m_demoConfig["videoPath"]));
    if (m_demoConfig.count("metadataPath"))
    {
        m_metaStream = std::ifstream(m_demoConfig["metadataPath"].get<std::string>());
    }

    if (!m_cap.isOpened())
    {
        crush(std::string("Could not open video file"));
    }

    m_nnClient->initializeServer();
}

void NNClientDemo::updateFrame(const std::string& resultJson)
{
    m_endPoint = std::chrono::high_resolution_clock::now();
    if (bool(m_demoConfig["displayProcessTime"]))
    {
        std::cout << "Process time: ";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(m_endPoint - m_startPoint).count();
        std::cout << " ms" << std::endl;
    }


    const int imageId = getImageId(resultJson);
    cv::Mat disp;

    if (m_nnClient->hasImage(imageId))
    {
        disp = m_nnClient->popImage(imageId);
        // imageId должен быть последовательным при логировании
        nnlab::json logJson = nnlab::json::parse(resultJson);
        logJson["image_id"] = frameCounter - 1;

        cv::Mat outputImage = m_nnClient->popOutputImage(imageId);
        if (!outputImage.empty())
        {
            std::vector<cv::Mat> channels;
            cv::split(disp, channels);
            channels.push_back(outputImage);
            cv::merge(channels, disp);
        }

        OutputRepresenterRegistry::pOutputRepresenter(std::string(m_demoConfig["onFrameCommandOutputType"]))->apply(disp, logJson.dump(), m_demoConfig.dump(4));
    }
    else
    {
        // frameCounter не должен расти пока не придет первый реальный ответ
        // этот счетчик используется для тестирования NNClient, пользователю он не важен
        frameCounter -= 1;
    }

    readAndProcessFrame();

}

void NNClientDemo::readAndProcessFrame()
{
    cv::Mat frame;
    m_cap >> frame;

    if (frame.empty())
    {
        emit finished();
    }
    else
    {
        std::string meta;
        if (std::getline(m_metaStream, meta))
        {
            std::cout << "Got meta: " << meta << std::endl;
        }
        cv::resize(frame, frame, m_nnClient->imageToSMSize());

        m_startPoint = std::chrono::high_resolution_clock::now();

        // возвращает идентификатор кадра frame в буфере
        const int newImageId = m_nnClient->pushAndProcessImage(frame, QString::fromStdString(meta));
    }

    ++frameCounter;
}

} // namespace nnlab
