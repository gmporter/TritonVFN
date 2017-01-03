#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <netinet/ether.h>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "Receiver.hh"

Receiver::Receiver(MyricomSNFNetworkInterface * rboard,
                   MyricomSNFNetworkInterface * sboard,
                   int coreToRunOn)
    : NetBump(rboard, sboard, coreToRunOn),
      lastseq(0), lasttime(0), insteadystate(false),
      nextReply(0)
{
    reply.data = new uint8_t[1500];
    memset(reply.data, 0, 1500);
    reply.length = 1500;
}

Receiver::~Receiver()
{ }

void Receiver::work()
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

    fprintf(stderr, "Received: %jd packets\n", numpkts);

#if 0
    uint64_t basis = 0;
    while (times.size() >= 2) {
        uint64_t dayend = times.front();
        times.pop_front();

        uint64_t daystart = times.front();
        times.pop_front();

        if (basis == 0)  {
            basis = dayend;
        }

        printf("day ended at %jd, started at %jd\n",
            dayend - basis, daystart - basis);
    }
#endif
}

void Receiver::incomingPacket(Packet p)
{
    uint64_t seqNum;
    memcpy(&seqNum, (uint8_t *) p.data, sizeof(uint64_t));

    if (lastseq == 0) {
        // this is the first packet we've seen
        lastseq = seqNum;
        lasttime = (p.timestamp / 1000);

    } else if (seqNum == (lastseq + 1)) {
        // just got an in-order packet
        lastseq = seqNum;
        lasttime = (p.timestamp / 1000);

    } else if (!insteadystate) {
        // out of order packet, program just started
        lastseq = seqNum;
        lasttime = (p.timestamp / 1000);
        insteadystate = true;

    } else {
        // out of order packet, in steady state
#if 0
        times.push_back(lasttime);
        times.push_back(p.timestamp / 1000);
#endif

        lastseq = seqNum;
        lasttime = (p.timestamp / 1000);

        // send some data
        for (int i = 0; i < 250; i++) {
            memcpy(reply.data, &nextReply, sizeof(uint64_t));
            nextReply++;
            sendPacket(reply);
        }
    }
}
