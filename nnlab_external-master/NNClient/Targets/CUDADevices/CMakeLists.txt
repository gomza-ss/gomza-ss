set(Target CUDADevices)
file(GLOB SOURCES CUDADevices.cpp)
add_executable(${Target} ${SOURCES})
target_link_libraries(${Target} Demo)
