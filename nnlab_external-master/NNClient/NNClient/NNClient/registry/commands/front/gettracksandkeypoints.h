#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(gettracksandkeypoints)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg< std::vector<Track>>("trackskeypoints")
    .OutputArg<int>("algorithmReady")
    .OutputArg<int>("image_id");

}
