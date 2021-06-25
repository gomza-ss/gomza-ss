#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getHelmetBboxesParser
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

    if (tokens.size() % 4 != 0) {
        errorMessage = std::string("Amount of tokens in server output ")
                       + serverOutput +
                       std::string(" is not a multiple of 4");
        return false;
    }

    std::vector<cv::Rect> bboxes;

    cv::Rect bbox;
    int tokenCnt = 0;

    try
    {
        for (const std::string& token : tokens)
        {
            switch (tokenCnt)
            {
            case 0:
                bbox.x = std::stoi(token);
                break;

            case 1:
                bbox.y = std::stoi(token);
                break;

            case 2:
                bbox.width = std::stoi(token);
                break;
            case 3:
                bbox.height = std::stoi(token);
                bboxes.push_back(bbox);
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

    if (!outputConfig.setParam("helmet_bboxes", bboxes, errorMessage))
    {
        return false;
    }

    return true;
};


REGISTER_COMMAND_BACK(gethelmetbboxes)
    .OutputArg<std::vector<cv::Rect>>("helmet_bboxes")
    .Parser(&getHelmetBboxesParser);

} // namespace nnlab
