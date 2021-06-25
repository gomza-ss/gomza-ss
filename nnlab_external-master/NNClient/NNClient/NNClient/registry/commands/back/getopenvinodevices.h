#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace nnlab
{

REGISTER_COMMAND_BACK(getopenvinodevices)
    .OutputArg<std::vector<std::string>>("devices")
    .DontExpectStatusOK()
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

            boost::remove_erase_if(tokens, [](const auto & t){return t.empty();});
            if (tokens.empty())
            {
                errorMessage = "Empty response";
                return false;
            }
            tokens.erase(tokens.begin());

            if (!outputConfig.setParam("devices", tokens, errorMessage))
            {
                return false;
            }


            return true;
        }
    );

}
