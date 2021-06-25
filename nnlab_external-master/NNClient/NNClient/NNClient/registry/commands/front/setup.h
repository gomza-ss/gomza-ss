#pragma once

#include <NNClient/core/Registry.h>
#include <NNClient/registry/setup/SetupHelper.h>

namespace nnlab
{

REGISTER_COMMAND_FRONT(setup)
    .InputArg<std::string>("module")
    .InputArg<std::string>("server")
    .InputArg<std::string>("task")
    .InputArg<int>("input_image_width")
    .InputArg<int>("input_image_height")
    .InputArg<int>("min_object")
    .InputArg<int>("max_object")
    .InputArg<cv::Rect>("zone", []() -> cv::Rect { return cv::Rect(); })
    .InputArg<bboxesType>("zones", []() -> bboxesType { return bboxesType(); })
    .SetPreFn
    (
        [](CommandProcessorBack& commandProcessorBack, Config& inputConfig, std::vector<std::string>& commandSequence, std::string& onFrameCommand, std::string& errorMessage)
        {
            std::string moduleName;
            if (!inputConfig.get<std::string>("module", moduleName)) 
            {
                errorMessage = std::string("Unsupported server name: ") + moduleName;
                return false;
            }           

            std::string deviceVendor;
            if (!inputConfig.get<std::string>("deviceVendor", deviceVendor))
            {
                errorMessage = std::string("Cannot obtain deviceVendor parameter");
                return false;
            }

            std::transform(deviceVendor.begin(), deviceVendor.end(), deviceVendor.begin(), [](const char c) -> char { return std::toupper(c);  });

            const NNClientMap<std::string, std::string> vendorToServer =
            {
                { NN_CLIENT_NVIDIA, std::string("Caffe2") },
                { NN_CLIENT_INTEL, std::string("OpenVINO") }
            };

            if (!vendorToServer.count(deviceVendor))
            {
                errorMessage = std::string("Unsupported device vendor: ") + deviceVendor;
                return false;
            }

            std::string taskName;
            if (!inputConfig.get<std::string>("task", taskName))
            {
                errorMessage = std::string("Cannot obtain task parameter");
                return false;
            }

            int nireq = 0;
            if (inputConfig.get<int>("nireq", nireq))
            {
                // if nireq is provided we expect parallel usage
                if (nireq <= 0)
                {
                    errorMessage = std::string("nireq must be > 0");
                    return false;
                }
            }

            // users tend to pass thresholds as ints when they are 0.0 or 1.0
            // handle such cases in the following workaround
            std::vector<std::string> paramNamesInConfig;
            inputConfig.getParamNames(paramNamesInConfig);

            for (const std::string& paramName : paramNamesInConfig)
            {
                if (paramName.find("threshold") == std::string::npos)
                {
                    continue;
                }

                int thresholdInt = 0;
                if (inputConfig.get<int>(paramName, thresholdInt))
                {
                    inputConfig.Arg<float>(paramName);
                    if (!inputConfig.setParam<float>(paramName, static_cast<float>(thresholdInt), errorMessage))
                    {
                        return false;
                    }
                }
            }

            const std::string taskStringName(moduleName + "_" + vendorToServer.at(deviceVendor) + "_" + taskName);
                        
            if (!TaskSetuperRegistry::pTaskSetuper(taskStringName))
            {
                errorMessage = std::string("Task ") + taskStringName + std::string(" was not registered");
                return false;
            }

            TaskSetuper taskSetuper = *(TaskSetuperRegistry::pTaskSetuper(taskStringName));

            if (!taskSetuper.setupInitializer(inputConfig, errorMessage))
            {
                return false;
            }

            int inputImageWidth = 0, inputImageHeight = 0;
            if (!inputConfig.get<int>("input_image_width", inputImageWidth) ||
                !inputConfig.get<int>("input_image_height", inputImageHeight))
            {
                errorMessage = std::string("Both input_image_width and input_image_height params have to be specified");
                return false;
            }

            taskSetuper.InputImageSize(cv::Size(inputImageWidth, inputImageHeight));

            int minObject = 0, maxObject = 0;
            if (!(inputConfig.get<int>("min_object", minObject) && inputConfig.get<int>("max_object", maxObject)))
            {
                if (inputConfig.get<int>("min_object", minObject)) 
                {
                    maxObject = minObject;
                } else if (inputConfig.get<int>("max_object", maxObject))
                {
                    minObject = maxObject;
                }
            }

            taskSetuper.MinObject(minObject);
            taskSetuper.MaxObject(maxObject);

            if (!taskSetuper.processDeployer(commandProcessorBack, inputConfig, errorMessage)) 
            {
                return false;
            }

            if (!taskSetuper.calculateNetInput(inputConfig, errorMessage))
            {
                return false;
            }

            commandSequence.clear();
            for (const auto &pair : taskSetuper.sequence())
            {
                if (pair.first(inputConfig))
                {
                    commandSequence.push_back(pair.second);
                }
            }

            if (!taskSetuper.onFrameCommandSetter(inputConfig, onFrameCommand, errorMessage))
            {
                return false;
            }

            return true;
        }
    );

} // namespace nnlab
