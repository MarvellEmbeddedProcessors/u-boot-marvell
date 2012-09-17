/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/*******************************************************************************
* mvNfp.h - Header File for Marvell NFP
*
* DESCRIPTION:
*       This header file contains macros, typedefs and function declarations
* 	specific to the Marvell Network Fast Processing.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __mvNfp_h__
#define __mvNfp_h__

#include "mvSysHwConfig.h"
#include "mvTypes.h"
#include "mvCommon.h"
#include "mvStack.h"
#include "mvList.h"
#include "mv802_3.h"
#include "gbe/mvEthRegs.h"
#include "pnc/mvPnc.h"

#include "mvNfpDefs.h"

/* Enable / Disable NFP Debug Prints: */

#define NFP_DBG(x...)  if (nfpDebugLevel & NFP_DBG_PRINT) mvOsPrintf(x)
/*#define NFP_WARN(x...)*/

#define NFP_WARN(x...)  if (nfpDebugLevel & NFP_WARN_PRINT) mvOsPrintf(x)
/*#define NFP_WARN(x...)*/

#define	NFP_NO_PRINT	0x0
#define	NFP_DBG_PRINT	0x1
#define	NFP_WARN_PRINT	0x2

/* defined in mvNfp.c */
extern MV_U32 nfpDebugLevel;

extern MV_U32	 nfp_ports[];
extern MV_U32    nfp_jhash_iv;

#ifdef CONFIG_MV_ETH_SWITCH
/* switchGroup coded by bits[4-7] of MSB in Marvell header */
#define NFP_SWITCH_GROUP(port, mh)       	\
		((nfp_ports[port] & NFP_F_PORT_MH) ? ((*(MV_U8 *)(mh)) >> 4) : NFP_INVALID_SWITCH_GROUP)
#else
# define NFP_SWITCH_GROUP(port, mh)      NFP_INVALID_SWITCH_GROUP
#endif /* CONFIG_MV_ETH_SWITCH */

/* NFP Operation Mode */
typedef enum {
	MV_NFP_DISABLED	= 0,
	MV_NFP_2_TUPLE	= 1,
	MV_NFP_5_TUPLE	= 2,
	MV_NFP_MODE_LAST
} MV_NFP_MODE;

typedef enum {
	MV_NFP_OFF = 0,
	MV_NFP_ON  = 1,
	MV_NFP_STATE_LAST
} MV_NFP_STATE;


static INLINE MV_U32 mv_jhash_array(const MV_U8 *k, MV_U32 length, MV_U32 initval)
{
	MV_U32 a, b, c, l;

	l = length;
	a = b = MV_JHASH_GOLDEN_RATIO;
	c = initval;

	while (l >= 12) {
		a += (k[0] + ((MV_U32) k[1] << 8) + ((MV_U32) k[2] << 16) + ((MV_U32) k[3] << 24));
		b += (k[4] + ((MV_U32) k[5] << 8) + ((MV_U32) k[6] << 16) + ((MV_U32) k[7] << 24));
		c += (k[8] + ((MV_U32) k[9] << 8) + ((MV_U32) k[10] << 16) + ((MV_U32) k[11] << 24));
		MV_JHASH_MIX(a, b, c);
		k += 12;
		l -= 12;
	}

	c += length;
	switch (l) {
	case 11:
		c += ((MV_U32) k[10] << 24);
	case 10:
		c += ((MV_U32) k[9] << 16);
	case 9:
		c += ((MV_U32) k[8] << 8);
	case 8:
		b += ((MV_U32) k[7] << 24);
	case 7:
		b += ((MV_U32) k[6] << 16);
	case 6:
		b += ((MV_U32) k[5] << 8);
	case 5:
		b += k[4];
	case 4:
		a += ((MV_U32) k[3] << 24);
	case 3:
		a += ((MV_U32) k[2] << 16);
	case 2:
		a += ((MV_U32) k[1] << 8);
	case 1:
		a += k[0];
	default:
		/* do nothing */;
	}

	MV_JHASH_MIX(a, b, c);
	return c;
}


static INLINE MV_U32 mv_jhash_2addr(int family, const MV_U8 *aa, const MV_U8 *ab, MV_U32 c, MV_U32 initval)
{
	MV_U32 a, b;

	if (family == MV_INET6) {
		a = mv_jhash_array(aa, 16, initval);
		b = mv_jhash_array(ab, 16, initval);
	} else {
		a = *((const MV_U32 *)(aa));
		b = *((const MV_U32 *)(ab));
	}

	return mv_jhash_3words(a, b, c, initval);
}

static INLINE MV_U32 mv_jhash_2macs(const MV_U8 *da, const MV_U8 *sa, MV_U32 c, MV_U32 initval)
{
	MV_U32 a, b;

	a = mv_jhash_array(da, MV_MAC_ADDR_SIZE, initval);
	b = mv_jhash_array(sa, MV_MAC_ADDR_SIZE, initval);

	return mv_jhash_3words(a, b, c, initval);
}

static INLINE MV_U32 mv_jhash_1addr(int family, const MV_U8 *aa, MV_U32 b, MV_U32 c, MV_U32 initval)
{
	MV_U32 a;

	if (family == MV_INET6)
		a = mv_jhash_array(aa, 16, initval);
	else
		a = *((const MV_U32 *)(aa));

	return mv_jhash_3words(a, b, c, initval);
}

/* L3 address copy. Supports AF_INET and AF_INET6 */
static inline MV_VOID l3_addr_copy(int family, u8 *dst, const u8 *src)
{
	const u32 *s = (const u32 *)src;
	u32 *d = (u32 *) dst;

	*d++ = *s++;		/* 4 */
	if (family == MV_INET)
		return;

	*d++ = *s++;		/* 8 */
	*d++ = *s++;		/* 12 */
	*d++ = *s++;		/* 16 */
}

static INLINE MV_U32 l3_addr_eq(int family, const MV_U8 *a, const MV_U8 *b)
{
	const MV_U32 *aa = (const MV_U32 *)a;
	const MV_U32 *bb = (const MV_U32 *)b;
	MV_U32 r;

	r = *aa++ ^ *bb++;	/* 4 */
	if (family == MV_INET)
		return !r;

	r |= *aa++ ^ *bb++;	/* 8 */
	r |= *aa++ ^ *bb++;	/* 12 */
	r |= *aa++ ^ *bb++;	/* 16 */

	return !r;
}

/*******************************
 * Hash tables size definitions
 *******************************/

#define NFP_DEV_HASH_BITS	7
#define NFP_DEV_HASH_SZ		(1 << NFP_DEV_HASH_BITS)
#define NFP_DEV_HASH_MASK	(NFP_DEV_HASH_SZ - 1)

#define	NFP_FDB_HASH_BITS	14
#define	NFP_FSB_HASH_SIZE	(1 << NFP_FDB_HASH_BITS)
#define	NFP_FDB_HASH_MASK	(NFP_FDB_HASH_SIZE - 1)

#define	NFP_BRIDGE_HASH_BITS	14
#define	NFP_BRIDGE_HASH_SIZE	(1 << NFP_BRIDGE_HASH_BITS)
#define	NFP_BRIDGE_HASH_MASK	(NFP_BRIDGE_HASH_SIZE - 1)

#define	NFP_ARP_HASH_BITS	14
#define	NFP_ARP_HASH_SIZE	(1 << NFP_ARP_HASH_BITS)
#define	NFP_ARP_HASH_MASK	(NFP_ARP_HASH_SIZE - 1)

#define	NFP_FIB_HASH_BITS	14
#define	NFP_FIB_HASH_SIZE	(1 << NFP_FIB_HASH_BITS)
#define	NFP_FIB_HASH_MASK	(NFP_FIB_HASH_SIZE - 1)

#define	NFP_CT_HASH_BITS	14
#define	NFP_CT_HASH_SIZE	(1 << NFP_CT_HASH_BITS)
#define	NFP_CT_HASH_MASK	(NFP_CT_HASH_SIZE - 1)

/*********
 * Flags
 *********/

/* Flags relevant for port capabilities */
#define NFP_F_PORT_MH           0x1

/* Flags relevant for NFP_IF_MAP, to be used in NFP_IF_MAP flags */
#define NFP_F_MAP_INT                    0x0001
#define NFP_F_MAP_EXT                    0x0002
#define NFP_F_MAP_SWITCH_PORT            0x0004
#define NFP_F_MAP_BRIDGE                 0x0008
#define NFP_F_MAP_BRIDGE_PORT            0x0010
#define NFP_F_MAP_TX_MH                  0x0020
#define NFP_F_MAP_VLAN_PVID              0x0040
#define NFP_F_MAP_VLAN_RX_DROP_TAGGED    0x0080
#define NFP_F_MAP_VLAN_RX_DROP_UNTAGGED  0x0100
#define NFP_F_MAP_VLAN_RX_DROP_UNKNOWN   0x0200
#define NFP_F_MAP_VLAN_RX_FLAGS          (NFP_F_MAP_VLAN_RX_DROP_TAGGED | NFP_F_MAP_VLAN_RX_DROP_UNTAGGED | \
										NFP_F_MAP_VLAN_RX_DROP_UNKNOWN)

#define NFP_F_MAP_VLAN_TX_TAGGED         0x0400
#define NFP_F_MAP_VLAN_TX_UNTAGGED       0x0800
#define NFP_F_MAP_VLAN_TX_FLAGS          (NFP_F_MAP_VLAN_TX_UNTAGGED | NFP_F_MAP_VLAN_TX_TAGGED)

#define NFP_F_MAP_PPPOE                  0x1000

/* Flags relevant for NFP Bridging, to be used in NFP_RULE_BRIDGE flags */
#define NFP_F_BR_SET_VLAN_PRIO  0x1
#define NFP_F_BR_SET_TXQ        0x2
#define NFP_F_BR_SET_TXP        0x4
#define NFP_F_BR_SET_MH         0x8
#define NFP_F_BR_NOT_EXIST      0x10

/* Flags relevant for NFP FIB rules, to be used in NFP_RULE_FIB flags */
#define NFP_F_FIB_BRIDGE_INV    0x1
#define NFP_F_FIB_ARP_INV       0x2
#define NFP_F_FIB_HWF           0x4
#define NFP_F_FIB_ALL_FLAGS	    (NFP_F_FIB_BRIDGE_INV | NFP_F_FIB_ARP_INV | NFP_F_FIB_HWF)

/* Flags relevant for 5 Tuple NFP mode (CT), to be used in NFP_RULE_CT flags */
#define	NFP_F_CT_SNAT           0x1
#define	NFP_F_CT_DNAT           0x2
#define NFP_F_CT_DROP           0x4
#define NFP_F_CT_SET_DSCP       0x8
#define NFP_F_CT_SET_VLAN_PRIO  0x10
#define NFP_F_CT_SET_TXQ        0x20
#define NFP_F_CT_SET_TXP        0x40
#define NFP_F_CT_SET_MH         0x80
#define NFP_F_CT_LIMIT          0x100
#define NFP_F_CT_FIB_INV        0x200
#define NFP_F_CT_HWF            0x400
#define NFP_F_CT_UDP_CSUM       0x800
#define NFP_F_CT_NOT_EXIST      0x1000


/* Flags relevant for classification info NFP_CLASSIFY_INFO */
#define NFP_F_SET_TXQ			0x01
#define NFP_F_SET_TXP			0x02
#define NFP_F_SET_MH			0x04
#define NFP_F_SET_VLAN_PRIO		0x08
#define NFP_F_SET_DSCP			0x10
#define NFP_F_SET_PRIO_TXQ		0x20
#define NFP_F_SET_PRIO_TXP		0x40
#define NFP_F_SET_PRIO_MH		0x80
#define NFP_F_SET_PRIO_VLAN_PRIO	0x100
#define NFP_F_SET_PRIO_DSCP		0x200
#define NFP_F_SET_EXACT_TXQ		0x400
#define NFP_F_SET_EXACT_TXP		0x800
#define NFP_F_SET_EXACT_MH		0x1000
#define NFP_F_SET_EXACT_VLAN_PRIO	0x2000
#define NFP_F_SET_EXACT_DSCP		0x4000

#define NFP_INVALID_VLAN          4096
#define NFP_INVALID_VPRIO         -1
#define NFP_INVALID_DSCP          -1
#define NFP_INVALID_TXP	          255
#define NFP_INVALID_TXQ	          255
#define NFP_INVALID_MH	          0xFFFF
#define NFP_INVALID_SWITCH_GROUP  255
#define NFP_INVALID_PORT          255

/*****************************
 * Classification definitions
 *****************************/

#ifdef NFP_CLASSIFY

/* 0..7 + one cell for global mapping regardless of the old VLAN Prio value (the '-1' option) */
#define NFP_VPRI_MAP_SIZE	9
#define NFP_VPRI_MAP_GLOBAL	8 /* index in the array for the global mapping */
#define NFP_VPRI_MIN		0
#define NFP_VPRI_MAX		7

typedef struct {
	MV_U16 eth_type; /* Used for bridging VLAN priority mapping */
	MV_U8 new_prio;
	MV_U8 valid;
} NFP_VPRI_MAP_INFO;


#define	MV_ETH_NFP_GLOBAL_MAP	-1

/* 0..63 + one cell for global mapping regardless of the old DSCP value (the '-1' option) */
#define NFP_DSCP_MAP_SIZE	65
#define NFP_DSCP_MAP_GLOBAL	64 /* index in the array for the global mapping */
#define NFP_DSCP_MIN		0
#define NFP_DSCP_MAX		63

typedef struct {
	MV_U8 new_dscp;
	MV_U8 valid;
} NFP_DSCP_MAP_INFO;

typedef struct {
	MV_U8 txq;
	MV_U8 valid;
} NFP_TXQ_MAP_INFO;

#define MV_NFP_CLASSIFY_FEATURES	5

typedef enum {
	MV_NFP_CLASSIFY_FEATURE_DSCP		= 0,
	MV_NFP_CLASSIFY_FEATURE_VPRIO		= 1,
	MV_NFP_CLASSIFY_FEATURE_TXQ		= 2,
	MV_NFP_CLASSIFY_FEATURE_TXP		= 3,
	MV_NFP_CLASSIFY_FEATURE_MH		= 4,
	MV_NFP_CLASSIFY_FEATURE_INVALID	= 5,
} MV_NFP_CLASSIFY_FEATURE;

typedef enum {
	MV_NFP_CLASSIFY_MODE_DISABLED	= 0,
	MV_NFP_CLASSIFY_MODE_EXACT	= 1,
	MV_NFP_CLASSIFY_MODE_PRIO	= 2,
	MV_NFP_CLASSIFY_MODE_HIGHEST	= 3,
	MV_NFP_CLASSIFY_MODE_LOWEST	= 4,
	MV_NFP_CLASSIFY_MODE_INVALID  = 5,
} MV_NFP_CLASSIFY_MODE;

typedef enum {
	MV_NFP_CLASSIFY_POLICY_HIGHEST	= 0,
	MV_NFP_CLASSIFY_POLICY_LOWEST	= 1,
	MV_NFP_CLASSIFY_POLICY_FIRST	= 2,
	MV_NFP_CLASSIFY_POLICY_LAST	= 3,
	MV_NFP_CLASSIFY_POLICY_EXACT	= 4,
	MV_NFP_CLASSIFY_POLICY_PRIORITY	= 5,
	MV_NFP_CLASSIFY_POLICY_INVALID = 6,
} MV_NFP_CLASSIFY_POLICY;

MV_STATUS mvNfpClassifyModeSet(MV_NFP_CLASSIFY_FEATURE feature, MV_NFP_CLASSIFY_MODE mode);
MV_NFP_CLASSIFY_MODE mvNfpClassifyModeGet(MV_NFP_CLASSIFY_FEATURE feature);

MV_STATUS mvNfpExactPolicySet(MV_NFP_CLASSIFY_FEATURE feature, MV_NFP_CLASSIFY_POLICY policy);
MV_NFP_CLASSIFY_POLICY mvNfpExactPolicyGet(MV_NFP_CLASSIFY_FEATURE feature);
MV_STATUS mvNfpPrioPolicySet(MV_NFP_CLASSIFY_POLICY policy);
MV_NFP_CLASSIFY_POLICY mvNfpPrioPolicyGet(MV_VOID);
MV_STATUS mvNfpMixedPolicySet(MV_NFP_CLASSIFY_FEATURE feature, MV_NFP_CLASSIFY_POLICY policy);
MV_NFP_CLASSIFY_POLICY mvNfpMixedPolicyGet(MV_NFP_CLASSIFY_FEATURE feature);

/* Priority classification API */
#define NFP_PRIO_MAP_SIZE 64
#define NFP_PRIO_MAX 63
#define NFP_PRIO_INVALID -1

/* Flags relevant for NFP_PRIO_CLASSIFY_INFO */
#define NFP_F_PRIO_DSCP		0x01
#define NFP_F_PRIO_VPRIO	0x02
#define NFP_F_PRIO_TXP		0x04
#define NFP_F_PRIO_TXQ		0x08
#define NFP_F_PRIO_MH		0x10

typedef struct {
	MV_U8 prio;
	MV_U8 valid;
} NFP_PRIO_MAP_INFO;


typedef struct {
	MV_U16 flags;
	MV_U8  txp;
	MV_U8  txq;
	int dscp;
	int vprio;
	MV_U16 mh;
} NFP_PRIO_CLASSIFY_INFO;

MV_STATUS mvNfpIifToPrioSet(int iif, MV_U8 prio);
MV_STATUS mvNfpIifVlanToPrioSet(int iif, MV_U8 vlan_prio, MV_U8 prio);
MV_STATUS mvNfpIifDscpToPrioSet(int iif, MV_U8 dscp, MV_U8 prio);

MV_STATUS mvNfpIifToPrioDel(int iif);
MV_STATUS mvNfpIifVlanToPrioDel(int iif, MV_U8 vlan_prio);
MV_STATUS mvNfpIifDscpToPrioDel(int iif, MV_U8 dscp);

MV_STATUS mvNfpPrioToDscpSet(int oif, MV_U8 prio, MV_U8 dscp);
MV_STATUS mvNfpPrioToVprioSet(int oif, MV_U8 prio, MV_U8 vlan_prio);
MV_STATUS mvNfpPrioToTxpSet(int oif, MV_U8 prio, MV_U8 txp);
MV_STATUS mvNfpPrioToTxqSet(int oif, MV_U8 prio, MV_U8 txq);
MV_STATUS mvNfpPrioToMhSet(int oif, MV_U8 prio, MV_U16 mh);

MV_STATUS mvNfpPrioToDscpDel(int oif, MV_U8 prio);
MV_STATUS mvNfpPrioToVprioDel(int oif, MV_U8 prio);
MV_STATUS mvNfpPrioToTxpDel(int oif, MV_U8 prio);
MV_STATUS mvNfpPrioToTxqDel(int oif, MV_U8 prio);
MV_STATUS mvNfpPrioToMhDel(int oif, MV_U8 prio);

MV_VOID mvNfpIngressPrioDump(int iif);
MV_VOID mvNfpEgressPrioDump(int oif);
#endif /* NFP_CLASSIFY */


/*****************
 * Interface Map
 *****************/

typedef struct nfp_if_map {

	struct nfp_if_map *nextMap;
	struct nfp_if_map *parentIf;
	struct nfp_if_map *virtIf;
	struct nfp_if_map *virtNext;
	int     ifIdx;
	char    name[16];
	MV_VOID *dev;
	MV_U8   port;
	MV_U8   switchGroup;
	MV_U16  pvid;
	MV_U16  txMh;
	MV_U8   mac[MV_MAC_ADDR_SIZE];
	MV_U16  vlanId;
	int     mtu;
	int     bridgeIf;
	MV_U32  flags;
#ifdef NFP_PPP
	MV_U16  sid;
	MV_U8   remoteMac[MV_MAC_ADDR_SIZE];
#endif /* NFP_PPP */

#ifdef NFP_CLASSIFY
	int prio;
	NFP_PRIO_MAP_INFO dscp_to_prio[NFP_DSCP_MAP_SIZE];
	NFP_PRIO_MAP_INFO vpri_to_prio[NFP_VPRI_MAP_SIZE];
	NFP_PRIO_CLASSIFY_INFO prio_to_classify[NFP_PRIO_MAP_SIZE];
#endif /* NFP_CLASSIFY */
} NFP_IF_MAP;

extern NFP_IF_MAP  *nfp_if_map[NFP_DEV_HASH_SZ];
extern NFP_IF_MAP  *nfp_if_real_map[NFP_MAX_PORTS][NFP_MAX_SWITCH_GROUPS];

static INLINE NFP_IF_MAP *mvNfpIfMapGet(int ifIdx)
{
	NFP_IF_MAP *ifMap = nfp_if_map[ifIdx & NFP_DEV_HASH_MASK];

	while (ifMap != NULL) {
		if (ifMap->ifIdx == ifIdx)
			return ifMap;
		ifMap = ifMap->nextMap;
	}
	return NULL;
}

static INLINE int mvNfpIfMapCmp(int port, MV_U8 switchGroup, NFP_IF_MAP *ifMap)
{
	if ((ifMap->port == port) && (ifMap->switchGroup == switchGroup))
		return MV_TRUE;

	return MV_FALSE;
}

static INLINE NFP_IF_MAP *mvNfpIfMapRealGet(int port, MV_U8 *mh)
{
	MV_U8		switchGroup;
	NFP_IF_MAP	*ifMap;

	/* Support port and switchGroup */
	switchGroup = NFP_SWITCH_GROUP(port, mh);
	if (switchGroup == NFP_INVALID_SWITCH_GROUP)
		ifMap = nfp_if_real_map[port][0];
	else
		ifMap = nfp_if_real_map[port][switchGroup];

	if ((ifMap) && mvNfpIfMapCmp(port, switchGroup, ifMap))
		return ifMap;

	return NULL;
}


/************
 * Bridging
 ************/

#ifdef NFP_BRIDGE

#define NFP_BRIDGE_INV		0
#define NFP_BRIDGE_LOCAL	1
#define NFP_BRIDGE_NON_LOCAL	2

typedef struct nfp_rule_fdb {
	struct nfp_rule_fdb *next;
	MV_U16 reserved;
	MV_U8  mac[MV_MAC_ADDR_SIZE];
	int    bridgeIf;
	int    if_index;
	MV_U32 age;
	MV_U8  status;

} NFP_RULE_FDB;

extern NFP_RULE_FDB **nfp_fdb_hash;

static INLINE MV_U32 mvNfpFdbRuleHash(int bridgeIf, const MV_U8 *mac)
{
	MV_U32 hash = 0;
	int    align = (MV_U32)mac & 3;

	switch (align) {
	case 0:
		/* SA - 4 byte alignement - BE support TBD */
		hash = mv_jhash_3words(bridgeIf, *(MV_U16 *)(mac), (*(MV_U16 *)(mac + 2)) | ((*(MV_U16 *)(mac + 4)) << 16),
							nfp_jhash_iv);
		break;

	case 2:
		/* DA - 2 byte alignement */
		hash = mv_jhash_3words(bridgeIf, *(MV_U16 *)(mac), *(MV_U32 *)(mac + 2), nfp_jhash_iv);
		break;

	default:
		mvOsPrintf("%s: Unexpected alignment: mac=%p\n", __func__, mac);
	}

	hash &= NFP_BRIDGE_HASH_MASK;
	return hash;
}

static INLINE MV_STATUS mvNfpFdbRuleCmp(MV_U16 bridgeIf, const MV_U8 *mac, NFP_RULE_FDB *rule)
{
	int    align = (MV_U32)mac & 3;

	switch (align) {
	case 0:
		/* SA - 4 byte alignement */
		if ((rule->bridgeIf == bridgeIf) &&
			(*(MV_U16 *)(rule->mac)) == (*(MV_U16 *)(mac)) &&
			(*(MV_U16 *)(rule->mac + 2)) == (*(MV_U16 *)(mac + 2)) &&
			(*(MV_U16 *)(rule->mac + 4)) == (*(MV_U16 *)(mac + 4)))
			return MV_TRUE;
		break;
	case 2:
		/* DA - 2 byte alignement */
		if ((rule->bridgeIf == bridgeIf) &&
			(*(MV_U16 *)(rule->mac)) == (*(MV_U16 *)(mac)) &&
			(*(MV_U32 *)(rule->mac + 2)) == (*(MV_U32 *)(mac + 2)))
			return MV_TRUE;
		break;
	default:
		mvOsPrintf("%s: Unexpected alignment: mac=%p\n", __func__, mac);
	}
	return MV_FALSE;
}

static INLINE NFP_RULE_FDB *mvNfpFdbLookup(MV_U16 bridgeIf, const MV_U8 *mac)
{
	MV_U32 hash;
	NFP_RULE_FDB *rule;

	hash = mvNfpFdbRuleHash(bridgeIf, mac);

	rule = nfp_fdb_hash[hash];
	while (rule) {
		if (mvNfpFdbRuleCmp(bridgeIf, mac, rule))
			return rule;

		rule = rule->next;
	}
	return NULL;
}

typedef struct nfp_rule_bridge {
	struct nfp_rule_bridge *next;
	MV_U16  reserved;
	MV_U8	da[MV_MAC_ADDR_SIZE];
	MV_U8	sa[MV_MAC_ADDR_SIZE];
	int	iif;
	int	oif;
	MV_U32	flags;
	MV_U32	age;
#ifdef NFP_CLASSIFY
	NFP_VPRI_MAP_INFO  vpri_map[NFP_VPRI_MAP_SIZE];
	MV_U8	txq;
	MV_U8	txp;
	MV_U16  mh;
#endif /* NFP_CLASSIFY */

} NFP_RULE_BRIDGE;

extern NFP_RULE_BRIDGE **nfp_bridge_hash;


static INLINE MV_U32 mvNfpBridgeRuleHash(const MV_U8 *da, const MV_U8 *sa, MV_U16 iif)
{
	MV_U32 hash = mv_jhash_2macs(da, sa, iif, nfp_jhash_iv);
	hash &= NFP_BRIDGE_HASH_MASK;
	return hash;
}


static INLINE MV_STATUS mvNfpBridgeRuleCmp(const MV_U8 *da, const MV_U8 *sa, MV_U16 iif, NFP_RULE_BRIDGE *rule)
{
	if ((iif == rule->iif) &&
	    (*(MV_U16 *)(rule->da)) == (*(MV_U16 *)(da))	 &&
	    (*(MV_U32 *)(rule->da + 2)) == (*(MV_U32 *)(da + 2)) &&
	    (*(MV_U32 *)(rule->sa)) == (*(MV_U32 *)(sa))	 &&
	    (*(MV_U16 *)(rule->sa + 4)) == (*(MV_U16 *)(sa + 4)))
		return MV_TRUE;

	return MV_FALSE;
}

static INLINE NFP_RULE_BRIDGE *mvNfpBridgeLookup(const MV_U8 *da, const MV_U8 *sa, MV_U16 iif)
{
	MV_U32 hash;
	NFP_RULE_BRIDGE *rule;

	hash = 	mvNfpBridgeRuleHash(da, sa, iif);

	rule = nfp_bridge_hash[hash];
	while (rule) {
		if (mvNfpBridgeRuleCmp(da, sa, iif, rule))
			return rule;

		rule = rule->next;
	}
	return NULL;
}
#endif /* NFP_BRIDGE */

typedef struct nfp_rule_arp {
	struct nfp_rule_arp *next;
	int	family;
	MV_U8	nextHopL3[MV_MAX_L3_ADDR_SIZE];
	MV_U8	da[MV_MAC_ADDR_SIZE];
	MV_U32	age;

} NFP_RULE_ARP;

#ifdef NFP_FIB
typedef struct nfp_rule_fib {
	struct nfp_rule_fib *next;
	int	family;
	MV_U8	srcL3[MV_MAX_L3_ADDR_SIZE];
	MV_U8	dstL3[MV_MAX_L3_ADDR_SIZE];
	MV_U16	mh;
	MV_U8	da[MV_MAC_ADDR_SIZE];
	MV_U8	sa[MV_MAC_ADDR_SIZE];
	MV_U32	age;
	MV_U8	defGtwL3[MV_MAX_L3_ADDR_SIZE];
	MV_U32	flags;
	int     oif;
	MV_U32	ref;
#ifdef NFP_CT
	MV_U16	ct_ref_count;
	MV_U16	ct_hwf_ref_count;
#endif /* NFP_CT */
} NFP_RULE_FIB;
#endif /* NFP_FIB */

#ifdef NFP_LIMIT
typedef struct {
	MV_U32	credit;
	MV_U32	creditMax;
	MV_U32	creditPerTick;
	MV_U32	lastUpdate;

	MV_U32	refCnt;
	MV_U32 	index;
	MV_U32	maxElapsedTicks;
} NFP_TBF_INFO;
#endif /* NFP_LIMIT */

#ifdef NFP_CT
typedef struct {
	int family;
	MV_U8  src_l3[MV_MAX_L3_ADDR_SIZE];
	MV_U8  dst_l3[MV_MAX_L3_ADDR_SIZE];
	MV_U16 sport;
	MV_U16 dport;
	MV_U8  proto;
} MV_NFP_CT_KEY;

typedef struct {
	MV_U32 flags;
	MV_U32 new_sip;
	MV_U32 new_dip;
	MV_U16 new_sport;
	MV_U16 new_dport;
	MV_U8 sa[MV_MAC_ADDR_SIZE];
	MV_U8 da[MV_MAC_ADDR_SIZE];
	MV_U8 out_port;
#ifdef NFP_CLASSIFY
	MV_U8 txp;
	MV_U8 txq;
	MV_U8 dscp;
	MV_U8 vprio;
	MV_U16 mh;
#endif /* NFP_CLASSIFY */
} MV_NFP_CT_INFO;

typedef struct nfp_rule_ct {
	struct nfp_rule_ct *next;
	/* 5 tuple key */
	int family;
	MV_U8 srcL3[MV_MAX_L3_ADDR_SIZE];
	MV_U8 dstL3[MV_MAX_L3_ADDR_SIZE];
	MV_U32 ports;
	MV_U16 proto;
	MV_U16 reserved;	/* for alignment */
	NFP_RULE_FIB *fib;
#ifdef NFP_NAT
	MV_U32 new_sip;
	MV_U32 new_dip;
	MV_U16 new_sport;
	MV_U16 new_dport;
#endif /* NFP_NAT */

#ifdef NFP_LIMIT
	/* Token Bucket Filter information */
	NFP_TBF_INFO *tbfInfo;
#endif /* NFP_LIMIT */

#ifdef NFP_CLASSIFY
	/* indexes 0..63 are the old dscp, values in the array cells are the new dscp. index 64 is for global mapping */
	NFP_DSCP_MAP_INFO  dscp_map[NFP_DSCP_MAP_SIZE];
	NFP_VPRI_MAP_INFO  vpri_map[NFP_VPRI_MAP_SIZE];
	NFP_TXQ_MAP_INFO   txq_map[NFP_DSCP_MAP_SIZE];
	MV_U8 txp;
	MV_U16 mh;
#endif /* NFP_CLASSIFY */
	MV_U32 flags;
	MV_U32 age;
	MV_U32 hit_cntr;
	MV_U8 visited;
} NFP_RULE_CT;
#endif /* NFP_CT */

typedef struct {
#ifdef NFP_STAT
	MV_U32 rx;
	MV_U32 iif_err;
	MV_U32 oif_err;
	MV_U32 mac_mcast;
	MV_U32 non_ip;
	MV_U32 ipv4;
	MV_U32 ipv6;
	MV_U32 ipv4_csum_err;
	MV_U32 ipv4_rx_frag;
	MV_U32 ttl_exp;
	MV_U32 l4_unknown;
	MV_U32 l4_csum_err;
	MV_U32 ip_tx_frag;
	MV_U32 ip_tx_frag_err;

#ifdef NFP_BRIDGE
	MV_U32 bridge_hit;
	MV_U32 bridge_miss;
	MV_U32 bridge_local;

	MV_U32 fdb_sa_miss;
	MV_U32 fdb_da_miss;
	MV_U32 fdb_port_miss;
	MV_U32 fdb_hit;
	MV_U32 fdb_local;
#endif /* NFP_BRIDGE */


#ifdef NFP_VLAN
	MV_U32 vlan_rx_tag_drop;
	MV_U32 vlan_rx_untag_drop;
	MV_U32 vlan_rx_unknown_drop;
	MV_U32 vlan_rx_found;
	MV_U32 vlan_rx_trans;
	MV_U32 vlan_tx_add;
	MV_U32 vlan_tx_remove;
	MV_U32 vlan_tx_replace;
#endif /* NFP_VLAN */

#ifdef NFP_PPP
	MV_U32 pppoe_rx_not_found;
	MV_U32 pppoe_rx_found;
	MV_U32 pppoe_tx_add;
	MV_U32 pppoe_tx_remove;
	MV_U32 pppoe_tx_replace;
#endif	/* NFP_PPP */

#ifdef NFP_FIB
	MV_U32 fib_hit;
	MV_U32 fib_miss;
	MV_U32 fib_inv;
#endif	/* NFP_FIB */

#ifdef NFP_CT
	MV_U32 ct_hit;
	MV_U32 ct_miss;
	MV_U32 ct_tcp_fin_rst;
#endif /* NFP_CT */

#ifdef NFP_NAT
	MV_U32 dnat_hit;
	MV_U32 dnat_miss;
	MV_U32 dnat_inv;
	MV_U32 snat_hit;
	MV_U32 snat_miss;
	MV_U32 snat_inv;
#endif	/* NFP_NAT */

#endif	/* NFP_STAT */
} NFP_STATS;

MV_VOID   mvNfpInit(MV_VOID);

MV_VOID   mvNfpPortCapSet(MV_U32 port, MV_U32 cap, MV_U32 on);
MV_U32    mvNfpPortCapGet(MV_U32 port);
MV_VOID   mvNfpDebugLevelSet(int dbgLevelFlags);

MV_VOID   mvNfpStats(MV_U32);
MV_VOID   mvNfpStats(MV_U32 port);

#ifdef NFP_BRIDGE
MV_STATUS mvNfpIfToBridgeAdd(int bridge_if, int port_if);
MV_STATUS mvNfpIfToBridgeDel(int bridge_if, int port_if);
MV_BOOL   mvNfpIfOnSameBridge(NFP_IF_MAP *ifMap1, NFP_IF_MAP *ifMap2);
#ifdef NFP_FDB_MODE
MV_STATUS _INIT mvNfpFdbInit(MV_VOID);
void mvNfpFdbDestroy(void);
MV_STATUS mvNfpFdbRuleAdd(NFP_RULE_FDB *rule2);
MV_STATUS mvNfpFdbRuleDel(NFP_RULE_FDB *rule2);
MV_STATUS mvNfpFdbRuleAge(NFP_RULE_FDB *rule2);
MV_VOID	  mvNfpFdbDump(MV_VOID);
MV_VOID	  mvNfpFdbFlushBridge(int ifindex);
#else
MV_STATUS _INIT mvNfpBridgeInit(MV_VOID);
MV_STATUS mvNfpBridgeRuleAdd(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeRuleDel(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeRuleAge(NFP_RULE_BRIDGE *rule2);
MV_VOID	  mvNfpBridgeDump(MV_VOID);

#ifdef NFP_CLASSIFY
MV_STATUS mvNfpBridgeTxqRuleAdd(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeTxqRuleDel(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeTxpRuleAdd(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeTxpRuleDel(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeMhRuleAdd(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeMhRuleDel(NFP_RULE_BRIDGE *rule2);
MV_STATUS mvNfpBridgeVlanPrioRuleAdd(NFP_RULE_BRIDGE *rule2, int eth_type, int new_prio);
MV_STATUS mvNfpBridgeVlanPrioRuleDel(NFP_RULE_BRIDGE *rule2, int eth_type);
#endif /* NFP_CLASSIFY */
#endif /* NFP_FDB_MODE */
#endif /* NFP_BRIDGE */

#ifdef NFP_VLAN
MV_STATUS mvNfpVlanPvidSet(int if_index, MV_U16 pvid);
MV_STATUS mvNfpVlanVidSet(int if_index, MV_U16 vid);
MV_STATUS mvNfpVlanVidGet(int if_index, MV_U16 *vid);
#endif /* NFP_VLAN */

#ifdef NFP_PPP
MV_STATUS mvNfpPppAdd(int ifIndex, MV_U16 sid, MV_U8 *remoteMac);
MV_STATUS mvNfpPppDel(int ifIndex);
#endif /* NFP_PPP */

#ifdef NFP_FIB
extern NFP_RULE_FIB **fib_hash;
extern MV_LIST_ELEMENT *fib_inv_list;
extern NFP_RULE_ARP **nfp_arp_hash;

static INLINE MV_U32 mvNfpArpRuleHash(int family, MV_U8 *nextHopL3)
{
	MV_U32 hash = mv_jhash_1addr(family, nextHopL3, (MV_U32)0, (MV_U32)0, nfp_jhash_iv);
	hash &= NFP_ARP_HASH_MASK;
	return hash;
}

static INLINE MV_STATUS mvNfpArpRuleCmp(int family, MV_U8 *nextHopL3, NFP_RULE_ARP *rule)
{
	if ((family == rule->family) && l3_addr_eq(family, rule->nextHopL3, nextHopL3))
		return MV_TRUE;

	return MV_FALSE;
}

static INLINE NFP_RULE_ARP *mvNfpArpLookup(int family, MV_U8 *nextHopL3)
{
	MV_U32 hash;
	NFP_RULE_ARP *rule;

	hash = mvNfpArpRuleHash(family, nextHopL3);

	rule = nfp_arp_hash[hash];
	while (rule) {
		if (mvNfpArpRuleCmp(family, nextHopL3, rule))
			return rule;

		rule = rule->next;
	}
	return NULL;
}

static INLINE MV_U32 mvNfpFibRuleHash(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3)
{
	MV_U32 hash = mv_jhash_2addr(family, src_l3, dst_l3, (MV_U32)0, nfp_jhash_iv);
	hash &= NFP_FIB_HASH_MASK;
	return hash;
}

static INLINE MV_STATUS mvNfpFibRuleCmp(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3, NFP_RULE_FIB *rule)
{
	if ((family == rule->family) &&
		l3_addr_eq(family, rule->srcL3, src_l3)	&&
		l3_addr_eq(family, rule->dstL3, dst_l3))
		return MV_TRUE;

	return MV_FALSE;
}

static INLINE NFP_RULE_FIB *mvNfpFibLookup(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3)
{
	MV_U32 hash;
	NFP_RULE_FIB *rule;

	hash = mvNfpFibRuleHash(family, src_l3, dst_l3);
	rule = fib_hash[hash];
	while (rule) {
		if (!(rule->family == family))
			continue;
		if (mvNfpFibRuleCmp(family, src_l3, dst_l3, rule))
			return rule;
		rule = rule->next;
	}
	return NULL;
}

MV_VOID   mvNfpModeSet(int mode);

MV_VOID   mvNfpIpInfoPrint(u32 dbgLevel, int family, u8 *ipAddr);
MV_VOID   mvNfp2TupleInfoPrint(u32 dbgLevel, int family, u8 *srcL3, u8 *dstL3);
MV_VOID   mvNfp5TupleInfoPrint(u32 dbgLevel, int family, u8 *srcL3, u8 *dstL3, u16 sport, u16 dport, u8 proto);

MV_STATUS mvNfpIfMapCreate(NFP_IF_MAP *ifMap);
MV_STATUS mvNfpIfVirtMap(int parentIf, int virtIf);
MV_STATUS mvNfpIfVirtUnmap(int virtIf);

MV_STATUS mvNfpIfFlagsSet(int ifIdx, MV_U32 flags);
MV_STATUS mvNfpIfFlagsClear(int ifIdx, MV_U32 flags);
MV_STATUS mvNfpIfMapMacUpdate(int ifIdx, const MV_U8 *mac);
MV_STATUS mvNfpIfMapMtuUpdate(int ifIdx, int mtu);
MV_STATUS mvNfpIfMapDelete(int ifIdx);
MV_VOID   mvNfpIfMapDump(MV_VOID);

MV_STATUS mvNfpFibInit(MV_VOID);
MV_VOID   mvNfpFibDump(MV_VOID);
MV_VOID   mvNfpFibClean(MV_VOID);
MV_VOID   mvNfpFibDestroy(MV_VOID);
MV_STATUS mvNfpFibRuleValid(NFP_RULE_FIB *fib, MV_LIST_ELEMENT *curr);

MV_STATUS mvNfpFibRuleAdd(NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleDel(NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleAge(NFP_RULE_FIB *fib2);


MV_STATUS mvNfpArpInit(MV_VOID);
MV_VOID   mvNfpArpDump(MV_VOID);
MV_VOID   mvNfpArpDestroy(MV_VOID);
MV_STATUS mvNfpArpRuleAdd(NFP_RULE_ARP *arp2);
MV_STATUS mvNfpArpRuleDel(NFP_RULE_ARP *arp2);
MV_STATUS mvNfpArpRuleAge(NFP_RULE_ARP *arp2);
MV_STATUS mvNfpArpRuleUpdateFibEntries(NFP_RULE_ARP *arp);

#endif /* NFP_FIB */

#ifdef NFP_CT
extern NFP_RULE_CT **ct_hash;

static INLINE MV_U32 mvNfpCtHashByTuple(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3,
					MV_U32 ports, MV_U16 proto)
{
	MV_U32 hash = mv_jhash_2addr(family, src_l3, dst_l3, (ports | proto), nfp_jhash_iv);
	hash &= NFP_CT_HASH_MASK;
	return hash;
}

static INLINE NFP_RULE_CT *mvNfpCtLookupByTuple(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3,
						 MV_U32 ports, MV_U16 proto)
{
	MV_U32 hash;
	NFP_RULE_CT *ct;

	hash = mvNfpCtHashByTuple(family, src_l3, dst_l3, ports, proto);
	ct = ct_hash[hash];
	while (ct) {
		if ((ct->family == family) &&
			l3_addr_eq(ct->family, ct->srcL3, src_l3) &&
			l3_addr_eq(ct->family, ct->dstL3, dst_l3) &&
			(ct->ports == ports) && (ct->proto == proto))
			return ct;
		ct = ct->next;
	}

	return NULL;
}

extern NFP_RULE_CT **ct_hash;
MV_STATUS mvNfpCtInit(MV_VOID);
MV_VOID mvNfpCtRuleFibUpdate(NFP_RULE_FIB *fib);
MV_VOID mvNfpCtRuleFibInvalidate(NFP_RULE_FIB *fib);
MV_STATUS mvNfpCtFilterModeSet(NFP_RULE_CT *ct2);
MV_STATUS mvNfpCtRuleUdpCsumSet(NFP_RULE_CT *ct2, int mode);
MV_STATUS mvNfpCtRateLimitSet(NFP_RULE_CT *ct2, int tbf_index);
MV_STATUS mvNfpCtRateLimitDel(NFP_RULE_CT *ct2);
#ifdef NFP_CLASSIFY
MV_STATUS mvNfpCtDscpRuleAdd(NFP_RULE_CT *ct2, int dscp, int new_dscp);
MV_STATUS mvNfpCtDscpRuleDel(NFP_RULE_CT *ct2, int dscp);
MV_STATUS mvNfpCtVlanPrioRuleAdd(NFP_RULE_CT *ct2, int prio, int new_prio);
MV_STATUS mvNfpCtVlanPrioRuleDel(NFP_RULE_CT *ct2, int prio);
MV_STATUS mvNfpCtTxqRuleAdd(NFP_RULE_CT *ct2, int dscp, int txq);
MV_STATUS mvNfpCtTxqRuleDel(NFP_RULE_CT *ct2, int dscp);
MV_STATUS mvNfpCtTxpRuleAdd(NFP_RULE_CT *ct2);
MV_STATUS mvNfpCtTxpRuleDel(NFP_RULE_CT *ct2);
MV_STATUS mvNfpCtMhRuleAdd(NFP_RULE_CT *ct2);
MV_STATUS mvNfpCtMhRuleDel(NFP_RULE_CT *ct2);
#endif /* NFP_CLASSIFY */
MV_STATUS mvNfpCtRuleDel(NFP_RULE_CT *rule);
MV_STATUS mvNfpCtRuleAge(NFP_RULE_CT *rule);
MV_VOID mvNfpCtDump(MV_VOID);
MV_VOID mvNfpCtClean(int family);
MV_VOID mvNfpCtDestroy(MV_VOID);

MV_VOID mvNfpCtCleanVisited(MV_U32 row, MV_U32 iterator_id);
MV_STATUS mvNfpCtFirstRuleGet(NFP_RULE_CT **rule, MV_U32 iterator_id);
MV_STATUS mvNfpCtNextRuleGet(NFP_RULE_CT **rule, MV_U32 iterator_id);
MV_STATUS mvNfpCtRuleMaxHitCntrGet(NFP_RULE_CT **rule);
MV_STATUS mvNfpCtRuleHwfSet(MV_NFP_CT_KEY *key, int mode);
MV_STATUS mvNfpCtRuleHitCntrGet(MV_NFP_CT_KEY *key, MV_U32 *hit_cntr);
MV_STATUS mvNfpCtRuleHitCntrSet(MV_NFP_CT_KEY *key, MV_U32 val);
MV_STATUS mvNfpCtRuleInfoGet(MV_NFP_CT_KEY *key, MV_NFP_CT_INFO *ct_info);
#endif /* NFP_CT */

#ifdef NFP_NAT
MV_STATUS mvNfpCtNatRuleAdd(NFP_RULE_CT *rule);
#endif /* NFP_NAT */

#ifdef NFP_LIMIT
void mvNfpCtTbfsDump(void);
int mvNfpTbfCreate(int limit, int burst_limit);
MV_STATUS mvNfpTbfDel(int tbf);
MV_STATUS mvNfpTbfProcess(NFP_TBF_INFO *tbf, MV_U32 packetSize);
#endif /* NFP_LIMIT */

#ifndef NFP_FDB_MODE
void mvNfpFlushBridge(int ifindex);
#endif

void mvNfpFlushFib(int ifindex);

#endif /* __mvNfp_h__ */
