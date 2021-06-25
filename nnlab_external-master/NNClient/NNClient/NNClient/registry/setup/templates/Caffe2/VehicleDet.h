#pragma once

#include <NNClient/registry/setup/TaskSetuper.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

static const TaskSetuper VehicleDetTemplateCaffe2 = TaskSetuper()
.TargetMinObject(10).TargetMaxObject(20)
.Param("models", string_vector{"model"})
.Param("model", std::string("Truck"))
.Param("channels", 3)
.Param("net_width", 480)
.Param("net_height", 288)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("fw_timeout_msec", 60000)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
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
.Command("isready")
.OnFrameCommandSetterFn("detectvehicles");

} // namespace nnlab
