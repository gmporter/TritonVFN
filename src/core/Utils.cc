#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "Utils.hh"
#include "gtest/gtest.h"

uint64_t Utils::utime() {
  struct timeval time;
  gettimeofday(&time, NULL);

  return (((uint64_t) time.tv_sec) * 1000000) + time.tv_usec;
}

unsigned short Utils::cksum(unsigned short *ip, int len) {
    int32_t sum = 0;  /* assume 32 bit long, 16 bit short */

    uint16_t * ptr = (uint16_t *)ip;
    while (len > 1) {
        sum += *ptr;
        ptr++;

        /* if high order bit set, fold */
        if (sum & 0x80000000)
            sum = (sum & 0xFFFF) + (sum >> 16);

        len -= 2;
    }

    /* take care of left over byte */
    if (len) sum += (uint16_t) *((uint8_t *)ip);

    while (sum>>16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return
        ~sum;
}

uint32_t Utils::createUDPPacket(uint8_t *destbuf, int payloadLength,
                                const uint8_t * srcmac, const uint8_t * dstmac,
                                const char * srcip, const char * dstip,
                                uint16_t srcport, uint16_t dstport) {

    uint32_t offset = 0;

    // mac addresses
    memcpy(destbuf, dstmac, 6);
    memcpy(destbuf+6, srcmac, 6);


    *(destbuf+ETH_PROTOCOL) = 0x08 ; // Type of packet is ip
    *(destbuf+ETH_PROTOCOL+1) = 0x00 ; // Type of packet is ip
    offset += ETH_LEN;

    struct ip *iph = (struct ip *) (destbuf+offset);

    //Fill the ip fields
    iph->ip_hl = 0x5;     // header length 
    iph->ip_v = 0x4;      // version number 
    iph->ip_tos = 0x0 && 0xff;    // type of service 
    iph->ip_len = htons(sizeof (struct ip) + \
                        sizeof (struct udphdr) + \
                        payloadLength); //total packet length
    iph->ip_id = htons (0x1234);  /* the value doesn't matter here */
    iph->ip_off = 0;
    iph->ip_ttl = 64; // time to live
    iph->ip_p = UDP_PROTOCOL; //ip protocol  
    iph->ip_sum = 0;
    iph->ip_src.s_addr = inet_addr(srcip); // src ip
    iph->ip_dst.s_addr = inet_addr(dstip); //dst ip
    iph->ip_sum = cksum((uint16_t*)(destbuf + ETH_LEN), 20);

    offset += 20;
    struct udphdr *uph = (struct udphdr*) (destbuf + offset);
    uph->source = htons(srcport);
    uph->dest = htons(dstport);
    uph->len = ntohs(UDP_LEN + payloadLength);
    uph->check = 0;

    offset += UDP_LEN;

    return offset; //offset to the payload
}
