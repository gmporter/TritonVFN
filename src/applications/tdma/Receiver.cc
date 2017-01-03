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

void timespec_add_ns(struct timespec *t, unsigned long long ns);

Receiver::Receiver(MyricomSNFNetworkInterface * rboard,
                   MyricomSNFNetworkInterface * sboard,
                   int coreToRunOn)
    : NetBump(rboard, sboard, coreToRunOn),
      lastseq(0), lasttime(0), insteadystate(false),
      initializing(true), nextReply(0)
{
    reply.data = new uint8_t[1500];
    memset(reply.data, 0, 1500);
    reply.length = 1500;
}

Receiver::~Receiver()
{ }

void Receiver::work()
{
    std::list<uint64_t> sleepTimes;
    uint64_t numpkts = 0;
    Packet p;

    while (!shouldShutDown) {
        recvPacket(p);

        if (p.length != 0) {
            numpkts++;
            incomingPacket(p);
        }

        if (!initializing) {
            while (!shouldShutDown) {
                // send some data
                for (int i = 0; i < 200; i++) {
                    memcpy(reply.data, &nextReply, sizeof(uint64_t));
                    nextReply++;
                    sendPacket(reply);
                }

                timespec_add_ns(&daystarttimer, 1000000);
                clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &daystarttimer, NULL);
            }
        }
    }

    fprintf(stderr, "Received: %jd packets\n", numpkts);
    fprintf(stderr, "Sent: %jd packets\n", nextReply-1);
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
        lastseq = seqNum;
        lasttime = (p.timestamp / 1000);
        initializing = false;
        if (clock_gettime(CLOCK_REALTIME, &daystarttimer)) {
            perror("clock_gettime");
        }
    }
}

void timespec_add_ns(struct timespec *t, unsigned long long ns)
{
    int tmp = ns / 1000000000;
    t->tv_sec += tmp;
    ns -= tmp * 1000000000;
    t->tv_nsec += ns;

    tmp = t->tv_nsec / 1000000000;
    t->tv_sec += tmp;
    t->tv_nsec -= tmp * 1000000000;
}
