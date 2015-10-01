/*
 * net/ndisc.c
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
#include <asm/unaligned.h>
#include "ndisc.h"

/* IPv6 destination address of packet waiting for ND */
IP6addr_t NetNDSolPacketIP6 = ZERO_IPV6_ADDR;
/* IPv6 address we are expecting ND advert from */
IP6addr_t NetNDRepPacketIP6 = ZERO_IPV6_ADDR;
/* MAC destination address of packet waiting for ND */
uchar *NetNDPacketMAC;
/* pointer to packet waiting to be transmitted after ND is resolved */
uchar *NetNDTxPacket;
uchar NetNDPacketBuf[PKTSIZE_ALIGN + PKTALIGN];
/* size of packet waiting to be transmitted */
int NetNDTxPacketSize;
/* the timer for ND resolution */
ulong NetNDTimerStart;
/* the number of requests we have sent so far */
int NetNDTry;

#define IP6_NDISC_OPT_SPACE(len) (((len)+2+7)&~7)

/**
 * Insert an iption into a neighbor discovery packet.
 * Returns the number of bytes inserted (which may be >= len)
 */
static int
ip6_ndisc_insert_option(struct nd_msg *ndisc, int type, u8 *data, int len)
{
	int space = IP6_NDISC_OPT_SPACE(len);

	ndisc->opt[0] = type;
	ndisc->opt[1] = space>>3;
	memcpy(&ndisc->opt[2], data, len);
	len += 2;

	/* fill the remainder with 0 */
	if ((space - len) > 0)
		memset(&ndisc->opt[len], 0, space - len);

	return space;
}

/**
 * Extract the Ethernet address from a neighbor discovery packet.
 * Note that the link layer address could be anything but the only networking
 * media that u-boot supports is Ethernet so we assume we're extracting a 6
 * byte Ethernet MAC address.
 */
static void
ip6_ndisc_extract_enetaddr(struct nd_msg *ndisc, uchar enetaddr[6])
{
	memcpy(enetaddr, &ndisc->opt[2], 6);
}

/**
 * Check to see if the neighbor discovery packet has
 * the specified option set.
 */
static int
ip6_ndisc_has_option(struct ip6_hdr *ip6, __u8 type)
{
	struct nd_msg *ndisc = (struct nd_msg *)(((uchar *)ip6) + IP6_HDR_SIZE);

	if (ip6->payload_len <= sizeof(struct icmp6hdr))
		return 0;

	return ndisc->opt[0] == type;
}

static void
ip6_send_ns(IP6addr_t *neigh_addr)
{
	IP6addr_t dst_adr;
	unsigned char enetaddr[6];
	struct nd_msg *msg;
	__u16 len;
	uchar *pkt;

	debug("sending neighbor solicitation for %pI6c our address %pI6c\n",
		neigh_addr, &NetOurLinkLocalIP6);

	/* calculate src, dest IPv6 addr and dest Eth addr */
	ip6_make_SNMA(&dst_adr, neigh_addr);
	ip6_make_mult_ethdstaddr(enetaddr, &dst_adr);
	len = sizeof(struct icmp6hdr) + IN6ADDRSZ +
		IP6_NDISC_OPT_SPACE(INETHADDRSZ);

	pkt = (uchar *)NetTxPacket;
	pkt += NetSetEther(pkt, enetaddr, PROT_IP6);
	pkt += ip6_add_hdr(pkt, &NetOurLinkLocalIP6, &dst_adr, IPPROTO_ICMPV6,
		IPV6_NDISC_HOPLIMIT, len);

	/* ICMPv6 - NS */
	msg = (struct nd_msg *)pkt;
	msg->icmph.icmp6_type = IPV6_NDISC_NEIGHBOUR_SOLICITATION;
	msg->icmph.icmp6_code = 0;
	msg->icmph.icmp6_cksum = 0;
	put_unaligned(0, &msg->icmph.icmp6_unused);

	/* Set the target address and llsaddr option */
	memcpy(&msg->target, neigh_addr, sizeof(IP6addr_t));
	ip6_ndisc_insert_option(msg, ND_OPT_SOURCE_LL_ADDR, NetOurEther,
		INETHADDRSZ);

	/* checksum */
	msg->icmph.icmp6_cksum = csum_ipv6_magic(&NetOurLinkLocalIP6, &dst_adr,
		len, IPPROTO_ICMPV6, csum_partial((__u8 *) msg, len, 0));

	pkt += len;

	/* send it! */
	NetSendPacket(NetTxPacket, (pkt - NetTxPacket));
}

static void
ip6_send_na(uchar *eth_dst_addr, IP6addr_t *neigh_addr, IP6addr_t *target)
{
	struct nd_msg *msg;
	__u16 len;
	uchar *pkt;

	debug("sending neighbor advertisement for %pI6c to %pI6c (%pM)\n",
		target, neigh_addr, eth_dst_addr);

	len = sizeof(struct icmp6hdr) + IN6ADDRSZ +
		IP6_NDISC_OPT_SPACE(INETHADDRSZ);

	pkt = (uchar *)NetTxPacket;
	pkt += NetSetEther(pkt, eth_dst_addr, PROT_IP6);
	pkt += ip6_add_hdr(pkt, &NetOurLinkLocalIP6, neigh_addr,
		IPPROTO_ICMPV6, IPV6_NDISC_HOPLIMIT, len);

	/* ICMPv6 - NS */
	msg = (struct nd_msg *)pkt;
	msg->icmph.icmp6_type = IPV6_NDISC_NEIGHBOUR_ADVERTISEMENT;
	msg->icmph.icmp6_code = 0;
	msg->icmph.icmp6_cksum = 0;
	put_unaligned(0, &msg->icmph.icmp6_unused);

	/* Set the target address and lltargetaddr option */
	memcpy(&msg->target, target, sizeof(IP6addr_t));
	ip6_ndisc_insert_option(msg, ND_OPT_TARGET_LL_ADDR, NetOurEther,
		INETHADDRSZ);

	/* checksum */
	msg->icmph.icmp6_cksum = csum_ipv6_magic(&NetOurLinkLocalIP6,
		neigh_addr, len, IPPROTO_ICMPV6,
		csum_partial((__u8 *) msg, len, 0));

	pkt += len;

	/* send it! */
	NetSendPacket(NetTxPacket, (pkt - NetTxPacket));
}

void
ip6_NDISC_Request(void)
{
	if (!ip6_addr_in_subnet(&NetOurIP6, &NetNDSolPacketIP6,
	    NetPrefixLength)) {
		if (ip6_is_unspecified_addr(&NetOurGatewayIP6)) {
			puts("## Warning: gatewayip6 is needed but not set\n");
			NetNDRepPacketIP6 = NetNDSolPacketIP6;
		} else {
			NetNDRepPacketIP6 = NetOurGatewayIP6;
		}
	} else {
		NetNDRepPacketIP6 = NetNDSolPacketIP6;
	}

	ip6_send_ns(&NetNDRepPacketIP6);
}

void
ip6_NDISC_TimeoutCheck(void)
{
	ulong t;

	if (ip6_is_unspecified_addr(&NetNDSolPacketIP6))
		return;

	t = get_timer(0);

	/* check for NDISC timeout */
	if ((t - NetNDTimerStart) > NDISC_TIMEOUT) {
		NetNDTry++;
		if (NetNDTry >= NDISC_TIMEOUT_COUNT) {
			puts("\nNeighbour discovery retry count exceeded; "
				"starting again\n");
			NetNDTry = 0;
			NetStartAgain();
		} else {
			NetNDTimerStart = t;
			ip6_NDISC_Request();
		}
	}
}

void
ip6_NDISC_init(void)
{
	NetNDPacketMAC = NULL;
	NetNDTxPacket = NULL;
	NetNDSolPacketIP6 = NetNullAddrIP6;
	NetNDRepPacketIP6 = NetNullAddrIP6;
	NetNDTxPacket = NULL;

	if (!NetNDTxPacket) {
		NetNDTxPacket = &NetNDPacketBuf[0] + (PKTALIGN - 1);
		NetNDTxPacket -= (ulong)NetNDTxPacket % PKTALIGN;
		NetNDTxPacketSize = 0;
	}
}

void
ndisc_receive(struct ethernet_hdr *et, struct ip6_hdr *ip6, int len)
{
	struct icmp6hdr *icmp =
		(struct icmp6hdr *)(((uchar *)ip6) + IP6_HDR_SIZE);
	struct nd_msg *ndisc = (struct nd_msg *)icmp;
	uchar neigh_eth_addr[6];

	switch (icmp->icmp6_type) {
	case IPV6_NDISC_NEIGHBOUR_SOLICITATION:
		debug("received neighbor solicitation for "
				"%pI6c from %pI6c\n",
				&ndisc->target, &ip6->saddr);
		if (ip6_is_our_addr(&ndisc->target) &&
		    ip6_ndisc_has_option(ip6, ND_OPT_SOURCE_LL_ADDR)) {
			ip6_ndisc_extract_enetaddr(ndisc, neigh_eth_addr);
			ip6_send_na(neigh_eth_addr, &ip6->saddr,
				&ndisc->target);
		}
		break;

	case IPV6_NDISC_NEIGHBOUR_ADVERTISEMENT:
		/* are we waiting for a reply ? */
		if (ip6_is_unspecified_addr(&NetNDSolPacketIP6))
			break;

		if ((memcmp(&ndisc->target, &NetNDRepPacketIP6,
						sizeof(IP6addr_t)) == 0) &&
		     ip6_ndisc_has_option(ip6, ND_OPT_TARGET_LL_ADDR)) {
			ip6_ndisc_extract_enetaddr(ndisc, neigh_eth_addr);

			/* save address for later use */
			if (NetNDPacketMAC != NULL)
				memcpy(NetNDPacketMAC,
						neigh_eth_addr, 6);

			/* modify header, and transmit it */
			memcpy(((struct ethernet_hdr *)NetNDTxPacket)->et_dest,
					neigh_eth_addr, 6);
			NetSendPacket(NetNDTxPacket, NetNDTxPacketSize);

			/* no ND request pending now */
			NetNDSolPacketIP6 = NetNullAddrIP6;
			NetNDTxPacketSize = 0;
			NetNDPacketMAC = NULL;
		}
		break;
	default:
		debug("Unexpected ICMPv6 type 0x%x\n", icmp->icmp6_type);
	}
}
