#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setmaxtimesincematch)
    .InputArg<int>("max_time_since_match");

}
