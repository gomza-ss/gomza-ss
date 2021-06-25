set(Target DeviceDetect)
file(GLOB SOURCES DeviceDetect.cpp)
add_executable(${Target} ${SOURCES})
target_link_libraries(${Target} Demo)
