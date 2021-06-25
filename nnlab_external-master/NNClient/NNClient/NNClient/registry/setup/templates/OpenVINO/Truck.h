#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

static const TaskSetuper TruckTemplateOpenVINO = TaskSetuper()
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/TSSM/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("Truck"))
.Param("channels", 3)
.Param("net_width", 960)
.Param("net_height", 544)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("target_label", 3)
.Param("iou_threshold", 0.001f)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("resizeinput")
.Command("assignsm")
.Command("setuptruckchecker")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
    {
        int nireq = 0;
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("detecttrucksasync");
        }
        else
        {
            onFrameCommand = std::string("detecttrucks");
        }

        return true;
    }
);

} // namespace nnlab
