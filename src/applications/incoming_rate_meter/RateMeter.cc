#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "RateMeter.hh"

RateMeter::RateMeter(MyricomSNFNetworkInterface * recvInterface,
                     int coreToRunOn, uint64_t _expectedNumPackets)
    : NetBump(recvInterface, /* outInterface */ NULL, coreToRunOn),
      expectedNumPackets(_expectedNumPackets)
{ }

RateMeter::~RateMeter()
{ }

void RateMeter::work()
{
    Packet p;
    uint64_t numPktsReceived = 0;
    std::list<uint64_t> sentTimes, receivedTimes;

    do { // wait till we get the first packet
        recvPacket(p);
    } while (p.length == 0);

    numPktsReceived++;
    sentTimes.push_back( (uint64_t) p.data );  // read sent time out of packet
    receivedTimes.push_back(p.timestamp);

    uint64_t start = Utils::utime();
    while (numPktsReceived < expectedNumPackets) {
        recvPacket(p);
        if (p.length > 0) {
            numPktsReceived++;
            printf("got %jd packets\n", numPktsReceived);
            uint64_t sentTime;
            memcpy(&sentTime, (uint8_t *)p.data + 14 + 40, sizeof(uint64_t));
            sentTimes.push_back(sentTime);
            receivedTimes.push_back(p.timestamp);
        }
    }
    uint64_t end = Utils::utime();

    sleep(3);

    MyricomSNFNetworkInterfaceStats stats;
    getInInterfaceStatistics(stats);

    printf("Num packets (expected/actual): %jd/%jd",
            numPktsReceived, stats.ring_pkt_recv);
    if (numPktsReceived == stats.ring_pkt_recv) {
        printf(" OK!\n");
    } else {
        printf(" MISMATCH!\n");
    }
    printf("utime() says receive burst lasted %jd microseconds\n", end-start);
    printf("actual num bytes received: %jd\n", stats.nic_bytes_recv);

    assert(sentTimes.size() == receivedTimes.size());

    while (!sentTimes.empty()) {
        uint64_t sentTime = sentTimes.front();
        sentTimes.pop_front();
        uint64_t receivedTime = receivedTimes.front();
        receivedTimes.pop_front();

        printf("sent:%jd \treceived:%jd\n", sentTime, receivedTime);
    }
}
