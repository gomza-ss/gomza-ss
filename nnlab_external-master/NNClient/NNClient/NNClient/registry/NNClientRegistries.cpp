#include <NNClient/registry/NNClientRegistries.h>

namespace nnlab
{

#define INITIALIZE_REGISTRY_MAP(RegistryName, ElemType)           \
NNClientMap<std::string, ElemType>& RegistryName##Registry::map() \
{                                                                 \
    static NNClientMap<std::string, ElemType> mapVal;             \
    return mapVal;                                                \
}

INITIALIZE_REGISTRY_MAP(CommandBack, CommandBack)
INITIALIZE_REGISTRY_MAP(CommandFront, CommandFront)
INITIALIZE_REGISTRY_MAP(TaskSetuper, TaskSetuper)

} // namespace nnlab
