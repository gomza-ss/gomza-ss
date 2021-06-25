#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setpredicttime)
    .InputArg<int>("predict_time");

}
