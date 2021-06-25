#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/CommandProcessorBack.h>

#include <string>

namespace nnlab
{

bool defaultDeploy
(
    CommandProcessorBack& commandProcessorBack,
    Config& inputConfig,
    std::string& errorMessage
);

bool setObjectSizesDeploy
(
    CommandProcessorBack& commandProcessorBack,
    Config& inputConfig,
    std::string& errorMessage
);

}
