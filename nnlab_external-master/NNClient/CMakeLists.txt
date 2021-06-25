cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/Demo/NMResource)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "CMAKE_BUILD_TYPE not defined, 'Release' will be used")
    set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(Qt5 REQUIRED Core)
find_package(OpenCV REQUIRED)
find_package(Boost 1.58 COMPONENTS filesystem)
find_package(log4cxx REQUIRED)

# NNClient's natural CMakeLists.txt location is freed for its users
add_subdirectory(NNClientCMake)
add_subdirectory(Demo)
add_subdirectory(Targets)
