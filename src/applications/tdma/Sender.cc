#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "Sender.hh"

Sender::Sender(MyricomSNFNetworkInterface * sendInterface,
                           int coreToRunOn)
    : NetBump(/* inInterface */ NULL, sendInterface, coreToRunOn)
{
}

Sender::~Sender()
{ }

void Sender::work()
{
    const int packetSize = 1500;
    Packet p;

    uint8_t * frame = new uint8_t[packetSize];
    memset(frame, 0, packetSize);

    p.data = frame;
    p.length = packetSize;

    uint64_t seqNum = 0;
    while (!shouldShutDown) {
        memcpy(p.data, &seqNum, sizeof(uint64_t));
        sendPacket(p);
        seqNum++;
    }
}

// sleep for specified number of microseconds
// true means the sleep was successful,
// false means the sleep was interrupted
bool Sender::usleep(uint64_t us)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = us * 1000;

    return(!clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, 0));
}
