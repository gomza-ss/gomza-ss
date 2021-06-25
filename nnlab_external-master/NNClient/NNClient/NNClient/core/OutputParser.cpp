#include <NNClient/core/OutputParser.h>

namespace nnlab
{
    
OutputParser& OutputParser::Command(const std::string& command)
{
    // we assure the command value is not changed
    // after initial Registration
    if (!m_command.empty()) {
        return *this;
    }

    m_command = command;
    return *this;
}

OutputParser& OutputParser::DontExpectStatusOK()
{
    m_expectStatusOK = false;
    return *this;
}

OutputParser& OutputParser::Parser(const ParserFuncType& parser)
{
    m_parserImpl = parser;
    return *this;
}

bool OutputParser::operator() (
    const std::string& serverOutput,
    Config& outputConfig,
    std::string& errorMessage) const
{
    errorMessage.clear();

    const std::string statusOk = m_command + std::string("ok");

    if (m_expectStatusOK && serverOutput.find(statusOk) != 0) {
        errorMessage = std::string("Expected server output to start with ") + statusOk +
            std::string(" got ") + serverOutput + std::string(" instead");
        return false;
    }

    if (m_parserImpl) {
        return m_parserImpl(serverOutput, outputConfig, errorMessage);
    }

    return true;
}

}
