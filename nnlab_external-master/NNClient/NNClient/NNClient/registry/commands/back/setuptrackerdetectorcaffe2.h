#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuptrackerdetectorcaffe2)
    .InputArg<int>("detector_architecture_size")
    .InputArg<std::string>("detector_architecture_key")
    .InputArg<int>("detector_weights_size")
    .InputArg<std::string>("detector_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height")
    .InputArg<int>("label")
    .InputArg<float>("threshold")
    .InputArg<int>("nireq")
    .InputArg<std::string>("resize_mode");

REGISTER_COMMAND_BACK(setuptrackerdetectormrcnncaffe2)
    .InputArg<int>("detector_architecture_size")
    .InputArg<std::string>("detector_architecture_key")
    .InputArg<int>("detector_weights_size")
    .InputArg<std::string>("detector_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height")
    .InputArg<int>("label")
    .InputArg<float>("threshold")
    .InputArg<int>("nireq");


}
