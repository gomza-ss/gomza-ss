#pragma once

#include <NNClient/core/Config.h>

#include <functional>
#include <string>
#include <vector>

namespace nnlab
{

class CommandSequence
{
public:

    using CommandSequenceFunc = std::function<bool(const Config &)>;

    CommandSequence& Command(const std::string& command);
       
    CommandSequence& CommandConditioned(const CommandSequenceFunc& f, const std::string& command);

    inline const auto& sequence () const
    {
        return m_sequence;
    }

private:

    std::vector<std::pair<CommandSequenceFunc, std::string>> m_sequence;

};

} // namespace nnlab
