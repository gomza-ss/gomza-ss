#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Auto_OpenVINO_LPCharDet,
                            getInstallPath() + "/Modules/Auto/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.TargetMinObject(10).TargetMaxObject(20)
.Param("data_prefix", getInstallPath() + std::string("/Modules/Auto/NN_servers/OpenVINOServer/data"))
.Param("models", string_vector{"model", "char_detector", "char_classifier"})
.Param("model", std::string("LPRnew"))
.Param("char_detector", std::string("CharDetectorUS"))
.Param("char_classifier", std::string("CharClassifierBrightness"))
.Param("channels", 3)
.Param("net_channels", 1)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("char_detector_device", std::string("CPU"))
.Param("char_classifier_device", std::string("CPU"))
.Param("async", false)
.Param("fw_timeout_msec", 60000)
.Param("char_detector_width", 160)
.Param("char_detector_height", 90)
.Param("char_detector_nthreads", 0)
.Param("char_detector_nireq", 0)
.Param("char_detector_detector", std::string("ENABLED"))
.Param("char_classifier_width", 32)
.Param("char_classifier_height", 32)
.Param("char_classifier_nthreads", 0)
.Param("char_classifier_nireq", 0)
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
.Command("setdetectorbytextbuffersopenvino")
.Command("resizeinput")
.Command("setupchardetectoropenvino")
.CommandConditioned("setupcharclassifieropenvino",
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
