#ifndef RECEIVER_HH
#define RECEIVER_HH

#include <string>
#include <list>
#include "core/NetBump.hh"

class Receiver : public NetBump {
public:
    Receiver(MyricomSNFNetworkInterface * rboard,
             MyricomSNFNetworkInterface * sboard,
             int coreToRunOn);
    virtual ~Receiver();

    virtual void work();

protected:

    void incomingPacket(Packet p);

    uint64_t lastseq, lasttime;
    bool insteadystate, initializing;

    std::list<uint64_t> times;

    uint64_t nextReply;
    Packet reply;

    struct timespec daystarttimer;
};

#endif /* RECEIVER_HH */
