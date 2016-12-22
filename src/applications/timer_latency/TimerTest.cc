#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "core/Utils.hh"
#include "TimerTest.hh"

#define BIG_DELTA 100000

TimerTest::TimerTest(int coreToRunOn) : NetBump(NULL, NULL, coreToRunOn)
{ }

TimerTest::~TimerTest()
{ }

void TimerTest::work()
{
    high = 0;
    low = -1;
    total = 0;
    num_readings = 0;
    num_big = 0;
    volatile int i;
    while (!shouldShutDown) {
        before = Utils::ntime();
        i = 0;
        while(i < 1000) { i++; }
        now = Utils::ntime();
        delta = now - before;
        if(delta > high)
            high = delta;
        if(delta < low);
            low = delta;
        if(delta >= BIG_DELTA){
            printf("got big, %jd\n", delta);
            num_big++;
        }
        total += delta;
        num_readings++;
    }
    avg = total/(num_readings * 1.0);
    printf("num_readings=%jd\tnum_big=%jd\thigh=%jd\tlow=%jd\tavg=%.2f\n", num_readings, num_big, high, low, avg);
}
