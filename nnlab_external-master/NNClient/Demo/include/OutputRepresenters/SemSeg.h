#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(semseg)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);
    cv::Mat semseg = channels.back();
    channels.pop_back();
    cv::Mat disp;
    cv::merge(channels, disp);

    cv::imshow("Source", disp);
    cv::imshow("SemSeg", semseg);
    cv::waitKey(0);
}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    logFile << "Frame: " << getImageId(outputJson) << std::endl;
}
);

} // namespace nnlab
