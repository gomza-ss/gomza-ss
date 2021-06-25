#pragma once

#include <NNClient/core/Config.h>

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(classification)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto classificationJson = json["classification"];
    NamedClassification classification;
    if (classificationJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field classification in output json");
    }
    try
    {
        classification = classificationJson.get<NamedClassification>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    cv::Mat disp = frame.clone();

    std::string dispText = classification.name + " " + std::to_string(classification.confidence);
    cv::putText(disp, dispText, cv::Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

    cv::imshow("Result", disp);
    cv::waitKey(1000);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);
    auto classificationJson = json["classification"];
    NamedClassification classification;
    if (classificationJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field classification in output json");
    }
    try
    {
        classification = classificationJson.get<NamedClassification>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    logFile << "Frame: " << getImageId(outputJson) << std::endl;
    logFile << classification.name << ' ' << classification.confidence << std::endl;
}
);

} // namespace nnlab
