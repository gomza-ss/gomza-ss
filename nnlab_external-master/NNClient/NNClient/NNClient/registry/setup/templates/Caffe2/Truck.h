#pragma once

#include <NNClient/registry/setup/TaskSetuper.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

static const TaskSetuper TruckTemplateCaffe2 = TaskSetuper()
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/TSSM/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("Truck"))
.Param("channels", 3)
.Param("net_width", 960)
.Param("net_height", 544)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("target_label", 2)
.Param("iou_threshold", 0.001f)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setnchannels")
.CommandConditioned("setdetectorbytextbufferscaffe2",
    [](const Config& inputConfig) -> bool
    {
        int nireq{ 0 };
        inputConfig.get<int>("nireq", nireq);

        return nireq <= 0;
    })
.CommandConditioned("setdetectorbytextbufferscaffe2async",
    [](const Config& inputConfig) -> bool
    {
        int nireq{ 0 };
        inputConfig.get<int>("nireq", nireq);

        return nireq > 0;
    })
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
