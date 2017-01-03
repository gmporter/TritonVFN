#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <gflags/gflags.h>

#include "Sender.hh"
#include "SimpleReceiver.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to generate packets from");
DEFINE_uint64(workerSendCPU, 4, "CPU to run packet generator thread on");
DEFINE_uint64(workerReceiveCPU, 10, "CPU to run packet receiver thread on");

volatile bool shouldStop = false;

void cntl_c_handler(int s) {
    shouldStop = true;
    fprintf(stderr, "Trying to stop...\n");
}

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal (SIGINT, cntl_c_handler);

  printf("Using board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * board = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  Sender * sender = new Sender(board, FLAGS_workerSendCPU);
  SimpleReceiver * receiver = new SimpleReceiver(board, FLAGS_workerReceiveCPU);

  sender->start();
  receiver->start();
  while (!shouldStop && !sender->workerIsDone() && !receiver->workerIsDone()) { } 
  sender->stop();
  receiver->stop();

  delete sender;
  delete receiver;
  delete board;

  return 0;
}
