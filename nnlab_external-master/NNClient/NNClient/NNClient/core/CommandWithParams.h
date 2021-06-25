#pragma once

#include <NNClient/core/Config.h>

#include <string>

namespace nnlab
{

bool commandWithParams(const std::string& command, const Config & config, std::string& output, std::string& errorMessage);

} // namespace nnlab
