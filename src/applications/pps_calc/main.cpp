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
#include "PpsCalc.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to generate packets from");
DEFINE_uint64(workerCPU, 6, "CPU to run rate meter thread on");

volatile bool shouldStop = false;

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

  PpsCalc * ppsCalc = new PpsCalc(board, FLAGS_workerCPU);

  ppsCalc->start();
  while (!shouldStop && !ppsCalc->workerIsDone()) { }
  ppsCalc->stop();

  delete ppsCalc;
  delete board;

  return 0;
}
