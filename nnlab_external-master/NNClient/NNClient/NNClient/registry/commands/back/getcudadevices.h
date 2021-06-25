#pragma once

#include <NNClient/registry/NNClientRegistries.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <sstream>

namespace nnlab
{

REGISTER_COMMAND_BACK(getcudadevices)
    .OutputArg<std::vector<CudaDeviceInfo> >("devices")
    .Parser
    (
        []
        (
            const std::string& serverOutput,
            Config& outputConfig,
            std::string& errorMessage
        ) -> bool
        {
            const std::string separator("|_|_|*321SEPARATOR*123|_|_|");

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

            std::vector<CudaDeviceInfo> infoList;

            CudaDeviceInfo devInfo;
            int tokenCnt = 0;

            try {
                for (const std::string& token : tokens)
                {
                    switch (tokenCnt)
                    {
                    case 0:
                        devInfo.GUID = token;
                        break;

                    case 1:
                        devInfo.name = token;
                        break;

                    case 2:
                        devInfo.availableMemory = std::stoi(token);
                        break;

                    case 3:
                        devInfo.totalMemory = std::stoi(token);
                        infoList.push_back(devInfo);
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


            if (!outputConfig.setParam("devices", infoList, errorMessage)) {
                return false;
            }


            return true;
        }
    );

}
