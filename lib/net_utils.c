/*
 * Generic network code. Moved from net.c
 *
 * Copyright 1994 - 2000 Neil Russell.
 * Copyright 2000 Roland Borde
 * Copyright 2000 Paolo Scaffardi
 * Copyright 2000-2002 Wolfgang Denk, wd@denx.de
 * Copyright 2009 Dirk Behme, dirk.behme@googlemail.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <linux/ctype.h>

IPaddr_t string_to_ip(const char *s)
{
	IPaddr_t addr;
	char *e;
	int i;

	if (s == NULL)
		return(0);

	for (addr=0, i=0; i<4; ++i) {
		ulong val = simple_strtoul(s, &e, 10);
		if (val > 255)
			return 0;
		addr <<= 8;
		addr |= val;
		if (*e == '.')
			s = e+1;
		else if (*e == '\0')
			break;
		else
			return 0;
	}

	return (htonl(addr));
}

/**
 * Parses an IP6addr_t from the given string. IPv6 address parsing is a bit
 * more complicated than v4 due to the flexible format and some of the special
 * cases (e.g. v4 mapped).
 *
 * Examples of valid strings:
 *   2001:db8::0:1234:1
 *   2001:0db8:0000:0000:0000:0000:1234:0001
 *   ::1
 *   ::ffff:192.168.1.1
 *
 * Examples of invalid strings
 *   2001:db8::0::0          (:: can only appear once)
 *   2001:db8:192.168.1.1::1 (v4 part can only appear at the end)
 *   192.168.1.1             (we don't implicity map v4)
 */
int string_to_ip6(const char *strpt, IP6addr_t *addrpt)
{
	int colon_count = 0;
	int found_double_colon = 0;
	int xstart = 0;  /* first zero (double colon) */
	int len = 7; /* numbers of zero words the double colon represents */
	int i;
	const char *s = strpt;

	if (strpt == NULL)
		return -1;

	/* First pass, verify the syntax and locate the double colon */
	for (;;) {
		while (isxdigit((int)*s))
			s++;
		if (*s == '\0')
			break;
		if (*s != ':') {
			if (*s == '.' && len >= 2) {
				while (s != strpt && *(s-1) != ':')
					--s;
				if (string_to_ip(s) != 0) {
					len -= 2;
					break;
				}
			}
			/* This could be a valid address */
			break;
		}
		if (s == strpt) {
			/* The address begins with a colon */
			if (*++s != ':')
				/* Must start with a double colon or a number */
				goto out_err;
		} else {
			s++;
			if (found_double_colon)
				len--;
			else
				xstart++;
		}

		if (*s == ':') {
			if (found_double_colon)
				/* Two double colons are not allowed */
				goto out_err;
			found_double_colon = 1;
			len -= xstart;
			s++;
		}

		if (++colon_count == 7)
			/* Found all colons */
			break;
	}

	if (colon_count == 0 || colon_count > 7)
		goto out_err;
	if (*--s == ':')
		len++;

	/* Second pass, read the address */
	s = strpt;
	for (i = 0; i < 8; i++) {
		int val = 0;
		char *end;

		if (found_double_colon && i >= xstart && i < xstart + len) {
			addrpt->u6_addr16[i] = 0;
			continue;
		}
		while (*s == ':')
			s++;

		if (i == 6 && isdigit((int)*s)) {
			IPaddr_t v4 = string_to_ip(s);
			if (v4 != 0) {
				/* Ending with :IPv4-address */
				addrpt->u6_addr32[3] = v4;
				break;
			}
		}

		val = simple_strtoul(s, &end, 16);
		if (*end != '\0' && *end != ':')
			goto out_err;
		addrpt->u6_addr16[i] = htons(val);
		s = end;
	}
	return 0;

out_err:
	return -1;
}
