#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(settrackernireq)
    .InputArg<int>("nireq");

}
