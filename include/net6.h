/**
 * Simple IPv6 network layer implementation.
 *
 * Based and/or adapted from the IPv4 network layer in net.[hc]
 *
 * (C) Copyright 2013 Allied Telesis Labs NZ
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */
#ifndef __NET6_H__
#define __NET6_H__

typedef union ip6addr_t {
	__u8	u6_addr8[16];
	__be16	u6_addr16[8];
	__be32	u6_addr32[4];
} IP6addr_t;

#define IN6ADDRSZ	sizeof(IP6addr_t)
#define INETHADDRSZ	sizeof(NetOurEther)

#define IPV6_ADDRSCOPE_INTF	0x01
#define IPV6_ADDRSCOPE_LINK	0x02
#define IPV6_ADDRSCOPE_AMDIN	0x04
#define IPV6_ADDRSCOPE_SITE	0x05
#define IPV6_ADDRSCOPE_ORG	0x08
#define IPV6_ADDRSCOPE_GLOBAL	0x0E

/**
 * struct ipv6hdr - Internet Protocol V6 (IPv6) header.
 *
 * IPv6 packet header as defined in RFC 2460.
 */
struct ip6_hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8    priority:4,
		version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8    version:4,
		priority:4;
#else
#error  "Please fix <asm/byteorder.h>"
#endif
	__u8		flow_lbl[3];
	__be16		payload_len;
	__u8		nexthdr;
	__u8		hop_limit;
	IP6addr_t	saddr;
	IP6addr_t	daddr;
};

#define IP6_HDR_SIZE (sizeof(struct ip6_hdr))

/* Handy for static initialisations of IP6addr_t, atlhough the
 * c99 '= { 0 }' idiom might work depending on you compiler. */
#define ZERO_IPV6_ADDR { { 0x00, 0x00, 0x00, 0x00, \
			  0x00, 0x00, 0x00, 0x00, \
			  0x00, 0x00, 0x00, 0x00, \
			  0x00, 0x00, 0x00, 0x00 } }

#define IPV6_LINK_LOCAL_PREFIX	0xfe80

struct udp_hdr {
	__be16		udp_src;	/* UDP source port		*/
	__be16		udp_dst;	/* UDP destination port		*/
	__be16		udp_len;	/* Length of UDP packet		*/
	__be16		udp_xsum;	/* Checksum			*/
};

#define IP6_UDPHDR_SIZE (sizeof(struct udp_hdr))

enum {
	__ND_OPT_PREFIX_INFO_END	= 0,
	ND_OPT_SOURCE_LL_ADDR		= 1,
	ND_OPT_TARGET_LL_ADDR		= 2,
	ND_OPT_PREFIX_INFO		= 3,
	ND_OPT_REDIRECT_HDR		= 4,
	ND_OPT_MTU			= 5,
	__ND_OPT_MAX
};

/* ICMPv6 */
#define IPPROTO_ICMPV6			58
/* hop limit for neighbour discovery packets */
#define IPV6_NDISC_HOPLIMIT             255
#define NDISC_TIMEOUT			5000UL
#define NDISC_TIMEOUT_COUNT             3

struct icmp6hdr {
	__u8	icmp6_type;
#define IPV6_ICMP_ECHO_REQUEST			128
#define IPV6_ICMP_ECHO_REPLY			129
#define IPV6_NDISC_ROUTER_SOLICITATION		133
#define IPV6_NDISC_ROUTER_ADVERTISEMENT		134
#define IPV6_NDISC_NEIGHBOUR_SOLICITATION	135
#define IPV6_NDISC_NEIGHBOUR_ADVERTISEMENT	136
#define IPV6_NDISC_REDIRECT			137
	__u8	icmp6_code;
	__be16	icmp6_cksum;

	union {
		__be32	un_data32[1];
		__be16	un_data16[2];
		__u8	un_data8[4];

		struct icmpv6_echo {
			__be16		identifier;
			__be16		sequence;
		} u_echo;

		struct icmpv6_nd_advt {
#if defined(__LITTLE_ENDIAN_BITFIELD)
			__be32		reserved:5,
					override:1,
					solicited:1,
					router:1,
					reserved2:24;
#elif defined(__BIG_ENDIAN_BITFIELD)
			__be32		router:1,
					solicited:1,
					override:1,
					reserved:29;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
		} u_nd_advt;

		struct icmpv6_nd_ra {
			__u8		hop_limit;
#if defined(__LITTLE_ENDIAN_BITFIELD)
			__u8		reserved:6,
					other:1,
					managed:1;

#elif defined(__BIG_ENDIAN_BITFIELD)
			__u8		managed:1,
					other:1,
					reserved:6;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
			__be16		rt_lifetime;
		} u_nd_ra;
	} icmp6_dataun;
#define icmp6_identifier	icmp6_dataun.u_echo.identifier
#define icmp6_sequence		icmp6_dataun.u_echo.sequence
#define icmp6_pointer		icmp6_dataun.un_data32[0]
#define icmp6_mtu		icmp6_dataun.un_data32[0]
#define icmp6_unused		icmp6_dataun.un_data32[0]
#define icmp6_maxdelay		icmp6_dataun.un_data16[0]
#define icmp6_router		icmp6_dataun.u_nd_advt.router
#define icmp6_solicited		icmp6_dataun.u_nd_advt.solicited
#define icmp6_override		icmp6_dataun.u_nd_advt.override
#define icmp6_ndiscreserved	icmp6_dataun.u_nd_advt.reserved
#define icmp6_hop_limit		icmp6_dataun.u_nd_ra.hop_limit
#define icmp6_addrconf_managed	icmp6_dataun.u_nd_ra.managed
#define icmp6_addrconf_other	icmp6_dataun.u_nd_ra.other
#define icmp6_rt_lifetime	icmp6_dataun.u_nd_ra.rt_lifetime
};

struct nd_msg {
	struct icmp6hdr	icmph;
	IP6addr_t	target;
	__u8		opt[0];
};

struct rs_msg {
	struct icmp6hdr	icmph;
	__u8		opt[0];
};

struct ra_msg {
	struct icmp6hdr	icmph;
	__u32		reachable_time;
	__u32		retrans_timer;
};

struct echo_msg {
	struct icmp6hdr	icmph;
	__u16		id;
	__u16		sequence;
};

struct nd_opt_hdr {
	__u8		nd_opt_type;
	__u8		nd_opt_len;
} __attribute__((__packed__));

extern IP6addr_t const NetNullAddrIP6;	/* NULL IPv6 address */
extern IP6addr_t NetOurGatewayIP6;	/* Our gateways IPv6 address */
extern IP6addr_t NetOurIP6;		/* Our IPv6 addr (0 = unknown) */
extern IP6addr_t NetOurLinkLocalIP6;	/* Our link local IPv6 addr */
extern u_int32_t NetPrefixLength;	/* Our prefixlength (0 = unknown) */
extern IP6addr_t NetServerIP6;		/* Server IPv6 addr (0 = unknown) */

#ifdef CONFIG_CMD_PING
extern IP6addr_t  NetPingIP6;             /* the ipv6 address to ping */
#endif


/* ::ffff:0:0/96 is reserved for v4 mapped addresses */
static inline int ipv6_addr_v4mapped(const IP6addr_t *a)
{
	return (a->u6_addr32[0] | a->u6_addr32[1] |
			(a->u6_addr32[2] ^ htonl(0x0000ffff))) == 0;
}

/* Intra-Site Automatic Tunnel Addressing Protocol Address */
static inline int ipv6_addr_is_isatap(const IP6addr_t *a)
{
	return (a->u6_addr32[2] | htonl(0x02000000)) == htonl(0x02005EFE);
}

/* Convert a string to an ipv6 address */
extern int string_to_ip6(const char *s, IP6addr_t *addr);

/* check that an IPv6 address is unspecified (zero) */
int ip6_is_unspecified_addr(IP6addr_t *addr);

/* check that an IPv6 address is ours */
int ip6_is_our_addr(IP6addr_t *addr);

void ip6_make_lladdr(IP6addr_t *lladr, unsigned char const enetaddr[6]);

void ip6_make_SNMA(IP6addr_t *mcast_addr, IP6addr_t *ip6_addr);

void ip6_make_mult_ethdstaddr(unsigned char enetaddr[6],
	IP6addr_t *mcast_addr);

/* check if neighbour is in the same subnet as us */
int ip6_addr_in_subnet(IP6addr_t *our_addr, IP6addr_t *neigh_addr,
	__u32 prefix_length);

unsigned int csum_partial(const unsigned char *buff, int len,
	unsigned int sum);

unsigned short int csum_ipv6_magic(IP6addr_t *saddr, IP6addr_t *daddr,
	__u16 len, unsigned short proto, unsigned int csum);

int ip6_add_hdr(uchar *xip, IP6addr_t *src, IP6addr_t *dest,
	int nextheader, int hoplimit, int payload_len);

/* send a neighbour discovery solicitation message */
extern void ip6_NDISC_Request(void);

/* call back routine when ND timer has gone off */
extern void ip6_NDISC_TimeoutCheck(void);

/* initialises the ND data */
extern void ip6_NDISC_init(void);

/* sends an IPv6 echo request to a host */
extern int ping6_send(void);

/* starts a Ping6 process */
extern void ping6_start(void);

/* handles reception of icmpv6 echo request/reply */
extern void ping6_receive(struct ethernet_hdr *et, struct ip6_hdr *ip6,
	int len);

/* Transmit UDP packet using IPv6, performing neighbour discovery if needed */
extern int net_send_udp_packet6(uchar *ether, IP6addr_t *dest,
				int dport, int sport, int len);

/* handler for incoming IPv6 echo packet */
extern void NetIP6PacketHandler(struct ethernet_hdr *et, struct ip6_hdr *ip6,
	int len);

#endif /* __NET6_H__ */
