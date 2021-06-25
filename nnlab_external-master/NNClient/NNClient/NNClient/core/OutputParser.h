#pragma once

#include <NNClient/core/Config.h>

namespace nnlab
{

class OutputParser
{
public:

    using ParserFuncType = std::function<bool(
        const std::string&, /* server output */
        Config&,            /* output config */
        std::string&        /* error message */
        )>;

public:

    OutputParser& Command(const std::string& command);
    OutputParser& Parser(const ParserFuncType& parser);
    OutputParser& DontExpectStatusOK();

    bool operator() (
        const std::string& serverOutput,
        Config& outputConfig,
        std::string& errorMessage
        ) const;

private:

    ParserFuncType m_parserImpl;
    std::string m_command;
    bool m_expectStatusOK{ true };
};

}
