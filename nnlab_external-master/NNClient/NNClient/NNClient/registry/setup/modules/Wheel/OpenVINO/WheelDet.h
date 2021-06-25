#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Wheel_OpenVINO_WheelDet,
                            getInstallPath() + "/Modules/Wheel/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/Wheel/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("Wheel"))
.Param("channels", 1)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
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
