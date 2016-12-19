#ifndef PPSCALC_HH
#define PPSCALC_HH

#include "core/NetBump.hh"

class TimerTest : public NetBump {
public:
    TimerTest(int coreToRunOn);
    virtual ~TimerTest();

    virtual void work();

private:
    uint64_t before, now, delta, high, low, total, num_readings, num_big;
    float avg;
};

#endif /* PPSCALC_HH */
