#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getHumanKeypointsParser
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
    if (tokens.empty())
    {
        errorMessage = "Empty response";
        return false;
    }
    tokens.erase(tokens.begin());

    int imageId = 0;
    try
    {
        imageId = std::stoi(tokens[0]);
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("image_id", imageId, errorMessage))
    {
        return false;
    }
    
    tokens.erase(tokens.begin());

    std::vector<std::vector<int>> keypoints;

    std::vector<int> singleHumanKeypoints;
    try
    {
        for (const std::string& token : tokens)
        {
            if (token == "human")
            {
                if (singleHumanKeypoints.size() % 2 == 1)
                {
                    errorMessage = "Bad command output: number of coordinates should be divisible by 2";
                    return false;
                }
                keypoints.push_back(singleHumanKeypoints);
                singleHumanKeypoints.clear();
            }
            else
            {
                singleHumanKeypoints.push_back(std::stoi(token));
            }
        }
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("keypoints", keypoints, errorMessage))
    {
        return false;
    }

    return true;
};


REGISTER_COMMAND_BACK(gethumankeypoints)
    .InputArg<int>("image_id")
    .OutputArg<std::vector<std::vector<int>>>("keypoints")
    .OutputArg<int>("image_id")
    .Parser(&getHumanKeypointsParser);

}
