#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(TK3_Caffe2_Helmet, getInstallPath() + "/NN_servers/Caffe2Server/Caffe2Server.exe")
.TargetMinObject(40).TargetMaxObject(200)
.Param("data_prefix", std::string(getInstallPath() + "/NN_servers/data/Caffe2"))
.Param("models", string_vector{"detector", "reid", "helmet"})
.Param("detector", std::string("PedestrianDetector"))
.Param("reid", std::string("PedestrianEncoder"))
.Param("helmet", std::string("SingleHelmet"))
.Param("channels", 3)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("label", -1)
.Param("reid_width", 64)
.Param("reid_height", 128)
.Param("age", 28)
.Param("threshold", 0.5f)
.Param("max_time_since_match", 1)
.Param("use_fp_det", true)
.Param("min_person_height", 0)
.Param("max_person_height", 0)
.Param("num_managers", 2)
.Param("threshold", 0.5f)
.Param("helmet_threshold", 0.5f)
.Param("absence_ratio_threshold", 0.5f)
.Param("window_size", 3)
.Param("helmet_net_width", 128)
.Param("helmet_net_height", 256)
.Param("absence_ratio_threshold", 0.5f)
.Param("nireq", 0)
.Param("resize_mode", std::string("FIXED_SIZE"))
.Param("reid_resize_mode", std::string("FIXED_SIZE"))
.Param("helmet_resize_mode", std::string("FIXED_SIZE"))
.ParamConditioned("zones",
    [](Config& inputConfig) -> boost::any 
{
    // this function is a workaround to use "zone" instead of "zones"
    // in helmet detector

    if (inputConfig.hasArgWithName("zones"))
    {
        inputConfig.removeParam("zones");
    }

    std::vector<cv::Rect> zones;
    cv::Rect zone;
    if (inputConfig.get<cv::Rect>("zone", zone))
    {
        zones.push_back(zone);
    }

    return zones;
})
.CommandConditioned("setupfpdetector",
    [](const Config& inputConfig) -> bool
{
    bool use_fp_det{ true };
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
.DeployCommand([](
                CommandProcessorBack&,
                Config& inputConfig,
                std::string& errorMessage) -> bool
{
    // pass window_size in msec to match timestamps
    const std::string windowSizeParamName("window_size");
    int window_size = 3;
    inputConfig.get<int>(windowSizeParamName, window_size);
    if (!inputConfig.setParam<int>(windowSizeParamName, window_size * 1000, errorMessage))
    {
        return false;
    }

    return true;
})
.DeployCommand(&setObjectSizesDeploy)
.DeployCommand(&defaultDeploy)
.Command("setnchannels")
.Command("setuptrackerdetectorcaffe2")
.Command("setuptrackerfeatureextractorcaffe2")
.Command("resizeinput")
.Command("assignsm")
.Command("settrackernireq")
.Command("setuphelmetabsencedetector")
.Command("setupsinglepersonhelmetdetectorcaffe2")
.Command("setuptracker")
.Command("setagemax")
.Command("setmaxtimesincematch")
.Command("isready")
.OnFrameCommandSetterFn("gethelmettracks");

} // namespace nnlab
