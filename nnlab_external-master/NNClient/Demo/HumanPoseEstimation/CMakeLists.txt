file(GLOB SOURCES src/*.cpp include/*.hpp)
set(Target human_pose_estimation)

add_library(${Target} ${SOURCES})

set(TARGET_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/include
)
set(TARGET_LIBRARIES
    ${OpenCV_LIBS}
)

target_include_directories(${Target} SYSTEM PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${Target} PUBLIC ${TARGET_INCLUDES})

target_link_libraries(${Target} ${TARGET_LIBRARIES})

if (WITH_OPENVINO)
    add_executable(human_pose_estimation_demo main.cpp)
    target_link_libraries(human_pose_estimation_demo human_pose_estimation OpenVINONetManager)
endif()

if (WITH_CAFFE2)
    add_executable(human_pose_estimation_demo_simplepose main_simplepose.cpp)
    target_link_libraries(human_pose_estimation_demo_simplepose human_pose_estimation Caffe2NetManager)
endif()

if (WITH_CAFFE2)
    add_subdirectory(Caffe2)
endif()