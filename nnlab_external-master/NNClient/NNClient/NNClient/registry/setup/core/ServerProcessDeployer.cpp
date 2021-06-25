#include <NNClient/registry/setup/core/ServerProcessDeployer.h>

#include <NNClient/core/Config.h>
#include <NNClient/core/CommandProcessorBack.h>

#include <functional>
#include <vector>

namespace nnlab
{

ServerProcessDeployer& ServerProcessDeployer::DeployCommand(const DeployFunc& deployFunc)
{
    m_deployFuncs.push_back(deployFunc);
    return *this;
}

bool ServerProcessDeployer::operator() (
    CommandProcessorBack& commandProcessorBack,
    Config& inputConfig,
    std::string& error_message)
{
    for (const auto& deployFunc : m_deployFuncs)
    {
        if (!deployFunc(commandProcessorBack, inputConfig, error_message))
        {
            return false;
        }
    }

    return true;
}

} // namespace nnlab
