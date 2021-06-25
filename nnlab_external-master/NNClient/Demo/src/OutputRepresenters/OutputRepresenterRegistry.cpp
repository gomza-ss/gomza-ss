#include <OutputRepresenters/OutputRepresenterRegistry.h>

#include <NNClient/core/Config.h>

#define SAFE_CALL(f, ...) if (f) f(__VA_ARGS__);

namespace
{
    std::ofstream logFile("log.txt");
}

namespace nnlab
{

OutputRepresenter& OutputRepresenter::VisFunc(const TVisFunc& visFunc)
{
    m_visFunc = visFunc;
    return *this;
}

OutputRepresenter& OutputRepresenter::LogFunc(const TLogFunc& logFunc)
{
    m_logFunc = logFunc;
    return *this;
}

void OutputRepresenter::apply(const cv::Mat& frame, const std::string& json, const std::string& demoJson) const
{
    SAFE_CALL(m_visFunc, frame, json, demoJson);
    SAFE_CALL(m_logFunc, logFile, json, demoJson);
}

NNClientMap<std::string, OutputRepresenter>& OutputRepresenterRegistry::map()
{
    static NNClientMap<std::string, OutputRepresenter> mapVal;
    return mapVal;
}

int getImageId(const std::string& outputJson)
{
    nnlab::json json = nnlab::json::parse(outputJson);
    return json["image_id"];
}

}

#include <OutputRepresenters/Chars.h>
#include <OutputRepresenters/Classification.h>
#include <OutputRepresenters/ClassifiedDetections.h>
#include <OutputRepresenters/Detections.h>
#include <OutputRepresenters/Keypoints.h>
#include <OutputRepresenters/MMROutput.h>
#include <OutputRepresenters/SemSeg.h>
#include <OutputRepresenters/Tracks.h>
#include <OutputRepresenters/TracksWithAlarmsAndHelmetBoxes.h>
#include <OutputRepresenters/TruckDetection.h>
#include <OutputRepresenters/TracksKeypoints.h>
