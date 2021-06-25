#include <NNClient/json/json.h>

namespace nnlab
{

bool jsonSafeParse(
    const std::string& desc,
    nlohmann::json& out,
    std::string& error_message
)
{
    try {
        out = nlohmann::json::parse(desc);
    }
    catch (const nlohmann::detail::exception& e)
    {
        error_message = e.what();
        return false;
    }

    return true;
}

int getIntFromJson(const nnlab::json& config, const std::string& paramName, const int defaultVal)
{
    auto iter = config.find(paramName);
    if (iter != config.end())
    {
        try
        {
            const int val = iter->get<int>();
            return val;
        }
        catch (const nnlab::json::exception&)
        {
            return defaultVal;
        }
    }

    return defaultVal;
}
   
} // namespace nnlab
