#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool detectTrucksParser
(
    const std::string& serverOutput,
    Config& outputConfig,
    std::string& errorMessage
)
{
    const std::string separator(" ");

    std::vector<std::string> tokens;
    boost::iter_split(tokens, serverOutput, boost::first_finder(separator));

    boost::remove_erase_if(tokens, [](const auto & t){ return t.empty();});
    if (tokens.size() != 3)
    {
        errorMessage = "Unexpected number of tokens in server output";
        return false;
    }

    int imageId = std::stoi(tokens[1]);
    if (!outputConfig.setParam("image_id", imageId, errorMessage))
    {
        return false;
    }

    static const std::array<std::string, 3> allowedRespones = {"no_object", "truck", "not_truck"};
    int responseId = std::stoi(tokens[2]);
    if (responseId >= static_cast<int>(allowedRespones.size()) || responseId < 0)
    {
        errorMessage = "Unexpected truck checker response";
        return false;
    }

    if (!outputConfig.setParam("truck_status", allowedRespones[responseId], errorMessage))
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(detecttrucks)
    .InputArg<int>("image_id")
    .InputArg<std::vector<cv::Rect>>("zones")
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<std::string>("truck_status")
    .OutputArg<int>("image_id")
    .Parser(&detectTrucksParser);

} // namespace nnlab
