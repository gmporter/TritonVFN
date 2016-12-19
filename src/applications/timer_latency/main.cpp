#include <assert.h>
#include <list>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <gflags/gflags.h>

#include "core/Utils.hh"
#include "TimerTest.hh"

DEFINE_uint64(workerCPU, 4, "CPU to run rate meter thread on");

bool shouldStop = false;

void cntl_c_handler(int s){
    shouldStop = true;
    fprintf(stderr, "Trying to stop...\n");
}

int
main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  signal (SIGINT, cntl_c_handler);

  TimerTest * timerTest = new TimerTest(FLAGS_workerCPU);

  timerTest->start();
  while (!shouldStop && !timerTest->workerIsDone()) { }
  timerTest->stop();

  delete timerTest;

  return 0;
}
