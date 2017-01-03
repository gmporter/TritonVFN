cmake_minimum_required(VERSION 2.6)
PROJECT(TritonVFN)
###########################################################################
# GTest - http://code.google.com/p/googletest/
###########################################################################
add_subdirectory(gtest-1.6.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
enable_testing(true)
###########################################################################
# TritonVFN (header file only library)
###########################################################################

set(CMAKE_CXX_FLAGS "-DNDEBUG -std=c++11")

include_directories(${TritonVFN_SOURCE_DIR})
add_subdirectory(core)
add_subdirectory(applications/delay_forwarder)
add_subdirectory(applications/pkt_gen)
add_subdirectory(applications/incoming_rate_meter)
add_subdirectory(applications/latency_meter)
add_subdirectory(applications/loss_detector)
add_subdirectory(applications/ping_pong)
add_subdirectory(applications/pps_calc)
add_subdirectory(applications/timer_latency)
add_subdirectory(applications/vlb_ping_pong)
add_subdirectory(applications/pkt_gen_mod)
add_subdirectory(applications/tdma)
