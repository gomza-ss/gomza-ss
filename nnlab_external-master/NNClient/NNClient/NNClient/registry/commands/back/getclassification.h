#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

bool getClassificationParser
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
    if (tokens.size() != 4)
    {
        errorMessage = "Unexpected number of tokens in server output";
        return false;
    }

    if (!outputConfig.setParam("image_id", std::stoi(tokens[1]), errorMessage))
    {
        return false;
    }

    NamedClassification classification;
    classification.name = tokens[2];

    try
    {
        classification.confidence = std::stof(tokens[3]);
    }
    catch (const std::invalid_argument& e)
    {
        errorMessage = e.what();
        return false;
    }

    if (!outputConfig.setParam("classification", classification, errorMessage))
    {
        return false;
    }

    return true;
};

REGISTER_COMMAND_BACK(getclassification)
    .InputArg<int>("image_id")
    .OutputArg<NamedClassification>("classification")
    .OutputArg<int>("image_id")
    .Parser(&getClassificationParser);

REGISTER_COMMAND_BACK(getclassificationasync)
    .InputArg<int>("image_id")
    .OutputArg<NamedClassification>("classification")
    .OutputArg<int>("image_id")
    .Parser(&getClassificationParser);

} // namespace nnlab
