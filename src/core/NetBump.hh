#ifndef NETBUMP_HH
#define NETBUMP_HH

#include <pthread.h>
#include "core/Packet.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"

class NetBump {
public:
    NetBump(MyricomSNFNetworkInterface * recv,
            MyricomSNFNetworkInterface * send,
            int coreToRunOn);
    virtual ~NetBump();

    void start();
    void stop();

    virtual void work() = 0;

    inline bool workerIsDone() const {
        return workerHasFinished;
    }

    const int coreToRunOn;

protected:
    void recvPacket(Packet & p);
    void sendPacket(Packet & p);
    void getInInterfaceStatistics(MyricomSNFNetworkInterfaceStats & stats);
    void getOutInterfaceStatistics(MyricomSNFNetworkInterfaceStats & stats);

    bool shouldShutDown;

private:
    MyricomSNFNetworkInterface * const recv, * const send;

    pthread_t threadHandle;

    void startWorkThread();
    void stopWorkThread();
    volatile bool workerHasFinished;

    static void * startThreadHelper(void * This) {
        ((NetBump *)This)->work();
        ((NetBump *)This)->workerHasFinished = true;
        return NULL;
    }
};

#endif /* NETBUMP_HH */
