#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(resizeinput)
    .InputArg<int>("input_image_width")
    .InputArg<int>("input_image_height");

}
