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
#include "mvOs.h"
#include "mvCommon.h"
#include "mv802_3.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "mvPp2Prs.h"
#include "pp2/gbe/mvPp2Gbe.h"
#include "mvPp2PrsHw.h"

#define PRS_DBG(X...)


/*-------------------------------------------------------------------------------*/
/*		Static varialbes for internal use				 */
/*-------------------------------------------------------------------------------*/

int etherTypeDsa;

int mvPrsDblVlanAiShadow[DBL_VLAN_SHADOW_SIZE];

static int mvPrsDblVlanAiShadowSet(int index)
{
	RANGE_VALIDATE(index, 1, DBL_VLAN_SHADOW_SIZE - 1);
	mvPrsDblVlanAiShadow[index] = 1;
	return MV_OK;
}


static int mvPrsDblVlanAiShadowClear(int index)
{
	RANGE_VALIDATE(index, 1, DBL_VLAN_SHADOW_SIZE - 1);
	mvPrsDblVlanAiShadow[index] = 0;
	return MV_OK;
}

static int mvPrsDblVlanAiShadowClearAll(void)
{
	int i;

	for (i = 1; i < DBL_VLAN_SHADOW_SIZE; i++)
		mvPrsDblVlanAiShadowClear(i);

	return MV_OK;
}
static int mvPrsDblVlanAiFreeGet(void)
{
	int i;
	/* start from 1, 0 not in used */
	for (i = 1; i < DBL_VLAN_SHADOW_SIZE; i++)
		if (mvPrsDblVlanAiShadow[i] == 0)
			return i;

	return MV_PRS_OUT_OF_RAGE;
}


/******************************************************************************
 * Common utilities
 ******************************************************************************/
/*
 * Helper: match ethertype
 */
static void mvPp2PrsMatchEtype(MV_PP2_PRS_ENTRY *pe, int offset, unsigned short ethertype)
{
	PRS_DBG("%s\n", __func__);

	mvPp2PrsSwTcamByteSet(pe, offset + 0, ethertype >> 8, 0xff);
	mvPp2PrsSwTcamByteSet(pe, offset + 1, ethertype & 0xFF, 0xff);
}

static void mvPp2PrsMatchMh(MV_PP2_PRS_ENTRY *pe, unsigned short mh)
{
	PRS_DBG("%s\n", __func__);

	mvPp2PrsSwTcamByteSet(pe, 0, mh >> 8, 0xff);
	mvPp2PrsSwTcamByteSet(pe, 1, mh & 0xFF, 0xff);
}

/******************************************************************************
 *
 * MAC Address Section
 *
 ******************************************************************************
 */

static MV_PP2_PRS_ENTRY *mvPrsMacDaFind(unsigned char *da)
{
	MV_PP2_PRS_ENTRY *pe;
	int tid;

	pe = mvPp2PrsSwAlloc(PRS_LU_MAC);

	/* Go through the all entires with PRS_LU_MAC */
	for (tid = PE_FIRST_FREE_TID ; tid <= PE_LAST_FREE_TID; tid++) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_MAC))
			continue;

		pe->index = tid;
		mvPp2PrsHwRead(pe);
		if (mvPp2PrsSwTcamBytesIgnorMaskCmp(pe, MV_ETH_MH_SIZE, MV_MAC_ADDR_SIZE, da) == EQUALS)
			return pe;
	}
	mvPp2PrsSwFree(pe);
	return NULL;
}

/* Accept special MAC DA - 6 bytes */
int mvPrsMacDaAccept(int port, unsigned char *da, int add)
{
	MV_PP2_PRS_ENTRY *pe = NULL;
	unsigned int     len, ports;
	int              tid;
	char             *name;

	/* Scan TCAM and see if entry with this MAC DA already exist */
	pe = mvPrsMacDaFind(da);
	if (pe == NULL) {
		/* No such entry */
		if (!add) {
			/* Can't remove - No such entry */
			return MV_ERROR;
		}
		/* Create new TCAM entry */

		/* Go through the all entires from first to last */
		tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

		/* Can't add - No free TCAM entries */
		if (tid == MV_PRS_OUT_OF_RAGE) {
			mvOsPrintf("%s: No free TCAM entiry\n", __func__);
			return MV_ERROR;
		}

		pe = mvPp2PrsSwAlloc(PRS_LU_MAC);
		pe->index = tid;

		mvPp2PrsSwTcamPortMapSet(pe, 0);

	}
	/* Update port mask */
	mvPp2PrsSwTcamPortSet(pe, port, add);
	mvPp2PrsSwTcamPortMapGet(pe, &ports);

	if (ports == 0) {
		if (add) {
			/* Internal error, port should be set in ports bitmap */
			return MV_ERROR;
		}
		/* No ports - invalidate the entry */
		mvPp2PrsHwInv(pe->index);
		mvPp2PrsShadowClear(pe->index);
		return MV_OK;

	}

	/* Continue - set next lookup */
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_DSA);

	/* set match on DA */
	len = MV_MAC_ADDR_SIZE;
	while (len--)
		mvPp2PrsSwTcamByteSet(pe, MV_ETH_MH_SIZE + len, da[len], 0xff);

	/* Set result info bits */
	if (MV_IS_BROADCAST_MAC(da)) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L2_BCAST, RI_L2_CAST_MASK);
		name = "bcast";
	/*TODO - remove mcast ,  handled in mvPrsMacAllMultiSet */
	} else if (MV_IS_MULTICAST_MAC(da)) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L2_MCAST, RI_L2_CAST_MASK);
		name = "mcast";
	} else {
		mvPp2PrsSwSramRiUpdate(pe, RI_L2_UCAST, RI_L2_CAST_MASK);
		name = "ucast";
	}

	mvPp2PrsSwSramRiSetBit(pe, RI_MAC_ME_BIT);

	/* shift to ethertype */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_MH_SIZE + 2 * MV_MAC_ADDR_SIZE, SRAM_OP_SEL_SHIFT_ADD);

	/* Write entry to TCAM */
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_MAC, name);

	mvPp2PrsSwFree(pe);
	return MV_OK;
}

/* Drop special MAC DA - 6 bytes */
int mvPrsMacDaDrop(int port, unsigned char *da, int add)
{
	return MV_OK;
}

int mvPrsMacDropAllSet(int port, int add)
{
	MV_PP2_PRS_ENTRY pe;

	if (mvPp2PrsShadowIsValid(PE_DROP_ALL)) {
		/* Entry exist - update port only */
		pe.index = PE_DROP_ALL;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);
		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_MAC);
		pe.index = PE_DROP_ALL;

		/* Non-promiscous mode for all ports - DROP unknown packets */
		mvPp2PrsSwSramRiSetBit(&pe, RI_DROP_BIT);
	/*	mvPp2PrsSwSramLuDoneSet(&pe);*/

		mvPp2PrsSwSramFlowidGenSet(&pe);
		mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_FLOWS);

		/* Update mvPrsShadowTbl */
		mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "drop-all");

		mvPp2PrsSwTcamPortMapSet(&pe, 0);
	}

	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}

/* Set port to promiscous mode */
int mvPrsMacPromiscousSet(int port, int add)
{
	MV_PP2_PRS_ENTRY pe;

	/* Promiscous mode - Accept unknown packets */

	if (mvPp2PrsShadowIsValid(PE_MAC_PROMISCOUS)) {
		/* Entry exist - update port only */
		pe.index = PE_MAC_PROMISCOUS;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);
		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_MAC);
		pe.index = PE_MAC_PROMISCOUS;

		/* Continue - set next lookup */
		mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_DSA);

		/* Set result info bits */
		mvPp2PrsSwSramRiUpdate(&pe, RI_L2_UCAST, RI_L2_CAST_MASK);

		/* shift to ethertype */
		mvPp2PrsSwSramShiftSet(&pe, MV_ETH_MH_SIZE + 2 * MV_MAC_ADDR_SIZE, SRAM_OP_SEL_SHIFT_ADD);

		/* mask all ports */
		mvPp2PrsSwTcamPortMapSet(&pe, 0);

		/* Update mvPrsShadowTbl */
		mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "promisc");
	}

	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}
/* 0 - reject, 1 - accept */
int mvPrsMacAllMultiSet(int port, int add)
{
	MV_PP2_PRS_ENTRY pe;

	/* Ethernet multicast address first byte is with 0x01 */
	unsigned char da_mc = 0x01;
	/* all multicast */

	if (mvPp2PrsShadowIsValid(PE_MAC_MC_ALL)) {
		/* Entry exist - update port only */
		pe.index = PE_MAC_MC_ALL;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);

		pe.index = PE_MAC_MC_ALL;

		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_MAC);

		/* Continue - set next lookup */
		mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_DSA);

		/* Set result info bits */
		mvPp2PrsSwSramRiUpdate(&pe, RI_L2_MCAST, RI_L2_CAST_MASK);

		mvPp2PrsSwTcamByteSet(&pe, MV_ETH_MH_SIZE, da_mc, 0xff);

		/* shift to ethertype */
		mvPp2PrsSwSramShiftSet(&pe, MV_ETH_MH_SIZE + 2 * MV_MAC_ADDR_SIZE, SRAM_OP_SEL_SHIFT_ADD);

		/* no ports */
		mvPp2PrsSwTcamPortMapSet(&pe, 0);

		/* Update mvPrsShadowTbl */
		mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "mcast-all");
	}

	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}

int mvPrsMhRxSpecialSet(int port, int add, unsigned short mh)
{
	MV_PP2_PRS_ENTRY pe;

	if (mvPp2PrsShadowIsValid(PE_RX_SPECIAL)) {
		/* Entry exist - update port only */
		pe.index = PE_RX_SPECIAL;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);
		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_MAC);
		pe.index = PE_RX_SPECIAL;

		mvPp2PrsSwSramRiUpdate(&pe, RI_CPU_CODE_RX_SPEC, RI_CPU_CODE_MASK);
		mvPp2PrsSwSramFlowidGenSet(&pe);
		mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_FLOWS);

		/* Update mvPrsShadowTbl */
		mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "RX special");

		mvPp2PrsSwTcamPortMapSet(&pe, 0);
	}

	mvPp2PrsMatchMh(&pe, mh);
	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}

/* Set default entires (place holder) for promiscous, non-promiscous and all-milticast MAC addresses */
static int mvPp2PrsMacInit(void)
{
	MV_PP2_PRS_ENTRY pe;

	mvPp2PrsSwClear(&pe);

	/* Non-promiscous mode for all ports - DROP unknown packets */
	pe.index = PE_MAC_NON_PROMISCOUS;
	mvPp2PrsSwTcamLuSet(&pe, PRS_LU_MAC);
	mvPp2PrsSwSramRiSetBit(&pe, RI_DROP_BIT);
/*	mvPp2PrsSwSramLuDoneSet(&pe);*/

	mvPp2PrsSwSramFlowidGenSet(&pe);
	mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_FLOWS);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "non-promisc");

	mvPp2PrsSwTcamPortMapSet(&pe, PORT_MASK);
	mvPp2PrsHwWrite(&pe);

	/* place holders only - no ports */
	mvPrsMacDropAllSet(0, 0);
	mvPrsMhRxSpecialSet(0, 0, 0);
	mvPrsMacPromiscousSet(0, 0);
	mvPrsMacAllMultiSet(0, 0);

	return MV_OK;
}
/******************************************************************************
 *
 * DSA Section
 *
 ******************************************************************************
 */

static int mvPp2PrsDsaTagEtherTypeSet(int port, int add, int tagged, int extend)
{
	MV_PP2_PRS_ENTRY pe;
	char name[PRS_TEXT_SIZE];
	int tid, shift, portMask;

	/* if packet is tagged continue check vlans */

	if (extend) {
		if (tagged) {
			tid = PE_ETYPE_EDSA_TAGGED;
			mvOsSPrintf(name, "Etype-EDSA-tagged");
		} else {
			tid = PE_ETYPE_EDSA_UNTAGGED;
			mvOsSPrintf(name, "Etype-EDSA-untagged");
		}
		portMask = 0;
		shift = 8;
	} else {

		if (tagged) {
			tid = PE_ETYPE_DSA_TAGGED;
			mvOsSPrintf(name, "Etype-DSA-tagged");
		} else {
			tid = PE_ETYPE_DSA_UNTAGGED;
			mvOsSPrintf(name, "Etype-DSA-untagged");
		}
		portMask = PORT_MASK;
		shift = 4;
	}

	if (mvPp2PrsShadowIsValid(tid)) {
		/* Entry exist - update port only */
		pe.index = tid;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);
		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_DSA);
		pe.index = tid;

		/* set etherType*/
		mvPp2PrsMatchEtype(&pe, 0, etherTypeDsa);
		mvPp2PrsMatchEtype(&pe, 2, 0);

		mvPp2PrsSwSramRiSetBit(&pe, RI_DSA_BIT);

		/* shift etherType + 2 byte reserved + tag*/
		mvPp2PrsSwSramShiftSet(&pe, 2 + MV_ETH_TYPE_LEN + shift, SRAM_OP_SEL_SHIFT_ADD);

		mvPp2PrsShadowSet(pe.index, PRS_LU_DSA, name);

		/* set tagged bit in DSA tag */
		/* TODO use define */
		if (tagged) {
			/* set bit 29 in dsa tag */
			mvPp2PrsSwTcamByteSet(&pe, MV_ETH_TYPE_LEN + 2 + 3, 0x20, 0x20);

			/* Clear all AI bits for next iteration */
			mvPp2PrsSwSramAiUpdate(&pe, 0, SRAM_AI_MASK);

			mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_VLAN);
		} else {
			/* Set result info bits - No valns ! */
			mvPp2PrsSwSramRiUpdate(&pe, RI_VLAN_NONE, RI_VLAN_MASK);
			mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_L2);
		}
		/* all ports enabled */
		mvPp2PrsSwTcamPortMapSet(&pe, portMask);
	}

	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}


static int mvPp2PrsDsaTagSet(int port, int add, int tagged, int extend)
{
	MV_PP2_PRS_ENTRY pe;
	char name[PRS_TEXT_SIZE];
	int tid, shift;

	/* if packet is tagged continue check vlans */

	if (extend) {
		if (tagged) {
			tid = PE_EDSA_TAGGED;
			mvOsSPrintf(name, "EDSA-tagged");
		} else {
			tid = PE_EDSA_UNTAGGED;
			mvOsSPrintf(name, "EDSA-untagged");
		}

		shift = 8;
	} else {

		if (tagged) {
			tid = PE_DSA_TAGGED;
			mvOsSPrintf(name, "DSA-tagged");
		} else {
			tid = PE_DSA_UNTAGGED;
			mvOsSPrintf(name, "DSA-untagged");
		}

		shift = 4;
	}

	if (mvPp2PrsShadowIsValid(tid)) {
		/* Entry exist - update port only */
		pe.index = tid;
		mvPp2PrsHwRead(&pe);
	} else {
		/* Entry doesn't exist - create new */
		mvPp2PrsSwClear(&pe);
		mvPp2PrsSwTcamLuSet(&pe, PRS_LU_DSA);
		pe.index = tid;


		/* shift 4 bytes if DSA tag , Skip 8 bytes if extand DSA tag*/
		mvPp2PrsSwSramShiftSet(&pe, shift, SRAM_OP_SEL_SHIFT_ADD);

		mvPp2PrsShadowSet(pe.index, PRS_LU_DSA, name);

		/* set tagged bit in DSA tag */
		/* TODO use define */
		if (tagged) {
			mvPp2PrsSwTcamByteSet(&pe, 3, 0x20, 0x20);

			/* Clear all AI bits for next iteration */
			mvPp2PrsSwSramAiUpdate(&pe, 0, SRAM_AI_MASK);

			mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_VLAN);
		} else {
			/* Set result info bits - No valns ! */
			mvPp2PrsSwSramRiUpdate(&pe, RI_VLAN_NONE, RI_VLAN_MASK);
			mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_L2);
		}

		mvPp2PrsSwTcamPortMapSet(&pe, 0);
	}

	mvPp2PrsSwTcamPortSet(&pe, port, add);

	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}



static int mvPp2PrsDsaInit(void)
{
	MV_PP2_PRS_ENTRY pe;

	etherTypeDsa = DSA_ETHER_TYPE;

	/* none tagged EDSA entry -place holder */
	mvPp2PrsDsaTagSet(0, 0, UNTAGGED, EDSA);

	/* tagged EDSA entry -place holder */
	mvPp2PrsDsaTagSet(0, 0, TAGGED, EDSA);

	/* none tagged DSA entry -place holder */
	mvPp2PrsDsaTagSet(0, 0, UNTAGGED, DSA);

	/* tagged DSA entry -place holder */
	mvPp2PrsDsaTagSet(0, 0, TAGGED, DSA);

	/* none tagged EDSA EtherType entry - place holder*/
	mvPp2PrsDsaTagEtherTypeSet(0, 0, UNTAGGED, EDSA);

	/* tagged EDSA EtherType entry - place holder*/
	mvPp2PrsDsaTagEtherTypeSet(0, 0, TAGGED, EDSA);

	/* none tagged DSA EtherType entry */
	mvPp2PrsDsaTagEtherTypeSet(0, 1, UNTAGGED, DSA);

	/* tagged DSA EtherType entry */
	mvPp2PrsDsaTagEtherTypeSet(0, 1, TAGGED, DSA);

	/* default entry , if DSA or EDSA tag not found */
	mvPp2PrsSwClear(&pe);
	mvPp2PrsSwTcamLuSet(&pe, PRS_LU_DSA);
	pe.index = PE_DSA_DEFAULT;
	mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_VLAN);

	/* shift 0 bytes */
	mvPp2PrsSwSramShiftSet(&pe, 0, SRAM_OP_SEL_SHIFT_ADD);
	mvPp2PrsShadowSet(pe.index, PRS_LU_MAC, "default-DSA-tag");

	/* Clear all AI bits for next iteration */
	mvPp2PrsSwSramAiUpdate(&pe, 0, SRAM_AI_MASK);

	/* match for all ports*/
	mvPp2PrsSwTcamPortMapSet(&pe, PORT_MASK);
	mvPp2PrsHwWrite(&pe);

	return MV_OK;
}

int mvPp2PrsEtypeDsaSet(unsigned int eType)
{
	int tid;

	MV_PP2_PRS_ENTRY pe;

	etherTypeDsa = eType;

	for (tid = PE_ETYPE_EDSA_TAGGED; tid <= PE_ETYPE_DSA_UNTAGGED; tid++) {

		pe.index = tid;

		mvPp2PrsHwRead(&pe);

		/* overrwite old etherType*/
		mvPp2PrsMatchEtype(&pe, 0, etherTypeDsa);

		mvPp2PrsHwWrite(&pe);
	}

	return MV_OK;
}

int mvPp2PrsEtypeDsaModeSet(int port, int extand)
{
	POS_RANGE_VALIDATE(port, MV_PP2_MAX_PORTS - 1);
	POS_RANGE_VALIDATE(extand, 1);

	if (extand) {
		mvPp2PrsDsaTagEtherTypeSet(port, 1, UNTAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, TAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, DSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, DSA);
	} else {
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, UNTAGGED, DSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, TAGGED, DSA);
	}
	return MV_OK;
}

int mvPp2PrsTagModeSet(int port, int type)
{

	POS_RANGE_VALIDATE(port, MV_PP2_MAX_PORTS-1);

	switch (type) {

	case MV_PP2_DSA_EXT:
		/* Add port to EDSA entries */
		mvPp2PrsDsaTagSet(port, 1, TAGGED, EDSA);
		mvPp2PrsDsaTagSet(port, 1, UNTAGGED, EDSA);
		/* remove port from DSA entries */
		mvPp2PrsDsaTagSet(port, 0, TAGGED, DSA);
		mvPp2PrsDsaTagSet(port, 0, UNTAGGED, DSA);
/*
		 remove port from all EtherType DSA/EDSA entries
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, DSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, DSA);
*/
		break;

	case MV_PP2_DSA:
		/* Add port to DSA entries */
		mvPp2PrsDsaTagSet(port, 1, TAGGED, DSA);
		mvPp2PrsDsaTagSet(port, 1, UNTAGGED, DSA);

		/* remove port from EDSA entries */
		mvPp2PrsDsaTagSet(port, 0, TAGGED, EDSA);
		mvPp2PrsDsaTagSet(port, 0, UNTAGGED, EDSA);
/*
		remove port from all EtherType DSA/EDSA entries
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, UNTAGGED, DSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 0, TAGGED, DSA);
*/
		break;

	case MV_PP2_MH:
	case MV_PP2_MH_NONE:

		/* remove port form EDSA and DSA entries */
		mvPp2PrsDsaTagSet(port, 0, TAGGED, DSA);
		mvPp2PrsDsaTagSet(port, 0, UNTAGGED, DSA);
		mvPp2PrsDsaTagSet(port, 0, TAGGED, EDSA);
		mvPp2PrsDsaTagSet(port, 0, UNTAGGED, EDSA);
/*
		Add port from all EtherType DSA/EDSA entries
		mvPp2PrsDsaTagEtherTypeSet(port, 1, UNTAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, TAGGED, EDSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, UNTAGGED, DSA);
		mvPp2PrsDsaTagEtherTypeSet(port, 1, TAGGED, DSA);
*/
		break;

	default:
		POS_RANGE_VALIDATE(type, MV_PP2_DSA_EXT);
	}

	return MV_OK;

}


/******************************************************************************
 *
 * VLAN Section
 *
 ******************************************************************************
 */

static MV_PP2_PRS_ENTRY *mvPrsVlanFind(unsigned short tpid, int ai)
{
	MV_PP2_PRS_ENTRY *pe;
	int tid;
	unsigned int riBits, aiBits, enable;
	unsigned char tpidArr[2];

	pe = mvPp2PrsSwAlloc(PRS_LU_VLAN);

#ifndef CONFIG_CPU_BIG_ENDIAN
	tpidArr[0] = ((unsigned char *)&tpid)[1];
	tpidArr[1] = ((unsigned char *)&tpid)[0];
#endif /* CONFIG_CPU_BIG_ENDIAN */

	/* Go through the all entires with PRS_LU_MAC */
	for (tid = PE_FIRST_FREE_TID ; tid <= PE_LAST_FREE_TID; tid++) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_VLAN))
			continue;

		pe->index = tid;

		mvPp2PrsHwRead(pe);
		if (mvPp2PrsSwTcamBytesIgnorMaskCmp(pe, 0, 2, tpidArr) == EQUALS) {
			mvPp2PrsSwSramRiGet(pe, &riBits, &enable);

			/* get Vlan type */
			riBits = (riBits & RI_VLAN_MASK);

			/* get current AI value Tcam */
			mvPp2PrsSwTcamAiGet(pe, &aiBits, &enable);

			/* clear double Vlan Bit */
			aiBits &= ~(1 << DBL_VLAN_AI_BIT);

			if (ai != aiBits)
				continue;

			if ((riBits == RI_VLAN_SINGLE) || (riBits == RI_VLAN_TRIPLE))
				return pe;
		}
	}
	mvPp2PrsSwFree(pe);
	return NULL;
}

static MV_PP2_PRS_ENTRY *mvPrsDoubleVlanFind(unsigned short tpid1, unsigned short tpid2)
{
	MV_PP2_PRS_ENTRY *pe;
	int tid;
	unsigned int bits, enable;
	unsigned char tpidArr1[2];
	unsigned char tpidArr2[2];

#ifndef CONFIG_CPU_BIG_ENDIAN
	tpidArr1[0] = ((unsigned char *)&tpid1)[1];
	tpidArr1[1] = ((unsigned char *)&tpid1)[0];
	tpidArr2[0] = ((unsigned char *)&tpid2)[1];
	tpidArr2[1] = ((unsigned char *)&tpid2)[0];
#endif /* CONFIG_CPU_BIG_ENDIAN */

	pe = mvPp2PrsSwAlloc(PRS_LU_VLAN);

	/* Go through the all entires with PRS_LU_MAC */
	for (tid = PE_FIRST_FREE_TID ; tid <= PE_LAST_FREE_TID; tid++) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_VLAN))
			continue;

		pe->index = tid;
		mvPp2PrsHwRead(pe);
		if ((mvPp2PrsSwTcamBytesIgnorMaskCmp(pe, 0, 2, tpidArr1) == EQUALS) &&
			(mvPp2PrsSwTcamBytesIgnorMaskCmp(pe, 4, 2, tpidArr2) == EQUALS)) {

			mvPp2PrsSwSramRiGet(pe, &bits, &enable);

			if ((bits & RI_VLAN_MASK) == RI_VLAN_DOUBLE)
				return pe;
		}
	}
	mvPp2PrsSwFree(pe);
	return NULL;
}

/* return last double vlan entry */
static int mvPpPrsDoubleVlanLast(void)
{
	MV_PP2_PRS_ENTRY pe;
	unsigned int bits, enable;
	int tid;

	for (tid = PE_LAST_FREE_TID; tid >= PE_FIRST_FREE_TID; tid--) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_VLAN))
			continue;

		pe.index = tid;
		mvPp2PrsHwRead(&pe);
		mvPp2PrsSwSramRiGet(&pe, &bits, &enable);

		if ((bits & RI_VLAN_MASK) == RI_VLAN_DOUBLE)
			return tid;
	}
	return tid;
}

/* return first Single or Triple vlan entry */
static int mvPpPrsVlanFirst(void)
{
	MV_PP2_PRS_ENTRY pe;
	unsigned int bits, enable;
	int tid;

	for (tid = PE_FIRST_FREE_TID; tid <= PE_LAST_FREE_TID; tid++) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_VLAN))
			continue;

		pe.index = tid;
		mvPp2PrsHwRead(&pe);
		mvPp2PrsSwSramRiGet(&pe, &bits, &enable);

		bits &= RI_VLAN_MASK;

		if ((bits == RI_VLAN_SINGLE) || (bits == RI_VLAN_TRIPLE))
			return tid;
	}
	return tid;
}

static int mvPp2PrsVlanAdd(unsigned short tpid, int ai, unsigned int portBmp)
{
	int lastDouble, tid, status = 0;
	MV_PP2_PRS_ENTRY *pe = NULL;
	char name[PRS_TEXT_SIZE];

	pe = mvPrsVlanFind(tpid, ai);

	if (pe == NULL) {

		/* Create new TCAM entry */
		tid = mvPp2PrsTcamFirstFree(PE_LAST_FREE_TID, PE_FIRST_FREE_TID);

		if (tid == MV_PRS_OUT_OF_RAGE) {
			mvOsPrintf("%s: No free TCAM entiry\n", __func__);
			return MV_ERROR;
		}

		/* get last double vlan tid */
		lastDouble = mvPpPrsDoubleVlanLast();

		if (tid <= lastDouble) {
			/* double vlan entries overlapping*/
			mvOsPrintf("%s:Can't add entry, please remove unnecessary triple or single vlans entries.\n", __func__);
			return MV_ERROR;
		}

		pe = mvPp2PrsSwAlloc(PRS_LU_VLAN);
		pe->index = tid;

		mvPp2PrsMatchEtype(pe, 0, tpid);

		/* Clear all AI bits for next iteration */
		status |= mvPp2PrsSwSramAiUpdate(pe, 0, SRAM_AI_MASK);

		/* Continue - set next lookup */
		status |= mvPp2PrsSwSramNextLuSet(pe, PRS_LU_L2);

		/* shift 4 bytes - skip 1 VLAN tags */
		status |= mvPp2PrsSwSramShiftSet(pe, MV_VLAN_HLEN, SRAM_OP_SEL_SHIFT_ADD);

		if (ai == SINGLE_VLAN_AI) {
			/* single vlan*/
			mvOsSPrintf(name, "single-VLAN");
			mvPp2PrsSwSramRiUpdate(pe, RI_VLAN_SINGLE, RI_VLAN_MASK);
		} else {
			/* triple vlan*/
			mvOsSPrintf(name, "triple-VLAN-%d", ai);
			ai |= (1 << DBL_VLAN_AI_BIT);
			mvPp2PrsSwSramRiUpdate(pe, RI_VLAN_TRIPLE, RI_VLAN_MASK);
		}

		status |= mvPp2PrsSwTcamAiUpdate(pe, ai, SRAM_AI_MASK);

		mvPp2PrsShadowSet(pe->index, PRS_LU_VLAN, name);
	}

	status |= mvPp2PrsSwTcamPortMapSet(pe, portBmp);

	if (status == 0)
		mvPp2PrsHwWrite(pe);

	mvPp2PrsSwFree(pe);

	return MV_OK;
}

static int mvPp2PrsVlanDel(unsigned short tpid, int ai)
{
	MV_PP2_PRS_ENTRY *pe = NULL;

	pe = mvPrsVlanFind(tpid, ai);

	if (pe == NULL) {
		/* No such entry */
		mvOsPrintf("Can't remove - No such entry\n");
		return MV_ERROR;
	}

	/* remove entry */
	mvPp2PrsHwInv(pe->index);
	mvPp2PrsShadowClear(pe->index);
	mvPp2PrsSwFree(pe);
	return MV_OK;
}

int mvPp2PrsDoubleVlanAdd(unsigned short tpid1, unsigned short tpid2, unsigned int portBmp)
{
	int tid, ai, status = 0;
	int firstVlan;
	MV_PP2_PRS_ENTRY *pe = NULL;
	char name[PRS_TEXT_SIZE];

	pe = mvPrsDoubleVlanFind(tpid1, tpid2);

	if (pe == NULL) {

		/* Create new TCAM entry */

		tid = mvPp2PrsTcamFirstFree(PE_FIRST_FREE_TID, PE_LAST_FREE_TID);

		if (tid == MV_PRS_OUT_OF_RAGE) {
			mvOsPrintf("%s: No free TCAM entiry\n", __func__);
			return MV_ERROR;
		}

		pe = mvPp2PrsSwAlloc(PRS_LU_VLAN);
		pe->index = tid;

		/* set AI value for new double vlan entry */
		ai = mvPrsDblVlanAiFreeGet();

		if (ai == MV_PRS_OUT_OF_RAGE) {
			mvOsPrintf("%s: Can't add - number of Double vlan rules reached to maximum.\n", __func__);
			return MV_ERROR;
		}

		/* get first single/triple vlan tid */
		firstVlan = mvPpPrsVlanFirst();

		if (tid >= firstVlan) {
			/* double vlan entries overlapping*/
			mvOsPrintf("%s:Can't add entry, please remove unnecessary double vlans entries.\n", __func__);
			return MV_ERROR;
		}

		mvPrsDblVlanAiShadowSet(ai);

		mvPp2PrsMatchEtype(pe, 0, tpid1);
		mvPp2PrsMatchEtype(pe, 4, tpid2);

		/* Set AI value in SRAM for double vlan */
		status |= mvPp2PrsSwSramAiUpdate(pe, (ai | (1 << DBL_VLAN_AI_BIT)), SRAM_AI_MASK);

		/* Continue - set next lookup, */
		status |= mvPp2PrsSwSramNextLuSet(pe, PRS_LU_VLAN);

		/* Set result info bits */
		status |= mvPp2PrsSwSramRiUpdate(pe, RI_VLAN_DOUBLE, RI_VLAN_MASK);

		/* shift 8 bytes - skip 2 VLAN tags */
		status |= mvPp2PrsSwSramShiftSet(pe, 2 * MV_VLAN_HLEN, SRAM_OP_SEL_SHIFT_ADD);

		/* Update mvPrsShadowTbl */
		mvOsSPrintf(name, "double-VLAN-%d", ai);
		mvPp2PrsShadowSet(pe->index, PRS_LU_VLAN, name);

	}
	/* set ports bitmap*/
	status |= mvPp2PrsSwTcamPortMapSet(pe, portBmp);

	if (status == 0)
		mvPp2PrsHwWrite(pe);

	mvPp2PrsSwFree(pe);

	return status;
}

int mvPp2PrsDoubleVlanDel(unsigned short tpid1, unsigned short tpid2)
{
	MV_PP2_PRS_ENTRY *pe = NULL;
	int ai, enable;


	pe = mvPrsDoubleVlanFind(tpid1, tpid2);

	if (pe == NULL) {
		/* No such entry */
		mvOsPrintf("Can't remove - No such entry\n");
		return MV_ERROR;
	}

	/* TODO - remove all corresponding vlan triples  */

	mvPp2PrsSwSramAiGet(pe, &ai, &enable);
	/* remove double vlan AI sign*/
	ai &= ~(1 << DBL_VLAN_AI_BIT);
	mvPrsDblVlanAiShadowClear(ai);

	/* remove entry */
	mvPp2PrsHwInv(pe->index);
	mvPp2PrsShadowClear(pe->index);
	mvPp2PrsSwFree(pe);
	return MV_OK;

}


int mvPp2PrsSingleVlan(unsigned short tpid, unsigned int portBmp, int add)
{
	if (add)
		return mvPp2PrsVlanAdd(tpid, SINGLE_VLAN_AI, portBmp);
	else
		return mvPp2PrsVlanDel(tpid, SINGLE_VLAN_AI);
}


int mvPp2PrsDoubleVlan(unsigned short tpid1, unsigned short tpid2, unsigned int portBmp, int add)
{
	if (add)
		return mvPp2PrsDoubleVlanAdd(tpid1, tpid2, portBmp);
	else
		return mvPp2PrsDoubleVlanDel(tpid1, tpid2);
}

int mvPp2PrsTripleVlan(unsigned short tpid1, unsigned short tpid2, unsigned short tpid3, unsigned int portBmp, int add)
{
	MV_PP2_PRS_ENTRY *pe;
	int ai, aiEnable;
	int status;

	pe = mvPrsDoubleVlanFind(tpid1, tpid2);

	if (!pe) {
		if (add)
			mvOsPrintf("User must enter first double vlan <0x%x,0x%x> before triple\n", tpid1, tpid2);
		else
			mvOsPrintf("Can't remove - No such entry\n");

		return MV_ERROR;
	}

	/* get AI value form double VLAN entry */
	mvPp2PrsSwSramAiGet(pe, &ai, &aiEnable);

	ai &= ~(1 << DBL_VLAN_AI_BIT);

	if (add)
		status = mvPp2PrsVlanAdd(tpid3, ai, portBmp);
	else
		status = mvPp2PrsVlanDel(tpid3, ai);

	mvPp2PrsSwFree(pe);

	return status;
}

/* Detect up to 2 successive VLAN tags:
 * Possible options:
 * 0x8100, 0x88A8
 * 0x8100, 0x8100
 * 0x8100
 * 0x88A8
 */
static int mvPp2PrsVlanInit(void)
{
	MV_PP2_PRS_ENTRY pe;

	mvPrsDblVlanAiShadowClearAll();

	/* double VLAN: 0x8100, 0x88A8 */
	if (mvPp2PrsDoubleVlan(MV_VLAN_TYPE, MV_VLAN_1_TYPE, PORT_MASK, 1))
		return MV_ERROR;

	/* double VLAN: 0x8100, 0x8100 */
	if (mvPp2PrsDoubleVlan(MV_VLAN_TYPE, MV_VLAN_TYPE, PORT_MASK, 1))
		return MV_ERROR;

	/* single VLAN: 0x88a8 */
	if (mvPp2PrsSingleVlan(MV_VLAN_1_TYPE, PORT_MASK, 1))
		return MV_ERROR;

	/* single VLAN: 0x8100 */
	if (mvPp2PrsSingleVlan(MV_VLAN_TYPE, PORT_MASK, 1))
		return MV_ERROR;

	/*---------------------------------*/
	/*  Set default double vlan entry  */
	/*---------------------------------*/
	mvPp2PrsSwClear(&pe);
	mvPp2PrsSwTcamLuSet(&pe, PRS_LU_VLAN);
	pe.index = PE_VLAN_DBL;
	/* Continue - set next lookup */
	mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_L2);

	/* double vlan AI bit */
	mvPp2PrsSwTcamAiUpdate(&pe, (1 << DBL_VLAN_AI_BIT), (1 << DBL_VLAN_AI_BIT));

	/* clear AI for next iterations */
	mvPp2PrsSwSramAiUpdate(&pe, 0, SRAM_AI_MASK);

	/* Set result info bits */
	mvPp2PrsSwSramRiUpdate(&pe, RI_VLAN_DOUBLE, RI_VLAN_MASK);

	mvPp2PrsSwTcamPortMapSet(&pe, PORT_MASK);
	mvPp2PrsHwWrite(&pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe.index, PRS_LU_VLAN, "double-VLAN-accept");

	/*---------------------------------*/
	/*   Set default vlan none entry   */
	/*---------------------------------*/

	mvPp2PrsSwClear(&pe);
	mvPp2PrsSwTcamLuSet(&pe, PRS_LU_VLAN);
	pe.index = PE_VLAN_NONE;
	/* Continue - set next lookup */
	mvPp2PrsSwSramNextLuSet(&pe, PRS_LU_L2);

	/* Set result info bits */
	mvPp2PrsSwSramRiUpdate(&pe, RI_VLAN_NONE, RI_VLAN_MASK);

	mvPp2PrsSwTcamPortMapSet(&pe, PORT_MASK);
	mvPp2PrsHwWrite(&pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe.index, PRS_LU_VLAN, "no-VLAN");

	return MV_OK;
}


/******************************************************************************
 *
 * Ethertype Section
 *
 ******************************************************************************
 */


static int mvPp2PrsEtypePppoe(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_L2);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_PPPOE_TYPE);
	mvPp2PrsSwSramShiftSet(pe, MV_PPPOE_HDR_SIZE, SRAM_OP_SEL_SHIFT_ADD);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_PPPOE);
	mvPp2PrsSwSramRiSetBit(pe, RI_PPPOE_BIT);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-PPPoE");
	mvPp2PrsSwFree(pe);

	return MV_OK;
}


/* match ip4 and ihl == 5 */
static int mvPp2PrsEtypeIp4(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* IPv4 without options */
	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_L2);
	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP_TYPE);
	mvPp2PrsSwTcamByteSet(pe, MV_ETH_TYPE_LEN + 0, 0x45, 0xff);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP4);
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP4, RI_L3_PROTO_MASK);

	/* Skip eth_type + 4 bytes of IP header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-ipv4");
	mvPp2PrsSwFree(pe);

	/* IPv4 with options */
	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_L2);

	pe->index = tid;
	mvPp2PrsMatchEtype(pe, 0, MV_IP_TYPE);
	mvPp2PrsSwTcamByteSet(pe, MV_ETH_TYPE_LEN + 0, 0x40, 0xf0);

	/* Skip eth_type + 4 bytes of IP header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP4);
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP4_OPT, RI_L3_PROTO_MASK);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-ipv4-opt");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}

static int mvPp2PrsEtypeArp(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}


	pe = mvPp2PrsSwAlloc(PRS_LU_L2);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP_ARP_TYPE);

	/* generate flow in the next iteration*/
	/*mvPp2PrsSwSramAiSetBit(pe, AI_DONE_BIT);*/
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_ARP, RI_L3_PROTO_MASK);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsHwWrite(pe);

	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-arp");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}


/* match ip6 */
static int mvPp2PrsEtypeIp6(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

		/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_L2);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP6_TYPE);

	/* Skip eth_type + 4 bytes of IPV6 header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP6);
/*
	there is no support in extension yet
*/
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP6, RI_L3_PROTO_MASK);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsHwWrite(pe);

	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-ipv6");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}


/* unknown ethertype */
static int mvPp2PrsEtypeUn(void)
{
	MV_PP2_PRS_ENTRY *pe;

	/* Default entry for PRS_LU_L2 - Unknown ethtype */
	pe = mvPp2PrsSwAlloc(PRS_LU_L2);
	pe->index = PE_ETH_TYPE_UN;

	/* generate flow in the next iteration*/
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	/* Set result info bits */
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_UN, RI_L3_PROTO_MASK);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_L2, "etype-unknown");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}

/*
 * pnc_etype_init - match basic ethertypes
 */
static int mvPp2PrsEtypeInit(void)
{
	int    rc;

	PRS_DBG("%s\n", __func__);

	rc = mvPp2PrsEtypePppoe();
	if (rc)
		return rc;

	rc = mvPp2PrsEtypeArp();
	if (rc)
		return rc;

	rc = mvPp2PrsEtypeIp4();
	if (rc)
		return rc;

	rc = mvPp2PrsEtypeIp6();
	if (rc)
		return rc;

	rc = mvPp2PrsEtypeUn();
	if (rc)
		return rc;

	return MV_OK;
}

/******************************************************************************
 *
 * PPPoE Section
 *
 ******************************************************************************
 */

static int mvPp2PrsIpv6Pppoe(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_PPPOE);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP6_PPP);

	/* there is no support in extension yet */
	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP6, RI_L3_PROTO_MASK);

	/* Skip eth_type + 4 bytes of IPV6 header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP6);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_PPPOE, "Ipv6-over-PPPoE");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}

static int mvPp2PrsIpv4Pppoe(void)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/** ipV4 over PPPoE without options **/

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_PPPOE);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP_PPP);
	mvPp2PrsSwTcamByteSet(pe, MV_ETH_TYPE_LEN + 0, 0x45, 0xff);

	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP4, RI_L3_PROTO_MASK);

	/* Skip eth_type + 4 bytes of IP header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP4);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_PPPOE, "Ipv4-over-PPPoE");

	mvPp2PrsSwFree(pe);


	/** ipV4 over PPPoE with options **/

	/* Go through the all entires from first to last */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);

	/* Can't add - No free TCAM entries */
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_PPPOE);

	pe->index = tid;

	mvPp2PrsMatchEtype(pe, 0, MV_IP_PPP);

	mvPp2PrsSwSramRiUpdate(pe, RI_L3_IP4_OPT, RI_L3_PROTO_MASK);

	/* Skip eth_type + 4 bytes of IP header */
	mvPp2PrsSwSramShiftSet(pe, MV_ETH_TYPE_LEN + 4, SRAM_OP_SEL_SHIFT_ADD);

	/* set L3 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L3, MV_ETH_TYPE_LEN, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_IP4);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_PPPOE, "Ipv4-over-PPPoE-opt");
	mvPp2PrsSwFree(pe);

	return MV_OK;
}


/* match etype = PPPOE */
static int mvPp2PrsPpppeInit(void)
{
	int rc;

	rc = mvPp2PrsIpv4Pppoe();
	if (rc)
		return rc;

	rc = mvPp2PrsIpv6Pppoe();
	if (rc)
		return rc;

	return MV_OK;
}



/******************************************************************************
 *
 * IPv4 Section
 *
 ******************************************************************************
 */

/* IPv4/TCP header parsing for fragmentation and L4 offset.  */
static int mvPp2PrsIp4Proto(unsigned short proto)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* TCP, Not Fragmented */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}
	pe = mvPp2PrsSwAlloc(PRS_LU_IP4);
	pe->index = tid;

	mvPp2PrsSwTcamByteSet(pe, 2, 0x00, 0x3f);
	mvPp2PrsSwTcamByteSet(pe, 3, 0x00, 0xff);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsSwTcamByteSet(pe, 5, proto, 0xff);

	if (proto == MV_IP_PROTO_TCP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_TCP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-tcp");
	} else if (proto == MV_IP_PROTO_UDP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_UDP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-udp");
	} else {
		mvOsPrintf("%s: IPv4 unsupported protocol %d\n", __func__, proto);
		mvPp2PrsSwFree(pe);
		return MV_ERROR;
	}

	/* set L4 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L4, sizeof(MV_IP_HEADER) - 4, SRAM_OP_SEL_OFFSET_ADD);

	/* Finished: go to flowid generation */
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsHwWrite(pe);

	mvPp2PrsSwFree(pe);

	/* TCP, Fragmented */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}
	pe = mvPp2PrsSwAlloc(PRS_LU_IP4);
	pe->index = tid;

	mvPp2PrsSwTcamByteSet(pe, 5, proto, 0xff);
	mvPp2PrsSwSramRiSetBit(pe, RI_IP_FRAG_BIT);

	if (proto == MV_IP_PROTO_TCP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_TCP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-tcp-frag");
	} else if (proto == MV_IP_PROTO_UDP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_UDP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-udp-frag");
	} else {
		mvOsPrintf("%s: IPv4 unsupported protocol %d\n", __func__, proto);
		mvPp2PrsSwFree(pe);
		return MV_ERROR;
	}

	/* set L4 offset */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L4, sizeof(MV_IP_HEADER) - 4, SRAM_OP_SEL_OFFSET_ADD);

	/* Finished: go to flowid generation */
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsHwWrite(pe);

	mvPp2PrsSwFree(pe);

	return MV_OK;
}


static int mvPp2PrsIp4Init(void)
{
	int rc;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* Set entries for TCP and UDP over IPv4 */
	rc = mvPp2PrsIp4Proto(MV_IP_PROTO_TCP);
	if (rc)
		return rc;

	rc = mvPp2PrsIp4Proto(MV_IP_PROTO_UDP);
	if (rc)
		return rc;

	/* Default IPv4 entry for unknown protocols */
	pe = mvPp2PrsSwAlloc(PRS_LU_IP4);
	pe->index = PE_IP4_PROTO_UN;

	/* generate flow in the next iteration*/
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	/* Set result info bits */
	mvPp2PrsSwSramRiUpdate(pe, RI_L4_OTHER, RI_L4_PROTO_MASK);

	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L4, sizeof(MV_IP_HEADER) - 4, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-l4-unknown");

	mvPp2PrsSwFree(pe);

	return MV_OK;
}

/******************************************************************************
 *
 * IPv6 Section
 *
 *******************************************************************************/
/* TODO continue from here */
/* IPv6 - detect TCP */

static int mvPp2PrsIp6Proto(unsigned short proto)
{
	int tid;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	/* TCP, Not Fragmented */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		return MV_ERROR;
	}
	pe = mvPp2PrsSwAlloc(PRS_LU_IP6);
	pe->index = tid;

	/* match TCP protocol */
	mvPp2PrsSwTcamByteSet(pe, 2, proto, 0xff);


	if (proto == MV_IP_PROTO_TCP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_TCP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv6-tcp");
	} else if (proto == MV_IP_PROTO_UDP) {
		mvPp2PrsSwSramRiUpdate(pe, RI_L4_UDP, RI_L4_PROTO_MASK);
		mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv4-udp");
	} else {
		mvOsPrintf("%s: IPv4 unsupported protocol %d\n", __func__, proto);
		mvPp2PrsSwFree(pe);
		return MV_ERROR;
	}
	/* Finished: go to flowid generation */
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);
	mvPp2PrsSwSramFlowidGenSet(pe);

	/* All ports */
	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);

	/* set L4 offset relatively to our current place */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L4, sizeof(MV_IP6_HEADER) - 4, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsHwWrite(pe);
	mvPp2PrsSwFree(pe);

	return MV_OK;
}

static int mvPp2PrsIp6Init(void)
{
	int tid, rc;
	MV_PP2_PRS_ENTRY *pe;

	PRS_DBG("%s\n", __func__);

	pe = mvPp2PrsSwAlloc(PRS_LU_IP6);

	/* Check hop limit */
	tid = mvPp2PrsTcamFirstFree(0, MV_PP2_PRS_TCAM_SIZE - 1);
	if (tid == MV_PRS_OUT_OF_RAGE) {
		mvOsPrintf("%s: No free TCAM entiry\n", __func__);
		mvPp2PrsSwFree(pe);
		return MV_ERROR;
	}
	pe->index = tid;

	mvPp2PrsSwTcamByteSet(pe, 3, 0x00, 0xff);
	mvPp2PrsSwSramRiUpdate(pe, (RI_L3_UN | RI_DROP_BIT), (RI_L3_PROTO_MASK | RI_DROP_MASK));

	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv6-hop-zero");

	mvPp2PrsSwFree(pe);

	/* Set entries for TCP and UDP over IPv6 */
	rc = mvPp2PrsIp6Proto(MV_IP_PROTO_TCP);
	if (rc)
		return rc;

	rc = mvPp2PrsIp6Proto(MV_IP_PROTO_UDP);
	if (rc)
		return rc;


	/* Default IPv6 entry for unknown protocols */
	pe = mvPp2PrsSwAlloc(PRS_LU_IP6);
	pe->index = PE_IP6_PROTO_UN;

	/* generate flow in the next iteration*/
	mvPp2PrsSwSramFlowidGenSet(pe);
	mvPp2PrsSwSramNextLuSet(pe, PRS_LU_FLOWS);

	/* Set result info bits */
	mvPp2PrsSwSramRiUpdate(pe, RI_L4_OTHER, RI_L4_PROTO_MASK);

	/* set L4 offset relatively to our current place */
	mvPp2PrsSwSramOffsetSet(pe, SRAM_OFFSET_TYPE_L4, sizeof(MV_IP6_HEADER) - 4, SRAM_OP_SEL_OFFSET_ADD);

	mvPp2PrsSwTcamPortMapSet(pe, PORT_MASK);
	mvPp2PrsHwWrite(pe);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_IP4, "ipv6-l4-unknown");

	mvPp2PrsSwFree(pe);
	return MV_OK;
}


/*
 ******************************************************************************
 *
 * flows
 *
 ******************************************************************************
*/

static MV_PP2_PRS_ENTRY *mvPrsFlowFind(int flow)
{
	MV_PP2_PRS_ENTRY *pe;
	int tid, bits, enable;

	pe = mvPp2PrsSwAlloc(PRS_LU_FLOWS);

	/* Go through the all entires with PRS_LU_MAC */
	for (tid = MV_PP2_PRS_TCAM_SIZE-1; tid >= 0; tid--) {
		if ((!mvPp2PrsShadowIsValid(tid)) || (mvPp2PrsShadowLu(tid) != PRS_LU_FLOWS))
			continue;

		pe->index = tid;
		mvPp2PrsHwRead(pe);
		mvPp2PrsSwSramAiGet(pe, &bits, &enable);

		/* sram store classification lookup id in AI bits [5:0] */
		if ((bits & FLOWID_MASK) == flow)
			return pe;
	}
	mvPp2PrsSwFree(pe);
	return NULL;
}

int mvPrsFlowIdGen(int tid, int flowId, unsigned int res, unsigned int resMask, int portBmp)
{
	MV_PP2_PRS_ENTRY *pe;
	char name[PRS_TEXT_SIZE];

	PRS_DBG("%s\n", __func__);

	POS_RANGE_VALIDATE(flowId, FLOWID_MASK);
	POS_RANGE_VALIDATE(tid, MV_PP2_PRS_TCAM_SIZE-1);

	/* Default configuration entry - overrwite is forbidden */
	if (mvPp2PrsShadowIsValid(tid) && (mvPp2PrsShadowLu(tid) != PRS_LU_FLOWS)) {
		mvOsPrintf("%s: Error, Tcam entry is in use\n", __func__);
		return MV_ERROR;
	}

	pe = mvPp2PrsSwAlloc(PRS_LU_FLOWS);
	pe->index = tid;

	mvPp2PrsSwSramAiUpdate(pe, flowId, FLOWID_MASK);
	mvPp2PrsSwSramLuDoneSet(pe);

	mvOsSPrintf(name, "flowId-%d", flowId);

	/* Update mvPrsShadowTbl */
	mvPp2PrsShadowSet(pe->index, PRS_LU_FLOWS, name);

	mvPp2PrsSwTcamPortMapSet(pe, portBmp);

	/*update result data and mask*/
	mvPp2PrsSwTcamWordSet(pe, TCAM_DATA_BYTE, res, resMask);

	mvPp2PrsHwWrite(pe);

	mvPp2PrsSwFree(pe);

	return MV_OK;

}


int mvPrsDefFlow(int port)
{

	MV_PP2_PRS_ENTRY *pe;
	int tid, mallocFlag = 0;
	char name[PRS_TEXT_SIZE];

	PRS_DBG("%s\n", __func__);

	POS_RANGE_VALIDATE(port, MV_PP2_MAX_PORTS - 1);

	pe = mvPrsFlowFind(FLOWID_DEF(port));

	/* Such entry not exist */
	if (!pe) {
		/* Go through the all entires from last to fires */
		tid = mvPp2PrsTcamFirstFree(MV_PP2_PRS_TCAM_SIZE - 1, 0);

		if (tid == MV_PRS_OUT_OF_RAGE) {
			mvOsPrintf("%s: No free TCAM entiry\n", __func__);
			return MV_ERROR;
		}

		mallocFlag = 1;
		pe = mvPp2PrsSwAlloc(PRS_LU_FLOWS);
		pe->index = tid;

		/* set flowID*/
		mvPp2PrsSwSramAiUpdate(pe, FLOWID_DEF(port), FLOWID_MASK);
		mvPp2PrsSwSramLuDoneSet(pe);

		mvOsSPrintf(name, "def-flowId-port-%d", port);

		/* Update mvPrsShadowTbl */
		mvPp2PrsShadowSet(pe->index, PRS_LU_FLOWS, name);

	}

	mvPp2PrsSwTcamPortMapSet(pe, (1 << port));

	mvPp2PrsHwWrite(pe);

	if (mallocFlag)
		mvPp2PrsSwFree(pe);

	return MV_OK;
}

/******************************************************************************
 *
 * Paeser Init
 *
 ******************************************************************************
 */
int mvPrsDefaultInit(void)
{
	int    port, rc;

	/*write zero to all the lines*/
	mvPp2PrsHwClearAll();

	mvPp2PrsHwInvAll();
	mvPp2PrsShadowClearAll();

	/* TODO: Mask & clear all interrupts */

	/* Always start from lookup = 0 */
	for (port = 0; port < MV_PP2_MAX_PORTS; port++)
		mvPp2PrsHwPortInit(port,
					PRS_LU_MAC/*first lu id*/,
					MV_PP2_PRS_PORT_LU_MAX, 0);

	rc = mvPp2PrsMacInit();
	if (rc)
		return rc;

	rc = mvPp2PrsDsaInit();
	if (rc)
		return rc;

	rc = mvPp2PrsVlanInit();
	if (rc)
		return rc;

	rc = mvPp2PrsEtypeInit();
	if (rc)
		return rc;

	rc = mvPp2PrsPpppeInit();
	if (rc)
		return rc;

	rc = mvPp2PrsIp4Init();
	if (rc)
		return rc;

	rc = mvPp2PrsIp6Init();
	if (rc)
		return rc;

	return MV_OK;

}
