#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupfpdetector)
    .InputArg<bboxesType>("zones")
    .InputArg<int>("average_window_size", []() -> int { return 10; })
    .InputArg<int>("duration_threshold", []() -> int { return 20; })
    .InputArg<int>("moving_duration_threshold", []() -> int { return 3; })
    .InputArg<float>("move_threshold", []() -> float { return 20.0f; })
    .InputArg<int>("check_window_size", []() -> int { return 5; })
    .InputArg<float>("iou_threshold", []() -> float { return 0.7f; })
    .InputArg<float>("check_ratio_threshold", []() -> float { return 0.8f; })
    .InputArg<int>("min_width", []() -> int { return 0; })
    .InputArg<int>("max_width", []() -> int { return 0; })
    .InputArg<int>("min_height", []() -> int { return 0; })
    .InputArg<int>("max_height", []() -> int { return 0; });

}
