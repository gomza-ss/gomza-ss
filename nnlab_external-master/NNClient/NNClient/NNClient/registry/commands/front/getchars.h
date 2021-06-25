#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(getchars)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<int>("lp_label", [](){ return -1; })
    .InputArg<float>("lp_threshold", [](){ return 0.5f; })
    .InputArg<int>("char_label", [](){ return -1; })
    .InputArg<float>("char_threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("lp_detections")
    .OutputArg<std::vector<ClassifiedDetectionBrightness>>("char_detections")
    .OutputArg<std::vector<int>>("char_ids")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(getcharsasync)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<int>("lp_label", [](){ return -1; })
    .InputArg<float>("lp_threshold", [](){ return 0.5f; })
    .InputArg<int>("char_label", [](){ return -1; })
    .InputArg<float>("char_threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("lp_detections")
    .OutputArg<std::vector<ClassifiedDetectionBrightness>>("char_detections")
    .OutputArg<std::vector<int>>("char_ids")
    .OutputArg<int>("image_id");

} // namespace nnlab
