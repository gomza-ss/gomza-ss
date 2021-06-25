#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setsizedivisibility)
    .InputArg<int>("size_divisibility");

}
