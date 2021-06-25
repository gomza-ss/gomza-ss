#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(
    Belt_Caffe2_BeltDet,
    getInstallPath() + "/Modules/Belt/NN_servers/Caffe2Server/Caffe2Server.exe")
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/Belt/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{ "model" })
.Param("model", std::string("Belt"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("async", false)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setnchannels")
.Command("setdetectorbytextbufferscaffe2")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn("detectbelts");

} // namespace nnlab