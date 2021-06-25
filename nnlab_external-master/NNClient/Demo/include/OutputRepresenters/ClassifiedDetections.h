#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(classified_detections)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    std::vector<nnlab::ClassifiedDetection> detections;

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
        cv::rectangle(disp, detections[i].det.bbox, cv::Scalar(0, 0, 255), 2);
        const std::string instanceText = std::to_string(detections[i].label) + std::string(" ") + detections[i].labelStr;
        cv::putText(disp, instanceText, detections[i].det.bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("Result", disp);
    cv::waitKey(1);

}
)
.LogFunc(
    [](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    std::vector<nnlab::ClassifiedDetection> detections;

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
        logFile << det.label << ' ' << det.det.bbox.x << ' ' << det.det.bbox.y << ' ' << det.det.bbox.width << ' ' << det.det.bbox.height <<
                    ' ' << det.labelStr << std::endl;
    }
}
);

} // namespace nnlab
