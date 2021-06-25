#pragma once

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <NNClient/json/json.h>
#include <NNClient/core/Config.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <render_human_pose.hpp>

#include <iostream>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(trackskeypoints)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string& demoConfig) -> void
{
    std::vector<Track> trackskeypoints;

    nlohmann::json json = nlohmann::json::parse(outputJson);
    auto trañksJson = json["trackskeypoints"];
    if (trañksJson.type() == nlohmann::json::value_t::null)
    {
        std::cout << outputJson << std::endl;
        crush("Cannot find field trackskeypoints in output json");
    }

    try
    {
        trackskeypoints = trañksJson.get<decltype(trackskeypoints)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    cv::Mat disp = frame.clone();
    std::vector<human_pose_estimation::HumanPose> poses;
    for (const auto& track : trackskeypoints)
    {
        human_pose_estimation::HumanPose humanPose;

        for (size_t i = 0; i < track.keypoints.size(); i += 2)
        {
            humanPose.keypoints.emplace_back(track.keypoints[i], track.keypoints[i + 1]);
        }

        poses.push_back(humanPose);

        cv::rectangle(disp, track.bbox, cv::Scalar(0, 0, 255), 2);
        cv::putText(disp, std::to_string(track.label), track.bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

    }

 

    const int keypointNumber = 18;

    
    human_pose_estimation::renderHumanPose(poses, disp, keypointNumber);
    
    cv::imshow("Result", disp);
    cv::waitKey(1);
}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    std::vector<Track> trackskeypoints;

    nlohmann::json json = nlohmann::json::parse(outputJson);
    auto trañksJson = json["trackskeypoints"];
    if (trañksJson.type() == nlohmann::json::value_t::null)
    {
        std::cout << outputJson << std::endl;
        crush("Cannot find field trackskeypoints in output json");
    }

    try
    {
        trackskeypoints = trañksJson.get<decltype(trackskeypoints)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    logFile << "Frame: " << getImageId(outputJson) << std::endl;

    for (const auto& track : trackskeypoints)
    {
        logFile << track.label << ' ' << track.bbox.x << ' ' << track.bbox.y << ' ' << track.bbox.width << ' ' << track.bbox.height << ' ';
        for (const auto& kp : track.keypoints)
        {
            logFile << kp << ' ';
        }
    }
    logFile << std::endl;
}
);

} // namespace nnlab
