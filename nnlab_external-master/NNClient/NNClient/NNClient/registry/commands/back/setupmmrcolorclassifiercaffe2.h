#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupmmrcolorclassifiercaffe2)
    .InputArg<int>("color_classifier_architecture_size")
    .InputArg<std::string>("color_classifier_architecture_key")
    .InputArg<int>("color_classifier_weights_size")
    .InputArg<std::string>("color_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<int>("color_classifier_width")
    .InputArg<int>("color_classifier_height")
    .InputArg<std::string>("color_classifier_resize_mode");

}
