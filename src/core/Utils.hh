#ifndef UTILS_HH
#define UTILS_HH

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

class Utils {
public:
    static uint64_t utime();
    static uint32_t createUDPPacket(uint8_t *buf, int payloadLength,
                                    const uint8_t * srcmac,
                                    const uint8_t * dstmac,
                                    const char * srcip,
                                    const char * dstip,
                                    uint16_t srcport, uint16_t dstport);

private:
    static const uint32_t ETH_LEN = 14;
    static const uint32_t UDP_LEN = 8;
    static const uint32_t ETH_PROTOCOL = 12;
    static const uint32_t UDP_PROTOCOL = 17;

    static unsigned short cksum(unsigned short *ip, int len);

};

#endif // UTILS_HH
