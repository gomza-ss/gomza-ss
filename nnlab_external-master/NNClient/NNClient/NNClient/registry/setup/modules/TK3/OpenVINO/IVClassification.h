#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(TK3_OpenVINO_IVClassification,
                            getInstallPath() + "/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Param("data_prefix", getInstallPath() + "/NN_servers/data/OpenVINO")
.Param("models", string_vector{"model"})
.Param("model", std::string("IVClassifier"))
.Param("text_labels", string_vector{})
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("fw_timeout_msec", 60000)
.Param("resize_mode", std::string("FIXED_SIZE"))
.Param("openvino_check_algorithm_type", 0 /* casual algorithm */)
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("setlabels")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
    {
        int nireq = 0;
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("getclassificationasync");
        }
        else
        {
            onFrameCommand = std::string("getclassification");
        }

        return true;
    }
);

} // namespace nnlab
