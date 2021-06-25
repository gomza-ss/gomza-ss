#pragma once

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <render_human_pose.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(keypoints)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{

    std::vector<std::vector<int> > keypoints;

    nnlab::json json = nlohmann::json::parse(outputJson);
    auto keypointsJson = json["keypoints"];
    if (keypointsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field keypoints in output json");
    }
    try
    {
        keypoints = keypointsJson.get<decltype(keypoints)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    std::vector<human_pose_estimation::HumanPose> poses;

    for (const auto& kpVec : keypoints)
    {
        human_pose_estimation::HumanPose humanPose;

        for (size_t i = 0; i < kpVec.size(); i += 2)
        {
            humanPose.keypoints.emplace_back(kpVec[i], kpVec[i + 1]);
        }

        poses.push_back(humanPose);
    }

    cv::Mat disp = frame.clone();

    const int keypointNumber = 18;

    human_pose_estimation::renderHumanPose(poses, disp, keypointNumber);

    cv::imshow("Result", disp);
    cv::waitKey(1);
}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    std::vector<std::vector<int> > keypoints;

    nnlab::json json = nlohmann::json::parse(outputJson);
    auto keypointsJson = json["keypoints"];
    if (keypointsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field keypoints in output json");
    }
    try
    {
        keypoints = keypointsJson.get<decltype(keypoints)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    const auto imageId = getImageId(outputJson);

    if (imageId > 0)
    {
        logFile << std::endl;
    }

    logFile << "Frame: " << imageId << std::endl;

    for (const auto& kpVec : keypoints)
    {
        for (size_t i = 0; i < kpVec.size(); i += 2)
        {
            logFile << kpVec[i] << " " << kpVec[i + 1] << " ";
        }
    }

}
);

} // namespace nnlab
