#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/Caffe2/VehicleDet.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(Vehicle_Caffe2_VehicleDet, getInstallPath() + "/Modules/VehicleNN/NN_servers/Caffe2Server/Caffe2Server.exe")
.Template(VehicleDetTemplateCaffe2)
.Param("data_prefix", getInstallPath() + std::string("/Modules/VehicleNN/NN_servers/Caffe2Server/data"));

} // namespace nnlab
