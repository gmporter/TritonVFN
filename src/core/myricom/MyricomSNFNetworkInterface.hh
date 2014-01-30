#ifndef MYRICOMNETWORKINTERFACE_HH
#define MYRICOMNETWORKINTERFACE_HH

#include <stdint.h>
#include "snf.h"
#include "core/Packet.hh"

typedef uint8_t myri_board_num_t;

typedef struct snf_ring_stats MyricomSNFNetworkInterfaceStats;

class MyricomSNFNetworkInterface {
  public:
    MyricomSNFNetworkInterface(myri_board_num_t boardNumber);
    virtual ~MyricomSNFNetworkInterface();

    void recvPacket(Packet & p);
    void sendPacket(Packet & p);

    const myri_board_num_t boardNum;

    inline void getStatistics(MyricomSNFNetworkInterfaceStats & stats) {
        snf_ring_getstats(rxring, &stats);
    }

  private:

    void open();
    void close();

    // handles for receiving packets
    snf_handle_t recvHandle;
    snf_ring_t rxring;  // receiving
    snf_inject_t txring; // sending
};

#endif // MYRICOMNETWORKINTERFACE_HH
