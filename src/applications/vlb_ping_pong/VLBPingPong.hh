#ifndef VLBPINGPONG_HH
#define VLBPINGPONG_HH

#include <string>
#include "core/NetBump.hh"

class VLBPingPong : public NetBump {
public:
    VLBPingPong(MyricomSNFNetworkInterface * iface,
                int coreToRunOn, std::string srcMACstr,
                uint64_t _packetSize, uint64_t _numAcksNeeded);
    virtual ~VLBPingPong();

    virtual void work();

    static const uint8_t PINGTYPE = 7;
    static const uint8_t PONGTYPE = 8;

    static void initPingPong(uint8_t * packet, bool hasPing, uint64_t seqNum, bool hasPong, uint64_t ackNum);
    static bool hasPing(uint8_t * packet);
    static bool hasPong(uint8_t * packet);
    static uint64_t getPingSeqNum(uint8_t * packet);
    static uint64_t getPongAckNum(uint8_t * packet);

    inline void gracefulShutdown() {
        shouldShutDown = true;
    }

private:
    const uint64_t packetSize;
    const uint64_t numAcksNeeded;
    uint8_t srcmac[6];

    void randomize(uint8_t * buf, uint64_t len);
    void randomIP(char * destbuf, int buflen);
    void randomPort(uint16_t * port);

    static const int PayloadOffset = 42;

    std::vector<uint64_t> sentAcks;
    std::vector<uint64_t> ackdPings;
};

#endif /* VLBPINGPONG_HH */
