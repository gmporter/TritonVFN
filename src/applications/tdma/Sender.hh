#ifndef SENDER_HH
#define SENDER_HH

#include <string>
#include "core/NetBump.hh"

class Sender : public NetBump {
public:
    Sender(MyricomSNFNetworkInterface * sendInterface, int coreToRunOn);
    virtual ~Sender();

    virtual void work();
private:

    static bool usleep(uint64_t us);
};

#endif /* SENDER_HH */
