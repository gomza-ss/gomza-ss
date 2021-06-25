#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(TK3_OpenVINO_Crosswalk, 
                            getInstallPath() + "/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("detector"))
.TargetMinObject(40).TargetMaxObject(200)
.Param("data_prefix", std::string(getInstallPath() + "/NN_servers/data/OpenVINO"))
.Param("models", string_vector{"detector", "reid"})
.Param("detector", std::string("PedestrianDetectorLight"))
.Param("reid", std::string("PedestrianEncoder"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("reid_device", std::string("CPU"))
.Param("nireq", 0)
.Param("reid_nireq", 0)
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
.Param("nthreads", 0)
.Param("reid_nthreads", 0)
.Param("use_faster_rcnn", false)
.Param("reid_batch_size", 1)
.Param("fw_timeout_msec", 25000)
.Param("resize_mode", std::string("KEEP_AR"))
.Param("reid_resize_mode", std::string("FIXED_SIZE"))
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setuptrackerdetectoropenvino")
.Command("setuptrackerfeatureextractoropenvino")
.Command("resizeinput")
.Command("assignsm")
.Command("settrackernireq")
.Command("setuptracker")
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
.OnFrameCommandSetterFn("gettracks");

} // namespace nnlab
