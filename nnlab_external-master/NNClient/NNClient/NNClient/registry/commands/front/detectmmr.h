#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(detectmmr)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<float>("threshold", []() { return 0.5f; })
    .InputArg<cv::Rect>("zone")
    .InputArg<int>("topk", []() -> int { return 3; })
    .OutputArg<int>("image_id")
    .OutputArg<Detection>("car_bbox")
    .OutputArg<MMRInfo>("mmrinfo")
    .OutputArg<int>("car_found");

REGISTER_COMMAND_FRONT(detectmmrasync)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<float>("threshold", []() { return 0.5f; })
    .InputArg<cv::Rect>("zone")
    .InputArg<int>("topk", []() -> int { return 3; })
    .OutputArg<int>("image_id")
    .OutputArg<Detection>("car_bbox")
    .OutputArg<MMRInfo>("mmrinfo")
    .OutputArg<int>("car_found");

} // namespace nnlab
