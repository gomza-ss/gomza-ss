#include <NNClient/registry/setup/core/CommandSequence.h>

namespace nnlab
{

CommandSequence& CommandSequence::Command(const std::string& command)
{
    return CommandConditioned([](const auto &) {return true; }, command);
}

CommandSequence& CommandSequence::CommandConditioned(const CommandSequenceFunc& f, const std::string& command)
{
    m_sequence.emplace_back(f, command);
    return *this;
}

}
