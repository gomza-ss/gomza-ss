#pragma once

#include <NNClient/core/CommandProcessorBack.h>

namespace nnlab
{

class ServerProcessDeployer
{
public:

    using DeployFunc =
        std::function<bool(
            CommandProcessorBack&,
            Config&,
            std::string&)>;

    ServerProcessDeployer& DeployCommand(const DeployFunc& deployFunc);
    bool operator() (
        CommandProcessorBack& commandProcesorBack,
        Config& inputConfig,
        std::string& error_message);

    inline const auto & getDeployCommands() const
    {
        return m_deployFuncs;
    }

private:

    std::vector<DeployFunc> m_deployFuncs;

};

} // namespace nnlab
