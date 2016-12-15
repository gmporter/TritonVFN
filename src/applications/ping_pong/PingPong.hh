#ifndef PINGPONG_HH
#define PINGPONG_HH

#include <string>
#include "core/NetBump.hh"

class PingPong : public NetBump {
public:
    PingPong(MyricomSNFNetworkInterface * sendInterface,
                 int coreToRunOn, std::string srcMACstr,
                 uint64_t _packetSize, uint64_t _epochTime);
    virtual ~PingPong();

    virtual void work();

    static const uint8_t PINGTYPE = 0;
    static const uint8_t PONGTYPE = 1;

    static void initPing(uint8_t * packet, uint64_t seqNum);
    static bool isPing(uint8_t * packet);
    static uint64_t getPingSeqNum(uint8_t * packet);
    static void initPong(uint8_t * packet, uint64_t ackNum);
    static bool isPong(uint8_t * packet);
    static uint64_t getPongAckNum(uint8_t * packet);

    inline void gracefulShutdown() {
        shouldShutDown = true;
    }

private:
    const uint64_t packetSize, epochTime;
    uint8_t srcmac[6];

    void randomize(uint8_t * buf, uint64_t len);
    void randomIP(char * destbuf, int buflen);
    void randomPort(uint16_t * port);

    static const int PayloadOffset = 42;

    std::list<uint64_t> ackdPings;
};

#endif /* PINGPONG_HH */
