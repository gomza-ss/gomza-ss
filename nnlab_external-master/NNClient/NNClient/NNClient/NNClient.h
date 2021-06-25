#pragma once

#include <NNClient/core/Macro.h>
#include <NNClient/json/json.h>

#include <QObject>

#include <set>

namespace nnlab
{

class CommandProcessorFront;
class DeviceFetcher;
class NNClientLogger;

//
// Основной пользовательский класс
// Для работы с NNClient он должен находится в потоке Qt с активный EventLoop
//
class NNClient: public QObject
{

    Q_OBJECT

public:

    // Конструктор принимает json config задачи в виде QString
    NNClient(const QString& config, QObject* parent = nullptr);
    ~NNClient();

    // Инициализирует NN-сервер для решения задачи
    // После инициализации сервер готов выполнять команды
    void initializeServer();

    // Обработчик кадра
    // Конкретная последовательность действий от пользователя скрыта и определяется задачей
    void processImage(int imageId);

    cv::Size imageToSMSize() const;

    //
    // Кадры хранятся в специальном буфере
    // imageId -- идентификатор кадра в буфере
    // Это число должно быть неотрицательным и уникальным для каждого кадра в буфере
    // imageId не обязано монотонно возрастать

    // Проверяет наличие кадра в буффере с идентификатором imageId
    bool hasImage(int imageId) const;
    // Отправляет кадр в буфер
    // Формат цветных изображений: BGR
    void pushImage(int imageId, const cv::Mat& frame, const QString & metadata = QString());
    // Добавляет новый кадр в буфер и отправляет его на обработку
    // Возвращает идентификатор добавленного кадра
    // Формат цветных изображений: BGR
    int pushAndProcessImage(const cv::Mat& frame, const QString & metadata = QString());
    // Получает кадр с идентификатором imageId из буффера
    // Если кадра с таким идентификатором нет, то возаращается пустой cv::Mat
    cv::Mat popImage(int imageId);

    // Получение списка устройств
    // Результат возвращается в слоте deviceListReady
    void getDeviceList();

    // Получает выходное изображение с идентификатором imageId из буффера
    // Если кадра с таким идентификатором нет, то возаращается пустой cv::Mat
    cv::Mat popOutputImage(int imageId);


public slots:

    void commandReady(const std::string& commandName, std::string outputJson);

private slots:

    void serverUnexpectedlyTerminatedCallback();

signals:

    void readyStdOut(QString str);
    void readyStdErr(QString str);

    void wroteToProcess(QString str);

    void fatalError(QString str);

    void serverInitialized();
    void frameResultReady(QString str);

    void deviceListReady(QString str);

private:

    bool checkJson(const std::string& jsonStr);

    void calledOnError(const std::string& errorMessage);

    void initializeHandler();
    void onFrameCommandHandler(const std::string& outputJson);

    int genNewImageId() const;
    void fillMaxBufferSize();

    void setServerInitialized();


private:

    CommandProcessorFront* m_cp;
    DeviceFetcher *m_df;
    NNClientLogger *m_logger;

    json m_taskConfig;
    size_t m_maxBufferSize;

    size_t m_numFatals;
    std::set<int> m_asyncIdsSentToRestart;

    NNClientMap<int, std::pair<cv::Mat, QString>> m_imageBuffer;
    NNClientMap<int, cv::Mat> m_imageOutputBuffer;

    bool m_initialized;

private:

    NN_CLIENT_DISABLE_COPY_AND_MOVE(NNClient)

};

} // namespace nnlab
