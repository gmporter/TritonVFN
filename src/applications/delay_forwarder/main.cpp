#include <list>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Packet.hh"
#include "DelayForwarder.hh"

#include <gflags/gflags.h>

bool shouldStop = false;

void cntl_c_handler(int s){
    shouldStop = true;
    fprintf(stderr, "Trying to stop...\n");
}

DEFINE_uint64(fwdNetBumpCPU, 4, "CPU to run the b0->b1 NetBump on");
DEFINE_uint64(revNetBumpCPU, 6, "CPU to run the b1->b0 NetBump on");
DEFINE_uint64(delayInMicros, 100, "Delay in microseconds");

int 
main(int argc, char *argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    signal (SIGINT, cntl_c_handler);

    printf("opening board 0\n");
    MyricomSNFNetworkInterface * board0 = new MyricomSNFNetworkInterface(0);

    printf("opening board 1\n");
    MyricomSNFNetworkInterface * board1 = new MyricomSNFNetworkInterface(1);

    DelayForwarder * fwdDelayForwarder = \
        new DelayForwarder(board0, board1,
                           FLAGS_fwdNetBumpCPU, FLAGS_delayInMicros);

    DelayForwarder * revDelayForwarder = \
        new DelayForwarder(board1, board0,
                           FLAGS_revNetBumpCPU, FLAGS_delayInMicros);

    fwdDelayForwarder->start();
    revDelayForwarder->start();

    while (!fwdDelayForwarder->workerIsDone() && !shouldStop) { }
    while (!revDelayForwarder->workerIsDone() && !shouldStop) { }

    fwdDelayForwarder->stop();
    revDelayForwarder->stop();

    delete revDelayForwarder;
    delete fwdDelayForwarder;
    printf("closing board 1\n");
    delete board1;
    printf("closing board 0\n");
    delete board0;

    printf("Exiting delay_forwarder\n");
    return 0;
}
