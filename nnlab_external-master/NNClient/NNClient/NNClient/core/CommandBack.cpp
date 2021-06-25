#include <NNClient/core/CommandBack.h>

namespace nnlab
{

CommandBack& CommandBack::Name(const std::string& name)
{
    m_outputParser.Command(name);

    return *this;
}

CommandBack& CommandBack::Parser(const OutputParser::ParserFuncType& parser)
{
    m_outputParser.Parser(parser);

    return *this;
}

CommandBack& CommandBack::DontExpectStatusOK()
{
    m_outputParser.DontExpectStatusOK();

    return *this;
}

const Config & CommandBack::inputConfig() const
{
    return m_inputConfig;
}

const Config & CommandBack::outputConfig() const
{
    return m_outputConfig;
}

const OutputParser & CommandBack::outputParser() const
{
    return m_outputParser;
}

}
