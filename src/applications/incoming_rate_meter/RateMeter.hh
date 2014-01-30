#ifndef RATEMETER_HH
#define RATEMETER_HH

#include "core/NetBump.hh"

class RateMeter : public NetBump {
public:
    RateMeter(MyricomSNFNetworkInterface * recvInterface,
              int coreToRunOn, uint64_t _expectedNumPackets);
    virtual ~RateMeter();

    virtual void work();

private:
    const uint64_t expectedNumPackets;
    std::list<uint64_t> sentTimes, receivedTimes;
};

#endif /* RATEMETER_HH */
