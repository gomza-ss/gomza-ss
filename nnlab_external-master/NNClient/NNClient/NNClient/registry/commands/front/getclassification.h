#pragma once

#include <NNClient/core/Registry.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(getclassification)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg<NamedClassification>("classification")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(getclassificationasync)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .OutputArg<NamedClassification>("classification")
    .OutputArg<int>("image_id");

} // namespace nnlab
