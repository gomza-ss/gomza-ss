#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Transit_OpenVINO_CouplerDet,
                            getInstallPath() + "/Modules/Transit/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/Transit/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("Coupler"))
.Param("channels", 1)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn("getbboxesjitter");

} // namespace nnlab
