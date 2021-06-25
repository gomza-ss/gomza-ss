#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/OpenVINO/MMR.h>

namespace nnlab
{

REGISTER_INTEL_TASK_SETUPER(TSSM_OpenVINO_MMR,
                            getInstallPath() + "/Modules/TSSM/NN_servers/OpenVINOServer/OpenVINOServer.exe",
                            std::string("model"))
.Template(MMRTemplateOpenVINO)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/TSSM/NN_servers/OpenVINOServer/data"));

} // namespace nnlab
