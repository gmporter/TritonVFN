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
    Packet p, response;
    bool shouldSend = true;

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
