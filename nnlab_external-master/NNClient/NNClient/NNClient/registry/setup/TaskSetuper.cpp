#include <NNClient/registry/setup/TaskSetuper.h>

namespace nnlab
{

TaskSetuper& TaskSetuper::Param(const std::string& name, const boost::any& value)
{
    m_setupInitializer.Param(name, value);
    return *this;
}

TaskSetuper& TaskSetuper::ParamConditioned(const std::string& name, 
                                           const SetupConfigInitializer::SetupConfigInitializerFunc& f)
{
    m_setupInitializer.ParamConditioned(name, f);
    return *this;
}

TaskSetuper& TaskSetuper::TargetMinObject(int targetMinObject)
{
    m_setupInitializer.TargetMinObject(targetMinObject);
    return *this;
}

TaskSetuper& TaskSetuper::TargetMaxObject(int targetMaxObject)
{
    m_setupInitializer.TargetMaxObject(targetMaxObject);
    return *this;
}

TaskSetuper& TaskSetuper::MinObject(int minObject)
{
    m_setupInitializer.MinObject(minObject);
    return *this;
}

TaskSetuper& TaskSetuper::MaxObject(int maxObject)
{
    m_setupInitializer.MaxObject(maxObject);
    return *this;
}

TaskSetuper& TaskSetuper::InputImageSize(const cv::Size& inputImageSize)
{
    m_setupInitializer.InputImageSize(inputImageSize);
    return *this;
}

TaskSetuper& TaskSetuper::OnFrameCommandSetterFn(const OnFrameCommandSetter::OnFrameCommandFunc& f)
{
    m_onFrameCommandSetter.OnFrameCommandSetterFn(f);
    return *this;
}

TaskSetuper& TaskSetuper::OnFrameCommandSetterFn(const std::string& onFrameCommandName)
{
    m_onFrameCommandSetter.OnFrameCommandSetterFn(onFrameCommandName);
    return *this;
}

TaskSetuper& TaskSetuper::DeployCommand(const ServerProcessDeployer::DeployFunc& deployFunc)
{
    m_processDeployer.DeployCommand(deployFunc);
    return *this;
}

TaskSetuper& TaskSetuper::Command(const std::string& command)
{
    m_commandSequence.Command(command);
    return *this;
}

TaskSetuper& TaskSetuper::CommandConditioned(const std::string& command, const CommandSequence::CommandSequenceFunc& f)
{
    m_commandSequence.CommandConditioned(f, command);
    return *this;
}

TaskSetuper& TaskSetuper::Template(const TaskSetuper& that)
{
    m_setupInitializer.TargetMaxObject(that.m_setupInitializer.getTargetMaxObject());
    m_setupInitializer.TargetMinObject(that.m_setupInitializer.getTargetMinObject());
    m_setupInitializer.MaxObject(that.m_setupInitializer.getMaxObject());
    m_setupInitializer.MinObject(that.m_setupInitializer.getMinObject());
    for (const auto& pair : that.m_setupInitializer.getParams())
    {
        Param(pair.first, pair.second);
    }
    OnFrameCommandSetterFn(that.m_onFrameCommandSetter.getOnFrameCommandSetterFn());
    for (const auto& cmd : that.m_processDeployer.getDeployCommands())
    {
        DeployCommand(cmd);
    }
    for (const auto& pair : that.m_commandSequence.sequence())
    {
        CommandConditioned(pair.second, pair.first);
    }

    return *this;
}


bool TaskSetuper::setupInitializer(
    Config& inputConfig,
    std::string& errorMessage)
{
    return m_setupInitializer(inputConfig, errorMessage);
}

bool TaskSetuper::calculateNetInput(
    Config& inputConfig,
    std::string& errorMessage)
{
    return m_setupInitializer.calculateNetInput(inputConfig, errorMessage);
}

bool TaskSetuper::processDeployer(
    CommandProcessorBack& commandProcessorBack,
    Config& inputConfig,
    std::string& error_message)
{
    return m_processDeployer(commandProcessorBack, inputConfig, error_message);
}

bool TaskSetuper::onFrameCommandSetter(
    const Config &inputConfig,
    std::string& onFrameCommand,
    std::string& errorMessage
)
{
    return m_onFrameCommandSetter(inputConfig, onFrameCommand, errorMessage);
}

} // namespace nnlab
