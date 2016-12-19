#ifndef PPSCALC_HH
#define PPSCALC_HH

#include "core/NetBump.hh"

class PpsCalc : public NetBump {
public:
    PpsCalc(MyricomSNFNetworkInterface * recvInterface, int coreToRunOn);
    virtual ~PpsCalc();

    virtual void work();

private:
    uint64_t before, now, bytes_received, pkts_received;
};

#endif /* PPSCALC_HH */
