#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/OpenVINO/VehicleDet.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(TK3_OpenVINO_VehicleDet,
                            getInstallPath() + "/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Template(VehicleDetTemplateOpenVINO)
.Param("data_prefix", getInstallPath() + "/NN_servers/data/OpenVINO");

} // namespace nnlab
