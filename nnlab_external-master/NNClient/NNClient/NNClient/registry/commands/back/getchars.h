#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getCharsParser
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

    std::vector<Detection> lpDetections;
    std::vector<ClassifiedDetectionBrightness> charDetections;
    std::vector<int> charIds;

    try
    {
        int singleLpTokenCnt = 0;
        int lpCounter = 0;
        ClassifiedDetectionBrightness currentDetection;
        for (const std::string& token : tokens)
        {
            if (token == "lp")
            {
                if (singleLpTokenCnt % 11 != 0)
                {
                    errorMessage = "Bad command output: inconsistent detections";
                    return false;
                }
                ++lpCounter;
                singleLpTokenCnt = 0;
            }
            else
            {
                switch (singleLpTokenCnt % 11)
                {
                case 0:
                    currentDetection.detection.det.label = std::stoi(token);
                    break;
                case 1:
                    currentDetection.detection.det.confidence = std::stof(token);
                    break;
                case 2:
                    currentDetection.detection.det.bbox.x = std::stoi(token);
                    break;
                case 3:
                    currentDetection.detection.det.bbox.y = std::stoi(token);
                    break;
                case 4:
                    currentDetection.detection.det.bbox.width = std::stoi(token);
                    break;
                case 5:
                    currentDetection.detection.det.bbox.height = std::stoi(token);
                    break;
                case 6:
                    currentDetection.detection.label = std::stoi(token);
                    break;
                case 7:
                    currentDetection.detection.confidence = std::stof(token);
                    break;
                case 8:
                    currentDetection.detection.labelStr = token;
                    break;
                case 9:
                    currentDetection.brightnessLabel = std::stoi(token);
                    break;
                case 10:
                    currentDetection.brightnessConfidence = std::stof(token);
                    if (singleLpTokenCnt == 10)
                    {
                        lpDetections.push_back(currentDetection.detection.det);
                    }
                    else
                    {
                        charDetections.push_back(currentDetection);
                        charIds.push_back(lpCounter);
                    }
                    break;
                default:
                    break;
                }

                ++singleLpTokenCnt;
            }
        }
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("lp_detections", lpDetections, errorMessage))
    {
        return false;
    }

    if (!outputConfig.setParam("char_detections", charDetections, errorMessage))
    {
        return false;
    }

    if (!outputConfig.setParam("char_ids", charIds, errorMessage))
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(getchars)
    .InputArg<int>("image_id")
    .InputArg<int>("lp_label", [](){ return -1; })
    .InputArg<float>("lp_threshold", [](){ return 0.5f; })
    .InputArg<int>("char_label", [](){ return -1; })
    .InputArg<float>("char_threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("lp_detections")
    .OutputArg<std::vector<ClassifiedDetectionBrightness>>("char_detections")
    .OutputArg<std::vector<int>>("char_ids")
    .OutputArg<int>("image_id")
    .Parser(&getCharsParser);

REGISTER_COMMAND_BACK(getcharsasync)
    .InputArg<int>("image_id")
    .InputArg<int>("lp_label", [](){ return -1; })
    .InputArg<float>("lp_threshold", [](){ return 0.5f; })
    .InputArg<int>("char_label", [](){ return -1; })
    .InputArg<float>("char_threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("lp_detections")
    .OutputArg<std::vector<ClassifiedDetectionBrightness>>("char_detections")
    .OutputArg<std::vector<int>>("char_ids")
    .OutputArg<int>("image_id")
    .Parser(&getCharsParser);

} // namespace nnlab
