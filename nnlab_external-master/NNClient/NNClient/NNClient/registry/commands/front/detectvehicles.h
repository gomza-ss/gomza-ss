#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(detectvehicles)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<int>("label", []() { return -1; })
    .InputArg<float>("threshold", []() { return 0.5f; })
    .DefaultSequence()
    .OutputArg<std::vector<ClassifiedDetection> >("detections")
    .OutputArg<int>("image_id");

} // namespace nnlab
