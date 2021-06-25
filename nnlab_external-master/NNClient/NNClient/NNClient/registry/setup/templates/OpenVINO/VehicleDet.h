#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

static const TaskSetuper VehicleDetTemplateOpenVINO = TaskSetuper()
.TargetMinObject(10).TargetMaxObject(20)
.Param("models", string_vector{"model"})
.Param("model", std::string("Truck"))
.Param("channels", 3)
.Param("net_width", 480)
.Param("net_height", 288)
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
.OnFrameCommandSetterFn("detectvehicles");

} // namespace nnlab
