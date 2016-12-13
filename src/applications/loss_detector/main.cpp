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
#include "LossDetector.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to generate packets from");
DEFINE_uint64(workerCPU, 6, "CPU to run rate meter thread on");

bool shouldStop = false;

void cntl_c_handler(int s){
    shouldStop = true;
    fprintf(stderr, "Trying to stop...\n");
}

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal (SIGINT, cntl_c_handler);

  printf("listening on board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * board = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  LossDetector * lossDetect = new LossDetector(board, FLAGS_workerCPU);
    
  lossDetect->start();
  while (!shouldStop && !lossDetect->workerIsDone()) { }
  lossDetect->stop();

  delete lossDetect;
  delete board;

  return 0;
}
