#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(BodyTrack_Caffe2_HKPDet, getInstallPath() + "/Modules/BodyTrack/NN_servers/Caffe2Server/Caffe2Server.exe")
.Param("data_prefix", std::string(getInstallPath() + "/Modules/BodyTrack/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("BodyTrack"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("fw_timeout_msec", 60000)
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.CommandConditioned("setupbodytrackcaffe2",
    [](const Config& inputConfig) -> bool
    {
        int nireq{ 0 };
        inputConfig.get<int>("nireq", nireq);

        return nireq <= 0;
    })
.CommandConditioned("setupbodytrackcaffe2async",
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
        int nireq{ 0 };
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("gethumankeypointsasync");
        }
        else
        {
            onFrameCommand = std::string("gethumankeypoints");
        }

        return true;
    });

} // namespace nnlab
