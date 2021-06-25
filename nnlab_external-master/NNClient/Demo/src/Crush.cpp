#include <Crush.h>

#include <iostream>

namespace nnlab
{

void crush(const std::string & errorMessage)
{
    std::cerr << errorMessage << std::endl;
    abort();
}

} // namespace nnlab
