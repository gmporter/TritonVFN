#include <pthread.h>
#include <list>
#include "gtest/gtest.h"
#include "core/Utils.hh"
#include "applications/ping_pong/PingPong.hh"

TEST(UtilsTest, Basic) {
    EXPECT_GT(Utils::utime(), 0);
}

TEST(PingWrappers, Basic) {
    uint8_t * p = new uint8_t[1500];

    PingPong::initPing(p, 0);
    EXPECT_TRUE(PingPong::isPing(p));
    EXPECT_FALSE(PingPong::isPong(p));
    EXPECT_EQ(PingPong::getPingSeqNum(p), 0);

    PingPong::initPing(p, 39);
    EXPECT_TRUE(PingPong::isPing(p));
    EXPECT_FALSE(PingPong::isPong(p));
    EXPECT_EQ(PingPong::getPingSeqNum(p), 39);

    PingPong::initPing(p, 817);
    EXPECT_TRUE(PingPong::isPing(p));
    EXPECT_FALSE(PingPong::isPong(p));
    EXPECT_EQ(PingPong::getPingSeqNum(p), 817);
}

TEST(PongWrappers, Basic) {
    uint8_t * p = new uint8_t[1500];

    PingPong::initPong(p, 0);
    EXPECT_TRUE(PingPong::isPong(p));
    EXPECT_FALSE(PingPong::isPing(p));
    EXPECT_EQ(PingPong::getPongAckNum(p), 0);

    PingPong::initPong(p, 51);
    EXPECT_TRUE(PingPong::isPong(p));
    EXPECT_FALSE(PingPong::isPing(p));
    EXPECT_EQ(PingPong::getPongAckNum(p), 51);

    PingPong::initPong(p, 912);
    EXPECT_TRUE(PingPong::isPong(p));
    EXPECT_FALSE(PingPong::isPing(p));
    EXPECT_EQ(PingPong::getPongAckNum(p), 912);
}
