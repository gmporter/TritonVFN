#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <list>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "PingPong.hh"

PingPong::PingPong(MyricomSNFNetworkInterface * iface,
                           int coreToRunOn, std::string srcMACstr,
                           uint64_t _packetSize, uint64_t _epochtime)
    : NetBump(iface, iface, coreToRunOn), packetSize(_packetSize),
      epochTime(_epochtime)
{
    /* initialize the random number generator */
    srand48(Utils::utime());

    /* set our source mac address */
    int octets[6];
    int numconverted;
    numconverted = sscanf(srcMACstr.c_str(), "%x:%x:%x:%x:%x:%x",
                   &octets[0], &octets[1], &octets[2],
                   &octets[3], &octets[4], &octets[5]);
    assert(numconverted == 6);
    for (int i = 0; i < 6; i++) {
        srcmac[i] = (uint8_t) octets[i];
    }
}

PingPong::~PingPong()
{ }

void PingPong::work()
{
    Packet ping_pkt, pong_pkt, in_pkt;

    uint8_t * pingpacket = new uint8_t[packetSize];
    uint8_t * pongpacket = new uint8_t[packetSize];

    uint8_t dstmac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    char srcip[128], dstip[128];
    uint16_t srcport, dstport;

    strncpy(dstip, "255.255.255.255", strlen("255.255.255.255")+1);

    randomIP(srcip, 128);
    randomPort(&srcport);
    randomPort(&dstport);

    // create the ping packet
    Utils::createUDPPacket(pingpacket, packetSize-(14+20+8),
                           srcmac, dstmac, srcip, dstip,
                           srcport, dstport);
    initPing(pingpacket, -1);
    ping_pkt.data = pingpacket;
    ping_pkt.length = packetSize;

    // create the pong packet
    Utils::createUDPPacket(pongpacket, packetSize-(14+20+8),
                           srcmac, dstmac, srcip, dstip,
                           srcport, dstport);
    initPong(pongpacket, -1);
    pong_pkt.data = pongpacket;
    pong_pkt.length = packetSize;

    uint64_t epochNum = 0;
    while (!shouldShutDown) {
        initPing((uint8_t *)ping_pkt.data, epochNum);
        sendPacket(ping_pkt);
        bool gotAckInThisEpoch = false;

        uint64_t start = Utils::utime();
        while (Utils::utime() - start < epochTime) {
            recvPacket(in_pkt);
            if (in_pkt.length > 0) {
                uint8_t * in_pkt_ptr = (uint8_t *) in_pkt.data;
                if (isPing(in_pkt_ptr)) {
                    // is PING
                    uint64_t inEpoch = getPingSeqNum(in_pkt_ptr);
                    initPong((uint8_t *)pong_pkt.data, inEpoch);
                    sendPacket(pong_pkt);

                } else {
                    // is PONG
                    uint64_t pongEpoch = getPongAckNum(in_pkt_ptr);
                    printf("cur_epoch: %ld pong_epoch: %ld\n", epochNum, pongEpoch);
                    if (!gotAckInThisEpoch && (pongEpoch == epochNum)) {
                        ackdPings.push_back(pongEpoch);
                        gotAckInThisEpoch = true;
                    }
                }
            }
        }

        epochNum++;
        gotAckInThisEpoch = false;
    }

    while (!ackdPings.empty()) {
        uint64_t ack = ackdPings.front();
        ackdPings.pop_front();
        printf("%jd\n", ack);
    }
}

void PingPong::randomize(uint8_t * buf, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        buf[i] = lrand48();
    }
}

void PingPong::randomIP(char * dstbuf, int buflen) {
    snprintf(dstbuf, buflen, "%ld.%ld.%ld.%ld",
             lrand48() % 256, lrand48() % 256,
             lrand48() % 256, lrand48() % 256);
}

void PingPong::randomPort(uint16_t * port) {
    *port = lrand48() && 0xFFFF;
}

void PingPong::initPing(uint8_t * packet, uint64_t seqNum)
{
    assert(packet);

    uint8_t * typePtr = packet+PayloadOffset;
    uint8_t * seqPtr = packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t);

    memcpy(typePtr, &PINGTYPE, sizeof(uint8_t));
    memcpy(seqPtr, &seqNum, sizeof(uint64_t));
}

void PingPong::initPong(uint8_t * packet, uint64_t ackNum)
{
    assert(packet);

    uint8_t * typePtr = packet+PayloadOffset;
    uint8_t * ackPtr = packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t);

    memcpy(typePtr, &PONGTYPE, sizeof(uint8_t));
    memcpy(ackPtr, &ackNum, sizeof(uint64_t));
}

bool PingPong::isPing(uint8_t * packet)
{
    assert(packet);
    uint8_t pktType = (uint8_t) *(packet+PayloadOffset);
    return (pktType == PINGTYPE);
}

uint64_t PingPong::getPingSeqNum(uint8_t * packet)
{
    assert(packet);
    uint64_t seqNum;

    uint8_t * seqPtr = packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t);

    memcpy(&seqNum, seqPtr, sizeof(uint64_t));
    return seqNum;
}

bool PingPong::isPong(uint8_t * packet)
{
    assert(packet);
    uint8_t pktType = (uint8_t) *(packet+PayloadOffset);
    return (pktType == PONGTYPE);
}

uint64_t PingPong::getPongAckNum(uint8_t * packet)
{
    assert(packet);
    uint64_t ackNum;

    uint8_t * ackPtr = packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t);

    memcpy(&ackNum, ackPtr, sizeof(uint64_t));
    return ackNum;
}
