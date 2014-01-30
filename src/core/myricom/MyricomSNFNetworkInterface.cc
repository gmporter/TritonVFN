#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <list>
#include <sys/prctl.h>

#include "core/Utils.hh"
#include "core/myricom/MyricomSNFNetworkInterface.hh"

MyricomSNFNetworkInterface::MyricomSNFNetworkInterface(
        myri_board_num_t boardNumber)
  : boardNum(boardNumber)
{
    open();
}

MyricomSNFNetworkInterface::~MyricomSNFNetworkInterface()
{
    close();
}

void MyricomSNFNetworkInterface::open()
{
    int rc;

    /* initialize the SNF library (ok to call multiple times) */
    snf_init(SNF_VERSION_API);

    /* open up the particular board for reading */
    rc = snf_open((uint32_t)boardNum, 1, NULL, 0, -1, &recvHandle);
    if (rc != 0) {
        errno = rc;
        perror("snf_open");
    }
    assert(rc == 0);

    /* open up a ring to collect packets on */
    rc = snf_ring_open(recvHandle, &rxring);
    assert(rc == 0);

    /* open the board for sending packets */
    rc = snf_inject_open((uint32_t)boardNum, 0, &txring);
    if (rc != 0) {
        errno = rc;
        perror("snf_inject_open");
    }
    assert(rc == 0);

    /* start listening for packets */
    snf_start(recvHandle);
}

void MyricomSNFNetworkInterface::close()
{
    int rc;

    rc = snf_ring_close(rxring);
    assert(rc == 0);

    rc = snf_close(recvHandle);
    assert(rc == 0);

    rc = snf_inject_close(txring);
    assert(rc == 0);
}

void MyricomSNFNetworkInterface::recvPacket(Packet & p)
{
    struct snf_recv_req recv_req;
    int rc;

    rc = snf_ring_recv(rxring, /*timeout*/ 0, &recv_req);

    if (rc == 0) {
        /* packet arrived */
        p.data = recv_req.pkt_addr;
        p.length = recv_req.length;
        p.timestamp = recv_req.timestamp;

    } else if (rc == EAGAIN) {
        /* there wasn't a packet to receive */
        p.data = NULL;
        p.length = 0;
        p.timestamp = 0;

    } else {
        assert(false);
    }
}

void MyricomSNFNetworkInterface::sendPacket(Packet & p)
{
    int rc;

    do {
        rc = snf_inject_send(txring, /*timeout*/ 0,
                             (const void *)p.data, p.length);
    } while (rc == EAGAIN);
}

