#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/Caffe2/VehicleDet.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(TK3_Caffe2_VehicleDet, getInstallPath() + "/NN_servers/Caffe2Server/Caffe2Server.exe")
.Template(VehicleDetTemplateCaffe2)
.Param("data_prefix", std::string(getInstallPath() + "/NN_servers/data/Caffe2"));

} // namespace nnlab
