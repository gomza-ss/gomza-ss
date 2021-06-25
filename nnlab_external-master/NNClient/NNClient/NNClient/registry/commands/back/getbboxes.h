#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getBboxesParser
(
    const std::string& tserverOutput,
    Config& outputConfig,
    std::string& errorMessage
)
{
    const auto& tokensToDetections = [](const std::vector<std::string>& tokens,
                                        std::vector<Detection>& detections)
    {
        Detection detection;
        int tokenCnt = 0;

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
    };

    std::string serverOutput = tserverOutput;

    const std::string separator(" ");
    const std::string okSeparator("ok");
    const std::string readySeparator("ready");

    // filter out ready ending string
    const auto readySepPos = serverOutput.find(readySeparator);
    if (readySepPos != std::string::npos)
    {
        const std::string readySubstring = serverOutput.substr(readySepPos);
        serverOutput = serverOutput.substr(0, readySepPos);

        std::vector<std::string> readySubstrTokens;
        boost::iter_split(readySubstrTokens, readySubstring, boost::first_finder(separator));
        boost::remove_erase_if(readySubstrTokens, [](const auto & t) { return t.empty(); });

        if (readySubstrTokens.size() != 2)
        {
            errorMessage = std::string("Server output ready substring has unexpected number of tokens: ") +
                           std::to_string(readySubstrTokens.size());
            return false;
        }

        try 
        {
            int algorithmReadyFlag = std::stoi(readySubstrTokens[1]);
            if (algorithmReadyFlag != 0 && algorithmReadyFlag != 1)
            {
                errorMessage = std::string("Unexpected ready flag value: ") +
                               std::to_string(algorithmReadyFlag);
                return false;
            }

            if (!outputConfig.setParam<int>("algorithmReady", algorithmReadyFlag, errorMessage)) 
            {
                return false;
            }
        }
        catch (const std::invalid_argument& e)
        {
            errorMessage = e.what();
            return false;
        }
    }

    std::vector<std::string> commandOutputs;

    // this is used to separate predicted tracks
    std::vector<std::string> commandOutTokens;
    boost::iter_split(commandOutTokens, serverOutput, boost::first_finder(okSeparator));

    std::vector<Detection> predictedDetections;

    if (commandOutTokens.size() != 2)
    {
        if (commandOutTokens.size() != 3)
        {
            errorMessage = std::string("unexpected number of ok tokens in gettracks");
            return false;
        }

        std::vector<std::string> predictedDetTokens;
        boost::iter_split(predictedDetTokens, commandOutTokens[commandOutTokens.size() - 1], boost::first_finder(separator));
        boost::remove_erase_if(predictedDetTokens, [](const auto & t) { return t.empty(); });

        if (predictedDetTokens.empty())
        {
            errorMessage = "Image id for predicted det not found";
            return false;
        }
        predictedDetTokens.erase(predictedDetTokens.begin());

        if (predictedDetTokens.size() % 5 != 0) {
            errorMessage = std::string("Amount of tokens in server output ")
                + serverOutput +
                std::string(" is not a multiple of 5");
            return false;
        }

        try
        {
            tokensToDetections(predictedDetTokens, predictedDetections);
        }
        catch (const std::invalid_argument& e)
        {
            errorMessage = e.what();
            return false;
        }

        serverOutput = serverOutput.substr(0, serverOutput.rfind("ok"));
    }

    if (outputConfig.hasArgWithName("predicted_detections"))
    {
        if (!outputConfig.setParam("predicted_detections", predictedDetections, errorMessage))
        {
            return false;
        }
    }

    // now move to standard parsing for getbboxes, gettracks

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

    if (tokens.size() % 5 != 0) {
        errorMessage = std::string("Amount of tokens in server output ")
                       + serverOutput +
                       std::string(" is not a multiple of 5");
        return false;
    }

    std::vector<Detection> detections;

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

REGISTER_COMMAND_BACK(getbboxes)
    .InputArg<int>("image_id")
    .InputArg<int>("label", [](){ return -1; })
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<int>("image_id")
    .Parser(&getBboxesParser);

REGISTER_COMMAND_BACK(getbboxesjitter)
    .InputArg<int>("image_id")
    .InputArg<int>("label", [](){ return -1; })
    .InputArg<float>("threshold", [](){ return 0.5f; })
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<int>("image_id")
    .Parser(&getBboxesParser);

REGISTER_COMMAND_BACK(gettracks)
    .InputArg<int>("image_id")
     // this timestamp is used in helmet detector
     // in other cases just default to zero
    .InputArg<int64_t>("timestamp", []() -> int64_t { return 0; })
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<std::vector<Detection>>("predicted_detections")
    .OutputArg<int>("image_id")
    .OutputArg<int>("algorithmReady", []() { return 1; })
    .Parser(&getBboxesParser);

REGISTER_COMMAND_BACK(getpredictedtracks)
    .InputArg<int>("image_id")
    .InputArg<int>("time")
    .OutputArg<std::vector<Detection>>("detections")
    .OutputArg<int>("image_id")
    .Parser(&getBboxesParser);

} // namespace nnlab
