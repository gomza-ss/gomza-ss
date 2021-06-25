#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/Caffe2/MMR.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(Auto_Caffe2_MMR, getInstallPath() + "/Modules/Auto/NN_servers/Caffe2Server/Caffe2Server.exe")
.Template(MMRTemplateCaffe2)
.Param("data_prefix", std::string(getInstallPath() + "/Modules/Auto/NN_servers/Caffe2Server/data"));

} // namespace nnlab
