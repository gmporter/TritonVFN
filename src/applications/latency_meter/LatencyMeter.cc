#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "LatencyMeter.hh"

LatencyMeter::LatencyMeter(MyricomSNFNetworkInterface * recvInterface,
                     int coreToRunOn, uint64_t _expectedNumPackets)
    : NetBump(recvInterface, /* outInterface */ NULL, coreToRunOn),
      expectedNumPackets(_expectedNumPackets)
{
    shouldShutDown = false;
    sentTimes.reserve(_expectedNumPackets);
    receivedTimes.reserve(_expectedNumPackets);
    numPktsReceived = 0;
    totalNumBytes = 0;
}

LatencyMeter::~LatencyMeter()
{ }

void LatencyMeter::recordIncomingPacket(Packet p)
{
    // record the time the packet was originally sent
    uint64_t sentTime;
    memcpy(&sentTime, (uint8_t *)p.data+42, sizeof(uint64_t));
    sentTimes[numPktsReceived] = sentTime;

    // record the time the packet actually got to us
    receivedTimes[numPktsReceived] = p.timestamp;

    totalNumBytes += p.length;
    numPktsReceived++;
}

void LatencyMeter::work()
{
    Packet p;
    uint64_t startTime = 0, endTime = 0;

    while (!shouldShutDown && (numPktsReceived < expectedNumPackets)) {
        recvPacket(p);
        if (p.length > 0) {
            if (startTime == 0) {
                startTime = Utils::utime();
            }
            recordIncomingPacket(p);
        }
    }
    endTime = Utils::utime();

    for (uint64_t i = 0; i < numPktsReceived; i++) {
        printf("%jd\n", (receivedTimes[i]/1000 - sentTimes[i]));
    }

    fprintf(stderr, "Received %jd / %jd packets (rate = %g)\n",
            numPktsReceived, expectedNumPackets,
            (numPktsReceived/expectedNumPackets*1.0));
    fprintf(stderr, "Received %jd bytes in %jd microseconds\n",
            totalNumBytes, (endTime - startTime));

    return;
}
