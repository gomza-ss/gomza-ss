#pragma once

#include <NNClient/registry/setup/TaskSetuper.h>
#include <NNClient/registry/setup/DeployCommands.h>

namespace nnlab
{

static const TaskSetuper MMRTemplateOpenVINO = TaskSetuper()
.TargetMinObject(10).TargetMaxObject(20)
.Param("models", string_vector{"model", "color_classifier", "mm_classifier"})
.Param("model", std::string("Truck"))
.Param("color_classifier", std::string("MMRcolor"))
.Param("mm_classifier", std::string("MMRmm"))
.Param("channels", 3)
.Param("net_width", 480)
.Param("net_height", 288)
.Param("net_channels", 3)
.Param("encrypted", true)
.Param("device", std::string("CPU"))
.Param("async", false)
.Param("fw_timeout_msec", 60000)
.Param("color_classifier_width", 224)
.Param("color_classifier_height", 224)
.Param("color_classifier_nthreads", 0)
.Param("color_classifier_nireq", 0)
.Param("color_classifier_device", std::string("CPU"))
.Param("color_classifier_resize_mode", std::string("FIXED_SIZE"))
.Param("mm_classifier_width", 224)
.Param("mm_classifier_height", 224)
.Param("mm_classifier_nthreads", 0)
.Param("mm_classifier_nireq", 0)
.Param("mm_classifier_device", std::string("CPU"))
.Param("mm_classifier_resize_mode", std::string("FIXED_SIZE"))
.Param("topk", 3)
.Param("resize_mode", std::string("FIXED_SIZE"))
.DeployCommand(&defaultDeploy)
.Command("setfwtimeout")
.Command("setnchannels")
.Command("setdetectorbytextbuffersopenvino")
.Command("resizeinput")
.Command("setupmmrcolorclassifieropenvino")
.Command("setupmmrmmclassifieropenvino")
.Command("assignsm")
.Command("setupmmr")
.Command("isready")
.OnFrameCommandSetterFn(
    [](const Config& config, std::string& onFrameCommand, std::string& /* errorMessage */) -> bool
    {
        int nireq = 0;
        if (config.get<int>("nireq", nireq))
        {
            onFrameCommand = std::string("detectmmrasync");
        }
        else
        {
            onFrameCommand = std::string("detectmmr");
        }

        return true;
    }
);

} // namespace nnlab
