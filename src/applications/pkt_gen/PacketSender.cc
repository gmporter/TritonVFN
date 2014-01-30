#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "core/Utils.hh"
#include "PacketSender.hh"

PacketSender::PacketSender(MyricomSNFNetworkInterface * sendInterface,
                           int coreToRunOn, uint64_t _numPacketsToGenerate,
                           uint64_t _packetSize)
    : NetBump(/* inInterface */ NULL, sendInterface, coreToRunOn),
      numPacketsToGenerate(_numPacketsToGenerate), packetSize(_packetSize)
{
    srand48(Utils::utime());
}

PacketSender::~PacketSender()
{ }

void PacketSender::work()
{
    uint32_t payloadOffset;

    Packet * packets = new Packet[numPacketsToGenerate];

    for (uint64_t i = 0; i < numPacketsToGenerate; i++) {
        uint8_t * packet = new uint8_t[packetSize];
        uint8_t srcmac[6], dstmac[6];
        char srcip[128], dstip[128];
        uint16_t srcport, dstport;

        srcmac = {0x00, 0x60, 0xdd, 0x46, 0x82, 0x68};
        dstmac = {0x00, 0x1B, 0x21, 0x9F, 0x8E, 0x7F};
        /*
        memset(srcmac, 0, 6);
        memset(dstmac, 0, 6);
        */

        /*
        strncpy(srcip, "192.168.4.72", strlen("192.168.4.72")+1);
        */
        strncpy(dstip, "192.168.5.72", strlen("192.168.5.72")+1);

        /*
        randomize(srcmac, 6);
        randomize(dstmac, 6);
        randomIP(dstip, 128);
        */
        randomIP(srcip, 128);
        randomPort(&srcport);
        randomPort(&dstport);

        payloadOffset = Utils::createUDPPacket(packet,
                                               packetSize-(14+20+8),
                                               srcmac, dstmac,
                                               srcip, dstip,
                                               srcport, dstport);
        packets[i].data = packet;
        packets[i].length = packetSize;
    }

    for (uint64_t i = 0; i < numPacketsToGenerate; i++) {

        uint8_t * packetdata = (uint8_t *) packets[i].data;

        // record the send time
        uint64_t sentTime = Utils::utime();
        memcpy(packetdata+payloadOffset, &sentTime, sizeof(uint64_t));

        // record the packet number
        memcpy(packetdata+payloadOffset+sizeof(uint64_t), &i, sizeof(uint64_t));

        sendPacket(packets[i]);
        //usleep(1);
    }

    delete [] packets;
}

void PacketSender::randomize(uint8_t * buf, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        buf[i] = lrand48();
    }
}

void PacketSender::randomIP(char * dstbuf, int buflen) {
    snprintf(dstbuf, buflen, "%d.%d.%d.%d",
             lrand48() % 256, lrand48() % 256,
             lrand48() % 256, lrand48() % 256);
}

void PacketSender::randomPort(uint16_t * port) {
    *port = lrand48() && 0xFFFF;
}
