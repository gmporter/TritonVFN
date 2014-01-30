#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <gflags/gflags.h>

#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Packet.hh"
#include "core/Utils.hh"
#include "LatencyMeter.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to receive packets from");
DEFINE_uint64(expectedNumPackets, UINT64_MAX, "Number of packets to expect");
DEFINE_uint64(workerCPU, 6, "CPU to run rate meter thread on");

LatencyMeter * latencyMeter;

void cntl_c_handler(int s){
    fprintf(stderr, "Trying to stop...\n");
    latencyMeter->gracefulShutdown();
}

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal (SIGINT, cntl_c_handler);

  fprintf(stderr, "listening on board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * board = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  latencyMeter = new LatencyMeter(board, FLAGS_workerCPU,
                                        FLAGS_expectedNumPackets);
    
  latencyMeter->start();
  while (!latencyMeter->workerIsDone()) { }
  latencyMeter->stop();

  delete latencyMeter;
  delete board;

  return 0;
}
