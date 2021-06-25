#include <NNClient/registry/setup/DeployCommands.h>

#include <NNClient/core/Macro.h>

namespace nnlab
{

bool defaultDeploy
(
    CommandProcessorBack& commandProcessorBack,
    Config& inputConfig,
    std::string& errorMessage
)
{
    if (!commandProcessorBack.setChannels(inputConfig))
    {
        errorMessage = "Unable to set nchannels";
        return false;
    }

    if (!commandProcessorBack.setSize(inputConfig))
    {
        errorMessage = "Unable to set input size";
        return false;
    }

    string_vector models;
    if (!inputConfig.get("models", models))
    {
        errorMessage = "Failed to get models list";
        return false;
    }

    if (models.empty())
    {
        errorMessage = "Models list is empty";
        return false;
    }

    for (const auto& model: models)
    {
        if (!commandProcessorBack.loadModel(model, inputConfig))
        {
            errorMessage = "Failed to load " + model + " model";
            return false;
        }
    }

    if (!commandProcessorBack.createSM(inputConfig, errorMessage))
    {
        errorMessage = "Failed to fill image SM";
        return false;
    }

    if (!commandProcessorBack.createProcess(inputConfig))
    {
        errorMessage = "Can't create process";
        return false;
    }

    // in the following lines we replace ' ' with '^' to pass path without spaces in server command
    std::string executablePath;
    if (!inputConfig.get<std::string>(NN_CLIENT_EXEC_PATH_PARAM_NAME, executablePath))
    {
        errorMessage = "TaskSetuper: Can't obtain executable path";
        return false;
    }

    std::replace(executablePath.begin(), executablePath.end(), ' ', '^');
    inputConfig.Arg<std::string>(NN_CLIENT_EXEC_PATH_TO_SERVER_PARAM_NAME);

    if (!inputConfig.setParam<std::string>(NN_CLIENT_EXEC_PATH_TO_SERVER_PARAM_NAME, executablePath, errorMessage))
    {
        errorMessage = "Failed to set path to server param";
        return false;
    }

    return true;
}

bool setObjectSizesDeploy
(
    CommandProcessorBack&,
    Config& inputConfig,
    std::string& errorMessage
)
{
    int minWidth = 0;
    int maxWidth = 0;

    inputConfig.get<int>("min_object", minWidth);
    inputConfig.get<int>("max_object", maxWidth);

    const std::string minWidthParamName("min_width");
    const std::string maxWidthParamName("max_width");

    inputConfig.Arg<int>(minWidthParamName);
    inputConfig.Arg<int>(maxWidthParamName);

    if (!inputConfig.setParam<int>(minWidthParamName, minWidth, errorMessage))
    {
        return false;
    }
    if (!inputConfig.setParam<int>(maxWidthParamName, maxWidth, errorMessage))
    {
        return false;
    }

    int minHeight = 0;
    int maxHeight = 0;

    inputConfig.get<int>("min_person_height", minHeight);
    inputConfig.get<int>("max_person_height", maxHeight);

    const std::string minHeightParamName("min_height");
    const std::string maxHeightParamName("max_height");

    inputConfig.Arg<int>(minHeightParamName);
    inputConfig.Arg<int>(maxHeightParamName);

    if (!inputConfig.setParam<int>(minHeightParamName, minHeight, errorMessage))
    {
        return false;
    }
    if (!inputConfig.setParam<int>(maxHeightParamName, maxHeight, errorMessage))
    {
        return false;
    }

    return true;
}

}
