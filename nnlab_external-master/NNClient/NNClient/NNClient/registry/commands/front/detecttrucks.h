#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(detecttrucks)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<std::vector<cv::Rect>>("zones")
    .InputArg<float>("threshold", []() { return 0.5f; })
    .OutputArg<std::string>("truck_status")
    .OutputArg<int>("image_id");

REGISTER_COMMAND_FRONT(detecttrucksasync)
    .TensorRequired()
    .DefaultSequence()
    .InputArg<int>("image_id")
    .InputArg<std::vector<cv::Rect>>("zones")
    .InputArg<float>("threshold", []() { return 0.5f; })
    .OutputArg<std::string>("truck_status")
    .OutputArg<int>("image_id");

} // namespace nnlab
