include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(timer-test main.cpp TimerTest.cc)

target_link_libraries(timer-test core snf gflags pthread)
