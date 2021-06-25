#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupmmrmmclassifiercaffe2)
    .InputArg<int>("mm_classifier_architecture_size")
    .InputArg<std::string>("mm_classifier_architecture_key")
    .InputArg<int>("mm_classifier_weights_size")
    .InputArg<std::string>("mm_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("mm_classifier_width")
    .InputArg<int>("mm_classifier_height")
    .InputArg<std::string>("mm_classifier_resize_mode");

}

