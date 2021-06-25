#include <NNClient/registry/setup/core/SetupConfigInitializer.h>
#include <NNClient/registry/setup/core/UpdateConfigIfNoParam.h>

#include <NNClient/core/Config.h>

#include <boost/any.hpp>
#include <string>

#include <opencv2/core.hpp>

#ifdef min
#undef min
#endif

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define UPDATE_INPUT_CONFIG(name, value) \
    if (!updateConfigIfNoParam<decltype(value)>( \
        inputConfig, \
        name, \
        value, \
        errorMessage)) { \
        return false; \
    }

namespace nnlab
{

SetupConfigInitializer::SetupConfigInitializer() {}

SetupConfigInitializer& SetupConfigInitializer::Param(const std::string& name, const boost::any& value)
{
    m_staticParams[name] = value;
    m_allParams[name] = value;
    return ParamConditioned(name, [value](const auto &) -> boost::any { return value; });
}

SetupConfigInitializer& SetupConfigInitializer::ParamConditioned(const std::string& name, 
                                                                 const SetupConfigInitializer::SetupConfigInitializerFunc& f)
{
    m_runtimeParams[name] = f;
    return *this;
}

SetupConfigInitializer& SetupConfigInitializer::TargetMinObject(int targetMinObject)
{
    m_targetMinObject = targetMinObject;
    return *this;
}

SetupConfigInitializer& SetupConfigInitializer::TargetMaxObject(int targetMaxObject)
{
    m_targetMaxObject = targetMaxObject;
    return *this;
}

SetupConfigInitializer& SetupConfigInitializer::MinObject(int minObject)
{
    m_minObject = minObject;
    return *this;
}

SetupConfigInitializer& SetupConfigInitializer::MaxObject(int maxObject)
{
    m_maxObject = maxObject;
    return *this;
}

SetupConfigInitializer& SetupConfigInitializer::InputImageSize(const cv::Size& inputImageSize)
{
    m_inputImageSize = inputImageSize;
    return *this;
}

bool SetupConfigInitializer::operator() (
    Config& inputConfig,
    std::string& errorMessage)
{
    for (const std::pair<std::string, SetupConfigInitializer::SetupConfigInitializerFunc>& paramUpdater : m_runtimeParams) {
        
        const std::string& paramName = paramUpdater.first;

        try
        {
            const boost::any paramValue = paramUpdater.second(inputConfig);
            m_allParams[paramName] = paramValue;

            if (paramValue.type() == typeid(int)) {
                UPDATE_INPUT_CONFIG(paramName, boost::any_cast<int>(paramValue))
            }
            else if (paramValue.type() == typeid(float)) {
                UPDATE_INPUT_CONFIG(paramName, boost::any_cast<float>(paramValue))
            }
            else if (paramValue.type() == typeid(bool)) {
                UPDATE_INPUT_CONFIG(paramName, static_cast<bool>(boost::any_cast<bool>(paramValue)))
            }
            else if (paramValue.type() == typeid(std::string)) {
                UPDATE_INPUT_CONFIG(paramName, boost::any_cast<std::string>(paramValue))
            }
            else if (paramValue.type() == typeid(string_vector)) {
                UPDATE_INPUT_CONFIG(paramName, boost::any_cast<string_vector>(paramValue))
            }
            else if (paramValue.type() == typeid(std::vector<cv::Rect>)) {
                UPDATE_INPUT_CONFIG(paramName, boost::any_cast<std::vector<cv::Rect> >(paramValue))
            }
            else {
                errorMessage = std::string("Usupported param type: ") + paramValue.type().name();
                return false;
            }
        }
        catch (const boost::bad_any_cast&)
        {
            errorMessage = std::string("Cast exception on parameter ") + paramName;
            return false;
        }
    }

    return true;
}

bool SetupConfigInitializer::calculateNetInput (
    Config& inputConfig,
    std::string& errorMessage)
{
    float scale = 1.0;
    if (m_minObject > 0 && m_maxObject > 0) {
        scale = min(float(m_targetMinObject) / float(m_minObject), float(m_targetMaxObject) / float(m_maxObject));
    }

    int net_width = static_cast<int>(scale  * float(m_inputImageSize.width) + 0.5f);
    int net_height = static_cast<int>(scale * float(m_inputImageSize.height) + 0.5f);

    int sizeDivisibility = 0;
    if (inputConfig.get<int>("size_divisibility", sizeDivisibility) && sizeDivisibility > 1)
    {
        net_width = static_cast<int>(std::roundf(static_cast<float>(net_width) / sizeDivisibility)) * sizeDivisibility;
        net_height = static_cast<int>(std::roundf(static_cast<float>(net_height) / sizeDivisibility)) * sizeDivisibility;
    }

    UPDATE_INPUT_CONFIG("net_width", net_width)
    UPDATE_INPUT_CONFIG("net_height", net_height)

    return true;
}

} // namespace nnlab
