#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setdetectorbytextbufferscaffe2async)
    .InputArg<int>("model_architecture_size")
    .InputArg<std::string>("model_architecture_key")
    .InputArg<int>("model_weights_size")
    .InputArg<std::string>("model_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("net_width")
    .InputArg<int>("net_height")
    .InputArg<int>("net_channels", []() {return -1; })
    .InputArg<int>("nireq")
    .InputArg<std::string>(NN_CLIENT_EXEC_PATH_TO_SERVER_PARAM_NAME)
    .InputArg<std::string>("resize_mode");

}
