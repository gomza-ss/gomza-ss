#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(KeypointsDetections_Caffe2_TK, getInstallPath() + "/Modules/KeypointsDetections/NN_servers/Caffe2Server/Caffe2Server.exe")
.TargetMinObject(40).TargetMaxObject(200)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/KeypointsDetections/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{"detector", "reid"})
.Param("detector", std::string("KeypointsDetections"))
.Param("reid", std::string("PedestrianEncoder"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("label", -1)
.Param("reid_width", 64)
.Param("reid_height", 128)
.Param("age", 28)
.Param("threshold", 0.5f)
.Param("max_time_since_match", 1)
.Param("use_fp_det", false)
.Param("min_person_height", 0)
.Param("max_person_height", 0)
.Param("predict_time", -1)
.Param("nireq", 0)
.Command("setnchannels")
.Command("setuptrackerdetectormrcnncaffe2")
.Command("setuptrackerfeatureextractormrcnncaffe2")
.Command("resizeinput")
.Command("assignsm")
.Command("settrackernireq")
.Command("setuptrackermrcnn")
.Command("setagemax")
.Command("setmaxtimesincematch")
.Command("isready")
.CommandConditioned("setupfpdetector",
    [](const Config& inputConfig) -> bool
    {
        bool use_fp_det{ false };
        inputConfig.get<bool>("use_fp_det", use_fp_det);

        if (!use_fp_det)
        {
            return false;
        }

        std::vector<cv::Rect> zones;
        inputConfig.get<decltype(zones)>("zones", zones);

        if (zones.empty())
        {
            return false;
        }

        return true;
    })
.CommandConditioned("setpredicttime",
    [](const Config& inputConfig) -> bool
    {
        int predict_time{ -1 };
        inputConfig.get<int>("predict_time", predict_time);
        return predict_time > 0;
    })
.DeployCommand(&setObjectSizesDeploy)
.DeployCommand(&defaultDeploy)
.OnFrameCommandSetterFn("gettracksandkeypoints");

} // namespace nnlab
