#ifndef PPSCALC_HH
#define PPSCALC_HH

#include "core/NetBump.hh"

class TimerTest : public NetBump {
public:
    TimerTest(int coreToRunOn);
    virtual ~TimerTest();

    virtual void work();

private:
};

#endif /* PPSCALC_HH */
