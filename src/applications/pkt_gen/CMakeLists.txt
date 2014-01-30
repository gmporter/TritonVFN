include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(pkt-gen main.cpp PacketSender.cc)

target_link_libraries(pkt-gen core snf gflags)
