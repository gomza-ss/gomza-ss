#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(getbboxes)
    .TensorRequired()
    .InputArg<int>("image_id")
    .InputArg<int>("label", []() { return -1; })
    .InputArg<float>("threshold", []() { return 0.5f; })
    .DefaultSequence()
    .OutputArg<std::vector<Detection> >("detections")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(getbboxesasync)
    .TensorRequired()
    .InputArg<int>("image_id")
    .InputArg<int>("label", []() { return -1; })
    .InputArg<float>("threshold", []() { return 0.5f; })
    .DefaultSequence()
    .OutputArg<std::vector<Detection> >("detections")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(getbboxesjitter)
.TensorRequired()
.InputArg<int>("image_id")
.InputArg<int>("label", []() { return -1; })
.InputArg<float>("threshold", []() { return 0.5f; })
.DefaultSequence()
.OutputArg<std::vector<Detection> >("detections")
.OutputArg<int>("image_id");

} // namespace nnlab
