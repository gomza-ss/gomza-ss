#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

std::vector<nnlab::Detection> getDetectionsFromJson(const nnlab::json &json, const std::string &field)
{
    std::vector<nnlab::Detection> detections;
    auto jsonField = json[field];
    if (jsonField.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field " + field + " detections in output json");
    }
    try
    {
        detections = jsonField.get<decltype(detections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    return detections;
}

std::vector<nnlab::ClassifiedDetectionBrightness> getClassifiedDetectionsBrightnessFromJson
(
    const nnlab::json &json,
    const std::string &field
)
{
    std::vector<nnlab::ClassifiedDetectionBrightness> detections;
    auto jsonField = json[field];
    if (jsonField.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field " + field + " detections in output json");
    }
    try
    {
        detections = jsonField.get<decltype(detections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    return detections;
}


std::vector<int> getCharIDsFromJson(const nnlab::json &json)
{
    std::vector<int> ids;
    const std::string field("char_ids");
    auto jsonField = json[field];
    if (jsonField.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field " + field + " detections in output json");
    }

    try
    {
        ids = jsonField.get<decltype(ids)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    return ids;
}

//
// Выходной формат
//
// lp_detections -- обрамляющие прямоугольники номеров
// char_detections -- обрамляющие прямоугольники символов
// char_ids -- вектор длины char_detections, хранящий индексы lp_detections
// То есть char_ids позволяет понять, к какому номеру из lp_detections относится текущий символ из char_detections
//

REGISTER_OUTPUT_REPRESENTER(chars)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto lpDetections = getDetectionsFromJson(json, "lp_detections");
    auto charDetections = getClassifiedDetectionsBrightnessFromJson(json, "char_detections");
    auto charIds = getCharIDsFromJson(json);

    cv::Mat disp = frame.clone();

    std::vector<cv::Scalar> colors = {cv::Scalar(0, 255, 0), cv::Scalar(255, 0, 0) };
    for (size_t i = 0; i < lpDetections.size(); ++i)
    {
        const auto& det = lpDetections[i];
        cv::rectangle(disp, det.bbox, cv::Scalar(0, 0, 255), 4);
        cv::putText(disp, std::to_string(det.label), det.bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, colors.back(), 2);
    }
    for (size_t i = 0; i < charDetections.size(); ++i)
    {
        const auto &det = charDetections[i];
        cv::rectangle(disp, det.detection.det.bbox, colors[det.brightnessLabel], 4);
        cv::putText(disp, det.detection.labelStr, det.detection.det.bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, colors[charIds[i]], 2);
    }

    cv::resize(disp, disp, disp.size() / 2);
    cv::imshow("Result", disp);
    cv::waitKey(1);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto lpDetections = getDetectionsFromJson(json, "lp_detections");
    auto charDetections = getClassifiedDetectionsBrightnessFromJson(json, "char_detections");

    logFile << "Frame: " << getImageId(outputJson) << std::endl;
    logFile << "Plates: " << getImageId(outputJson) << std::endl;
    for (const auto &det : lpDetections)
    {
        logFile << det.label << ' ' << det.bbox.x << ' ' << det.bbox.y << ' ' << det.bbox.width << ' ' << det.bbox.height << std::endl;
    }
    logFile << "Chars:" << std::endl;
    for (const auto &det : charDetections)
    {
        logFile
            << det.detection.label << ' '
            << det.detection.det.bbox.x << ' '
            << det.detection.det.bbox.y << ' '
            << det.detection.det.bbox.width << ' '
            << det.detection.det.bbox.height << ' '
            << det.brightnessLabel << ' '
            << det.brightnessConfidence << std::endl;
    }
}
);

} // namespace nnlab
