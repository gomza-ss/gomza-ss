#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setnchannels)
    .InputArg<int>("channels");

}
