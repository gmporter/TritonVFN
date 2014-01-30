#ifndef DELAYFORWARDER_HH
#define DELAYFORWARDER_HH

#include <list>
#include "core/NetBump.hh"
#include "core/Utils.hh"

class DelayForwarder : public NetBump {
public:
    DelayForwarder(MyricomSNFNetworkInterface * recvInterface,
                   MyricomSNFNetworkInterface * sendInterface,
                   int cpuToRunOn,
                   uint64_t delayInMicroseconds);
    virtual ~DelayForwarder();

    virtual void work();

private:
    const uint64_t delayInMicros;
    std::list<Packet *> freelist, sendlist;

    inline bool packetIsReadyToSend(Packet * p) const {
        if ( (p->timestamp + (delayInMicros * 1000)) <=
             (Utils::utime() * 1000) ) {
            return true;
        } else {
            return false;
        }
    }
};

#endif /* DELAYFORWARDER_HH */
