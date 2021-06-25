#pragma once

#include <NNClient/core/Macro.h>
#include <NNClient/core/NNClientTypes.h>

namespace nnlab
{

#define DECLARE_REGISTRY(RegistryName, ElemType)                          \
class RegistryName##Registry                                              \
{                                                                         \
public:                                                                   \
                                                                          \
    static ElemType& add##ElemType(std::string ElemType##Name)            \
    {                                                                     \
        auto& m = map();                                                  \
        m.emplace(std::make_pair(ElemType##Name, ElemType()));            \
        return m[ElemType##Name];                                         \
    }                                                                     \
                                                                          \
    static const ElemType* p##ElemType(const std::string& ElemType##Name) \
    {                                                                     \
        auto& m = map();                                                  \
        auto it = m.find(ElemType##Name);                                 \
        if (it != m.end()) {                                              \
            return &it->second;                                           \
        } else {                                                          \
            return nullptr;                                               \
        }                                                                 \
    }                                                                     \
                                                                          \
private:                                                                  \
                                                                          \
    RegistryName##Registry() = delete;                                    \
    static NNClientMap<std::string, ElemType>& map();                     \
};

}
