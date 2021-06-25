#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(gettracks)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<std::vector<Detection>>("predicted_detections")
    .OutputArg<int>("algorithmReady")
    .OutputArg<int>("image_id");

}
