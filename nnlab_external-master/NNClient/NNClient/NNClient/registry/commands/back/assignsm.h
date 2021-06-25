#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(assignsm)
    .InputArg<std::string>("smkey");

} // namespace nnlab
