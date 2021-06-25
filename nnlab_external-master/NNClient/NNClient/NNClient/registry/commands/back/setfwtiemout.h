#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setfwtimeout)
    .InputArg<int>("fw_timeout_msec");

}
