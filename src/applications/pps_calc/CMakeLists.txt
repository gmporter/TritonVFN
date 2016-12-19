include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(pps-calc main.cpp PpsCalc.cc)

target_link_libraries(pps-calc core snf gflags pthread)
