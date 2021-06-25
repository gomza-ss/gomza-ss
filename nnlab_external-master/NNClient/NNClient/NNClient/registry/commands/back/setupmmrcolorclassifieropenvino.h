#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupmmrcolorclassifieropenvino)
    .InputArg<int>("color_classifier_architecture_size")
    .InputArg<std::string>("color_classifier_architecture_key")
    .InputArg<int>("color_classifier_weights_size")
    .InputArg<std::string>("color_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("color_classifier_device")
    .InputArg<int>("color_classifier_width")
    .InputArg<int>("color_classifier_height")
    .InputArg<int>("color_classifier_nthreads")
    .InputArg<int>("color_classifier_nireq")
    .InputArg<std::string>("color_classifier_resize_mode");

}
