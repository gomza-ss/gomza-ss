#pragma once

#include <NNClient/registry/NNClientRegistries.h>
#include <NNClient/registry/setup/templates/Caffe2/Truck.h>

namespace nnlab
{

REGISTER_NVIDIA_TASK_SETUPER(Platon_Caffe2_Truck, getInstallPath() + "/Modules/TSSM/NN_servers/Caffe2Server/Caffe2Server.exe")
.Template(TruckTemplateCaffe2);

} // namespace nnlab
