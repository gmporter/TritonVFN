#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <netinet/ether.h>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "SimpleReceiver.hh"

SimpleReceiver::SimpleReceiver(MyricomSNFNetworkInterface * intf, int coreToRunOn)
    : NetBump(intf, intf, coreToRunOn),
    lastseq(0), numgaps(0), pktsMissed(0)
{
    progStartTime = Utils::utime();
}

SimpleReceiver::~SimpleReceiver()
{ }

void SimpleReceiver::work()
{
    uint64_t numpkts = 0;
    Packet p;

    while (!shouldShutDown) {
        recvPacket(p);
        if (p.length != 0) {
            numpkts++;
            incomingPacket(p);
        }
    }

    uint64_t now = Utils::utime();

    fprintf(stderr, "Received: %jd packets\n", numpkts);
    fprintf(stderr, "Observed: %jd gaps\n", numgaps);
    fprintf(stderr, "Observed: %jd missing packets\n", pktsMissed);
    fprintf(stderr, "Ran for %jd microseconds (%.2g seconds)\n",
        now - progStartTime, (now - progStartTime) / 1000000.0);

#if 0
    while (!seqNums.empty()) {
        uint64_t seq = seqNums.front();
        seqNums.pop_front();

        printf("%jd\n", seq);
    }
#endif
}

void SimpleReceiver::incomingPacket(Packet p)
{
    uint64_t seqNum;
    memcpy(&seqNum, (uint8_t *) p.data, sizeof(uint64_t));

    if (lastseq == 0) {
        lastseq = seqNum;
    } else if (lastseq + 1 != seqNum) {
        numgaps++;
        pktsMissed += (seqNum - lastseq);
        lastseq = seqNum;
    } else {
        lastseq = seqNum;
    }

#if 0
    seqNums.push_back(seqNum);
#endif
}
