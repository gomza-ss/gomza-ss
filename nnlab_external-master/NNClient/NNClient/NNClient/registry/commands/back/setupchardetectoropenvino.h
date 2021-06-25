#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupchardetectoropenvino)
    .InputArg<int>("char_detector_architecture_size")
    .InputArg<std::string>("char_detector_architecture_key")
    .InputArg<int>("char_detector_weights_size")
    .InputArg<std::string>("char_detector_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("char_detector_device")
    .InputArg<int>("char_detector_width")
    .InputArg<int>("char_detector_height")
    .InputArg<int>("char_detector_nthreads")
    .InputArg<int>("char_detector_nireq")
    .InputArg<std::string>("char_detector_detector")
    .InputArg<std::string>("char_detector_resize_mode");

}
