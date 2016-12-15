include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_library(pingpong_lib PingPong.cc)
add_executable(pingpong main.cpp)
target_link_libraries(pingpong pingpong_lib core snf gflags pthread)

add_executable(pingpong-tests pingpong-tests.cpp) 
target_link_libraries(pingpong-tests pingpong_lib core gtest gtest_main) 
