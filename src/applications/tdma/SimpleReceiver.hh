#ifndef SIMPLERECEIVER_HH
#define SIMPLERECEIVER_HH

#include <string>
#include <list>
#include "core/NetBump.hh"

class SimpleReceiver : public NetBump {
public:
    SimpleReceiver(MyricomSNFNetworkInterface * sendInterface, int coreToRunOn);
    virtual ~SimpleReceiver();

    virtual void work();

protected:

    void incomingPacket(Packet p);

    uint64_t lastseq, numgaps, pktsMissed;
    uint64_t progStartTime;

#if 0
    std::list<uint64_t> seqNums;
#endif
};

#endif /* SIMPLERECEIVER_HH */
