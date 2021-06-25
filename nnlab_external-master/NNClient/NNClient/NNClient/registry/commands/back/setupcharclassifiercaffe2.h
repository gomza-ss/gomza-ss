#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupcharclassifiercaffe2)
    .InputArg<int>("char_classifier_architecture_size")
    .InputArg<std::string>("char_classifier_architecture_key")
    .InputArg<int>("char_classifier_weights_size")
    .InputArg<std::string>("char_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("char_classifier_width")
    .InputArg<int>("char_classifier_height")
    .InputArg<std::string>("char_classifier_resize_mode");

}
