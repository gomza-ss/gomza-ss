#pragma once

#include <NNClient/core/Config.h>

#include <string>

namespace nnlab
{

class OnFrameCommandSetter
{
public:

    using OnFrameCommandFunc = std::function<bool(const Config&, std::string&, std::string&)>;

    void OnFrameCommandSetterFn(const OnFrameCommandFunc& fn);
    void OnFrameCommandSetterFn(const std::string& onFrameCommand);

    bool operator() (const Config& inputConfig, std::string& onFrameCommand, std::string& errorMessage);

    inline auto getOnFrameCommandSetterFn() const
    {
        return m_onFrameCommandFunc;
    }
       
private:

    OnFrameCommandFunc m_onFrameCommandFunc;

};

} // namespace nnlab
