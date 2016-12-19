include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_library(vlbpingpong_lib VLBPingPong.cc)
add_executable(vlbpingpong main.cpp)
target_link_libraries(vlbpingpong vlbpingpong_lib core snf gflags pthread)
