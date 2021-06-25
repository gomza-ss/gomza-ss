#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(BodyTrack_OpenVINO_HKPDet, 
                            getInstallPath() + "/Modules/BodyTrack/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Param("data_prefix", std::string(getInstallPath() + "/Modules/BodyTrack/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model"})
.Param("model", std::string("BodyTrack"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("openvino_check_algorithm_type", 0 /* casual algorithm */)
.Param("fw_timeout_msec", 60000)
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setupbodytrackopenvino")
.Command("resizeinput")
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
    {
        int nireq = 0;
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("gethumankeypointsasync");
        }
        else
        {
            onFrameCommand = std::string("gethumankeypoints");
        }

        return true;
    }
);

} // namespace nnlab
