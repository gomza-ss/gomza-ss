#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setupmmrmmclassifieropenvino)
    .InputArg<int>("mm_classifier_architecture_size")
    .InputArg<std::string>("mm_classifier_architecture_key")
    .InputArg<int>("mm_classifier_weights_size")
    .InputArg<std::string>("mm_classifier_weights_key")
    .InputArg<bool>("encrypted")
    .InputArg<std::string>("mm_classifier_device")
    .InputArg<int>("mm_classifier_width")
    .InputArg<int>("mm_classifier_height")
    .InputArg<int>("mm_classifier_nthreads")
    .InputArg<int>("mm_classifier_nireq")
    .InputArg<std::string>("mm_classifier_resize_mode");

}
