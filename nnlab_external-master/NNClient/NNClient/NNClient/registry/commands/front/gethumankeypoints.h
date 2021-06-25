#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(gethumankeypoints)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg<std::vector<std::vector<int> > >("keypoints")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(gethumankeypointsasync)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg<std::vector<std::vector<int> > >("keypoints")
    .OutputArg<int>("image_id");

} // namespace nnlab
