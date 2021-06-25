#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/OutputParser.h>

namespace nnlab
{

class CommandBack
{
public:
    CommandBack& Name(const std::string& name);

    CommandBack& Parser(const OutputParser::ParserFuncType& parser);

    CommandBack& DontExpectStatusOK();

    template<typename T>
    CommandBack& InputArg(const std::string& name)
    {
        m_inputConfig.Arg<T>(name);

        return *this;
    }

    template<typename T, typename Function, typename... Args>
    CommandBack& InputArg(const std::string& name,
                Function && defaultInit,
                Args... args)
    {
        m_inputConfig.Arg<T>(name, std::forward<Function>(defaultInit), std::forward<Args>(args) ...);

        return *this;
    }

    template<typename T>
    CommandBack& OutputArg(const std::string& name)
    {
        m_outputConfig.Arg<T>(name);

        return *this;
    }

    template<typename T, typename Function, typename... Args>
    CommandBack& OutputArg(const std::string& name,
                Function && defaultInit,
                Args... args)
    {
        m_outputConfig.Arg<T>(name, std::forward<Function>(defaultInit), std::forward<Args>(args) ...);

        return *this;
    }

    const Config & inputConfig () const;

    const Config & outputConfig () const;

    const OutputParser & outputParser () const;

private:
    Config m_inputConfig;
    Config m_outputConfig;
    OutputParser m_outputParser;
};

} // namespace nnlab
