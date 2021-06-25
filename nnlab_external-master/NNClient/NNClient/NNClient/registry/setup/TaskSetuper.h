#pragma once

#include <NNClient/registry/setup/core/SecurosSearchUtils.h>
#include <NNClient/registry/setup/core/SetupConfigInitializer.h>
#include <NNClient/registry/setup/core/ServerProcessDeployer.h>
#include <NNClient/registry/setup/core/CommandSequence.h>
#include <NNClient/registry/setup/core/OnFrameCommandSetter.h>

#include <map>

namespace nnlab
{

struct TaskSetuper
{

    TaskSetuper& Param(const std::string& name, const boost::any& value);
    TaskSetuper& ParamConditioned(const std::string& name,
                                  const SetupConfigInitializer::SetupConfigInitializerFunc& f);

    TaskSetuper& TargetMinObject(int targetMinObject);
    TaskSetuper& TargetMaxObject(int targetMaxObject);
    TaskSetuper& MinObject(int minObject);
    TaskSetuper& MaxObject(int maxObject);
    TaskSetuper& InputImageSize(const cv::Size& inputImageSize);

    TaskSetuper& OnFrameCommandSetterFn(const OnFrameCommandSetter::OnFrameCommandFunc& f);
    TaskSetuper& OnFrameCommandSetterFn(const std::string& onFrameCommandName);

    TaskSetuper& DeployCommand(const ServerProcessDeployer::DeployFunc& deployFunc);

    TaskSetuper& Command(const std::string& command);
    TaskSetuper& CommandConditioned(const std::string& command, const CommandSequence::CommandSequenceFunc& f);

    TaskSetuper& Template(const TaskSetuper& that);

    bool setupInitializer(
        Config& inputConfig,
        std::string& errorMessage);

    bool calculateNetInput(
        Config& inputConfig,
        std::string& errorMessage);

    bool processDeployer(
        CommandProcessorBack& commandProcessorBack,
        Config& inputConfig,
        std::string& error_message);

    bool onFrameCommandSetter(
        const Config &inputConfig,
        std::string& onFrameCommand,
        std::string& errorMessage
    );

    const auto& sequence() const 
    {
        return m_commandSequence.sequence();
    }

    template<typename T>
    bool getTaskParam(const std::string& key, T& val, std::string& errorMessage) const
    {
        return m_setupInitializer.getTaskParam<T>(key, val, errorMessage);
    }

private:

    SetupConfigInitializer m_setupInitializer;
    ServerProcessDeployer m_processDeployer;
    CommandSequence m_commandSequence;
    OnFrameCommandSetter m_onFrameCommandSetter;
};

} // namespace nnlab
