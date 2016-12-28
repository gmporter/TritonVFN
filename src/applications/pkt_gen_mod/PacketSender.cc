#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "PacketSender.hh"

PacketSender::PacketSender(MyricomSNFNetworkInterface * sendInterface,
                           int coreToRunOn, std::string srcMACstr,
                           uint64_t _numPacketsToGenerate,
                           uint64_t _packetSize)
    : NetBump(/* inInterface */ NULL, sendInterface, coreToRunOn),
      numPacketsToGenerate(_numPacketsToGenerate), packetSize(_packetSize)
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

PacketSender::~PacketSender()
{ }

void PacketSender::work()
{
    uint32_t payloadOffset;

    Packet * packets = new Packet[1];

    for (uint64_t i = 0; i < 1; i++) {
        uint8_t * packet = new uint8_t[packetSize];
        uint8_t dstmac[6] = {0x00, 0x60, 0xdd, 0x46, 0x82, 0x53};
        char srcip[128], dstip[128];
        uint16_t srcport, dstport;

        strncpy(dstip, "192.168.0.203", strlen("192.168.0.203")+1);

        randomIP(srcip, 128);
        // randomPort(&srcport);
        // randomPort(&dstport);
        srcport = 7777;
        dstport = 7777;

        payloadOffset = Utils::createUDPPacket(packet,
                                               packetSize-(14+20+8),
                                               srcmac, dstmac,
                                               srcip, dstip,
                                               srcport, dstport);
        packets[i].data = packet;
        packets[i].length = packetSize;
    }

    for (uint64_t i = 0; i < numPacketsToGenerate; i++) {

        uint8_t * packetdata = (uint8_t *) packets[0].data;

        // record the send time
        uint64_t sentTime = Utils::utime();
        memcpy(packetdata+payloadOffset, &sentTime, sizeof(uint64_t));

        // record the packet number
        memcpy(packetdata+payloadOffset+sizeof(uint64_t), &i, sizeof(uint64_t));

        sendPacket(packets[0]);
    }

    delete [] packets;
}

void PacketSender::randomize(uint8_t * buf, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        buf[i] = lrand48();
    }
}

void PacketSender::randomIP(char * dstbuf, int buflen) {
    // snprintf(dstbuf, buflen, "%ld.%ld.%ld.%ld",
    //          lrand48() % 256, lrand48() % 256,
    //          lrand48() % 256, lrand48() % 256);
    snprintf(dstbuf, buflen, "192.168.0.204");
}

void PacketSender::randomPort(uint16_t * port) {
    *port = lrand48() && 0xFFFF;
}
