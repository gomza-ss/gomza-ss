#pragma once

#include <NNClient/core/Config.h>

#include <boost/any.hpp>
#include <string>

namespace nnlab
{

class SetupConfigInitializer
{

public:

    using SetupConfigInitializerFunc = std::function<boost::any(Config &)>;

    SetupConfigInitializer();

    SetupConfigInitializer& Param(const std::string& name, const boost::any& value);
    SetupConfigInitializer& ParamConditioned(const std::string& name, const SetupConfigInitializerFunc& f);

    SetupConfigInitializer& TargetMinObject(int targetMinObject);
    SetupConfigInitializer& TargetMaxObject(int targetMaxObject);

    SetupConfigInitializer& MinObject(int minObject);
    SetupConfigInitializer& MaxObject(int maxObject);

    SetupConfigInitializer& InputImageSize(const cv::Size& inputImageSize);

    inline int getTargetMinObject() const
    {
        return m_targetMinObject;
    }

    inline int getTargetMaxObject() const
    {
        return m_targetMaxObject;
    }

    inline int getMinObject() const
    {
        return m_minObject;
    }

    inline int getMaxObject() const
    {
        return m_maxObject;
    }

    inline cv::Size getInputImageSize() const
    {
        return m_inputImageSize;
    }

    inline const auto & getParams() const
    {
        return m_allParams;
    }

    bool operator() (
        Config& inputConfig,
        std::string& errorMessage);

    bool calculateNetInput(
        Config& inputConfig,
        std::string& errorMessage);

    template<typename T>
    bool getTaskParam(const std::string& key, T& val, std::string& errorMessage) const
    {
        if (!m_allParams.count(key))
        {
            errorMessage = std::string("Param with key ") + key + std::string(" not found in SetupConfigInitializer");
            return false;
        }

        try
        {
            val = boost::any_cast<T>(m_allParams.at(key));
        }
        catch (const boost::bad_any_cast& e)
        {
            errorMessage = std::string("SetupConfigInitializer: ")  + e.what();
            return false;
        }

        return true;
    }

private:

    NNClientMap<std::string, boost::any> m_staticParams;
    NNClientMap<std::string, SetupConfigInitializerFunc> m_runtimeParams;

    NNClientMap<std::string, boost::any> m_allParams;

    int m_minObject{ 0 }, m_maxObject{ 0 };
    int m_targetMinObject{ 0 }, m_targetMaxObject{ 0 };
    cv::Size m_inputImageSize;
};

} // namespace nnlab
