#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setdetectorbytextbuffersopenvino)
    .InputArg<int>("model_architecture_size")
    .InputArg<std::string>("model_architecture_key")
    .InputArg<int>("model_weights_size")
    .InputArg<std::string>("model_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("device", []() -> std::string { return std::string("CPU"); })
    .InputArg<int>("net_width")
    .InputArg<int>("net_height")
    .InputArg<int>("nthreads", []() -> int { return 0; })
    .InputArg<int>("nireq", []() -> int { return 1; })
    .InputArg<bool>("use_faster_rcnn", []() -> bool { return false; })
    .InputArg<std::string>("resize_mode");

}
