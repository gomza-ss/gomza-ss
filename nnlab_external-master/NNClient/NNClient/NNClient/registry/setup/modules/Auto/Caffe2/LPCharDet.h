#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(Auto_Caffe2_LPCharDet, getInstallPath() + "/Modules/Auto/NN_servers/Caffe2Server/Caffe2Server.exe")
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/Auto/NN_servers/Caffe2Server/data"))
.Param("models", string_vector{"model", "char_detector", "char_classifier"})
.Param("model", std::string("LPRnew"))
.Param("char_detector", std::string("CharDetectorUS"))
.Param("char_classifier", std::string("CharClassifierBrightness"))
.Param("channels", 3)
.Param("net_channels", 1)
.Param("encrypted", true)
.Param("device", std::string("0"))
.Param("fw_timeout_msec", 60000)
.Param("char_detector_width", 160)
.Param("char_detector_height", 90)
.Param("char_classifier_width", 32)
.Param("char_classifier_height", 32)
.Param("lp_label", -1)
.Param("lp_threshold", 0.5f)
.Param("char_label", -1)
.Param("char_threshold", 0.5f)
.Param("char_detector_resize_mode", std::string("KEEP_AR"))
.Param("char_classifier_resize_mode", std::string("FIXED_SIZE"))
.Param("resize_mode", std::string("KEEP_AR"))
.Param("enable_char_classifier", true)
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
.Command("setupchardetectorcaffe2")
.CommandConditioned("setupcharclassifiercaffe2",
    [](const Config& inputConfig) -> bool
    {
        bool enable{ false };
        inputConfig.get<bool>("enable_char_classifier", enable);

        return enable;
    })
.Command("assignsm")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
{
    int nireq = 0;
    if (config.get<int>("nireq", nireq))
    {
        onFrameCommand = std::string("getcharsasync");
    }
    else
    {
        onFrameCommand = std::string("getchars");
    }

    return true;
}
);

} // namespace nnlab
