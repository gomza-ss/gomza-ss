#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setuptrackerfeatureextractoropenvino)
    .InputArg<int>("reid_architecture_size")
    .InputArg<std::string>("reid_architecture_key")
    .InputArg<int>("reid_weights_size")
    .InputArg<std::string>("reid_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("reid_device")
    .InputArg<int>("reid_width")
    .InputArg<int>("reid_height")
    .InputArg<int>("reid_batch_size")
    .InputArg<int>("reid_nthreads")
    .InputArg<int>("reid_nireq")
    .InputArg<std::string>("reid_resize_mode");

}
