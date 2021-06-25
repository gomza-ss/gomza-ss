#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(truck_detection)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto responseJson = json["truck_status"];
    std::string response;
    if (responseJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field truck_status in output json");
    }
    try
    {
        response = responseJson.get<std::string>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    cv::Mat disp = frame.clone();

    cv::putText(disp, response, cv::Point(0, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

    cv::resize(disp, disp, disp.size() / 2);
    cv::imshow("Result", disp);
    cv::waitKey(1000);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto responseJson = json["truck_status"];
    std::string response;
    if (responseJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field truck_status in output json");
    }
    try
    {
        response = responseJson.get<std::string>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    logFile << "Frame: " << getImageId(outputJson) << std::endl;
    logFile << response << std::endl;
}
);

} // namespace nnlab
