#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool detectMMRAsyncParser
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

    const int emptyTokensCount = 2;
    if (tokens.size() == emptyTokensCount)
    {
        try
        {
            const int imageId = std::stoi(tokens[1]);
            if (!outputConfig.setParam("image_id", imageId, errorMessage))
            {
                return false;
            }
        }
        catch (const std::invalid_argument& e)
        {
            errorMessage = e.what();
            return false;
        }

        if (!outputConfig.setParam("car_found", 0, errorMessage))
        {
            return false;
        }

        return true;
    }

    try
    {
        const int imageId = std::stoi(tokens[1]);
        if (!outputConfig.setParam("image_id", imageId, errorMessage))
        {
            return false;
        }
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    Detection carBbox;
    try
    {
        carBbox.bbox.x = std::stoi(tokens[2]);
        carBbox.bbox.y = std::stoi(tokens[3]);
        carBbox.bbox.width = std::stoi(tokens[4]);
        carBbox.bbox.height = std::stoi(tokens[5]);
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }
    carBbox.label = 1;

    if (!outputConfig.setParam("car_bbox", carBbox, errorMessage))
    {
        return false;
    }

    const auto colorIt = std::find(tokens.begin(), tokens.end(), "color");
    if (colorIt == tokens.end())
    {
        errorMessage = "\"color\" token not found in server output";
        return false;
    }

    const auto modelIt = std::find(tokens.begin(), tokens.end(), "model");
    if (modelIt == tokens.end())
    {
        errorMessage = "\"model\" token not found in server output";
        return false;
    }

    const auto colorPos = std::distance(tokens.begin(), colorIt);
    const auto modelPos = std::distance(tokens.begin(), modelIt);

    if (modelPos < colorPos)
    {
        errorMessage = "\"model\" token came before \"color\" token";
        return false;
    }

    const int numMMRColorEntryFields = 3;
    const auto modelColorPosDist = modelPos - (colorPos + 1);
    if ((modelColorPosDist == 0) || (modelColorPosDist % numMMRColorEntryFields != 0))
    {
        errorMessage = std::string("distance between \"model\" and (\"color\" + 1) tokens") +
            std::string("should be > 0 and divisible by") +
            std::to_string(numMMRColorEntryFields);
        return false;
    }

    MMRInfo outputMMRInfo;
    for (auto pos = colorPos + 1; pos < modelPos; pos += numMMRColorEntryFields)
    {
        try
        {
            MMRColorEntry colorEntry;
            colorEntry.id = std::stoi(tokens[pos]);

            std::string colorName = tokens[pos + 1];
            std::replace(colorName.begin(), colorName.end(), '^', ' ');
            colorEntry.name = colorName;

            colorEntry.probability = std::stof(tokens[pos + 2]);
            outputMMRInfo.colors.push_back(colorEntry);
        }
        catch (const std::invalid_argument& e)
        {
            errorMessage = e.what();
            return false;
        }
    }

    const int numMMRModelEntryFields = 4;
    const auto endModelPosDist = tokens.size() - (modelPos + 1);
    if ((endModelPosDist == 0) || (endModelPosDist % numMMRModelEntryFields != 0))
    {
        errorMessage = std::string("distance between final token and (\"model\" + 1) token") +
            std::string("should be > 0 and divisible by") +
            std::to_string(numMMRModelEntryFields);
        return false;
    }

    for (decltype(tokens.size()) pos = modelPos + 1; pos < tokens.size(); pos += numMMRModelEntryFields)
    {
        try
        {
            MMRModelEntry modelEntry;
            modelEntry.id = std::stoi(tokens[pos]);

            std::string modelName = tokens[pos + 1];
            std::replace(modelName.begin(), modelName.end(), '^', ' ');
            modelEntry.name = modelName;

            std::string modelType = tokens[pos + 2];
            modelEntry.type = modelType;

            modelEntry.probability = std::stof(tokens[pos + 3]);
            outputMMRInfo.models.push_back(modelEntry);
        }
        catch (const std::invalid_argument& e)
        {
            errorMessage = e.what();
            return false;
        }
    }

    if (!outputConfig.setParam("mmrinfo", outputMMRInfo, errorMessage))
    {
        return false;
    }

    if (!outputConfig.setParam("car_found", 1, errorMessage))
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(detectmmrasync)
    .InputArg<int>("image_id")
    .InputArg<float>("threshold", []() -> float { return 0.5f; })
    .InputArg<cv::Rect>("zone")
    .InputArg<int>("topk", []() -> int { return 3; })
    .OutputArg<int>("image_id")
    .OutputArg<Detection>("car_bbox", []() -> Detection {
        Detection result;
        result.label = 0;
        result.bbox = cv::Rect(0, 0, 0, 0);
        return result;
    })
    .OutputArg<MMRInfo>("mmrinfo", []() -> MMRInfo { return MMRInfo(); })
    .OutputArg<int>("car_found")
    .Parser(&detectMMRAsyncParser);

} // namespace nnlab
