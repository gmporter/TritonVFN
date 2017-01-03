#include <list>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include <time.h>
#include <list>
#include "core/Utils.hh"
#include "TimerTest.hh"

TimerTest::TimerTest(int coreToRunOn) : NetBump(NULL, NULL, coreToRunOn)
{ }

TimerTest::~TimerTest()
{ }

void TimerTest::work()
{
    if (prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0) == -1) {
        fprintf(stderr, "set prctl()\n");
        return;
    }

    int slackval = prctl(PR_GET_TIMERSLACK, 0, 0, 0, 0);
    if (slackval == -1) {
        fprintf(stderr, "get prctl()\n");
        return;
    }
    fprintf(stderr, "Timer slack value is: %d nanoseconds\n", slackval);

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 400000;

    std::list<uint64_t> times;

    for (int i = 0; i < 5000 && !shouldShutDown; i++) {
        if (clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, 0)) {
            perror("clock_nanosleep");
            break;
        }
        times.push_back(Utils::utime());
    }

    while (!times.empty()) {
        uint64_t t = times.front();
        times.pop_front();

        printf("%jd\n", t);
    }
}
