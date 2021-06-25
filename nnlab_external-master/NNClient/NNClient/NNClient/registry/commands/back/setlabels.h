#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_BACK(setlabels)
    .InputArg<string_vector>("text_labels");

}
