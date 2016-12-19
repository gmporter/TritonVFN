#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include "core/NetBump.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"

NetBump::NetBump(MyricomSNFNetworkInterface * inInterface,
                 MyricomSNFNetworkInterface * outInterface,
                 int _coreToRunOn)
    : coreToRunOn(_coreToRunOn), recv(inInterface), send(outInterface)
{
    shouldShutDown = false;
    workerHasFinished = false;
}

NetBump::~NetBump()
{ }

void NetBump::start()
{
    startWorkThread();
}

void NetBump::stop()
{
    stopWorkThread();
}

void NetBump::recvPacket(Packet & p)
{
    if (recv) {
        recv->recvPacket(p);
    } else {
        assert(false);
    }
}

void NetBump::sendPacket(Packet & p)
{
    if (send) {
        send->sendPacket(p);
    } else {
        assert(false);
    }
}

void NetBump::getInInterfaceStatistics(MyricomSNFNetworkInterfaceStats & stats)
{
    assert(recv != NULL);
    recv->getStatistics(stats);
}

void NetBump::getOutInterfaceStatistics(MyricomSNFNetworkInterfaceStats & stats)
{
    assert(send != NULL);
    send->getStatistics(stats);
}

void NetBump::startWorkThread()
{
    int rc;
    pthread_attr_t attr;
    sched_param param;
    cpu_set_t cpus;

    /* create thread attributes */
    rc = pthread_attr_init(&attr);
    assert(rc == 0);

    /* set processor affinity */
    CPU_ZERO(&cpus);
    CPU_SET(coreToRunOn, &cpus);
    rc = pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
    assert(rc == 0);

    /* Set priority */
    rc = pthread_attr_getschedparam(&attr, &param);
    assert(rc == 0);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    rc = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    assert(rc == 0);
    rc = pthread_attr_setschedparam(&attr, &param);
    assert(rc == 0);

    /* create the actual work thread */
    rc = pthread_create(&threadHandle, &attr, startThreadHelper, this);
    assert(rc == 0);

    /* clean up */
    rc = pthread_attr_destroy(&attr);
    assert(rc == 0);
}

void NetBump::stopWorkThread()
{
    int rc;

    shouldShutDown = true;
    rc = pthread_join(threadHandle, (void **)NULL);
    assert(rc == 0);
}
