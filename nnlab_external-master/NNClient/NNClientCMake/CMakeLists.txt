set(Target NNClient)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/NNClient/NNClient/*.h ${CMAKE_SOURCE_DIR}/NNClient/NNClient/*.cpp)

add_library(${Target} ${SOURCES})

if(MSVC)
    target_compile_options(${Target} PRIVATE /W4 /WX)
endif()

target_include_directories(${Target} PUBLIC ${CMAKE_SOURCE_DIR}/NNClient)
target_include_directories(${Target} SYSTEM PUBLIC ${Qt5Core_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${LOG4CXX_INCLUDE})
target_link_libraries(${Target} ${Qt5Core_LIBRARIES} ${OpenCV_LIBS} Boost::boost Boost::filesystem ${LOG4CXX_LIB})
