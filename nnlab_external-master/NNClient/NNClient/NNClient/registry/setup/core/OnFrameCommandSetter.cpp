#include <NNClient/registry/setup/core/OnFrameCommandSetter.h>

namespace nnlab
{

void OnFrameCommandSetter::OnFrameCommandSetterFn(const OnFrameCommandFunc& fn)
{
    m_onFrameCommandFunc = fn;
}

void OnFrameCommandSetter::OnFrameCommandSetterFn(const std::string& onFrameCommandIn)
{
    m_onFrameCommandFunc = [onFrameCommandIn](const Config&, std::string& onFrameCommand, std::string&) -> bool
    {
        onFrameCommand = onFrameCommandIn;
        return true;
    };
}

bool OnFrameCommandSetter::operator() (const Config& inputConfig, std::string& onFrameCommand, std::string& errorMessage)
{
    return m_onFrameCommandFunc(inputConfig, onFrameCommand, errorMessage);
}

} // namespace nnlab
