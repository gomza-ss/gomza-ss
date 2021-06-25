#include <NNClient/core/Config.h>

#include <NNClient/json/json.h>
#include <NNClient/core/Macro.h>

#include <iostream>

namespace
{

boost::any defaultAny(const nlohmann::json::value_t& type)
{
    static const nnlab::NNClientMap<nlohmann::json::value_t, boost::any> defaultAnyMap =
    {
        { nlohmann::json::value_t::string, boost::optional<std::string>() },
        { nlohmann::json::value_t::number_float, boost::optional<float>() },
        { nlohmann::json::value_t::boolean, boost::optional<bool>() },
        { nlohmann::json::value_t::number_integer, boost::optional<int>() },
        { nlohmann::json::value_t::number_unsigned, boost::optional<int>() }
    };

    return defaultAnyMap.at(type);
}

}

namespace nnlab
{

bool Config::initialize(const std::string& desc, std::string& error_message, bool ignoreExtra)
{
    error_message.clear();
    
    if (desc.empty()) {
        return true;
    }

    nlohmann::json json;
    if (!jsonSafeParse(desc, json, error_message)) {
        return false;
    }

    for (const auto& el: json.items())
    {
        const auto& key = el.key();
        const auto& val = el.value();
        std::string name;
        if (!jsonSafeCast(key, name, error_message)) {
            return false;
        }

        const bool nameInConfig(m_params.find(name) != m_params.end());

        if (ignoreExtra && !nameInConfig)
        {
            continue;
        }
        
        if (!nameInConfig)
        {
            m_params[name] = defaultAny(val.type());
        }
        const auto& paramType = m_params.at(name).type();

#define PARSE(valT) \
    if (paramType == typeid(boost::optional<valT>)) \
    { \
        auto & param = boost::any_cast<boost::optional<valT> &>(m_params[key]); \
        param = val.get<valT>(); \
        continue; \
    }
        try
        {
            PARSE(int)
            PARSE(int64_t)
            PARSE(float)
            PARSE(bool)
            PARSE(std::string)
            PARSE(cv::Rect)
            PARSE(std::vector<cv::Rect>)
            PARSE(string_vector)

            error_message = std::string("Unknown type: ") + paramType.name();
            return false;
        }
        catch (boost::bad_any_cast& e)
        {
            error_message = e.what();
            return false;
        }
        catch (nlohmann::json::exception& e)
        {
            error_message = std::string("Failed to parse json: ") + e.what();
            return false;
        }

#undef PARSE

    }
    
    return true;
}

bool Config::initialize(const Config& config, std::string& error_message)
{
    error_message.clear();

    for (auto & pair: m_params)
    {
        if (config.m_params.count(pair.first))
        {
            pair.second = config.m_params.at(pair.first);
        }
    }

    return true;
}

namespace
{

template<typename T>
bool convert_to_type(const boost::any& param,
    T& retVal)
{
    try
    {
        if (param.type() == typeid(boost::optional<T>))
        {
            retVal = boost::any_cast<boost::optional<T>>(param).value();
        }
        else
        {
            return false;
        }
    }
    catch (boost::bad_any_cast&)
    {
        return false;
    }
    catch (boost::bad_optional_access&)
    {
        return false;
    }
    return true;
}

template<typename T>
bool to_string(const T& /*val*/,
               std::string& /*stringRepr*/)
{
    return false;
}

template<>
bool to_string(const int& val,
               std::string& stringRepr)
{
    try {
        stringRepr = std::to_string(val);
        return true;
    } catch (const std::invalid_argument&)
    { //-V565
    }
    return false;
}

template<>
bool to_string(const int64_t& val,
    std::string& stringRepr)
{
    try {
        stringRepr = std::to_string(val);
        return true;
    }
    catch (const std::invalid_argument&)
    { //-V565
    }
    return false;
}

template<>
bool to_string(const float& val,
               std::string& stringRepr)
{
    try 
    {
        stringRepr = std::to_string(val);
        return true;
    } catch (const std::invalid_argument&)
    {//-V565
    }
    return false;
}

template<>
bool to_string(const bool& val,
    std::string& stringRepr)
{
    try {
        stringRepr = std::to_string(static_cast<int>(val));
        return true;
    }
    catch (const std::invalid_argument&)
    {//-V565
    }
    return false;
}

template<>
bool to_string(const std::string& val,
               std::string& stringRepr)
{
    stringRepr = val;
    return true;
}

template<>
bool to_string(const cv::Rect& val,
    std::string& stringRepr)
{   
    stringRepr =
        std::to_string(val.tl().x) + ' '
        + std::to_string(val.tl().y) + ' '
        + std::to_string(val.br().x) + ' '
        + std::to_string(val.br().y);
       
    return true;
}

template<>
bool to_string(const std::vector<cv::Rect>& val,
    std::string& stringRepr)
{
    stringRepr = "[";
    for (const auto & bbox : val)
    {
        stringRepr +=
            '{'
                + std::to_string(bbox.x) + ','
                + std::to_string(bbox.y) + ','
                + std::to_string(bbox.width) + ','
                + std::to_string(bbox.height)
                + "},";
    }
    if (!val.empty())
    {
        stringRepr.pop_back();
    }
    stringRepr += ']';

    return true;
}

template<>
bool to_string(const string_vector& val, std::string& stringRepr)
{
    stringRepr = "[";
    for (const auto & str : val)
    {
        stringRepr += str + ',';
    }
    if (!val.empty())
    {
        stringRepr.pop_back();
    }
    stringRepr += ']';

    return true;
}

template<>
bool to_string(const boost::any& val,
               std::string& stringRepr)
{
    if (val.empty()) {
        return false;
    }

#define TRY_RETURN_TYPE(valT) \
    { \
        valT param; \
        if (convert_to_type<valT>(val, param)) { \
            return to_string(param, stringRepr); \
        } \
    }

    TRY_RETURN_TYPE(bool)
    TRY_RETURN_TYPE(int)
    TRY_RETURN_TYPE(int64_t)
    TRY_RETURN_TYPE(float)
    TRY_RETURN_TYPE(double)
    TRY_RETURN_TYPE(std::string)
    TRY_RETURN_TYPE(cv::Rect)
    TRY_RETURN_TYPE(std::vector<cv::Rect>)
    TRY_RETURN_TYPE(string_vector)

#undef TRY_RETURN_TYPE

    return false;
}

} // namespace

bool Config::argsStringRepr(std::string& output, std::string& errorMsg) const
{
    errorMsg.clear();

    for (std::string paramName : m_paramNamesOrdered) {

        const auto& param = m_params.at(paramName);

        std::string paramStringRepr;
        
        if (to_string(param, paramStringRepr))
        {
            // Для работы сервера необходимо, чтобы float был 0.5 а не 0,5
            if (param.type() == typeid(boost::optional<float>) || param.type() == typeid(boost::optional<double>) ||
                param.type() == typeid(float) || param.type() == typeid(double))
            {
                std::replace(paramStringRepr.begin(), paramStringRepr.end(), ',', '.');
            }

            output += std::string(" ") + paramStringRepr;
        } else {
            errorMsg = std::string("parameter ") + paramName + std::string(" is of unsupported type: ") + param.type().name();
            return false;
        }
        
        #undef TO_STRING_TYPE
    }

    return true;
}

bool Config::initialized(std::string& notInitializedElemName) const
{
    for (const auto& pair: m_params) {
        if (!valInitialised(pair.first)) {
            notInitializedElemName = pair.first;
            return false;
        }
    }
    return true;

}

bool Config::desc(std::string& output, std::string& errorMsg) const
{
    nlohmann::json json;

    for (const auto&pair : m_params)
    {
        const auto& key = pair.first;
        const auto& val = pair.second;
        if (!valInitialised(key)) {
            errorMsg = std::string("value ") + key +
                       std::string(" was not initialized");
            return false;
        }

#define TRY_HANDLE_TYPE(valT) \
        { \
            valT param; \
            if (convert_to_type<valT>(val, param)) { \
                json[std::string(key)] = param; \
                continue; \
            } \
        }

        TRY_HANDLE_TYPE(int)
        TRY_HANDLE_TYPE(float)
        TRY_HANDLE_TYPE(std::string)
        TRY_HANDLE_TYPE(std::vector<CudaDeviceInfo>)
        TRY_HANDLE_TYPE(Detection)
        TRY_HANDLE_TYPE(std::vector<Detection>)
        TRY_HANDLE_TYPE(std::vector<cv::Rect>)
        TRY_HANDLE_TYPE(std::vector<int>)
        TRY_HANDLE_TYPE(std::vector<std::vector<int>>)
        TRY_HANDLE_TYPE(std::vector<Track>)
        TRY_HANDLE_TYPE(std::vector<ClassifiedDetection>)
        TRY_HANDLE_TYPE(std::vector<ClassifiedDetectionBrightness>)
        TRY_HANDLE_TYPE(MMRInfo)
        TRY_HANDLE_TYPE(string_vector)
        TRY_HANDLE_TYPE(NamedClassification)

#undef TRY_HANDLE_STD_TYPE

        errorMsg = std::string("cannot convert argument ") + key;
        return false;


    }

    json["status"] = 0;
    json["errorMsg"] = errorMsg;

    const int indent = 4;
    output = json.dump(indent);

    return true;
}

bool Config::valInitialised(const std::string& name) const
{
#define TRY_GET(valT) \
    if (m_params.at(name).type() == typeid(boost::optional<valT>)) \
    { \
        const auto &param = boost::any_cast<const boost::optional<valT> &>(m_params.at(name)); \
        return (bool)param; \
    }

    TRY_GET(int)//-V807
    TRY_GET(int64_t)//-V807
    TRY_GET(float)//-V807
    TRY_GET(bool)//-V807
    TRY_GET(std::string)
    TRY_GET(cv::Rect)
    TRY_GET(Detection)
    TRY_GET(std::vector<Detection>)
    TRY_GET(std::vector<cv::Rect>)
    TRY_GET(std::vector<int>)
    TRY_GET(std::vector<std::vector<int> >)
    TRY_GET(std::vector<CudaDeviceInfo>)
    TRY_GET(std::vector<Track>)
    TRY_GET(std::vector<ClassifiedDetection>)
    TRY_GET(std::vector<ClassifiedDetectionBrightness>)
    TRY_GET(MMRInfo)
    TRY_GET(string_vector)
    TRY_GET(NamedClassification)

    return false;
    
#undef TRY_GET

}

bool Config::hasArgWithName(const std::string& name) const
{
    return m_params.count(name) > 0;
}

bool Config::removeParam(const std::string& name)
{
    if (!hasArgWithName(name))
    {
        return false;
    }

    m_params.erase(name);
    return true;
}

void Config::getParamNames(std::vector<std::string>& paramNames) const
{
    paramNames.clear();
    for (const auto& keyVal: m_params)
    {
        paramNames.push_back(keyVal.first);
    }
}

} // namespace nnlab
