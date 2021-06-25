#pragma once

#include <NNClient/registry/commands/back/getbboxes.h>
#include <NNClient/registry/commands/back/gethumankeypoints.h>

#include <iostream>
namespace nnlab
{
bool getBboxesAndHumanKeypointsParser
(
    const std::string& serverOutput,
    Config& outputConfig,
    std::string& errorMessage
)
{
    const std::string separator(" ");

    std::vector<std::string> tokens;
    boost::iter_split(tokens, serverOutput, boost::first_finder(separator));

    boost::remove_erase_if(tokens, [](const auto & t) { return t.empty(); });
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
    std::vector<Track> trackskeypoints;

    try
    {
        Track tmpTrack;
        int tokenCnt = 0;
        for (const std::string& token : tokens)
        {
            if (token == "human")
            {
                if (tmpTrack.keypoints.size() % 2 == 1)
                {
                    errorMessage = "Bad command output: number of coordinates should be divisible by 2";
                    return false;
                }
                trackskeypoints.push_back(tmpTrack);
                tmpTrack.keypoints.clear();
                tokenCnt = 0;
            }
            else
            {
                switch (tokenCnt)
                {
                case 0:
                    tmpTrack.label = std::stoi(token);
                    break;

                case 1:
                    tmpTrack.bbox.x = std::stoi(token);
                    break;

                case 2:
                    tmpTrack.bbox.y = std::stoi(token);
                    break;

                case 3:
                    tmpTrack.bbox.width = std::stoi(token);
                    break;
                case 4:
                    tmpTrack.bbox.height = std::stoi(token);
                    break;

                default:
                    tmpTrack.keypoints.push_back(std::stoi(token));
                    break;
                }
                ++tokenCnt;
            }
        }
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("trackskeypoints", trackskeypoints, errorMessage))  // coping!!!
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(gettracksandkeypoints)
    .InputArg<int>("image_id")
    .OutputArg< std::vector<Track>>("trackskeypoints")
    .OutputArg<int>("image_id")
    .OutputArg<int>("algorithmReady", []() { return 1; })
    .Parser(&getBboxesAndHumanKeypointsParser);

}