#pragma once

#include <NNClient/core/NNClientTypes.h>
#include <NNClient/json/json.h>

#include <boost/any.hpp>
#include <functional>
#include <boost/optional.hpp>
#include <unordered_map>
#include <string>

namespace nnlab
{

using string_vector = std::vector<std::string>;

class Config
{
public:
    bool initialize(const std::string& desc, std::string& error_code, bool ignoreExtra = true);
    bool initialize(const Config& config, std::string& error_code);

    template<typename T>
    Config& Arg(const std::string& name)
    {
        m_params[name] = boost::optional<T>();

        m_paramNamesOrdered.push_back(name);

        return *this;
    }


    template<typename T, typename Function, typename... Args>
    Config& Arg(const std::string& name,
        Function && defaultInit,
        Args... args)
    {
        boost::any param = static_cast<boost::optional<T>>(std::invoke(std::forward<Function>(defaultInit), std::forward(args) ...));

        m_params[name] = std::move(param);

        m_paramNamesOrdered.push_back(name);

        return *this;
    }

    bool argsStringRepr(std::string& output, std::string& errorMsg) const;
    bool initialized(std::string& notInitializedElemName) const;

    bool desc(std::string& output, std::string& errorMsg) const;

    template <typename T>
    bool setParam(const std::string& name, T value, std::string& errorMsg)
    {
        if (m_params.count(name) == 0)
        {
            errorMsg = std::string("No param named ") + name;
            return false;
        }

        m_params[name] = boost::optional<T>(std::move(value));

        return true;
    }

    template <typename T>
    bool get(const std::string& name, T& object) const
    {
        if (m_params.count(name) == 0)
        {
            return false;
        }

        try
        {
            object = boost::any_cast<boost::optional<T>>(m_params.at(name)).value();
        }
        catch (std::bad_cast &)
        {
            return false;
        }
        catch (boost::bad_optional_access &)
        {
            return false;
        }

        return true;
    }

    bool removeParam(const std::string& name);
    bool hasArgWithName(const std::string& name) const;
    void getParamNames(std::vector<std::string>& paramNames) const;

private:

    bool valInitialised(const std::string& name) const;

private:

    NNClientMap<std::string, boost::any> m_params;
    std::vector<std::string> m_paramNamesOrdered;

};

template <typename Ret, typename ... Args>
auto to_std_function(Ret(*fn)(Args ...))
{
    return std::function<Ret(Args ...)>(fn);
}

} // namespace nnlab
