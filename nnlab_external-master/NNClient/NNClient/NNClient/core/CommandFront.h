#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/CommandProcessorBack.h>
#include <NNClient/core/OutputParser.h>

#include <functional>
#include <vector>

namespace nnlab
{

struct CommandFront
{
    using preFnType = std::function<bool(CommandProcessorBack &, Config &, std::vector<std::string>&, std::string&, std::string & )>;

    std::string name;
    Config inputConfig;
    Config outputConfig;
    OutputParser outputParser;
    std::vector<std::string> commandSequence;
    bool tensorRequired{false};
    bool tensorOutputRequired{false};
    preFnType preFn;

    CommandFront& Name(const std::string & name);

    CommandFront& TensorRequired();

    CommandFront& TensorOutputRequired();

    template<typename T>
    CommandFront& InputArg(const std::string& name)
    {
        inputConfig.Arg<T>(name);

        return *this;
    }


    template<typename T, typename Function, typename... Args>
    CommandFront& InputArg(const std::string& name,
                Function && defaultInit,
                Args... args)
    {
        inputConfig.Arg<T>(name, std::forward<Function>(defaultInit), std::forward<Args>(args) ...);

        return *this;
    }

    template<typename T>
    CommandFront& OutputArg(const std::string& name)
    {
        outputConfig.Arg<T>(name);

        return *this;
    }


    template<typename T, typename Function, typename... Args>
    CommandFront& OutputArg(const std::string& name,
        Function && defaultInit,
        Args... args)
    {
        outputConfig.Arg<T>(name, std::forward<Function>(defaultInit), std::forward<Args>(args) ...);

        return *this;
    }

    CommandFront& CommandSequence(const std::vector<std::string> & sequence);

    CommandFront& SetPreFn(preFnType preFn);

    CommandFront& DefaultSequence();

    CommandFront& EnableDynamicInputSize();

};

} // namespace nnlab
