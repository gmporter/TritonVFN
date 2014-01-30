#ifndef LATENCYMETER_HH
#define LATENCYMETER_HH

#include <vector>
#include "core/NetBump.hh"

class LatencyMeter : public NetBump {
public:
    LatencyMeter(MyricomSNFNetworkInterface * recvInterface,
              int coreToRunOn, uint64_t _expectedNumPackets);
    virtual ~LatencyMeter();

    virtual void work();

    inline void gracefulShutdown() {
        shouldShutDown = true;
    }

private:
    volatile bool shouldShutDown; 
    const uint64_t expectedNumPackets;
    uint64_t numPktsReceived, totalNumBytes;
    std::vector<uint64_t> sentTimes, receivedTimes;

    void recordIncomingPacket(Packet p);
};

#endif /* LATENCYMETER_HH */
