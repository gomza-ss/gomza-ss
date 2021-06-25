#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupsinglepersonhelmetdetectorcaffe2)
    .InputArg<int>("helmet_architecture_size")
    .InputArg<std::string>("helmet_architecture_key")
    .InputArg<int>("helmet_weights_size")
    .InputArg<std::string>("helmet_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<float>("helmet_threshold")
    .InputArg<int>("num_managers")
    .InputArg<int>("helmet_net_width")
    .InputArg<int>("helmet_net_height")
    .InputArg<std::string>("helmet_resize_mode");

}
