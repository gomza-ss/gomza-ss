#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

REGISTER_COMMAND_BACK(getalarmids)
    .OutputArg<std::vector<int>>("ids")
    .Parser
    (
        []
        (
            const std::string& serverOutput,
            Config& outputConfig,
            std::string& errorMessage
        ) -> bool
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

            std::vector<int> ids;
            ids.reserve(tokens.size());
            try
            {
                boost::transform(tokens, std::back_inserter(ids), [] (const auto & t) {return std::stoi(t);});
            }
            catch (std::invalid_argument& e)
            {
                errorMessage = e.what();
                return false;
            }

            if (!outputConfig.setParam("ids", ids, errorMessage))
            {
                return false;
            }

            return true;

        }
    );

} // namespace nnlab
