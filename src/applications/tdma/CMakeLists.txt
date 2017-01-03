include_directories("/opt/snf/include")
link_directories("${TRITONVFN_BINARY_DIR}/core"
                 "/opt/snf/lib")

add_executable(sender main-sender.cpp Sender.cc SimpleReceiver.cc)
add_executable(receiver main-receiver.cpp Receiver.cc)

target_link_libraries(sender core snf gflags pthread)
target_link_libraries(receiver core snf gflags pthread)
