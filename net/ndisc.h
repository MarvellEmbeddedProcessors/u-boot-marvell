/*
 * net/ndisc.h
 *
 * (C) Copyright 2013 Allied Telesis Labs NZ
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

/* IPv6 destination address of packet waiting for ND */
extern IP6addr_t NetNDSolPacketIP6;
/* IPv6 address we are expecting ND advert from */
extern IP6addr_t NetNDRepPacketIP6;
/* MAC destination address of packet waiting for ND */
extern uchar *NetNDPacketMAC;
/* pointer to packet waiting to be transmitted after ND is resolved */
extern uchar *NetNDTxPacket;
extern uchar NetNDPacketBuf[PKTSIZE_ALIGN + PKTALIGN];
/* size of packet waiting to be transmitted */
extern int NetNDTxPacketSize;
/* the timer for ND resolution */
extern ulong NetNDTimerStart;
/* the number of requests we have sent so far */
extern int NetNDTry;


void ndisc_receive(struct ethernet_hdr *et, struct ip6_hdr *ip6, int len);
void ip6_NDISC_Request(void);
void ip6_NDISC_TimeoutCheck(void);
