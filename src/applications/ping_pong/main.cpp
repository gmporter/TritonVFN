#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <gflags/gflags.h>

#include "PingPong.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to generate packets from");
DEFINE_uint64(packetSizeInBytes, 64, "Packet size (in bytes)");
DEFINE_uint64(workerCPU, 4, "CPU to run packet generator thread on");
DEFINE_string(srcMACstr, "00:11:22:33:44:55", "Source MAC address");
DEFINE_uint64(epochTime, 20, "Epoch length (in us)");

PingPong * pingpong;

void cntl_c_handler(int s) {
    fprintf(stderr, "Trying to stop...\n");
    pingpong->gracefulShutdown();
}

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal(SIGINT, cntl_c_handler);

  fprintf(stderr, "Using board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * board = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  pingpong = new PingPong(board, FLAGS_workerCPU, FLAGS_srcMACstr,
                                 FLAGS_packetSizeInBytes, FLAGS_epochTime);

  pingpong->start();
  while (!pingpong->workerIsDone()) { } 
  pingpong->stop();

  delete pingpong;
  delete board;

  return 0;
}
