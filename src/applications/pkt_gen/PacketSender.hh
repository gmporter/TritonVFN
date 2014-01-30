#ifndef PACKETSENDER_HH
#define PACKETSENDER_HH

#include "core/NetBump.hh"

class PacketSender : public NetBump {
public:
    PacketSender(MyricomSNFNetworkInterface * sendInterface,
                 int coreToRunOn, uint64_t _numPacketsToGenerate,
                 uint64_t _packetSize);
    virtual ~PacketSender();

    virtual void work();
private:
    const uint64_t numPacketsToGenerate, packetSize;

    void randomize(uint8_t * buf, uint64_t len);
    void randomIP(char * destbuf, int buflen);
    void randomPort(uint16_t * port);
};

#endif /* PACKETSENDER_HH */
