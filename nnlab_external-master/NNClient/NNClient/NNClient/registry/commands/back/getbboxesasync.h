#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getBboxesAsyncParser
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
    if (tokens.size() < 2)
    {
        errorMessage = "Empty response";
        return false;
    }
    tokens.erase(tokens.begin());

    if ((tokens.size() - 1) % 5 != 0) {
        errorMessage = std::string("Amount of tokens in server output ")
                       + serverOutput +
                       std::string(" is not a multiple of 5");
        return false;
    }

    std::vector<Detection> detections;

    Detection detection;
    int tokenCnt = 0;

    int imageId;
    try
    {
        imageId = std::stoi(tokens[0]);
        tokens.erase(tokens.begin());

        for (const std::string& token : tokens)
        {
            switch (tokenCnt)
            {
            case 0:
                detection.label = std::stoi(token);
                break;

            case 1:
                detection.bbox.x = std::stoi(token);
                break;

            case 2:
                detection.bbox.y = std::stoi(token);
                break;

            case 3:
                detection.bbox.width = std::stoi(token);
                break;
            case 4:
                detection.bbox.height = std::stoi(token);
                detections.push_back(detection);
                tokenCnt = -1;
                break;
            default:
                break;
            }
            ++tokenCnt;
        }
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("detections", detections, errorMessage))
    {
        return false;
    }
    if (!outputConfig.setParam("image_id", imageId, errorMessage))
    {
        return false;
    }

    return true;
};


REGISTER_COMMAND_BACK(getbboxesasync)
    .InputArg<int>("image_id")
    .InputArg<int>("label", [](){ return -1; })
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<int>("image_id")
    .Parser(&getBboxesAsyncParser);

} // namespace nnlab
