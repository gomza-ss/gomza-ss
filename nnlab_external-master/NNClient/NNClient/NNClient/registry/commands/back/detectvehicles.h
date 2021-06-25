#pragma once

#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

bool detectVehiclesParser
(
    const std::string& serverOutput,
    Config& outputConfig,
    std::string& errorMessage
)
{
    const auto& tokensToDetections = [](const std::vector<std::string>& tokens,
                                        std::vector<ClassifiedDetection>& detections)
    {
        ClassifiedDetection detection;
        detections.clear();
        int tokenCnt = 0;

        for (const std::string& token : tokens)
        {
            switch (tokenCnt)
            {
            case 0:
                detection.label = std::stoi(token);
                detection.det.label = std::stoi(token);
                break;

            case 1:
                detection.det.bbox.x = std::stoi(token);
                break;

            case 2:
                detection.det.bbox.y = std::stoi(token);
                break;

            case 3:
                detection.det.bbox.width = std::stoi(token);
                break;
            case 4:
                detection.det.bbox.height = std::stoi(token);
                break;
            case 5:
                detection.confidence = std::stof(token);
                detection.det.confidence = std::stof(token);
                break;
            case 6:
                detection.labelStr = token;
                detections.push_back(detection);
                tokenCnt = -1;
                break;
            default:
                break;
            }
            ++tokenCnt;
        }
    };

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

    if (tokens.empty())
    {
        errorMessage = "Image id not found";
        return false;
    }

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

    if (tokens.size() % 7 != 0) {
        errorMessage = std::string("Amount of tokens in server output ")
                       + serverOutput +
                       std::string(" is not a multiple of 7");
        return false;
    }

    std::vector<ClassifiedDetection> detections;

    try
    {
        tokensToDetections(tokens, detections);
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

    return true;
};

REGISTER_COMMAND_BACK(detectvehicles)
    .InputArg<int>("image_id")
    .InputArg<int>("label", [](){ return -1; })
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<int>("image_id")
    .Parser(&detectVehiclesParser);

} // namespace nnlab
