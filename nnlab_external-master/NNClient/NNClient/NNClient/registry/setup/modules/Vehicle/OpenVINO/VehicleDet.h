#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/OpenVINO/VehicleDet.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(Vehicle_OpenVINO_VehicleDet,
                            getInstallPath() + "/Modules/VehicleNN/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Template(VehicleDetTemplateOpenVINO)
.Param("data_prefix", getInstallPath() + std::string("/Modules/VehicleNN/NN_servers/OpenVINOServer/data"));

} // namespace nnlab
