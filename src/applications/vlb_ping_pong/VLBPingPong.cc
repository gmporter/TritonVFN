#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "VLBPingPong.hh"

VLBPingPong::VLBPingPong(MyricomSNFNetworkInterface * iface,
                           int coreToRunOn, std::string srcMACstr,
                           uint64_t _packetSize, uint64_t _numAcksNeeded)
    : NetBump(iface, iface, coreToRunOn), packetSize(_packetSize),
      numAcksNeeded(_numAcksNeeded)
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

    ackdPings.reserve(_numAcksNeeded + 10);
}

VLBPingPong::~VLBPingPong()
{ }

void VLBPingPong::work()
{
    Packet ping_pong_pkt, in_pkt;

    uint8_t * pingpongpacket = new uint8_t[packetSize];

    uint8_t dstmac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    char srcip[128], dstip[128];
    uint16_t srcport, dstport;

    strncpy(dstip, "255.255.255.255", strlen("255.255.255.255")+1);

    randomIP(srcip, 128);
    randomPort(&srcport);
    randomPort(&dstport);

    // create the ping packet
    Utils::createUDPPacket(pingpongpacket, packetSize-(14+20+8),
                           srcmac, dstmac, srcip, dstip,
                           srcport, dstport);
    ping_pong_pkt.data = pingpongpacket;
    ping_pong_pkt.length = packetSize;

    uint64_t pingNum = 0;
    bool shouldPong = false;
    uint64_t pongNum = 0;
    uint64_t pongsReceived = 0;
    while (!shouldShutDown) {
        recvPacket(in_pkt);
        if (in_pkt.length > 0) {
            uint8_t * in_pkt_ptr = (uint8_t *) in_pkt.data;

            if (shouldPong = hasPing(in_pkt_ptr))
                pongNum = getPingSeqNum(in_pkt_ptr);

            if (hasPong(in_pkt_ptr)) {
                ackdPings.push_back(getPongAckNum(in_pkt_ptr));
                pongsReceived++;
                if(pongsReceived >= numAcksNeeded)
                    shouldShutDown = true;
            }
        }
        else
            shouldPong = false;

        initPingPong((uint8_t *)ping_pong_pkt.data, true, pingNum, shouldPong, pongNum);
        sendPacket(ping_pong_pkt);
        pingNum++;
    }

    recvPacket(in_pkt);
    while (in_pkt.length > 0) {
        uint8_t * in_pkt_ptr = (uint8_t *) in_pkt.data;
        if (shouldPong = hasPing(in_pkt_ptr))
            pongNum = getPingSeqNum(in_pkt_ptr);

        if (hasPong(in_pkt_ptr))
            ackdPings.push_back(getPongAckNum(in_pkt_ptr));

        if(shouldPong){
            initPingPong((uint8_t *)ping_pong_pkt.data, false, 0, shouldPong, pongNum);
            sendPacket(ping_pong_pkt);
        }
        recvPacket(in_pkt);
    }

    for(std::vector<uint64_t>::const_iterator i = ackdPings.begin(); i != ackdPings.end(); ++i) {
        printf("%jd\n", *i);
    }
}

void VLBPingPong::randomize(uint8_t * buf, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        buf[i] = lrand48();
    }
}

void VLBPingPong::randomIP(char * dstbuf, int buflen) {
    snprintf(dstbuf, buflen, "%ld.%ld.%ld.%ld",
             lrand48() % 256, lrand48() % 256,
             lrand48() % 256, lrand48() % 256);
}

void VLBPingPong::randomPort(uint16_t * port) {
    *port = lrand48() && 0xFFFF;
}

void VLBPingPong::initPingPong(uint8_t * packet, bool hasPing, uint64_t seqNum, bool hasPong, uint64_t ackNum)
{
    assert(packet);

    uint8_t nullByte = 0;
    uint64_t nullVal = 0;

    uint8_t * typePtr = packet + PayloadOffset;
    uint8_t * seqPtr = typePtr + sizeof(uint8_t) + sizeof(uint64_t);

    if(hasPing){
        memcpy(typePtr, &PINGTYPE, sizeof(uint8_t));
        memcpy(seqPtr, &seqNum, sizeof(uint64_t));
    }
    else{
        memcpy(typePtr, &nullByte, sizeof(uint8_t));
        memcpy(seqPtr, &nullVal, sizeof(uint64_t));
    }

    typePtr = seqPtr + sizeof(uint64_t);
    seqPtr = typePtr + sizeof(uint8_t) + sizeof(uint64_t);

    if(hasPong){
        memcpy(typePtr, &PONGTYPE, sizeof(uint8_t));
        memcpy(seqPtr, &ackNum, sizeof(uint64_t));
    }
    else{
        memcpy(typePtr, &nullByte, sizeof(uint8_t));
        memcpy(seqPtr, &nullVal, sizeof(uint64_t));
    }
}

bool VLBPingPong::hasPing(uint8_t * packet)
{
    assert(packet);
    uint8_t pktType = (uint8_t) *(packet+PayloadOffset);
    return (pktType == PINGTYPE);
}

uint64_t VLBPingPong::getPingSeqNum(uint8_t * packet)
{
    assert(packet);
    uint64_t seqNum;

    uint8_t * seqPtr = packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t);

    memcpy(&seqNum, seqPtr, sizeof(uint64_t));
    return seqNum;
}

bool VLBPingPong::hasPong(uint8_t * packet)
{
    assert(packet);
    uint8_t pktType = (uint8_t) *(packet+PayloadOffset+sizeof(uint8_t)+sizeof(uint64_t)*2);
    return (pktType == PONGTYPE);
}

uint64_t VLBPingPong::getPongAckNum(uint8_t * packet)
{
    assert(packet);
    uint64_t ackNum;

    uint8_t * ackPtr = packet+PayloadOffset+sizeof(uint8_t)*2+sizeof(uint64_t)*3;

    memcpy(&ackNum, ackPtr, sizeof(uint64_t));
    return ackNum;
}
