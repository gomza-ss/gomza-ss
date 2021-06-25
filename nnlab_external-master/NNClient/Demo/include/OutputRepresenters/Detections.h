#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(detections)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    std::vector<nnlab::Detection> detections;

    nnlab::json json = nlohmann::json::parse(outputJson);
    auto detectionsJson = json["detections"];
    if (detectionsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field detections in output json");
    }
    try
    {
        detections = detectionsJson.get<decltype(detections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    cv::Mat disp = frame.clone();

    for (size_t i = 0; i < detections.size(); ++i) {
        cv::rectangle(disp, detections[i].bbox, cv::Scalar(0, 0, 255), 2);
        cv::putText(disp, std::to_string(detections[i].label), detections[i].bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("Result", disp);
    cv::waitKey(1);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    std::vector<nnlab::Detection> detections;

    nnlab::json json = nlohmann::json::parse(outputJson);
    auto detectionsJson = json["detections"];
    if (detectionsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field detections in output json");
    }
    try
    {
        detections = detectionsJson.get<decltype(detections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    logFile << "Frame: " << getImageId(outputJson) << std::endl;

    for (const auto det : detections)
    {
        logFile << det.label << ' ' << det.bbox.x << ' ' << det.bbox.y << ' ' << det.bbox.width << ' ' << det.bbox.height << std::endl;
    }
}
);

} // namespace nnlab
