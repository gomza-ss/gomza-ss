#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupcharclassifieropenvino)
    .InputArg<int>("char_classifier_architecture_size")
    .InputArg<std::string>("char_classifier_architecture_key")
    .InputArg<int>("char_classifier_weights_size")
    .InputArg<std::string>("char_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("char_classifier_device")
    .InputArg<int>("char_classifier_width")
    .InputArg<int>("char_classifier_height")
    .InputArg<int>("char_classifier_nthreads")
    .InputArg<int>("char_classifier_nireq")
    .InputArg<std::string>("char_classifier_resize_mode");

}
