#pragma once

#define NN_CLIENT_CONCATENATE_IMPL(s1, s2) s1##s2
#define NN_CLIENT_CONCATENATE(s1, s2) NN_CLIENT_CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#define NN_CLIENT_ANONYMOUS_VARIABLE(str) NN_CLIENT_CONCATENATE(str, __COUNTER__)
#else
#define NN_CLIENT_ANONYMOUS_VARIABLE(str) NN_CLIENT_CONCATENATE(str, __LINE__)
#endif

#ifdef _MSC_VER
#define NN_CLIENT_UNUSED_VAR
#else
#define NN_CLIENT_UNUSED_VAR __attribute__((__unused__))
#endif

#define NN_CLIENT_DISABLE_COPY_AND_MOVE(Class) \
    Class(const Class&) = delete;              \
    Class(Class&&) = delete;                   \
    Class& operator=(const Class&) = delete;   \
    Class& operator=(Class&&) = delete;

#define NN_CLIENT_INTEL std::string("INTEL")
#define NN_CLIENT_NVIDIA std::string("NVIDIA")

#define NN_CLIENT_EXEC_PATH_PARAM_NAME "executable_path"
#define NN_CLIENT_EXEC_PATH_TO_SERVER_PARAM_NAME "executable_path_to_server"

#define NN_CLIENT_DEVICE_VENDOR_PARAM_NAME "device_vendor"
#define NN_CLIENT_DEVICE_PARAM_NAME "device"

#define NN_CLIENT_REFERENCE_PATH_PARAM_NAME "reference_path"
