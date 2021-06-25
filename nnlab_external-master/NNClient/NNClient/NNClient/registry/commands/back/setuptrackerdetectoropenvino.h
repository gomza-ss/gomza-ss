#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuptrackerdetectoropenvino)
    .InputArg<int>("detector_architecture_size")
    .InputArg<std::string>("detector_architecture_key")
    .InputArg<int>("detector_weights_size")
    .InputArg<std::string>("detector_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("device")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height")
    .InputArg<int>("nthreads")
    .InputArg<int>("nireq")
    .InputArg<bool>("use_faster_rcnn")
    .InputArg<int>("label")
    .InputArg<float>("threshold")
    .InputArg<std::string>("resize_mode");

}
