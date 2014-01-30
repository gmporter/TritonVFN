#include <list>
#include <stdio.h>
#include <assert.h>
#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"
#include "DelayForwarder.hh"

DelayForwarder::DelayForwarder(MyricomSNFNetworkInterface * recvInterface,
                               MyricomSNFNetworkInterface * sendInterface,
                               int coreToRunOn,
                               uint64_t delayInMicroseconds)
    : NetBump(recvInterface, sendInterface, coreToRunOn),
    delayInMicros(delayInMicroseconds)
{
    /* fill the send list up with pre-allocated packets.
     * At 10Gbps, you can send 15 minimum-sized packets.
     * To be safe we assume 150 packets per microsecond of delay
     */
    if (delayInMicroseconds > 0) {
        for (uint64_t i = 0; i < 150*delayInMicroseconds; i++) {
            Packet * p = new Packet();
            p->data = 0;
            p->length = 0;
            p->timestamp = 0;
            freelist.push_back(p);
        }
    }
}

DelayForwarder::~DelayForwarder()
{ 
    while (!freelist.empty()) {
        Packet * p = freelist.front();
        freelist.pop_front();

        delete p;
    }

    while (!sendlist.empty()) {
        Packet * p = sendlist.front();
        sendlist.pop_front();

        delete p;
    }
}

void DelayForwarder::work()
{
    if (delayInMicros == 0) {
        Packet p;

        while (!shouldShutDown) {
            recvPacket(p);
            if (p.length > 0) {
                sendPacket(p);
            }
        }

    } else {
        Packet * p = freelist.front();
        freelist.pop_front();
        
        while (!shouldShutDown) {

            /* if we got a packet, put it on the sendlist */
            recvPacket(*p);
            if (p->length > 0) {
                sendlist.push_back(p);

                if (freelist.empty()) { assert(false); }
                p = freelist.front();
                freelist.pop_front();
            }

            /* send any packets that have been delayed enough */
            while (!sendlist.empty() && packetIsReadyToSend(sendlist.front())) {
                Packet * s = sendlist.front();
                sendPacket(*s);
                sendlist.pop_front();
                freelist.push_back(s);
            }

        }
    }
}
