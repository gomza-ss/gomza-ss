#include <InitLog4cxx.h>

#include <log4cxx/propertyconfigurator.h>

#include <boost/filesystem.hpp>

namespace nnlab
{

void initLog4cxx()
{
    const boost::filesystem::path currSourceFolder(boost::filesystem::path(__FILE__).parent_path());
    const boost::filesystem::path logConfigPath = currSourceFolder / std::string("NNClient.LoggerSetts.txt");

    try 
    {
        if (boost::filesystem::is_regular_file(logConfigPath))
        {
            log4cxx::PropertyConfigurator::configure(logConfigPath.string());
        }
    }
    catch (boost::filesystem::filesystem_error&)
    {
    }

}

} // namespace nnlab
