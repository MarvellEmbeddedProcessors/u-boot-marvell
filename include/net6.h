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

#endif /* __NET6_H__ */
