#include <pthread.h>
#include "gtest/gtest.h"
#include "core/Utils.hh"
#include "core/CircularFifo.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"

#define NUM_CONCURRENCY_TESTS 10
#define CONCURRENCY_TEST_ITERS 1000000

TEST(UtilsTest, Basic) {
    EXPECT_GT(Utils::utime(), 0);
}

TEST(CircularFifo, Basic) {
    CircularFifo<SimpleInteger, 1000> f;
    EXPECT_EQ(f.isEmpty(), 1);
    EXPECT_EQ(f.isFull(), 0);

    SimpleInteger ints[3];
    ints[0].val=100;
    ints[1].val=200;
    ints[2].val=300;

    f.push(ints[0]);
    f.push(ints[1]);

    EXPECT_EQ(f.isEmpty(), 0);
    EXPECT_EQ(f.isFull(), 0);

    SimpleInteger ret;
    f.pop(ret);
    EXPECT_EQ(100, ret.val);

    f.push(ints[2]);

    f.pop(ret);
    EXPECT_EQ(200, ret.val);
    f.pop(ret);
    EXPECT_EQ(300, ret.val);

    EXPECT_EQ(f.isEmpty(), 1);
    EXPECT_EQ(f.isFull(), 0);
}

void * producer(void * arg)
{
    cpu_set_t cpuset;
    pthread_t thread;
    SimpleInteger ints[CONCURRENCY_TEST_ITERS];

    CircularFifo<SimpleInteger, 100> * f = \
        (CircularFifo<SimpleInteger, 100> *)arg;

    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);

    int rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    assert(rc == 0);
    
    int i = 0;
    for (int j = 0; j < NUM_CONCURRENCY_TESTS; j++) {
        do {
            ints[i].val = i;
            if (!f->isFull()) {
                f->push(ints[i]);
                i++;
            }
        } while (i < CONCURRENCY_TEST_ITERS);
        i = 0;
    }
}

void * consumer(void * arg)
{
    cpu_set_t cpuset;
    pthread_t thread;
    CircularFifo<SimpleInteger, 100> * f = \
        (CircularFifo<SimpleInteger, 100> *)arg;
    SimpleInteger integer;

    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);

    int rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    assert(rc == 0);

    int i = 0;
    for (int j = 0; j < NUM_CONCURRENCY_TESTS; j++) {
        do {
            if (!f->isEmpty()) {
                f->pop(integer);
                EXPECT_EQ(i, integer.val);
                i++;
            }
        } while (i < CONCURRENCY_TEST_ITERS);
        i = 0;
    }
}

TEST(CircularFifo, ConcurrencyTest) {
    CircularFifo<SimpleInteger, 100> * f = \
        new CircularFifo<SimpleInteger, 100>();

    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, (void *)f);
    pthread_create(&consumer_thread, NULL, consumer, (void *)f);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
}

TEST(MyricomSNFNetworkInterface, Basic) {
    MyricomSNFNetworkInterface * board0 = new MyricomSNFNetworkInterface(0);
    delete board0;

    MyricomSNFNetworkInterface * board1 = new MyricomSNFNetworkInterface(1);
    delete board1;
}

TEST(packetComparison1, UDPPacketGenerator) {
    const uint8_t srcmac[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    const uint8_t dstmac[] = {0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    
    uint8_t packet[64];
    uint32_t payloadOffset = Utils::createUDPPacket(packet,
                                                    0,
                                                    srcmac,
                                                    dstmac,
                                                    "192.168.1.2",
                                                    "192.168.14.15",
                                                    5555, 6666);
    
    EXPECT_EQ(14+20+8, payloadOffset);

    const uint8_t reference[] = {0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x00, 0x45, 0x00, 0x00, 0x1c, 0x12, 0x34, 0x00, 0x00, 0x40, 0x11, 0xd8, 0x3b, 0xc0, 0xa8, 0x01, 0x02, 0xc0, 0xa8, 0x0e, 0x0f, 0x15, 0xb3, 0x1a, 0x0a, 0x00, 0x08, 0x00, 0x00};

    // we ignore the UDP checksum because we haven't implemented that yet
    uint16_t nullchecksum = 0;
    memcpy(packet+payloadOffset-2, &nullchecksum, 2);
    for (int i = 0; i < payloadOffset; i++) {
        EXPECT_EQ( reference[i], packet[i] );
    }
}

TEST(packetComparison2, UDPPacketGenerator) {
    const uint8_t srcmac[] = {0xff, 0x2D, 0x3a, 0x12, 0xca, 0x05};
    const uint8_t dstmac[] = {0xaa, 0xab, 0xc0, 0x0c, 0x23, 0x11};
    
    uint8_t packet[64];
    uint32_t payloadOffset = Utils::createUDPPacket(packet,
                                                    10,
                                                    srcmac,
                                                    dstmac,
                                                    "10.0.1.1",
                                                    "192.168.128.113",
                                                    5555, 6666);
    
    EXPECT_EQ(14+20+8, payloadOffset);
    const uint8_t payload[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    memcpy(packet+payloadOffset, payload, 10);

    const uint8_t reference[] = {0xaa, 0xab, 0xc0, 0x0c, 0x23, 0x11, 0xff, 0x2d, 0x3a, 0x12, 0xca, 0x05, 0x08, 0x00, 0x45, 0x00, 0x00, 0x26, 0x12, 0x34, 0x00, 0x00, 0x40, 0x11, 0x1c, 0x79, 0x0a, 0x00, 0x01, 0x01, 0xc0, 0xa8, 0x80, 0x71, 0x15, 0xb3, 0x1a, 0x0a, 0x00, 0x12, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for (int i = 0; i < payloadOffset+10; i++) {
        EXPECT_EQ( reference[i], packet[i] );
    }
}
