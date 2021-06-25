file(GLOB SOURCES src/*.cpp include/*.hpp)
set(Target human_pose_estimation_mrcnn)

add_library(${Target} ${SOURCES})

set(TARGET_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/include
)
set(TARGET_LIBRARIES
    ${OpenCV_LIBS}
    Caffe2NetManager
    human_pose_estimation
)

target_include_directories(${Target} SYSTEM PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${Target} PUBLIC ${TARGET_INCLUDES})

target_link_libraries(${Target} ${TARGET_LIBRARIES})

add_executable(human_pose_estimation_demo_mrcnn main.cpp)
target_link_libraries(human_pose_estimation_demo_mrcnn human_pose_estimation_mrcnn)
