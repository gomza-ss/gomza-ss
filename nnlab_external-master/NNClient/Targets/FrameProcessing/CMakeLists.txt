set(Target FrameProcessing)
file(GLOB SOURCES FrameProcessing.cpp)
add_executable(${Target} ${SOURCES})
target_link_libraries(${Target} Demo)
