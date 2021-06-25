#pragma once

#include <OutputRepresenters/OutputRepresenterRegistry.h>
#include <Crush.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace nnlab
{

REGISTER_OUTPUT_REPRESENTER(tracks_with_alarms_and_helmet_boxes)
.VisFunc(
[](const cv::Mat& frame, const std::string& outputJson, const std::string& demoJsonStr) -> void
{

    nnlab::json demoJson = nlohmann::json::parse(demoJsonStr);

    cv::Rect zone;

    auto zoneJson = demoJson["zone"];
    if (zoneJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field zone in output json");
    }
    try
    {
        zone = zoneJson.get<decltype(zone)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    nnlab::json json = nlohmann::json::parse(outputJson);

    std::vector<int> alarmIds;

    auto alarmIdsJson = json["ids"];
    if (alarmIdsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field ids in output json");
    }
    try
    {
        alarmIds = alarmIdsJson.get<decltype(alarmIds)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    std::vector<nnlab::Detection> personDetections;

    auto personDetectionsJson = json["detections"];
    if (personDetectionsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field detections in output json");
    }
    try
    {
        personDetections = personDetectionsJson.get<decltype(personDetections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    cv::Mat disp = frame.clone();
    cv::rectangle(frame, zone, cv::Scalar(0, 255, 0), -1);
    cv::Mat blended;
    cv::addWeighted(frame, 0.2, disp, 0.8, 0, blended);
    disp = blended;

    std::vector<int> alarmsToShow;
    for (size_t i = 0; i < personDetections.size(); ++i) {

        const cv::Scalar okColor(0, 255, 0);
        const cv::Scalar alarmColor(0, 0, 255);

        const bool alarm(std::find(alarmIds.begin(), alarmIds.end(), personDetections[i].label) != alarmIds.end());

        if (alarm)
        {
            alarmsToShow.push_back(personDetections[i].label);
        }
        
        cv::rectangle(disp, personDetections[i].bbox, (alarm ? alarmColor : okColor), 2);
        cv::putText(disp, std::to_string(personDetections[i].label), personDetections[i].bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    }

    std::vector<cv::Rect> helmetDetections;

    auto helmetDetectionsJson = json["helmet_bboxes"];
    if (helmetDetectionsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field helmet_bboxes in output json");
    }
    try
    {
        helmetDetections = helmetDetectionsJson.get<decltype(helmetDetections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }


    for (size_t i = 0; i < helmetDetections.size(); ++i) {
        cv::rectangle(disp, helmetDetections[i], cv::Scalar(0, 0, 255), 2);
    }

    std::string alarmString("Alarm:");

    for (size_t i = 0; i < alarmsToShow.size(); ++i)
    {
        alarmString += std::string(" ");
        alarmString += std::to_string(alarmsToShow[i]);
    }

    cv::putText(disp, alarmString, cv::Point(frame.cols / 100, frame.rows / 28), 
        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
    
    cv::imshow("Result", disp);
    cv::waitKey(1);

}
)
.LogFunc(
[](std::ofstream& logFile, const std::string& outputJson, const std::string&) -> void
{
    nnlab::json json = nlohmann::json::parse(outputJson);

    std::vector<int> alarmIds;

    auto alarmIdsJson = json["ids"];
    if (alarmIdsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field ids in output json");
    }
    try
    {
        alarmIds = alarmIdsJson.get<decltype(alarmIds)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    std::vector<nnlab::Detection> personDetections;

    auto personDetectionsJson = json["detections"];
    if (personDetectionsJson.type() == nlohmann::json::value_t::null)
    {
        crush("Cannot find field detections in output json");
    }
    try
    {
        personDetections = personDetectionsJson.get<decltype(personDetections)>();
    }
    catch (const nnlab::json::exception& e)
    {
        crush(std::string("Failed to parse output json: ") + e.what());
    }

    logFile << "Frame: " << getImageId(outputJson) << std::endl;

    for (const auto det : personDetections)
    {
        logFile << det.label << ' ' << det.bbox.x << ' ' << det.bbox.y << ' ' << det.bbox.width << ' ' << det.bbox.height << std::endl;
    }

    logFile << "Alarm:";
    for (auto id : alarmIds)
    {
        logFile << " " << id;
    }
    logFile << std::endl;

}
);

}
