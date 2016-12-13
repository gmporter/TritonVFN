#ifndef LOSSDETECTOR_HH
#define LOSSDETECTOR_HH

#include "core/NetBump.hh"

class LossDetector : public NetBump {
public:
    LossDetector(MyricomSNFNetworkInterface * recvInterface, int coreToRunOn);
    virtual ~LossDetector();

    virtual void work();

private:
    std::list<uint64_t> sentTimes, sentPacketNums, recvTimes;
};

#endif /* LOSSDETECTOR_HH */
