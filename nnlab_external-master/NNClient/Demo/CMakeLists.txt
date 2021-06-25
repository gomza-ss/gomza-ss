set(Target Demo)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/HumanPoseEstimation)

file(GLOB_RECURSE SOURCES src/*.cpp include/*.h)

add_library(${Target} ${SOURCES})

target_include_directories(${Target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${Target} NNClient human_pose_estimation)
