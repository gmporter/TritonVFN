#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <gflags/gflags.h>

#include "PacketSender.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"

DEFINE_uint64(boardNum, 0, "Myricom board number to generate packets from");
DEFINE_uint64(numPackets, 1000, "Number of packets to generate");
DEFINE_uint64(packetSizeInBytes, 64, "Packet size (in bytes)");
DEFINE_uint64(workerCPU, 4, "CPU to run packet generator thread on");

int 
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  printf("Sending on board %jd\n", FLAGS_boardNum);
  MyricomSNFNetworkInterface * board = \
    new MyricomSNFNetworkInterface(FLAGS_boardNum);

  PacketSender * sender = new PacketSender(board, FLAGS_workerCPU,
                                           FLAGS_numPackets,
                                           FLAGS_packetSizeInBytes);

  sender->start();
  while (!sender->workerIsDone()) { } 
  sender->stop();

  delete sender;
  delete board;

  return 0;
}
