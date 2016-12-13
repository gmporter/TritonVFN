include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(loss-detector main.cpp LossDetector.cc)

target_link_libraries(loss-detector core snf gflags pthread)
