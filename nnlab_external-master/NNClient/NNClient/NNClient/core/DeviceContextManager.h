#pragma once

#include <NNClient/core/Config.h>
#include <NNClient/core/Macro.h>

#include <QString>

namespace nnlab
{

class DeviceContextManager
{
public:

    DeviceContextManager(const Config& config);
    ~DeviceContextManager();

private:

    QString m_cached_CUDA_VISIBLE_DEVICES;

private:

    NN_CLIENT_DISABLE_COPY_AND_MOVE(DeviceContextManager)

};

} // namespace nnlab
