#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(getsemseg)
    .TensorRequired()
    .TensorOutputRequired()
    .EnableDynamicInputSize()
    .InputArg<int>("image_id")
    .InputArg<int>("min_size", []() { return 384; })
    .InputArg<float>("threshold", []() { return 0.5f; })
    .DefaultSequence()
    .OutputArg<int>("image_id");

} // namespace nnlab
