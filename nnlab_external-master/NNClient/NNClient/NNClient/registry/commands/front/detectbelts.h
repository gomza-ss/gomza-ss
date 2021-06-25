#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(detectbelts)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<int>("label", []() { return -1; })
    .InputArg<float>("driver_threshold", []() { return 0.5f; })
    .InputArg<float>("passenger_threshold", []() { return 0.5f; })
    .InputArg<bboxesType>("zones", []() {return bboxesType{}; })
    .DefaultSequence()
    .OutputArg<std::vector<ClassifiedDetection> >("detections")
    .OutputArg<int>("image_id");

} // namespace nnlab
