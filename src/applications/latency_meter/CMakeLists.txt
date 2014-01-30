include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(latency-meter main.cpp LatencyMeter.cc)

target_link_libraries(latency-meter core snf gflags)
