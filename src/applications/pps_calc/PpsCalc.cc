#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "PpsCalc.hh"

#define SECOND 1000000000

PpsCalc::PpsCalc(MyricomSNFNetworkInterface * recvInterface,
                                 int coreToRunOn)
    : NetBump(recvInterface, /* outInterface */ NULL, coreToRunOn)
{ }

PpsCalc::~PpsCalc()
{ }

void PpsCalc::work()
{
    bytes_received = 0;
    before = Utils::ntime();
    now = before;
    while (!shouldShutDown) {
        Packet p;

        /* keep trying to get a packet */
        recvPacket(p);
        bytes_received += p.length;
        if(p.length > 0)
            pkts_received++;

        now = Utils::ntime();
        if(now - before >= SECOND){
            printf("%jd pps\t%jd bytes/sec\n", pkts_received, bytes_received);
            bytes_received = 0;
            pkts_received = 0;
            before = now;
        }
    }
}
