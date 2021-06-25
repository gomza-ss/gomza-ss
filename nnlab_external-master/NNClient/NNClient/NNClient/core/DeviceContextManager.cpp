#include <NNClient/core/DeviceContextManager.h>

namespace
{
const std::string CUDA_VISIBLE_DEVICES("CUDA_VISIBLE_DEVICES");
} // namespace

namespace nnlab
{

DeviceContextManager::DeviceContextManager(const Config& config)
{
    std::string deviceVendor;
    config.get<std::string>(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, deviceVendor);

    if (deviceVendor == NN_CLIENT_NVIDIA)
    {
        m_cached_CUDA_VISIBLE_DEVICES = QString::fromLocal8Bit(qgetenv(CUDA_VISIBLE_DEVICES.c_str()));
    }

    std::string new_CUDA_VISIBLE_DEVICES;
    if (config.get<std::string>(NN_CLIENT_DEVICE_PARAM_NAME, new_CUDA_VISIBLE_DEVICES))
    {
        qputenv(CUDA_VISIBLE_DEVICES.c_str(), new_CUDA_VISIBLE_DEVICES.c_str());
    }
}

DeviceContextManager::~DeviceContextManager()
{
    qputenv(CUDA_VISIBLE_DEVICES.c_str(), m_cached_CUDA_VISIBLE_DEVICES.toLocal8Bit());
}

} // namespace nnlab
