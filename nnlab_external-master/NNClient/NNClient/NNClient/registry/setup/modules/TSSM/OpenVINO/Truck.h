#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/OpenVINO/Truck.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(TSSM_OpenVINO_Truck,
                            getInstallPath() + "/Modules/TSSM/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Template(TruckTemplateOpenVINO);

} // namespace nnlab
