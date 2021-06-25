#pragma once

#include <NNClient/core/Registry.h>

#include <NNClient/core/CommandBack.h>
#include <NNClient/core/CommandFront.h>
#include <NNClient/registry/setup/TaskSetuper.h>

namespace nnlab
{

DECLARE_REGISTRY(CommandBack, CommandBack)
DECLARE_REGISTRY(CommandFront, CommandFront)
DECLARE_REGISTRY(TaskSetuper, TaskSetuper)

#define REGISTER_COMMAND_BACK(name)                                                         \
void COMMAND_BACK_##CommandBackRegistry##name(){};                                          \
static auto* NN_CLIENT_ANONYMOUS_VARIABLE(CommandBackRegistry##name) NN_CLIENT_UNUSED_VAR = \
    &CommandBackRegistry::addCommandBack(#name).Name(#name)

#define REGISTER_COMMAND_FRONT(name)                                                                 \
void COMMAND_##CommandFrontRegistry##name(){};                                                       \
static CommandFront* NN_CLIENT_ANONYMOUS_VARIABLE(CommandFrontRegistry##name) NN_CLIENT_UNUSED_VAR = \
    &CommandFrontRegistry::addCommandFront(#name).Name(#name)

#define REGISTER_TASK_SETUPER(name)                                                                \
void TASK_SETUPER_##SetuperRegistry##name() {};                                                    \
static TaskSetuper* NN_CLIENT_ANONYMOUS_VARIABLE(TaskSetuperRegistry##name) NN_CLIENT_UNUSED_VAR = \
    &TaskSetuperRegistry::addTaskSetuper(#name)

#define REGISTER_TASK_SETUPER_WITH_EXEC_PATH(name, exec_path) \
REGISTER_TASK_SETUPER(name).Param(NN_CLIENT_EXEC_PATH_PARAM_NAME, std::string(exec_path))

#define REGISTER_INTEL_TASK_SETUPER(name, exec_path, reference_path) \
REGISTER_TASK_SETUPER_WITH_EXEC_PATH(name, exec_path)                \
.Param(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, NN_CLIENT_INTEL)          \
.Param(NN_CLIENT_REFERENCE_PATH_PARAM_NAME, reference_path)

#define REGISTER_NVIDIA_TASK_SETUPER(name, exec_path) \
REGISTER_TASK_SETUPER_WITH_EXEC_PATH(name, exec_path).Param(NN_CLIENT_DEVICE_VENDOR_PARAM_NAME, NN_CLIENT_NVIDIA) \
.Command("checkfreegpumemory")

}
