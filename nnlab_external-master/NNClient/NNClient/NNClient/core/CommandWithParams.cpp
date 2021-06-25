#include <NNClient/core/CommandWithParams.h>
#include <NNClient/core/Registry.h>

namespace nnlab
{

bool commandWithParams(const std::string& command, const Config & config, std::string& output, std::string& errorMessage)
{
    output.clear();
    errorMessage.clear();

    output = command;

    return config.argsStringRepr(output, errorMessage);
}

}
