#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(mmroutput)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    cv::Mat disp = frame.clone();

    nnlab::json json = nlohmann::json::parse(outputJson);
    auto carFoundJson = json["car_found"];
    if (carFoundJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field car_found in output json");
    }

    const bool carFound = static_cast<bool>(static_cast<int>(carFoundJson));

    if (carFound)
    {
        nnlab::Detection car_bbox;

        auto detectionsJson = json["car_bbox"];
        if (detectionsJson.type() == nlohmann::json::value_t::null)
        {
            crush("Cannot find field car_bbox in output json");
        }
        try
        {
            car_bbox = detectionsJson.get<decltype(car_bbox)>();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse output json: ") + e.what());
        }

        nnlab::MMRInfo mmrInfo;

        auto mmrInfoJson = json["mmrinfo"];
        if (mmrInfoJson.type() == nlohmann::json::value_t::null)
        {
            crush("Cannot find field mmrinfo in output json");
        }
        try
        {
            mmrInfo = mmrInfoJson.get<decltype(mmrInfo)>();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse output json: ") + e.what());
        }

        const std::vector<MMRColorEntry>& colorEntries = mmrInfo.colors;
        const std::vector<MMRModelEntry>& modelEntries = mmrInfo.models;

        if (colorEntries.empty())
        {
            crush(std::string("Empty color entry"));
        }

        if (modelEntries.empty())
        {
            crush(std::string("Empty model entry"));
        }

        std::string maxProbModelName = modelEntries[0].name;
        // for cv::putText
        std::replace(maxProbModelName.begin(), maxProbModelName.end(), '\t', ' ');

        std::string maxProbModelType = modelEntries[0].type;


        const float maxProbModelProbability = modelEntries[0].probability;
        const int maxProbModelId = modelEntries[0].id;

        std::string maxProbColorName = colorEntries[0].name;
        // for cv::putText
        std::replace(maxProbColorName.begin(), maxProbColorName.end(), '\t', ' ');

        const float maxProbColorProbability = colorEntries[0].probability;
        const int maxProbColorId = colorEntries[0].id;

        cv::rectangle(disp, car_bbox.bbox, cv::Scalar(0, 0, 255), 2);

        cv::putText(disp, maxProbColorName + " " + maxProbModelName + " " + maxProbModelType, cv::Point(0, 25), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(disp, "color conf: " + std::to_string(maxProbColorProbability), cv::Point(0, 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(disp, "model conf: " + std::to_string(maxProbModelProbability), cv::Point(0, 75), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(disp, "color id: " + std::to_string(maxProbColorId), cv::Point(0, 100), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(disp, "model id: " + std::to_string(maxProbModelId), cv::Point(0, 125), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("Result", disp);
    cv::waitKey(1500);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto carFoundJson = json["car_found"];
    if (carFoundJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field car_found in output json");
    }

    const bool carFound = static_cast<bool>(static_cast<int>(carFoundJson));

    if (carFound)
    {
        nnlab::Detection car_bbox;

        auto detectionsJson = json["car_bbox"];
        if (detectionsJson.type() == nlohmann::json::value_t::null)
        {
            crush("Cannot find field car_bbox in output json");
        }
        try
        {
            car_bbox = detectionsJson.get<decltype(car_bbox)>();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse output json: ") + e.what());
        }

        nnlab::MMRInfo mmrInfo;

        auto mmrInfoJson = json["mmrinfo"];
        if (mmrInfoJson.type() == nlohmann::json::value_t::null)
        {
            crush("Cannot find field mmrinfo in output json");
        }
        try
        {
            mmrInfo = mmrInfoJson.get<decltype(mmrInfo)>();
        }
        catch (const nnlab::json::exception& e)
        {
            crush(std::string("Failed to parse output json: ") + e.what());
        }

        const std::vector<MMRColorEntry>& colorEntries = mmrInfo.colors;
        const std::vector<MMRModelEntry>& modelEntries = mmrInfo.models;

        if (colorEntries.empty())
        {
            crush(std::string("Empty color entry"));
        }

        if (modelEntries.empty())
        {
            crush(std::string("Empty model entry"));
        }

        std::string maxProbModelName = modelEntries[0].name;
        std::string maxProbModelType = modelEntries[0].type;
        const int maxProbModelId = modelEntries[0].id;
        std::string maxProbColorName = colorEntries[0].name;
        const int maxProbColorId = colorEntries[0].id;

        logFile << "Frame: " << getImageId(outputJson) << std::endl;
        logFile << maxProbModelName << ' ' << maxProbModelType << ' ' << maxProbModelId << ' ' << maxProbColorName << ' ' << maxProbColorId << std::endl;
        logFile << car_bbox.bbox.x << ' ' << car_bbox.bbox.y << ' ' << car_bbox.bbox.width << ' ' << car_bbox.bbox.height << std::endl;
    }
}
);

} // namespace nnlab
