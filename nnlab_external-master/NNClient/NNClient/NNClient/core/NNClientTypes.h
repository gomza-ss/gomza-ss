#pragma once

#include <unordered_map>
#include <opencv2/core.hpp>

namespace nnlab
{

template<typename Key, typename Value>
using NNClientMap = std::unordered_map<Key, Value>;

struct Detection
{
    int label;
    cv::Rect bbox;
    float confidence = -1.0f;
};

struct ClassifiedDetection
{
    Detection det;
    int label;
    float confidence;
    std::string labelStr;
};

struct ClassifiedDetectionBrightness
{
    ClassifiedDetection detection;
    int brightnessLabel;
    float brightnessConfidence;
};

struct Track
{
    int label;
    cv::Rect bbox;
    std::vector<int> keypoints;
};

struct CudaDeviceInfo
{
    std::string name;
    std::string GUID;
    size_t availableMemory;
    size_t totalMemory;
};

struct MMRColorEntry
{
    size_t id;
    std::string name;
    float probability;
};

struct MMRModelEntry
{
    size_t id;
    std::string name;
    std::string type;
    float probability;
};

struct MMRInfo
{
    std::vector<MMRColorEntry> colors;
    std::vector<MMRModelEntry> models;
};

struct NamedClassification
{
    std::string name;
    float confidence;
};


using bboxesType = std::vector<cv::Rect>;

}
