#pragma once

#include <cstdlib>

namespace NNLab
{
    static std::string NMResourcePath()
    {
        #ifdef _WIN32
            const std::string defaultValue("//fs/nnlab/nm_resource/");
        #else
            char* homeEnv = std::getenv("HOME");
            const std::string defaultValue = std::string(homeEnv) + std::string("/NNLab/nm_resource/");
        #endif

        // don't free memory returned by getenv
        // https://stackoverflow.com/questions/4237812/should-i-free-delete-char-returned-by-getenv
        char* getEnvOut = std::getenv("NM_RESOURCE");

        if (getEnvOut == nullptr) {
            return defaultValue;
        }

        std::string envValue = std::string(getEnvOut);

        if (envValue.empty()) {
            return defaultValue;
        }

        std::replace(envValue.begin(), envValue.end(), '\\', '/');
        if (envValue[envValue.size() - 1] != '/') {
            envValue.push_back('/');
        }

        return envValue;
    }
}
