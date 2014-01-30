include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(incomding-rate-meter main.cpp RateMeter.cc)

target_link_libraries(incomding-rate-meter core snf gflags)
