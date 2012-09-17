/*******************************************************************************
Copyright (C) Marvell Interfdbional Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
Interfdbional Ltd. and/or its affiliates ("Marvell") under the following
alterfdbive licensing terms.  Once you have made an election to distribute the
File under one of the following license alterfdbives, please (i) delete this
introductory statement regarding license alterfdbives, (ii) delete the two
license alterfdbives that you have not elected to use and (iii) preserve the
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
* mvNfpFib.c - Marvell Fast Network Processing
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

#include "mvOs.h"
#include "mvDebug.h"
#include "mvList.h"
#include "gbe/mvNeta.h"
#include "mvNfpDefs.h"
#include "mvNfp.h"

MV_LIST_ELEMENT *fib_inv_list = NULL;
NFP_RULE_FIB **fib_hash = NULL;
NFP_RULE_ARP **nfp_arp_hash = NULL;

MV_STATUS _INIT mvNfpFibInit(void)
{
	MV_U32 bytes = sizeof(NFP_RULE_FIB *) * NFP_FIB_HASH_SIZE;

	fib_hash = (NFP_RULE_FIB **)mvOsMalloc(bytes);
	if (fib_hash == NULL) {
		mvOsPrintf("NFP (fib): not enough memory\n");
		return MV_NO_RESOURCE;
	}
	mvOsMemset(fib_hash, 0, bytes);

	fib_inv_list = mvListCreate();
	if (fib_inv_list == NULL) {
		mvOsPrintf("NFP (fib): not enough memory\n");
		mvOsFree(fib_hash);
		return MV_NO_RESOURCE;
	}

	mvOsPrintf("NFP (fib) init %d entries, %d bytes\n", NFP_FIB_HASH_SIZE, bytes);

	return MV_OK;
}

/* Move FIB rule from fib_inv_list to fib_hash */
MV_STATUS mvNfpFibRuleValid(NFP_RULE_FIB *fib, MV_LIST_ELEMENT *curr)
{
	MV_U32	hash;

	if (!(fib->flags & (NFP_F_FIB_ARP_INV | NFP_F_FIB_BRIDGE_INV))) {
		/* FIB rule became ready */

		/* Delete FIB rule from incomplete list */
		mvListDel(curr);

		/* Add fib to hash table */
		hash = mvNfpFibRuleHash(fib->family, fib->srcL3, fib->dstL3);
		fib->next = fib_hash[hash];
		fib_hash[hash] = fib;

#ifdef NFP_CT
		/* Update incomplete CT entires */
		mvNfpCtRuleFibUpdate(fib);
#endif /* NFP_CT */

		return MV_OK;
	}
	return MV_BAD_PARAM;
}

static void mvNfpFibRulePrint(NFP_RULE_FIB *fib)
{
	if (fib->family == MV_INET)
		mvOsPrintf("IPv4: " MV_IPQUAD_FMT "->" MV_IPQUAD_FMT"\n",
			   MV_IPQUAD(fib->srcL3), MV_IPQUAD(fib->dstL3));
	else
		mvOsPrintf("IPv6: " MV_IP6_FMT "->" MV_IP6_FMT"\n",
					MV_IP6_ARG(fib->srcL3), MV_IP6_ARG(fib->dstL3));

	mvOsPrintf("     mh=%2.2x:%2.2x, da=" MV_MACQUAD_FMT ", sa=" MV_MACQUAD_FMT,
				((MV_U8 *) &fib->mh)[0], ((MV_U8 *) &fib->mh)[1], MV_MACQUAD(fib->da), MV_MACQUAD(fib->sa));
#ifdef NFP_CT
	mvOsPrintf("\n     fib=%p, oif=%d, ref=%d, age=%d, ct_ref_count=%d, ct_hwf_ref_count=%d\n",
				fib, fib->oif, fib->ref, fib->age, fib->ct_ref_count, fib->ct_hwf_ref_count);
#else
	mvOsPrintf("\n     fib=%p, oif=%d, ref=%d, age=%d\n",
				fib, fib->oif, fib->ref, fib->age);
#endif /* NFP_CT */

	if (fib->flags & NFP_F_FIB_ALL_FLAGS)
		mvOsPrintf("     Flags: ");
	if (fib->flags & NFP_F_FIB_HWF)
		mvOsPrintf("NFP_F_FIB_HWF ");
	if (fib->flags & NFP_F_FIB_BRIDGE_INV)
		mvOsPrintf("NFP_F_FIB_BRIDGE_INV ");
	if (fib->flags & NFP_F_FIB_ARP_INV)
		mvOsPrintf("NFP_F_FIB_ARP_INV ");

	mvOsPrintf("\n");
}

MV_STATUS mvNfpArpRuleAdd(NFP_RULE_ARP *arp2)
{
	MV_U32          hash;
	NFP_RULE_FIB    *fib;
	NFP_RULE_ARP    *arp;
	MV_LIST_ELEMENT	*curr, *tmp;

	if (!nfp_arp_hash || !fib_hash || !fib_inv_list)
		return MV_FAIL;

	/* Check if such rule already exist - update it */
	arp = mvNfpArpLookup(arp2->family, arp2->nextHopL3);
	if (arp) {
		/* Rule exists - update it and the relevant FIB entries */
		mvOsMemcpy(arp, arp2, sizeof(NFP_RULE_ARP));
		mvNfpArpRuleUpdateFibEntries(arp);
	} else { /* Add new rule to ARP hash table */
		arp = (NFP_RULE_ARP *)mvOsMalloc(sizeof(NFP_RULE_ARP));
		if (!arp) {
			mvOsPrintf("%s: NFP (arp) OOM\n", __func__);
			return MV_FAIL;
		}
		mvOsMemcpy(arp, arp2, sizeof(NFP_RULE_ARP));
		hash = mvNfpArpRuleHash(arp2->family, arp2->nextHopL3);
		arp->next = nfp_arp_hash[hash];
		nfp_arp_hash[hash] = arp;
	}

	/* Update incomplete FIB entires */
	curr = fib_inv_list->next;
	while (curr) {
		fib = (NFP_RULE_FIB *)curr->data;
		tmp = curr->next;
		if ((arp2->family == fib->family) &&
			(l3_addr_eq(arp2->family, arp2->nextHopL3, fib->defGtwL3))) {

			/* Found incomplete FIB entry - copy DA from arp2 */
			mvOsMemcpy((MV_U8 *)&fib->da, (MV_U8 *)&arp2->da, MV_MAC_ADDR_SIZE);
			fib->flags &= ~NFP_F_FIB_ARP_INV;

#ifdef NFP_BRIDGE
			if (fib->flags & NFP_F_FIB_BRIDGE_INV) {
				NFP_IF_MAP      *outIfMap;
#ifdef NFP_FDB_MODE
				NFP_RULE_FDB *fdb;

				/* Check FDB hash */
				fdb = mvNfpFdbLookup(fib->oif, fib->da);
				if (fdb) {
					outIfMap = mvNfpIfMapGet(fdb->if_index);
#else
				NFP_RULE_BRIDGE *bridge;

				/* Check BRIDGE hash */
				bridge = mvNfpBridgeLookup(fib->da, fib->sa, fib->oif);
				if (bridge) {
					outIfMap = mvNfpIfMapGet(bridge->oif);
#endif /* NFP_FDB_MODE */
					fib->oif = outIfMap->ifIdx;
					fib->mh = outIfMap->txMh;
					fib->flags &= ~NFP_F_FIB_BRIDGE_INV;
				} else {
					NFP_WARN("%s: bridgeLookup failed, oif=%d, da="MV_MACQUAD_FMT"\n",
						__func__, fib->oif, MV_MACQUAD(fib->da));
				}
			}
#endif /* NFP_BRIDGE */

			mvNfpFibRuleValid(fib, curr);
		}
		curr = tmp;
	}
	return MV_OK;
}

/* Update all routes with rule2->defGtwL3 */
MV_STATUS mvNfpArpRuleUpdateFibEntries(NFP_RULE_ARP *arp)
{
	int		i;
	NFP_RULE_FIB	*fib;

	if (!fib_hash)
		return MV_FAIL;

	/* Update relevant FIB rules */
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		while (fib) {
			if ((arp->family == fib->family) &&
				(l3_addr_eq(arp->family, arp->nextHopL3, fib->defGtwL3)))
				mvOsMemcpy((MV_U8 *)&fib->da, (MV_U8 *)&arp->da, MV_MAC_ADDR_SIZE);
			fib = fib->next;
		}
	}
	return MV_OK;
}

/* Delete ARP Rule and invalidate all routes with rule2->defGtwL3 */
MV_STATUS mvNfpArpRuleDel(NFP_RULE_ARP *arp2)
{
	int				i;
	MV_U32          hash;
	NFP_RULE_ARP	*arp, *arp_prev;
	NFP_RULE_FIB	*fib, *fib_prev;

	if (!nfp_arp_hash || !fib_hash)
		return MV_FAIL;

	/* Find rule in ARP hash table and delete it */
	hash = mvNfpArpRuleHash(arp2->family, arp2->nextHopL3);

	arp = nfp_arp_hash[hash];
	arp_prev = NULL;

	while (arp) {
		if (mvNfpArpRuleCmp(arp2->family, arp2->nextHopL3, arp)) {
			/* Found: delete ARP rule */
			if (arp_prev)
				arp_prev->next = arp->next;
			else
				nfp_arp_hash[hash] = arp->next;

			NFP_DBG("NFP (arp) del %p\n", arp);
			mvOsFree(arp);
			break;
		}
		arp_prev = arp;
		arp = arp->next;
	}
	if (arp == NULL)
		return MV_NOT_FOUND;

	/* Invalidate relevant FIB rules */
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		fib_prev = NULL;
		while (fib) {
			if ((arp2->family == fib->family) &&
				(l3_addr_eq(arp2->family, arp2->nextHopL3, fib->defGtwL3))) {

				/* Invalidate FIB route. FIXME - update HWF rule */
				fib->flags |= NFP_F_FIB_ARP_INV;

				/* Remove FIB rule from hash table */
				if (fib_prev)
					fib_prev->next = fib->next;
				else
					fib_hash[i] = fib->next;

				/* Add FIB rule to incomplete list */
				mvListAddHead(fib_inv_list, (MV_ULONG)fib);
#ifdef NFP_CT
				/* Invalidate relevant CT rules */
				mvNfpCtRuleFibInvalidate(fib);
#endif /* NFP_CT */
			}
			fib_prev = fib;
			fib = fib->next;
		}
	}
	return MV_OK;
}

/* If exist routing entires with the rule2->defGtwL3 - prevent aging */
MV_STATUS mvNfpArpRuleAge(NFP_RULE_ARP *arp2)
{
	int		i;
	NFP_RULE_FIB	*fib;

	arp2->age = 0;
	if (!nfp_arp_hash || !fib_hash)
		return MV_FAIL;

	/* FIXME: Find rule in ARP hash table */

	/* prevent ARP aging if there are at least one valid FIB rule use it */
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		while (fib) {
			if ((arp2->family == fib->family) &&
				(l3_addr_eq(arp2->family, arp2->nextHopL3, fib->defGtwL3))) {

				/* Prevent neigbour aging */
				arp2->age = 1;
				return MV_OK;
			}
			fib = fib->next;
		}
	}
	return MV_NOT_FOUND;
}

static NFP_RULE_FIB *mvNfpFibRuleInvalidLookup(int family, const MV_U8 *srcL3, const MV_U8 *dstL3)
{
	MV_LIST_ELEMENT	*curr;
	NFP_RULE_FIB *currRule;

	if (!fib_inv_list)
		return NULL;

	curr = fib_inv_list->next;
	while (curr) {
		currRule = (NFP_RULE_FIB *)curr->data;
		if (mvNfpFibRuleCmp(family, srcL3, dstL3, currRule))
			return currRule;

		curr = curr->next;
	}
	return NULL;
}

/* prevent FIB aging if there are 5 tuple rules based on this FIB rule */
MV_STATUS mvNfpFibRuleAge(NFP_RULE_FIB *fib2)
{
	NFP_RULE_FIB *fib;

	fib = mvNfpFibRuleInvalidLookup(fib2->family, fib2->srcL3, fib2->dstL3);
	if (fib) {
#ifdef NFP_CT
		fib2->age = (fib->ct_hwf_ref_count > 0) ? 1 : fib->age;
		fib->age = (fib->ct_hwf_ref_count > 0) ? fib->age : 0;
#else
		fib2->age = fib->age;
		fib->age = 0;
#endif /* NFP_CT */
		return MV_OK;
	}

	fib = mvNfpFibLookup(fib2->family, fib2->srcL3, fib2->dstL3);
	if (fib) {
#ifdef NFP_CT
			fib2->age = (fib->ct_hwf_ref_count > 0) ? 1 : fib->age;
			fib->age = (fib->ct_hwf_ref_count > 0) ? fib->age : 0;
#else
			fib2->age = fib->age;
			fib->age = 0;
#endif /* NFP_CT */
		return MV_OK;
	}

	fib2->age = 0;
	return MV_NOT_FOUND;
}

MV_STATUS mvNfpFibRuleAdd(NFP_RULE_FIB *fib2)
{
	MV_U32 hash, ref = 0;
	NFP_RULE_FIB *fib;
	MV_LIST_ELEMENT	*element;
	NFP_IF_MAP      *outIfMap;

	/* sanity checks */
	if (!fib_hash || !fib_inv_list)
		return MV_FAIL;

	/* If this rule currently exists in the Invalid Rules DB, delete it */
	fib = mvNfpFibRuleInvalidLookup(fib2->family, fib2->srcL3, fib2->dstL3);
	if (fib) {
		element = mvListFind(fib_inv_list, (MV_ULONG)fib);
		mvListDel(element);
	}
	/* If this rule currently exists as a valid rule, we want to update it */
	/* so we save the reference count, but delete and re-add it */
	fib = mvNfpFibLookup(fib2->family, fib2->srcL3, fib2->dstL3);
	if (fib) {
		ref = fib->ref;
		mvNfpFibRuleDel(fib);
	}

	outIfMap = mvNfpIfMapGet(fib2->oif);
	memcpy(fib2->sa, outIfMap->mac, MV_MAC_ADDR_SIZE);
	fib2->mh = outIfMap->txMh;

	if (fib2->flags & NFP_F_FIB_ARP_INV) {
#ifdef NFP_PPP
		if (outIfMap->flags & NFP_F_MAP_PPPOE) {
			mvOsMemcpy(fib2->da, outIfMap->remoteMac, MV_MAC_ADDR_SIZE);
			fib2->flags &= ~NFP_F_FIB_ARP_INV;
		} else {
#endif /* NFP_PPP */
			NFP_RULE_ARP  *arp;
			if (fib2->family == MV_INET) {
				NFP_DBG("%s: ArpLookup, defGtwL3="MV_IPQUAD_FMT"\n",
							__func__, MV_IPQUAD(fib2->defGtwL3));
			} else {
				NFP_DBG("%s: ArpLookup, defGtwL3="MV_IP6_FMT"\n",
							__func__, MV_IP6_ARG(fib2->defGtwL3));
			}

			/* lookup for ARP entry */
			arp = mvNfpArpLookup(fib2->family, fib2->defGtwL3);
			if (arp) {
				mvOsMemcpy(fib2->da, arp->da, MV_MAC_ADDR_SIZE);
				fib2->flags &= ~NFP_F_FIB_ARP_INV;
			} else {
				if (fib2->family == MV_INET) {
					NFP_WARN("%s: ArpLookup failed, defGtwL3="MV_IPQUAD_FMT"\n",
								__func__, MV_IPQUAD(fib2->defGtwL3));
				} else {
					NFP_WARN("%s: ArpLookup failed, defGtwL3="MV_IP6_FMT"\n",
								__func__, MV_IP6_ARG(fib2->defGtwL3));
				}
			}
#ifdef NFP_PPP
		}
#endif /* NFP_PPP */
	}
#ifdef NFP_BRIDGE
	/* ARP (da) is known, bridge (oif) is unknown */
	if ((fib2->flags & (NFP_F_FIB_BRIDGE_INV | NFP_F_FIB_ARP_INV)) == NFP_F_FIB_BRIDGE_INV) {

#ifdef NFP_FDB_MODE
		NFP_RULE_FDB  *fdb;

		/* lookup for FDB entry */
		fdb = mvNfpFdbLookup(fib2->oif, fib2->da);
		if (fdb) {
			outIfMap = mvNfpIfMapGet(fdb->if_index);
#else
		NFP_RULE_BRIDGE  *bridge;

		/* lookup for Bridging entry */
		bridge = mvNfpBridgeLookup(fib2->da, fib2->sa, fib2->oif);
		if (bridge) {
			outIfMap = mvNfpIfMapGet(bridge->oif);
#endif /* NFP_FDB_MODE */
			fib2->oif = outIfMap->ifIdx;
			fib2->mh = outIfMap->txMh;
			fib2->flags &= ~NFP_F_FIB_BRIDGE_INV;
		} else {
			NFP_WARN("%s: mvNfpFdbLookup failed, oif=%d, mac="MV_MACQUAD_FMT"\n",
					__func__, fib2->oif, MV_MACQUAD(fib2->da));
		}
	}
#endif /* NFP_BRIDGE */

	if (fib2->flags & (NFP_F_FIB_BRIDGE_INV | NFP_F_FIB_ARP_INV)) {
		/* Put FIB entry to incomplete list */
		fib = (NFP_RULE_FIB *)mvOsMalloc(sizeof(NFP_RULE_FIB));
		if (!fib) {
			mvOsPrintf("%s: NFP (fib) OOM\n", __func__);
			return MV_FAIL;
		}
		mvOsMemcpy(fib, fib2, sizeof(NFP_RULE_FIB));
		mvListAddHead(fib_inv_list, (MV_ULONG)fib);
		return MV_OK;
	}

	/* FIB Entry is valid - add it to hash */
	hash = mvNfpFibRuleHash(fib2->family, fib2->srcL3, fib2->dstL3);

	fib = (NFP_RULE_FIB *)mvOsMalloc(sizeof(NFP_RULE_FIB));
	if (!fib) {
		mvOsPrintf("%s: NFP (fib) OOM\n", __func__);
		return MV_FAIL;
	}
	mvOsMemcpy(fib, fib2, sizeof(NFP_RULE_FIB));
	fib->ref = ref;

	fib->next = fib_hash[hash];
	fib_hash[hash] = fib;

#ifdef NFP_CT
	/* Update incomplete CT entires */
	mvNfpCtRuleFibUpdate(fib);
#endif /* NFP_CT */

	NFP_DBG("NFP (fib) add %p\n", fib);
	return MV_OK;
}

MV_STATUS mvNfpFibRuleDel(NFP_RULE_FIB *fib2)
{
	MV_U32 hash;
	NFP_RULE_FIB *fib, *prev;
	MV_LIST_ELEMENT	*element;

	/* If this rule currently exists in the Invalid Rules DB, delete it */
	fib = mvNfpFibRuleInvalidLookup(fib2->family, fib2->srcL3, fib2->dstL3);
	if (fib) {
		element = mvListFind(fib_inv_list, (MV_ULONG)fib);
		mvListDel(element);
		mvOsFree(fib);
		return MV_OK;
	}

	hash = mvNfpFibRuleHash(fib2->family, fib2->srcL3, fib2->dstL3);
	fib = fib_hash[hash];

	prev = NULL;
	while (fib) {
		if (mvNfpFibRuleCmp(fib2->family, fib2->srcL3, fib2->dstL3, fib)) {
			if (prev)
				prev->next = fib->next;
			else
				fib_hash[hash] = fib->next;

			NFP_DBG("NFP (fib) del %p\n", fib);
			mvOsFree(fib);
#ifdef NFP_CT
			/* Invalidate relevant CT rules */
			mvNfpCtRuleFibInvalidate(fib2);
#endif /* NFP_CT */
			return MV_OK;
		}
		prev = fib;
		fib = fib->next;
	}
	return MV_NOT_FOUND;
}

void mvNfpFibClean(void)
{
	int i;
	NFP_RULE_FIB *fib, *next;
	MV_LIST_ELEMENT	*curr, *tmp;

	/* Clean FIB incomplete rules list */
	if (fib_inv_list) {
		curr = fib_inv_list->next;
		while (curr) {
			tmp = curr->next;
			mvListDel(curr);
			curr = tmp;
		}
	}

	/* Clean FIB hash table */
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		while (fib) {
			next = fib->next;
			mvOsFree(fib);
			fib = next;
		}
		fib_hash[i] = NULL;
	}
}

void mvNfpFibDestroy(void)
{
	if (fib_hash != NULL)
		mvOsFree(fib_hash);

	mvListDestroy(fib_inv_list);
}

void mvNfpFibDump(void)
{
	MV_U32 i;
	NFP_RULE_FIB *fib;
	MV_LIST_ELEMENT	*curr;

	mvOsPrintf("\n(fib_hash)\n");
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];

		while (fib) {
			mvOsPrintf(" [%5d] ", i);
			mvNfpFibRulePrint(fib);
			fib = fib->next;
		}
	}
	if (!fib_inv_list) {
		mvOsPrintf("(fib_inv_list) does not exist\n");
		return;
	}
	mvOsPrintf("(fib_inv_list)\n");
	curr = fib_inv_list->next;
	while (curr) {
		fib = (NFP_RULE_FIB *)curr->data;
		mvNfpFibRulePrint(fib);
		curr = curr->next;
	}
}


static void mvNfpArpRulePrint(NFP_RULE_ARP *arp)
{
	if (arp->family == MV_INET)
		mvOsPrintf("IPv4: nextHop=" MV_IPQUAD_FMT, MV_IPQUAD(arp->nextHopL3));
	else
		mvOsPrintf("IPv6: nextHop=" MV_IP6_FMT, MV_IP6_ARG(arp->nextHopL3));

	mvOsPrintf(" da=" MV_MACQUAD_FMT " age=%u\n",
				MV_MACQUAD(arp->da), (mvNfpArpRuleAge(arp) == MV_OK) ? 1 : 0);
}

MV_STATUS _INIT mvNfpArpInit(void)
{
	MV_U32 bytes = sizeof(NFP_RULE_ARP *) * NFP_ARP_HASH_SIZE;

	nfp_arp_hash = (NFP_RULE_ARP **)mvOsMalloc(bytes);
	if (nfp_arp_hash == NULL) {
		mvOsPrintf("NFP (arp): not enough memory\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(nfp_arp_hash, 0, bytes);

	mvOsPrintf("NFP (arp) init %d entries, %d bytes\n", NFP_ARP_HASH_SIZE, bytes);

	return MV_OK;
}

void mvNfpArpDestroy(void)
{
	if (nfp_arp_hash != NULL)
		mvOsFree(nfp_arp_hash);
}

void mvNfpArpDump(void)
{
	MV_U32 i;
	NFP_RULE_ARP *arp;

	mvOsPrintf("\n(arp)\n");
	for (i = 0; i < NFP_ARP_HASH_SIZE; i++) {
		arp = nfp_arp_hash[i];

		while (arp) {
			mvOsPrintf(" [%5d] ", i);
			mvNfpArpRulePrint(arp);
			arp = arp->next;
		}
	}
}

void mvNfpFlushFib(int ifindex)
{
	NFP_RULE_FIB *fib;
	MV_LIST_ELEMENT	*curr;
	int i;
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];

		while (fib) {
			if (fib->oif == ifindex)
				mvNfpFibRuleDel(fib);

			fib = fib->next;
		}
	}
	if (fib_inv_list) {
		curr = fib_inv_list->next;
		while (curr) {
			fib = (NFP_RULE_FIB *)curr->data;
			if (fib->oif == ifindex)
				mvNfpFibRuleDel(fib);
			curr = curr->next;
		}
	}
}

