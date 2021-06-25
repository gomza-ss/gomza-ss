#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Auto_OpenVINO_LPDet,
                            getInstallPath() + "/Modules/Auto/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/Auto/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("LPRold"))
.Param("channels", 1)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("fw_timeout_msec", 60000)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
    {
        int nireq = 0;
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("getbboxesasync");
        }
        else
        {
            onFrameCommand = std::string("getbboxes");
        }

        return true;
    }
);

} // namespace nnlab
