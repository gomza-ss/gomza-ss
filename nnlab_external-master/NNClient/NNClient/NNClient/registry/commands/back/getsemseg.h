#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getSemSegParser
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
    if (tokens.size() != 2)
    {
        errorMessage = "Unexpected number of tokens in server output";
        return false;
    }

    int imageId = std::stoi(tokens[1]);
    if (!outputConfig.setParam("image_id", imageId, errorMessage))
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(getsemseg)
    .InputArg<int>("image_id")
    .InputArg<int>("min_size", []() { return 384; })
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<int>("image_id")
    .Parser(&getSemSegParser);

} // namespace nnlab
