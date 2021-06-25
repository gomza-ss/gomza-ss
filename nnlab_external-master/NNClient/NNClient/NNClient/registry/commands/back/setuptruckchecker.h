#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuptruckchecker)
    .InputArg<int>("target_label", []() -> int { return 2; })
    .InputArg<float>("iou_threshold", []() -> float { return 0.001f; });

}
