#pragma once

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <NNClient/json/json.h>
#include <NNClient/core/Config.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

 REGISTER_OUTPUT_REPRESENTER(tracks)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string& demoConfig) -> void
{
    bboxesType zones;

    auto zonesJson = (nlohmann::json::parse(demoConfig))["zones"];
    if (zonesJson.type() != nlohmann::json::value_t::null)
    {
        try
        {
            zones = zonesJson.get<decltype(zones)>();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse demoConfig json: ") + e.what());
        }
    }
    
    const auto& parseDetections = [](const nnlab::json& json, const std::string& key, std::vector<Detection>& detections)
    {
        auto detectionsJson = json[key];
        if (detectionsJson.type() == nlohmann::json::value_t::null)
        {
            crush("Cannot find field detections in output json");
        }
        try
        {
            detections = detectionsJson.get<std::vector<Detection> >();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse output json: ") + e.what());
        }
    };

    std::vector<nnlab::Detection> detections;
    nnlab::json json = nlohmann::json::parse(outputJson);
    parseDetections(json, "detections", detections);

    std::vector<nnlab::Detection> predictedDetections;
    parseDetections(json, "predicted_detections", predictedDetections);

    cv::Mat disp = frame.clone();

    for (size_t i = 0; i < detections.size(); ++i) {
        cv::rectangle(disp, detections[i].bbox, cv::Scalar(0, 0, 255), 2);
        cv::putText(disp, std::to_string(detections[i].label), detections[i].bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    }

    for (size_t i = 0; i < predictedDetections.size(); ++i) {
        cv::rectangle(disp, predictedDetections[i].bbox, cv::Scalar(255, 0, 0), 2);
        cv::putText(disp, std::to_string(predictedDetections[i].label), predictedDetections[i].bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    }

    const cv::Scalar zoneColor(255, 255, 0);
    for (const auto& zone : zones) {
        cv::rectangle(disp, zone, zoneColor, 4);
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

}
