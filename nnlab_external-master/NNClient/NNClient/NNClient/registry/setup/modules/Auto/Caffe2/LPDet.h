#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(Auto_Caffe2_LPDet, getInstallPath() + "/Modules/Auto/NN_servers/Caffe2Server/Caffe2Server.exe")
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/Auto/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("LPRold"))
.Param("channels", 1)
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
