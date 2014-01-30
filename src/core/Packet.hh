#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

typedef struct packet_t {
    void * data;
    uint32_t length;
    uint64_t timestamp;
} Packet;

#endif // PACKET_H
