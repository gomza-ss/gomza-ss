#pragma once

#include <NNClient/thirdparty/nlohmann/json.h>
#include <NNClient/core/NNClientTypes.h>

namespace nnlab
{
    using json = ::nlohmann::json;
}

namespace nlohmann
{

    template<>
    struct adl_serializer<cv::Rect>
    {
        static inline void to_json(json& j, const cv::Rect& rect)
        {
            j =
                nlohmann::json
                {
                    {"x", rect.x},
                    {"y", rect.y},
                    {"w", rect.width},
                    {"h", rect.height}
                };
        }

        static inline void from_json(const json& j, cv::Rect& rect)
        {
            j.at("x").get_to(rect.x);
            j.at("y").get_to(rect.y);
            j.at("w").get_to(rect.width);
            j.at("h").get_to(rect.height);
        }
    };

} // namespace nlohmann

namespace nnlab
{

template<typename IT, typename OT>
bool jsonSafeCast(
    const IT& value,
    OT& out,
    std::string& error_message
)
{
    try {
        out = static_cast<OT>(value);
    }
    catch (nlohmann::detail::exception& e) {
        error_message = e.what();
        return false;
    }
    return true;
}

bool jsonSafeParse(
    const std::string& desc,
    nlohmann::json& out,
    std::string& error_message
);

inline void to_json(nlohmann::json& j, const CudaDeviceInfo& info)
{
    j =
        nlohmann::json
        {
            {"name", info.name},
            {"GUID", info.GUID},
            {"availableMemory", info.availableMemory},
            {"totalMemory", info.totalMemory}
        };
}

inline void to_json(nlohmann::json& j, const Detection& det)
{
    j = nlohmann::json{ {"label", det.label} };
    nlohmann::json bboxJson(det.bbox);
    j["bbox"] = bboxJson;
    if (det.confidence >= 0.0f)
    {
        j["confidence"] = det.confidence;
    }
}

inline void from_json(const nlohmann::json& j, Detection& det)
{
    j.at("label").get_to(det.label);
    j.at("bbox").get_to(det.bbox);
    if (j.count("confidence"))
    {
        j.at("confidence").get_to(det.confidence);
    }
    else
    {
        const float ignoreConfValue = -1.0f;
        det.confidence = ignoreConfValue;
    }
}

inline void to_json(nlohmann::json& j, const ClassifiedDetection& det)
{
    j =
        nlohmann::json
        {
            {"detection", det.det},
            {"label", det.label},
            {"confidence", det.confidence},
            { "label_str", det.labelStr}
        };
}

inline void from_json(const nlohmann::json& j, ClassifiedDetection& det)
{
    j.at("detection").get_to(det.det);
    j.at("label").get_to(det.label);
    j.at("confidence").get_to(det.confidence);
    j.at("label_str").get_to(det.labelStr);
}

inline void to_json(nlohmann::json& j, const ClassifiedDetectionBrightness& det)
{
    j =
        nlohmann::json
    {
        {"detection", det.detection.det},
        {"label", det.detection.label},
        {"confidence", det.detection.confidence},
        { "label_str", det.detection.labelStr},
        {"brightness_label", det.brightnessLabel},
        {"brightness_confidence", det.brightnessConfidence}
    };
}

inline void from_json(const nlohmann::json& j, ClassifiedDetectionBrightness& det)
{
    j.at("detection").get_to(det.detection.det);
    j.at("label").get_to(det.detection.label);
    j.at("confidence").get_to(det.detection.confidence);
    j.at("label_str").get_to(det.detection.labelStr);
    j.at("brightness_label").get_to(det.brightnessLabel);
    j.at("brightness_confidence").get_to(det.brightnessConfidence);
}

inline void to_json(nlohmann::json& j, const MMRInfo& mmrinfo)
{
    j.clear();

    j["colors"] = nlohmann::json::array();

    for (const auto& entry: mmrinfo.colors)
    {
        j["colors"].push_back(nlohmann::json
        {
            {"id", entry.id},
            {"name", entry.name},
            {"probability", entry.probability }
        });
    }

    j["models"] = nlohmann::json::array();

    for (const auto& entry : mmrinfo.models)
    {
        j["models"].push_back(nlohmann::json
            {
                {"id", entry.id},
                {"name", entry.name},
                {"type", entry.type},
                {"probability", entry.probability }
            });
    }
}

inline void from_json(const nlohmann::json& j, MMRInfo& mmrinfo)
{
    mmrinfo.colors.clear();
    for (const auto& jcolor: j["colors"])
    {
        MMRColorEntry colorEntry;
        jcolor.at("id").get_to(colorEntry.id);
        jcolor.at("name").get_to(colorEntry.name);
        jcolor.at("probability").get_to(colorEntry.probability);
        mmrinfo.colors.push_back(colorEntry);
    }

    mmrinfo.models.clear();
    for (const auto& jmodel : j["models"])
    {
        MMRModelEntry modelEntry;
        jmodel.at("id").get_to(modelEntry.id);
        jmodel.at("name").get_to(modelEntry.name);
        jmodel.at("type").get_to(modelEntry.type);
        jmodel.at("probability").get_to(modelEntry.probability);
        mmrinfo.models.push_back(modelEntry);
    }
}

int getIntFromJson(const nnlab::json& config, const std::string& paramName, const int defaultVal);

inline void to_json(nlohmann::json& j, const Track& t) {
    j = 
        nlohmann::json
    {
        {"label", t.label},
        {"bbox", nlohmann::json(t.bbox)},
        {"keypoints", t.keypoints}
    };
}

inline void from_json(const nlohmann::json& j, Track& t) {
    j.at("label").get_to(t.label);
    j.at("bbox").get_to(t.bbox);
    j.at("keypoints").get_to(t.keypoints);
}

inline void to_json(nlohmann::json& j, const NamedClassification& t) {
    j =
        nlohmann::json
        {
            {"name", t.name},
            {"confidence", t.confidence}
        };
}

inline void from_json(const nlohmann::json& j, NamedClassification& t) {
    j.at("name").get_to(t.name);
    j.at("confidence").get_to(t.confidence);
}

} // nnlab
