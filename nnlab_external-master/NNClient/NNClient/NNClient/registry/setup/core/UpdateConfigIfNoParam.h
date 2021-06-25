#pragma once

#include <NNClient/core/Config.h>

namespace nnlab
{

template<typename T>
bool updateConfigIfNoParam(Config& inputConfig, const std::string& param, const T& val, std::string& errorMessage)
{
    T unusedVal;
    if (!inputConfig.get<T>(param, unusedVal)) {
        inputConfig.Arg<T>(param);
        if (!inputConfig.setParam<T>(param, val, errorMessage)) {
            return false;
        }
    }

    return true;
}

}
