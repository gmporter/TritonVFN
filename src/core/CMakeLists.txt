include_directories("/opt/snf/include")
#include_directories ("${CMAKE_CURRENT_SOURCE_DIR}"
#                     "${CMAKE_CURRENT_SOURCE_DIR}/myricom")

FILE(GLOB core_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
                     "${CMAKE_CURRENT_SOURCE_DIR}/myricom/*.cc")
link_directories("/opt/snf/lib")

add_library(core ${core_FILES})
target_link_libraries(core snf)

add_executable(core-tests unit-tests.cpp) 
target_link_libraries(core-tests core gtest gtest_main) 
add_test(core-tests core-tests)
