#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(setupfpdetector)
    .InputArg<bboxesType>("zones", [](){return bboxesType{};})
    .InputArg<int>("average_window_size_fp_det")
    .InputArg<int>("duration_threshold")
    .InputArg<int>("moving_duration_threshold")
    .InputArg<float>("move_threshold")
    .InputArg<int>("check_window_size")
    .InputArg<float>("iou_threshold")
    .InputArg<float>("check_ratio_threshold")
    .InputArg<int>("min_width")
    .InputArg<int>("max_width")
    .InputArg<int>("min_height")
    .InputArg<int>("max_height")
    .DefaultSequence();

}
