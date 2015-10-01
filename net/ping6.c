/*
 * net/ping6.c
 *
 * (C) Copyright 2013 Allied Telesis Labs NZ
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */
#define DEBUG
#include <common.h>
#include <net.h>
#include <net6.h>
#include "ndisc.h"

static ushort SeqNo;

/* the ipv6 address to ping */
IP6addr_t NetPingIP6;

int
ip6_make_ping(uchar *eth_dst_addr, IP6addr_t *neigh_addr, uchar *pkt)
{
	struct echo_msg *msg;
	__u16 len;
	uchar *pkt_old = pkt;

	len = sizeof(struct echo_msg);

	pkt += NetSetEther(pkt, eth_dst_addr, PROT_IP6);
	pkt += ip6_add_hdr(pkt, &NetOurIP6, neigh_addr, IPPROTO_ICMPV6,
		IPV6_NDISC_HOPLIMIT, len);

	/* ICMPv6 - Echo */
	msg = (struct echo_msg *)pkt;
	msg->icmph.icmp6_type = IPV6_ICMP_ECHO_REQUEST;
	msg->icmph.icmp6_code = 0;
	msg->icmph.icmp6_cksum = 0;
	msg->icmph.icmp6_identifier = 0;
	msg->icmph.icmp6_sequence = htons(SeqNo++);
	msg->id = msg->icmph.icmp6_identifier; /* these seem redundant */
	msg->sequence = msg->icmph.icmp6_sequence;

	/* checksum */
	msg->icmph.icmp6_cksum = csum_ipv6_magic(&NetOurIP6, neigh_addr, len,
			IPPROTO_ICMPV6, csum_partial((__u8 *) msg, len, 0));

	pkt += len;

	return pkt - pkt_old;
}

int ping6_send(void)
{
	uchar *pkt;
	static uchar mac[6];

	/* always send arp request */
	memcpy(mac, NetEtherNullAddr, 6);

	NetNDSolPacketIP6 = NetPingIP6;
	NetNDPacketMAC = mac;

	pkt = NetNDTxPacket;
	pkt += ip6_make_ping(mac, &NetPingIP6, pkt);

	/* size of the waiting packet */
	NetNDTxPacketSize = (pkt - NetNDTxPacket);

	/* and do the ARP request */
	NetNDTry = 1;
	NetNDTimerStart = get_timer(0);
	ip6_NDISC_Request();
	return 1;	/* waiting */
}

static void
ping6_timeout(void)
{
	eth_halt();
	net_set_state(NETLOOP_FAIL);	/* we did not get the reply */
}

void
ping6_start(void)
{
	printf("Using %s device\n", eth_get_name());
	NetSetTimeout(10000UL, ping6_timeout);

	ping6_send();
}

void
ping6_receive(struct ethernet_hdr *et, struct ip6_hdr *ip6, int len)
{
	struct icmp6hdr *icmp =
		(struct icmp6hdr *)(((uchar *)ip6) + IP6_HDR_SIZE);
	IP6addr_t src_ip;

	switch (icmp->icmp6_type) {
	case IPV6_ICMP_ECHO_REPLY:
		src_ip = ip6->saddr;
		if (memcmp(&NetPingIP6, &src_ip, sizeof(IP6addr_t)) != 0)
			return;
		net_set_state(NETLOOP_SUCCESS);
		break;
	case IPV6_ICMP_ECHO_REQUEST:
		debug("Got ICMPv6 ECHO REQUEST from %pI6c\n", &ip6->saddr);
		/* ignore for now.... */
		break;
	default:
		debug("Unexpected ICMPv6 type 0x%x\n", icmp->icmp6_type);
	}

}

