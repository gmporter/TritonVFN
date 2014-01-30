include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(delay-forwarder main.cpp DelayForwarder.cc)

target_link_libraries(delay-forwarder core snf gflags)
