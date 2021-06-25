#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(TK3_Caffe2_IVClassification, getInstallPath() + "/NN_servers/Caffe2Server/Caffe2Server.exe")
.Param("data_prefix", getInstallPath() + "/NN_servers/data/Caffe2")
.Param("models", string_vector{"model"})
.Param("model", std::string("IVClassifier"))
.Param("text_labels", string_vector{})
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("fw_timeout_msec", 60000)
.Param("resize_mode", std::string("FIXED_SIZE"))
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setclassifiercaffe2")
.Command("setlabels")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn("getclassification");

} // namespace nnlab
