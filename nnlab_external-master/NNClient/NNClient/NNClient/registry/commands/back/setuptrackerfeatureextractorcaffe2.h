#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuptrackerfeatureextractorcaffe2)
    .InputArg<int>("reid_architecture_size")
    .InputArg<std::string>("reid_architecture_key")
    .InputArg<int>("reid_weights_size")
    .InputArg<std::string>("reid_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("reid_width")
    .InputArg<int>("reid_height")
    .InputArg<std::string>("reid_resize_mode");

REGISTER_COMMAND_BACK(setuptrackerfeatureextractormrcnncaffe2)
    .InputArg<int>("reid_architecture_size")
    .InputArg<std::string>("reid_architecture_key")
    .InputArg<int>("reid_weights_size")
    .InputArg<std::string>("reid_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("reid_width")
    .InputArg<int>("reid_height");

}
