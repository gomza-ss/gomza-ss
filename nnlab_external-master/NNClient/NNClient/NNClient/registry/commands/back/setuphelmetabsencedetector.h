#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuphelmetabsencedetector)
    .InputArg<int>("window_size")
    .InputArg<float>("absence_ratio_threshold")
    .InputArg<float>("helmet_threshold")
    .InputArg<cv::Rect>("zone")
    .InputArg<int>("input_image_width")
    .InputArg<int>("input_image_height")
    .InputArg<float>("minFPS", []() -> float {
        // time is in msec, so we divide fps by 1000
        const auto minFPS = 7.0f / 1000.0f ;
        return minFPS;
    });

}
