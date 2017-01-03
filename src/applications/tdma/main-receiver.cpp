#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/prctl.h>

#include <gflags/gflags.h>

#include "Receiver.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"

DEFINE_uint64(boardNum, 0, "Myricom board to use");
DEFINE_uint64(workerCPU, 4, "CPU to run packet receiver thread on");
DEFINE_uint64(burstSize, 200, "Packets to send during a given day");

volatile bool shouldStop = false;

void cntl_c_handler(int s)
{
    shouldStop = true;
    fprintf(stderr, "Trying to stop...\n");
}

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal(SIGINT, cntl_c_handler);

  if (prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0) == -1) {
    perror("prctl");
    return 1;
  }

  fprintf(stderr, "Using board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * rboard = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  MyricomSNFNetworkInterface * sboard = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  Receiver * receiver = new Receiver(rboard, sboard, FLAGS_workerCPU);

  receiver->start();
  while (!shouldStop && !receiver->workerIsDone()) { } 
  receiver->stop();

  // should delete receiver, sboard, and rboard, but that
  // causes a crash for some reason

  return 0;
}
