#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Auto_OpenVINO_UVSSSegmentation,
                            getInstallPath() + "/Modules/Auto/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Param("data_prefix", std::string(getInstallPath() + "/Modules/Auto/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("UVSSSegmentation"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("fw_timeout_msec", 60000)
.Param("threshold", 0.5f)
.Param("size_divisibility", 32)
.Param("input_image_width", 640)
.Param("input_image_height", 384)
.Param("min_size", 384)
.Param("resize_mode", std::string("KEEP_AR"))
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("setsizedivisibility")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn("getsemseg");

} // namespace nnlab
