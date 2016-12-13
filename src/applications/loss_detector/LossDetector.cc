#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "LossDetector.hh"

LossDetector::LossDetector(MyricomSNFNetworkInterface * recvInterface,
                                 int coreToRunOn)
    : NetBump(recvInterface, /* outInterface */ NULL, coreToRunOn)
{ }

LossDetector::~LossDetector()
{ }

void LossDetector::work()
{
    while (!shouldShutDown) {
        Packet p;

        /* keep trying to get a packet */
        recvPacket(p);
        if (p.length == 0) {
            continue;
        }

        // skip 42 bytes, which represent the ethernet, IP, and UDP headers
        uint64_t * ptr = (uint64_t *) (((uint8_t *) p.data)+42);
        sentTimes.push_back( *ptr );          // sent time
        sentPacketNums.push_back( *(ptr+1) ); // pkt number
        recvTimes.push_back( Utils::utime() );
    }
    
    assert(sentTimes.size() == sentPacketNums.size());

    while (!sentTimes.empty()) {
        uint64_t sentTime = sentTimes.front();
        sentTimes.pop_front();
        uint64_t pktNum = sentPacketNums.front();
        sentPacketNums.pop_front();
        uint64_t recvTime = recvTimes.front();
        recvTimes.pop_front();

        printf("pkt_num %jd sent_time %jd recv_time %jd\n",
               pktNum, sentTime, recvTime);
    }
}
