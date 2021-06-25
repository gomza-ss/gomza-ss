#pragma once

#include <NNClient/core/Registry.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(gethelmettracks)
    .TensorRequired()
    .CommandSequence({ "gettracks", "getalarmids", "gethelmetbboxes" })
    .InputArg<int>("image_id")
    .InputArg<int64_t>("timestamp")
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<std::vector<int>>("ids")
    .OutputArg<int>("algorithmReady", []() -> int { return 1; })
    .OutputArg<std::vector<cv::Rect>>("helmet_bboxes")
    .OutputArg<int>("image_id");

} // namespace nnlab
