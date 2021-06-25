#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab 
{

REGISTER_COMMAND_BACK(setupbodytrackcaffe2)
    .InputArg<int>("model_architecture_size")
    .InputArg<std::string>("model_architecture_key")
    .InputArg<int>("model_weights_size")
    .InputArg<std::string>("model_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height");

REGISTER_COMMAND_BACK(setupbodytrackmrcnncaffe2)
    .InputArg<int>("model_architecture_size")
    .InputArg<std::string>("model_architecture_key")
    .InputArg<int>("model_weights_size")
    .InputArg<std::string>("model_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height");

}
