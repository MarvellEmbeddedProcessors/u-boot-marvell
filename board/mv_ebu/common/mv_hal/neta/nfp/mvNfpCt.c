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
* mvNfpCt.c - Marvell Fast Network Processing
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

/* includes */
#include "mvOs.h"
#include "mvDebug.h"
#include "mvList.h"
#include "gbe/mvNeta.h"
#include "mvNfpDefs.h"
#include "mvNfp.h"

MV_LIST_ELEMENT *ct_inv_list = NULL;
NFP_RULE_CT **ct_hash = NULL;
int ct_iterator_row = 0;
#ifdef NFP_LIMIT
MV_LIST_ELEMENT *tbfs;
#endif /* NFP_LIMIT */

MV_STATUS _INIT mvNfpCtInit(MV_VOID)
{
	MV_U32 bytes = sizeof(NFP_RULE_CT *) * NFP_CT_HASH_SIZE;

	ct_hash = (NFP_RULE_CT **)mvOsMalloc(bytes);
	if (ct_hash == NULL) {
		mvOsPrintf("NFP (ct): not enough memory for CT database\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(ct_hash, 0, bytes);
	ct_inv_list = mvListCreate();
	if (ct_inv_list == NULL) {
		mvOsPrintf("NFP (ct): not enough memory for CT database\n");
		mvOsFree(ct_hash);
		return MV_NO_RESOURCE;
	}

	mvOsPrintf("NFP (ct) init %d entries, %d bytes\n", NFP_CT_HASH_SIZE, bytes);
#ifdef NFP_LIMIT
	tbfs = mvListCreate();
	if (tbfs == NULL) {
		mvOsPrintf("NFP (ct): not enough memory for TBF database\n");
		mvListDestroy(ct_inv_list);
		mvOsFree(ct_hash);
		return MV_NO_RESOURCE;
	}
#endif /* NFP_LIMIT */
	return MV_OK;
}

static INLINE MV_U32 mvNfpCtHash(NFP_RULE_CT *ct)
{
	MV_U32 hash = mv_jhash_2addr(ct->family, (const MV_U8 *)&ct->srcL3, (const MV_U8 *)&ct->dstL3,
					(ct->ports | ct->proto), nfp_jhash_iv);
	hash &= NFP_CT_HASH_MASK;
	return hash;
}

static INLINE NFP_RULE_CT *mvNfpCtLookup(NFP_RULE_CT *ct2)
{
	MV_U32 hash;
	NFP_RULE_CT *ct;

	if (!ct_hash)
		return NULL;

	hash = mvNfpCtHash(ct2);
	ct = ct_hash[hash];
	while (ct) {
		if ((ct->family == ct2->family) &&
			l3_addr_eq(ct->family, ct->srcL3, ct2->srcL3) &&
			l3_addr_eq(ct->family, ct->dstL3, ct2->dstL3) &&
			(ct->ports == ct2->ports) && (ct->proto == ct2->proto))
			return ct;
		ct = ct->next;
	}

	return NULL;
}

/* Move CT rule from ct_inv_list to ct_hash */
static MV_STATUS mvNfpCtRuleValid(NFP_RULE_CT *ct, MV_LIST_ELEMENT *curr)
{
	MV_U32	hash;

	if (((ct->flags & NFP_F_CT_DROP) || !(ct->flags & NFP_F_CT_FIB_INV)) && !(ct->flags & NFP_F_CT_NOT_EXIST)) {
		/* CT rule became ready */

		/* Delete CT rule from incomplete list */
		mvListDel(curr);

		/* Add ct to hash table */
		hash = mvNfpCtHash(ct);
		ct->next = ct_hash[hash];
		ct_hash[hash] = ct;
		ct->visited = 0;

		return MV_OK;
	}
	return MV_BAD_PARAM;
}

MV_VOID mvNfpCtRuleFibUpdate(NFP_RULE_FIB *fib)
{
	NFP_RULE_CT	*ct;
	MV_LIST_ELEMENT	*curr, *tmp;
	int i;

	/* Update FIB rule pointers in CT hash */
	for (i = 0; i < NFP_CT_HASH_SIZE; i++) {
		ct = ct_hash[i];
		while (ct) {
			if ((fib->family == ct->family) &&
				(l3_addr_eq(fib->family, fib->srcL3, ct->srcL3)) &&
#ifdef NFP_NAT
				((!(ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) ||
				((ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, (MV_U8 *)&(ct->new_dip)))))) {
#else
				(l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) {
#endif /* NFP_NAT */
				/* Invalidate CT route */
				if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
					(ct->fib)->ct_ref_count--;
				if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
					(ct->fib)->ct_hwf_ref_count--;
				ct->flags &= ~NFP_F_CT_FIB_INV;
				ct->fib = fib;
				(ct->fib)->ct_ref_count++;
				if (ct->flags & NFP_F_CT_HWF)
					(ct->fib)->ct_hwf_ref_count++;
			}
			ct = ct->next;
		}
	}

	/* Update FIB rule pointers in CT invalid list */
	if (!ct_inv_list)
		return;

	curr = ct_inv_list->next;
	while (curr) {
		ct = (NFP_RULE_CT *)curr->data;
		tmp = curr->next;
		if ((fib->family == ct->family) &&
			(l3_addr_eq(fib->family, fib->srcL3, ct->srcL3)) &&
#ifdef NFP_NAT
			((!(ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) ||
			((ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, (MV_U8 *)&(ct->new_dip)))))) {
#else
			(l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) {
#endif /* NFP_NAT */
			/* Found incomplete CT entry with a matching FIB rule */
			if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_ref_count--;
			if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_hwf_ref_count--;
			ct->fib = fib;
			(ct->fib)->ct_ref_count++;
			if (ct->flags & NFP_F_CT_HWF)
				(ct->fib)->ct_hwf_ref_count++;
			ct->flags &= ~NFP_F_CT_FIB_INV;
			mvNfpCtRuleValid(ct, curr);
		}
		curr = tmp;
	}
}

MV_VOID mvNfpCtRuleFibInvalidate(NFP_RULE_FIB *fib)
{
	int i;
	NFP_RULE_CT *ct, *ct_prev;
	MV_LIST_ELEMENT	*curr;

	for (i = 0; i < NFP_CT_HASH_SIZE; i++) {
		ct = ct_hash[i];
		ct_prev = NULL;
		while (ct) {
			if ((fib->family == ct->family) &&
				(l3_addr_eq(fib->family, fib->srcL3, ct->srcL3)) &&
#ifdef NFP_NAT
				((!(ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) ||
				((ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, (MV_U8 *)&(ct->new_dip)))))) {
#else
				(l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) {
#endif /* NFP_NAT */
				/* Invalidate CT route */
				if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
					(ct->fib)->ct_ref_count--;
				if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
					(ct->fib)->ct_hwf_ref_count--;
				ct->flags |= NFP_F_CT_FIB_INV;
				ct->fib = NULL;

				if (!(ct->flags & NFP_F_CT_DROP)) {
					/* Remove CT rule from hash table only if it is not a DROP rule */
					if (ct_prev)
						ct_prev->next = ct->next;
					else
						ct_hash[i] = ct->next;

					/* Add CT rule to incomplete list */
					mvListAddHead(ct_inv_list, (MV_ULONG)ct);
				}
			}
			ct_prev = ct;
			ct = ct->next;
		}
	}

	/* Invalidate FIB rule pointers in CT invalid list (possible in classification context rules) */
	if (!ct_inv_list)
		return;

	curr = ct_inv_list->next;
	while (curr) {
		ct = (NFP_RULE_CT *)curr->data;
		if ((fib->family == ct->family) &&
			(l3_addr_eq(fib->family, fib->srcL3, ct->srcL3)) &&
#ifdef NFP_NAT
			((!(ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) ||
			((ct->flags & NFP_F_CT_DNAT) && (l3_addr_eq(fib->family, fib->dstL3, (MV_U8 *)&(ct->new_dip)))))) {
#else
			(l3_addr_eq(fib->family, fib->dstL3, ct->dstL3))) {
#endif /* NFP_NAT */
			if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_ref_count--;
			if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_hwf_ref_count--;
			ct->flags |= NFP_F_CT_FIB_INV;
			ct->fib = NULL;
		}
		curr = curr->next;
	}
}


static MV_VOID mvNfpCtRulePrint(NFP_RULE_CT *ct)
{
#ifdef NFP_CLASSIFY
	int i;
#endif /* NFP_CLASSIFY */

	if (ct->family == MV_INET)
		mvOsPrintf("IPv4: "MV_IPQUAD_FMT":%d->"MV_IPQUAD_FMT":%d",
			MV_IPQUAD(ct->srcL3), MV_16BIT_BE(ct->ports & 0xFFFF),
			MV_IPQUAD(ct->dstL3), MV_16BIT_BE(ct->ports >> 16));
	else /* MV_INET6 */
		mvOsPrintf(" IPv6: "MV_IP6_FMT":%d->"MV_IP6_FMT":%d",
			MV_IP6_ARG(ct->srcL3), MV_16BIT_BE(ct->ports & 0xFFFF),
			MV_IP6_ARG(ct->dstL3), MV_16BIT_BE(ct->ports >> 16));

	if (ct->proto == MV_IP_PROTO_TCP)
		mvOsPrintf(" TCP - ");
	else if (ct->proto == MV_IP_PROTO_UDP)
		mvOsPrintf(" UDP - ");
	else
		mvOsPrintf(" %-2d - ", ct->proto);

	if (ct->flags & NFP_F_CT_DROP)
		mvOsPrintf("Drop, ");
	else
		mvOsPrintf("Forward, ");

	mvOsPrintf("flags=0x%04x, ", ct->flags);

#ifdef NFP_NAT
	if (ct->flags & NFP_F_CT_DNAT)
		mvOsPrintf("DNAT: "MV_IPQUAD_FMT":%d, ", MV_IPQUAD(((MV_U8 *)&ct->new_dip)), MV_16BIT_BE(ct->new_dport));

	if (ct->flags & NFP_F_CT_SNAT)
		mvOsPrintf("SNAT: "MV_IPQUAD_FMT":%d, ", MV_IPQUAD(((MV_U8 *)&ct->new_sip)), MV_16BIT_BE(ct->new_sport));
#endif /* NFP_NAT */


#ifdef NFP_CLASSIFY
	if (ct->flags & NFP_F_CT_NOT_EXIST)
		mvOsPrintf("This rule is in classification context\n");

	if (ct->flags & NFP_F_CT_SET_MH)
		mvOsPrintf("MH=0x%X ", ct->mh);

	if (ct->flags & NFP_F_CT_SET_TXP)
		mvOsPrintf("txp=0x%X ", ct->txp);
#endif /* NFP_CLASSIFY */

	mvOsPrintf("age=%d, hit=%d\n", ct->age, ct->hit_cntr);

#ifdef NFP_CLASSIFY
	if (ct->flags & NFP_F_CT_SET_TXQ) {
		mvOsPrintf(" DSCP to TXQ Map: ");
		mvOsPrintf("DSCP        TXQ\n");
		for (i = 0; i <= NFP_DSCP_MAP_GLOBAL; i++) {
			if (ct->txq_map[i].valid) {
				if (i == NFP_DSCP_MAP_GLOBAL)
					mvOsPrintf("           Global     %2d\n", ct->txq_map[i].txq);
				else
					mvOsPrintf("           %2d         %2d\n", i, ct->txq_map[i].txq);
			}
		}
	}

	if (ct->flags & NFP_F_CT_SET_DSCP) {
		mvOsPrintf(" DSCP Map: ");
		mvOsPrintf("Old        New\n");
		for (i = 0; i <= NFP_DSCP_MAP_GLOBAL; i++) {
			if (ct->dscp_map[i].valid) {
				if (i == NFP_DSCP_MAP_GLOBAL)
					mvOsPrintf("           Global     %2d\n", ct->dscp_map[i].new_dscp);
				else
					mvOsPrintf("           %2d         %2d\n", i, ct->dscp_map[i].new_dscp);
			}
		}
	}

	if (ct->flags & NFP_F_CT_SET_VLAN_PRIO) {
		mvOsPrintf(" VPri Map: ");
		mvOsPrintf("Old        New\n");
		for (i = 0; i <= NFP_VPRI_MAP_GLOBAL; i++) {
			if (ct->vpri_map[i].valid) {
				if (i == NFP_VPRI_MAP_GLOBAL)
					mvOsPrintf("           Global     %2d\n", ct->vpri_map[i].new_prio);
				else
					mvOsPrintf("           %2d         %2d\n", i, ct->vpri_map[i].new_prio);
			}
		}
	}
#endif /* NFP_CLASSIFY */
}

static NFP_RULE_CT *mvNfpCtRuleInvalidLookup(NFP_RULE_CT *ct2)
{
	MV_LIST_ELEMENT	*curr;
	NFP_RULE_CT *ct;

	if (!ct_inv_list)
		return NULL;

	curr = ct_inv_list->next;
	while (curr) {
		ct = (NFP_RULE_CT *)curr->data;
		if ((ct->family == ct2->family) &&
			l3_addr_eq(ct->family, ct->srcL3, ct2->srcL3) &&
			l3_addr_eq(ct->family, ct->dstL3, ct2->dstL3) &&
			(ct->ports == ct2->ports) &&
			(ct->proto == ct2->proto))
			return ct;

		curr = curr->next;
	}
	return NULL;
}

MV_STATUS   mvNfpCtRuleAge(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	ct = mvNfpCtRuleInvalidLookup(ct2);
	if (ct) {
		ct2->age = (ct->flags & NFP_F_CT_HWF) ? 1 : ct->age;
		ct->age = (ct->flags & NFP_F_CT_HWF) ? ct->age : 0;
		return MV_OK;
	}

	ct = mvNfpCtLookup(ct2);
	if (ct) {
		ct2->age = (ct->flags & NFP_F_CT_HWF) ? 1 : ct->age;
		ct->age = (ct->flags & NFP_F_CT_HWF) ? ct->age : 0;
		return MV_OK;
	}

	ct2->age = 0;
	return MV_NOT_FOUND;
}

/* create rule from classification context                                           *
 *     - search and update if relevant FIB rule exist                                *
 *     - set NFP_F_CT_NOT_EXIST flag                                                 *
 *     - add rule to invalid rule list (NFP_F_CT_NOT_EXIST is set)                   *
 *     - if mvNfpCtFilterModeSet is called later, then clear NFP_F_CT_NOT_EXIST flag *
 * Return: pointer to created rule                                                   */
NFP_RULE_CT *mvNfpCtClassifyRuleCreate(NFP_RULE_CT *ct2)
{
	/* rule not exist - create rule and mark it as "not exist" */
	NFP_RULE_CT *ct = (NFP_RULE_CT *)mvOsMalloc(sizeof(NFP_RULE_CT));
	if (!ct) {
		mvOsPrintf("%s: OOM\n", __func__);
		return NULL;
	}

	mvOsMemcpy(ct, ct2, sizeof(NFP_RULE_CT));

	ct->flags |= NFP_F_CT_NOT_EXIST;
	ct->flags |= NFP_F_CT_FIB_INV;
	/* this rule is invalid until mvNfpCtFilterModeSet will be called */
	mvListAddHead(ct_inv_list, (MV_ULONG)ct);
	return ct;
}

#ifdef NFP_NAT
MV_STATUS mvNfpCtNatRuleAdd(NFP_RULE_CT *nat2)
{
	NFP_RULE_CT *nat;
	MV_U32 hash;

	/* Update rule if it exists as a valid rule or as an invalid rule */
	nat = mvNfpCtLookup(nat2);
	if (!nat)
		nat = mvNfpCtRuleInvalidLookup(nat2);

	if (nat) {
		if (nat->flags & NFP_F_CT_NOT_EXIST)
			nat->flags &= ~NFP_F_CT_NOT_EXIST;
		if (nat2->flags & NFP_F_CT_SNAT) {
			nat->new_sip = nat2->new_sip;
			nat->new_sport = nat2->new_sport;
			nat->flags |= NFP_F_CT_SNAT;
		}
		if (nat2->flags & NFP_F_CT_DNAT) {
			nat->new_dip = nat2->new_dip;
			nat->new_dport = nat2->new_dport;
			nat->flags |= NFP_F_CT_DNAT;
		}
		/* copy other information to nat2 - classification, tbf, etc.. */
		mvOsMemcpy(nat2, nat, sizeof(NFP_RULE_CT));

		/* delete previous rule, and add it again later (will search for fib again)    */
		/* delete is important because it updates fib->ct_ref_count, rate limit, etc.. */
		mvNfpCtRuleDel(nat);
	}

	nat = (NFP_RULE_CT *)mvOsMalloc(sizeof(NFP_RULE_CT));
	if (!nat) {
		mvOsPrintf("%s: OOM\n", __func__);
		return MV_FAIL;
	}
	mvOsMemcpy(nat, nat2, sizeof(NFP_RULE_CT));

	if (nat->flags & NFP_F_CT_DNAT)
		nat->fib = mvNfpFibLookup(nat2->family, nat2->srcL3, (const MV_U8 *)&(nat2->new_dip));
	else
		nat->fib = mvNfpFibLookup(nat2->family, nat2->srcL3, nat2->dstL3);

	if (nat->fib) {
		(nat->fib)->ct_ref_count++; /* update FIB reference count */
		hash = mvNfpCtHash(nat2);
		nat->next = ct_hash[hash];
		ct_hash[hash] = nat;
		nat->visited = 0;
	} else {
		mvListAddHead(ct_inv_list, (MV_ULONG)nat);
	}

	NFP_DBG("NFP (nat) add %p\n", nat);

	return MV_OK;
}
#endif /* NFP_NAT */

MV_STATUS mvNfpCtFilterModeSet(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct, *new_ct;
	MV_LIST_ELEMENT	*element;
	MV_U32 hash;

	/* The rule can be in one of several initial states, each requires different handling	*/
	/* The rule can exists as valid, exist as invalid or not exist at this point		*/

	ct = mvNfpCtLookup(ct2);
	if (ct) {
		/* Rule exists as valid */
		/* Either it has valid FIB information, or it is a DROP rule */

		if (ct2->flags & NFP_F_CT_DROP) {
			/* Updated rule says DROP, so we don't care if FIB information exists or not */
			ct->flags |= NFP_F_CT_DROP;
		} else {
			/* Updated rule says FORWARD */
			ct->flags &= ~NFP_F_CT_DROP;
			if (ct->flags & NFP_F_CT_FIB_INV) {
				/* need to move this rule to the invalid list */
				new_ct = (NFP_RULE_CT *)mvOsMalloc(sizeof(NFP_RULE_CT));
				if (!new_ct) {
					mvOsPrintf("%s: OOM\n", __func__);
					return MV_FAIL;
				}
				mvOsMemcpy(new_ct, ct, sizeof(NFP_RULE_CT));
#ifdef NFP_LIMIT
				if (new_ct->tbfInfo)
					new_ct->tbfInfo->refCnt++;
#endif /* NFP_LIMIT */
				mvNfpCtRuleDel(ct);
				mvListAddHead(ct_inv_list, (MV_ULONG)new_ct);
			}
		}
		return MV_OK;
	}


	ct = mvNfpCtRuleInvalidLookup(ct2);
	/* Rule exists as invalid */
	if (ct) {
		/* this rule was created by classification API  *
		 *     mark it as "real" rule                   *
		 *     search for relevant fib rule             */
		if (ct->flags & NFP_F_CT_NOT_EXIST) {
			ct->flags &= ~NFP_F_CT_NOT_EXIST;
			ct->flags |= NFP_F_CT_FIB_INV;
			ct->fib = mvNfpFibLookup(ct2->family, ct2->srcL3, ct2->dstL3);
			if (ct->fib) {
				(ct->fib)->ct_ref_count++;
				ct->flags &= ~NFP_F_CT_FIB_INV;
			}
		}
		if (ct2->flags & NFP_F_CT_DROP || !(ct->flags & NFP_F_CT_FIB_INV)) {
			/* need to move this rule to the valid rules database */
			if (ct2->flags & NFP_F_CT_DROP)
				ct->flags |= NFP_F_CT_DROP;
			element = mvListFind(ct_inv_list, (MV_ULONG)ct);
			if (!element)
				return MV_FAIL;
			return mvNfpCtRuleValid(ct, element);
		} else {
			/* no need to do anything special - clearing DROP flag just for clarity, it is already cleared */
			ct->flags &= ~NFP_F_CT_DROP;
			return MV_OK;
		}
	}


	/* Rule doesn't exist, need to create  a new one */
	ct = (NFP_RULE_CT *)mvOsMalloc(sizeof(NFP_RULE_CT));
	if (!ct) {
		mvOsPrintf("%s: OOM\n", __func__);
		return MV_FAIL;
	}

	mvOsMemcpy(ct, ct2, sizeof(NFP_RULE_CT));

	ct->flags |= NFP_F_CT_FIB_INV;
	ct->fib = mvNfpFibLookup(ct2->family, ct2->srcL3, ct2->dstL3);
	if (ct->fib) {
		(ct->fib)->ct_ref_count++;
		ct->flags &= ~NFP_F_CT_FIB_INV;
	}

	if ((ct->flags & NFP_F_CT_DROP) || !(ct->flags & NFP_F_CT_FIB_INV)) {
		hash = mvNfpCtHash(ct2);
		ct->next = ct_hash[hash];
		ct_hash[hash] = ct;
		ct->visited = 0;
	} else {
		mvListAddHead(ct_inv_list, (MV_ULONG)ct);
	}

	NFP_DBG("NFP (ct filter mode) set %p\n", ct);

	return MV_OK;
}

#ifdef NFP_LIMIT
void mvNfpCtTbfsDump(void)
{
	MV_LIST_ELEMENT	*curr;
	NFP_TBF_INFO *tbf_data;
	mvOsPrintf("Tbfs list:\n");
	if (!tbfs)
		return;
	curr = tbfs->next;
	while (curr) {
		tbf_data = (NFP_TBF_INFO *)curr->data;
		mvOsPrintf("index: %d,  limit: %d,  burst: %d, refCnt: %d\n", tbf_data->index,
				tbf_data->creditPerTick * mvOsGetTicksFreq(), tbf_data->creditMax, tbf_data->refCnt);
		curr = curr->next;
	}
	mvOsPrintf("\n");
}

/* limit units = KBytes/sec, burst_limit units = Kbytes */
int mvNfpTbfCreate(int limit, int burst_limit)
{
	NFP_TBF_INFO *tbf;

	if (limit < 0 || burst_limit < 0 || !tbfs)
		return -1;

	tbf = mvOsMalloc(sizeof(NFP_TBF_INFO));
	if (!tbf) {
		mvOsPrintf("%s: OOM\n", __func__);
		return -1;
	}

	tbf->refCnt = 0;
	tbf->credit = tbf->creditMax = (burst_limit * 1000); /* Bytes */
	/* convert from KBytes/sec to Bytes/tick */
	tbf->creditPerTick = (limit * 1000) / mvOsGetTicksFreq(); /* Bytes per tick */
	if (tbf->creditPerTick == 0)
		tbf->maxElapsedTicks = 0;
	else
		tbf->maxElapsedTicks = tbf->creditMax / tbf->creditPerTick; /* ticks */
	tbf->lastUpdate = mvOsGetTicks(); /* ticks */

	/* calculate index */
	if (tbfs->next)
		tbf->index = ((NFP_TBF_INFO *)(tbfs->next->data))->index + 1;
	else
		tbf->index = 0;

	if (!mvListAddHead(tbfs, (MV_U32)tbf)) {
		mvOsPrintf("%s: OOM\n", __func__);
		mvOsFree(tbf);
		return -1;
	}
	return tbf->index;
}

NFP_TBF_INFO *mvNfpTbfGet(int tbf)
{
	MV_LIST_ELEMENT	*curr;
	NFP_TBF_INFO *tbf_data;
	if (tbf < 0 || !tbfs)
		return NULL;
	curr = tbfs->next;
	while (curr) {
		tbf_data = (NFP_TBF_INFO *)curr->data;
		if (tbf_data->index == tbf)
			return tbf_data;
		curr = curr->next;
	}
	return NULL;
}

MV_STATUS mvNfpTbfDel(int tbf)
{
	MV_LIST_ELEMENT *element;
	NFP_TBF_INFO *tbf_data = mvNfpTbfGet(tbf);
	if (!tbf_data) {
		mvOsPrintf("%s: Invalid Token Bucket Filter index (%d)\n", __func__, tbf);
		return MV_BAD_PARAM;
	}
	/* delete tbf only if there are no rules attached */
	if (!tbf_data->refCnt) {
		element = mvListFind(tbfs, (MV_ULONG)tbf_data);
		mvListDel(element);
		mvOsFree(tbf_data);
	}
	return MV_OK;
}

MV_STATUS mvNfpCtRateLimitSet(NFP_RULE_CT *ct2, int tbf_index)
{
	NFP_RULE_CT *ct;
	NFP_TBF_INFO *tbf = mvNfpTbfGet(tbf_index);

	if (!ct2)
		return MV_BAD_PARAM;
	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		if (tbf) {
			if (ct->tbfInfo)
				(ct->tbfInfo)->refCnt--;
			tbf->refCnt++;
			ct->tbfInfo = tbf;
			ct->flags |= NFP_F_CT_LIMIT;
		} else {
			mvOsPrintf("%s: Invalid Token Bucket Filter index (%d)\n", __func__, tbf_index);
			return MV_BAD_PARAM;
		}
	} else {
		mvOsPrintf("%s Error: Could not find existing 5 tuple rule\n", __func__);
		return MV_NOT_FOUND;
	}

	NFP_DBG("NFP (ct rate limit) set %p\n", ct);

	return MV_OK;
}

MV_STATUS mvNfpCtRateLimitDel(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	if (!ct2)
		return MV_BAD_PARAM;
	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		if (ct->tbfInfo)
			(ct->tbfInfo)->refCnt--;

		ct->tbfInfo = NULL;
		ct->flags &= ~NFP_F_CT_LIMIT;
		return MV_OK;
	}

	return MV_NOT_FOUND;
}

MV_STATUS mvNfpTbfProcess(NFP_TBF_INFO *tbf, MV_U32 packetSize)
{
	MV_U32 ticks = mvOsGetTicks();
	MV_U32 elapsed;

	if (!tbf)
		return MV_CONTINUE;
	/* Update credit */
	elapsed = ticks - tbf->lastUpdate;
	tbf->lastUpdate = ticks;
	/* safe check if elapsed time is higher than "time that gives maximum credit" */
	if (elapsed > tbf->maxElapsedTicks) {
		tbf->credit = tbf->creditMax;
	} else {
		tbf->credit += elapsed * tbf->creditPerTick;
		if (tbf->credit > tbf->creditMax)
			tbf->credit = tbf->creditMax;
	}

	/* Check result */
	if (packetSize > tbf->credit)
		return MV_DROPPED;
	tbf->credit -= packetSize;
	return MV_CONTINUE;
}
#endif /* NFP_LIMIT */

#ifdef NFP_CLASSIFY
/* Add DSCP mapping for an existing 5 tuple rule */
MV_STATUS mvNfpCtDscpRuleAdd(NFP_RULE_CT *ct2, int dscp, int new_dscp)
{
	NFP_RULE_CT *ct;

	/* sanity: chack new_dscp parameter */
	if ((new_dscp < NFP_DSCP_MIN) || (new_dscp > NFP_DSCP_MAX)) {
		mvOsPrintf("%s Error: new_dscp value (%d) is out of range\n", __func__, new_dscp);
		return MV_BAD_PARAM;
	}

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);
	/* rule is not exist - create rule from classification context */
	if (!ct) {
		ct = mvNfpCtClassifyRuleCreate(ct2);
		if (!ct)
			return MV_FAIL;
	}

	/* Update rule DSCP map table */
	if (ct2->flags & NFP_F_CT_SET_DSCP) {
		if (dscp == MV_ETH_NFP_GLOBAL_MAP) {
			ct->dscp_map[NFP_DSCP_MAP_GLOBAL].new_dscp = new_dscp;
			ct->dscp_map[NFP_DSCP_MAP_GLOBAL].valid = MV_TRUE;
		} else if ((dscp >= NFP_DSCP_MIN) && (dscp <= NFP_DSCP_MAX)) {
			ct->dscp_map[dscp].new_dscp = new_dscp;
			ct->dscp_map[dscp].valid = MV_TRUE;
		} else {
			mvOsPrintf("%s Error: dscp value (%d) is out of range\n", __func__, dscp);
			return MV_BAD_PARAM;
		}
		ct->flags |= NFP_F_CT_SET_DSCP;
	} else {
		mvOsPrintf("%s Error: NFP_F_CT_SET_DSCP flag is not set\n", __func__);
		return MV_BAD_PARAM;
	}

	NFP_DBG("NFP (ct dscp) set %p\n", ct);

	return MV_OK;
}

static INLINE MV_STATUS mvNfpIsDscpSet(NFP_RULE_CT *ct)
{
	int i;

	for (i = 0; i <= NFP_DSCP_MAP_GLOBAL; i++)
		if (ct->dscp_map[i].valid)
			return MV_TRUE;

	return MV_FALSE;
}

MV_STATUS mvNfpCtDscpRuleDel(NFP_RULE_CT *ct2, int dscp)
{
	NFP_RULE_CT *ct;

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		if (dscp == MV_ETH_NFP_GLOBAL_MAP) {
			ct->dscp_map[NFP_DSCP_MAP_GLOBAL].new_dscp = 0;
			ct->dscp_map[NFP_DSCP_MAP_GLOBAL].valid = MV_FALSE;
		} else if ((dscp >= NFP_DSCP_MIN) && (dscp <= NFP_DSCP_MAX)) {
			ct->dscp_map[dscp].new_dscp = 0;
			ct->dscp_map[dscp].valid = MV_FALSE;
		} else {
			mvOsPrintf("%s Error: dscp value (%d) is out of range\n", __func__, dscp);
			return MV_BAD_PARAM;
		}

		if (!mvNfpIsDscpSet(ct))
			ct->flags &= ~NFP_F_CT_SET_DSCP;

		return MV_OK;
	}

	return MV_NOT_FOUND;
}

MV_STATUS mvNfpCtVlanPrioRuleAdd(NFP_RULE_CT *ct2, int prio, int new_prio)
{
	NFP_RULE_CT *ct;

	/* sanity: chack new_prio parameter */
	if ((new_prio < NFP_VPRI_MIN) || (new_prio > NFP_VPRI_MAX)) {
		mvOsPrintf("%s Error: new_prio value (%d) is out of range\n", __func__, new_prio);
		return MV_BAD_PARAM;
	}

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);
	/* rule is not exist - create rule from classification context */
	if (!ct) {
		ct = mvNfpCtClassifyRuleCreate(ct2);
		if (!ct)
			return MV_FAIL;
	}

	/* Update rule VLAN Priority map table */
	if (ct2->flags & NFP_F_CT_SET_VLAN_PRIO) {
		if (prio == MV_ETH_NFP_GLOBAL_MAP) {
			ct->vpri_map[NFP_VPRI_MAP_GLOBAL].new_prio = new_prio;
			ct->vpri_map[NFP_VPRI_MAP_GLOBAL].valid = MV_TRUE;
		} else if ((prio >= NFP_VPRI_MIN) && (prio <= NFP_VPRI_MAX)) {
			ct->vpri_map[prio].new_prio = new_prio;
			ct->vpri_map[prio].valid = MV_TRUE;
		} else {
			mvOsPrintf("%s Error: prio value (%d) is out of range\n", __func__, prio);
			return MV_BAD_PARAM;
		}
		ct->flags |= NFP_F_CT_SET_VLAN_PRIO;
	} else {
		mvOsPrintf("%s: NFP_F_CT_SET_VLAN_PRIO flag is not set\n", __func__);
		return MV_BAD_PARAM;
	}

	NFP_DBG("NFP (ct vpri) set %p\n", ct);

	return MV_OK;
}

static INLINE MV_STATUS mvNfpIsVpriSet(NFP_RULE_CT *ct)
{
	int i;

	for (i = 0; i <= NFP_VPRI_MAP_GLOBAL; i++)
		if (ct->vpri_map[i].valid)
			return MV_TRUE;

	return MV_FALSE;
}

MV_STATUS mvNfpCtVlanPrioRuleDel(NFP_RULE_CT *ct2, int prio)
{
	NFP_RULE_CT *ct;

	/* sanity: chack prio parameter */
	if ((prio < MV_ETH_NFP_GLOBAL_MAP) || (prio > NFP_VPRI_MAP_GLOBAL)) {
		mvOsPrintf("%s Error: prio value (%d) is out of range\n", __func__, prio);
		return MV_BAD_PARAM;
	}

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		if (prio == MV_ETH_NFP_GLOBAL_MAP) {
			ct->vpri_map[NFP_VPRI_MAP_GLOBAL].new_prio = 0;
			ct->vpri_map[NFP_VPRI_MAP_GLOBAL].valid = MV_FALSE;
		} else {
			ct->vpri_map[prio].new_prio = 0;
			ct->vpri_map[prio].valid = MV_FALSE;
		}
		if (!mvNfpIsVpriSet(ct))
			ct->flags &= ~NFP_F_CT_SET_VLAN_PRIO;

		return MV_OK;
	}

	return MV_NOT_FOUND;
}

MV_STATUS mvNfpCtTxqRuleAdd(NFP_RULE_CT *ct2, int dscp, int txq)
{
	NFP_RULE_CT *ct;

	/* sanity: chack txq parameter */
	if ((txq < 0) || (txq >= CONFIG_MV_ETH_TXQ)) {
		mvOsPrintf("%s Error: txq (%d) is out of range\n", __func__, txq);
		return MV_BAD_PARAM;
	}

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);
	/* rule is not exist - create rule from classification context */
	if (!ct) {
		ct = mvNfpCtClassifyRuleCreate(ct2);
		if (!ct)
			return MV_FAIL;
	}

	/* Update rule DSCP to TXQ map table */
	if (ct2->flags & NFP_F_CT_SET_TXQ) {
		if (dscp == MV_ETH_NFP_GLOBAL_MAP) {
			ct->txq_map[NFP_DSCP_MAP_GLOBAL].txq = txq;
			ct->txq_map[NFP_DSCP_MAP_GLOBAL].valid = MV_TRUE;
		} else if ((dscp >= NFP_DSCP_MIN) && (dscp <= NFP_DSCP_MAX)) {
			ct->txq_map[dscp].txq = txq;
			ct->txq_map[dscp].valid = MV_TRUE;
		} else {
			mvOsPrintf("%s Error: dscp value (%d) is out of range\n", __func__, dscp);
			return MV_BAD_PARAM;
		}
		ct->flags |= NFP_F_CT_SET_TXQ;
	} else {
		mvOsPrintf("%s Error: NFP_F_CT_SET_TXQ flag is not set\n", __func__);
		return MV_BAD_PARAM;
	}

	NFP_DBG("NFP (ct txq) set %p\n", ct);

	return MV_OK;
}

static INLINE MV_STATUS mvNfpIsTxqSet(NFP_RULE_CT *ct)
{
	int i;

	for (i = 0; i <= NFP_DSCP_MAP_GLOBAL; i++)
		if (ct->txq_map[i].valid)
			return MV_TRUE;

	return MV_FALSE;
}

MV_STATUS mvNfpCtTxqRuleDel(NFP_RULE_CT *ct2, int dscp)
{
	NFP_RULE_CT *ct;

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		if (dscp == MV_ETH_NFP_GLOBAL_MAP) {
			ct->txq_map[NFP_DSCP_MAP_GLOBAL].txq = 0;
			ct->txq_map[NFP_DSCP_MAP_GLOBAL].valid = MV_FALSE;
		} else if ((dscp >= NFP_DSCP_MIN) && (dscp <= NFP_DSCP_MAX)) {
			ct->txq_map[dscp].txq = 0;
			ct->txq_map[dscp].valid = MV_FALSE;
		} else {
			mvOsPrintf("%s Error: dscp value (%d) is out of range\n", __func__, dscp);
			return MV_BAD_PARAM;
		}

		if (!mvNfpIsTxqSet(ct))
			ct->flags &= ~NFP_F_CT_SET_TXQ;

		return MV_OK;
	}

	return MV_NOT_FOUND;
}

MV_STATUS mvNfpCtTxpRuleAdd(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);
	/* rule is not exist - create rule from classification context */
	if (!ct) {
		ct = mvNfpCtClassifyRuleCreate(ct2);
		if (!ct)
			return MV_FAIL;
	}

	/* Update rule Txq table */
	if (ct2->flags & NFP_F_CT_SET_TXP) {
		ct->txp = ct2->txp;
		ct->flags |= NFP_F_CT_SET_TXP;
	} else {
		mvOsPrintf("%s: NFP_F_CT_SET_TXP flag is not set\n", __func__);
		return MV_BAD_PARAM;
	}

	NFP_DBG("NFP (ct txp) set %p\n", ct);

	return MV_OK;
}

MV_STATUS mvNfpCtTxpRuleDel(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		ct->txp = 0;
		ct->flags &= ~NFP_F_CT_SET_TXP;
		return MV_OK;
	}

	return MV_NOT_FOUND;
}

MV_STATUS mvNfpCtMhRuleAdd(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	/* sanity: chack mh parameter */
	if (ct2->mh < 0) {
		mvOsPrintf("%s Error: mh (%d) is out of range\n", __func__, ct2->mh);
		return MV_BAD_PARAM;
	}

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);
	/* rule is not exist - create rule from classification context */
	if (!ct) {
		ct = mvNfpCtClassifyRuleCreate(ct2);
		if (!ct)
			return MV_FAIL;
	}

	/* Update rule MH table */
	if (ct2->flags & NFP_F_CT_SET_MH) {
		ct->mh = ct2->mh;
		ct->flags |= NFP_F_CT_SET_MH;
	} else {
		mvOsPrintf("%s: NFP_F_CT_SET_MH flag is not set\n", __func__);
		return MV_BAD_PARAM;
	}

	NFP_DBG("NFP (ct mh) set %p\n", ct);

	return MV_OK;
}

MV_STATUS mvNfpCtMhRuleDel(NFP_RULE_CT *ct2)
{
	NFP_RULE_CT *ct;

	/* Rule must exist already, but it can be valid or invalid */
	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (ct) {
		ct->mh = 0;
		ct->flags &= ~NFP_F_CT_SET_MH;
		return MV_OK;
	}

	return MV_NOT_FOUND;
}

#endif /* NFP_CLASSIFY */

MV_STATUS mvNfpCtRuleDel(NFP_RULE_CT *ct2)
{
	MV_U32 hash;
	NFP_RULE_CT *ct, *prev;
	MV_LIST_ELEMENT	*element;

	/* If this rule currently exists in the Invalid Rules DB, delete it */
	ct = mvNfpCtRuleInvalidLookup(ct2);
	if (ct) {
		if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
			(ct->fib)->ct_ref_count--;
		if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
			(ct->fib)->ct_hwf_ref_count--;
		element = mvListFind(ct_inv_list, (MV_ULONG)ct);
#ifdef NFP_LIMIT
		if (ct->tbfInfo)
			mvNfpCtRateLimitDel(ct);
#endif /* NFP_LIMIT */
		mvListDel(element);
		mvOsFree(ct);
		return MV_OK;
	}

	hash = mvNfpCtHash(ct2);
	ct = ct_hash[hash];

	prev = NULL;
	while (ct) {
		if ((ct->family == ct2->family) &&
			l3_addr_eq(ct->family, ct->srcL3, ct2->srcL3) &&
			l3_addr_eq(ct->family, ct->dstL3, ct2->dstL3) &&
			(ct->ports == ct2->ports) &&
			(ct->proto == ct2->proto)) {
			if (ct->fib && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_ref_count--;
			if ((ct->fib) && (ct->flags & NFP_F_CT_HWF) && !(ct->flags & NFP_F_CT_FIB_INV))
				(ct->fib)->ct_hwf_ref_count--;
			if (prev)
				prev->next = ct->next;
			else
				ct_hash[hash] = ct->next;

			NFP_DBG("NFP (ct) del %p\n", ct);
#ifdef NFP_LIMIT
			if (ct->tbfInfo)
				mvNfpCtRateLimitDel(ct);
#endif /* NFP_LIMIT */
			mvOsFree(ct);
			return MV_OK;
		}
		prev = ct;
		ct = ct->next;
	}

	return MV_NOT_FOUND;
}

void    mvNfpCtClean(int family)
{
	int             i;
	NFP_RULE_CT    *ct, *next;
#ifdef NFP_LIMIT
	NFP_TBF_INFO *tbf_data;
#endif /* NFP_LIMIT */
	MV_LIST_ELEMENT	*curr, *tmp;

	/* Clean CT incomplete rules list */
	if (ct_inv_list) {
		curr = ct_inv_list->next;
		while (curr) {
			tmp = curr->next;
			ct = (NFP_RULE_CT *)curr->data;
			if (ct->family == family) {
#ifdef NFP_LIMIT
			if (ct->tbfInfo)
				mvNfpCtRateLimitDel(ct);
#endif /* NFP_LIMIT */
			mvOsFree(ct);
			mvListDel(curr);
			}
			curr = tmp;
		}
	}

	/* Clean CT hash table */
	for (i = 0; i < NFP_CT_HASH_SIZE; i++) {
		ct = ct_hash[i];

		while (ct) {
			next = ct->next;
			if (ct->family == family) {
#ifdef NFP_LIMIT
			if (ct->tbfInfo)
				mvNfpCtRateLimitDel(ct);
#endif /* NFP_LIMIT */
			mvOsFree(ct);
			}
			ct = next;
		}
		ct_hash[i] = NULL;
	}
#ifdef NFP_LIMIT
	/* clean tbfs */
	if (tbfs) {
		curr = tbfs->next;
		while (curr) {
			tbf_data = (NFP_TBF_INFO *)mvListDel(curr);
			mvOsFree(tbf_data);
			curr = tbfs->next;
		}
	}

#endif /* NFP_LIMIT */
}

void	mvNfpCtDestroy(void)
{
	if (ct_hash)
		mvOsFree(ct_hash);
#ifdef NFP_LIMIT
	if (tbfs)
		mvOsFree(tbfs);
#endif /* NFP_LIMIT */
	mvListDestroy(ct_inv_list);
}

MV_VOID mvNfpCtCleanVisited(MV_U32 row, MV_U32 iterator_id)
{
	NFP_RULE_CT *curr  = ct_hash[row];

	while (curr) {
		curr->visited = curr->visited & ~(1 << iterator_id);
		curr = curr->next;
	}
}

MV_STATUS mvNfpCtFirstRuleGet(NFP_RULE_CT **rule, MV_U32 iterator_id)
{
	mvNfpCtCleanVisited(ct_iterator_row, iterator_id);
	ct_iterator_row  = 0;
	return mvNfpCtNextRuleGet(rule, iterator_id);
}

MV_STATUS mvNfpCtNextRuleGet(NFP_RULE_CT **rule, MV_U32 iterator_id)
{
	NFP_RULE_CT *curr;

	while (ct_iterator_row < NFP_CT_HASH_SIZE) {
		curr  = ct_hash[ct_iterator_row];
		/* skip visited and HWF processed rules */
		while (curr && ((curr->visited & (1 << iterator_id))
				|| (curr->flags & NFP_F_CT_HWF)))
			curr = curr->next;
		if (!curr) { /* reached end of line */
			mvNfpCtCleanVisited(ct_iterator_row, iterator_id);
			ct_iterator_row++;
			continue;
		}
		curr->visited = 1; /* update - this rule is now visited by iterator */
		*rule = curr;
		return MV_OK;
	}
	/* reached end of DB - no rule is found */
	ct_iterator_row = 0; /* next call start from the begining of the DB */
	return MV_NOT_FOUND;
}

MV_STATUS mvNfpCtRuleMaxHitCntrGet(NFP_RULE_CT **rule)
{
	int i;
	MV_U32 max = -1;
	NFP_RULE_CT *curr, *max_rule = NULL;

	for (i = 0; i < NFP_CT_HASH_SIZE; i++) {
		curr = ct_hash[i];
		while (curr) {
			if (curr->flags & NFP_F_CT_HWF) {
				curr = curr->next;
				continue;
			}
			if (curr->hit_cntr > max || !max_rule) {
				max = curr->hit_cntr;
				max_rule = curr;
			}
			curr = curr->next;
		}
	}
	if (!max_rule)
		return MV_NOT_FOUND;
	*rule = max_rule;
	return MV_OK;
}

/* 0 - set zero to UDP csum on TX, 1 - recalculate UDP csum on TX */
MV_STATUS mvNfpCtRuleUdpCsumSet(NFP_RULE_CT *ct2, int mode)
{
	NFP_RULE_CT *ct;

	if (!ct2 || (mode < 0) || (mode > 1) || (ct2->proto != MV_IP_PROTO_UDP))
		return MV_BAD_PARAM;

	ct = mvNfpCtLookup(ct2);
	if (!ct)
		ct = mvNfpCtRuleInvalidLookup(ct2);

	if (!ct)
		return MV_NOT_FOUND;

	if (mode)
		ct->flags |= NFP_F_CT_UDP_CSUM;
	else
		ct->flags &= ~NFP_F_CT_UDP_CSUM;

	return MV_OK;
}

/* mode = 1:HWF,  0:NFP  */
MV_STATUS mvNfpCtRuleHwfSet(MV_NFP_CT_KEY *key, int mode)
{
	NFP_RULE_CT *rule;
	MV_U32 nflags = 0, ports;

	if (!key || (mode < 0) || (mode > 1))
		return MV_BAD_PARAM;

	ports = (MV_16BIT_BE(key->dport) << 16) | MV_16BIT_BE(key->sport);
	rule = mvNfpCtLookupByTuple(key->family, key->src_l3, key->dst_l3, ports, key->proto);
	if (!rule)
		return MV_NOT_FOUND;

	/* sanity check */
	if (!rule->fib || (rule->flags & NFP_F_CT_FIB_INV))
		return MV_NOT_FOUND;

	if (mode)
		nflags = (rule->flags | NFP_F_CT_HWF);
	else
		nflags = (rule->flags & ~NFP_F_CT_HWF);

	if (rule->flags != nflags) {
		/* Mode is changed - update hwf ref count */
		if (mode) /* go to HWF mode */
			rule->fib->ct_hwf_ref_count++;
		else {
			/* exit from HWF mode */
			rule->fib->ct_hwf_ref_count--;
			rule->hit_cntr = 0;
		}
	}
	rule->flags = nflags;
	return MV_OK;
}

MV_STATUS mvNfpCtRuleHitCntrGet(MV_NFP_CT_KEY *key, MV_U32 *hit_cntr)
{
	NFP_RULE_CT *rule;
	MV_U32 ports;

	if (!key || !hit_cntr)
		return MV_BAD_PARAM;
		ports = (MV_16BIT_BE(key->dport) << 16) | MV_16BIT_BE(key->sport);
	rule = mvNfpCtLookupByTuple(key->family, key->src_l3, key->dst_l3, ports, key->proto);
	if (!rule)
		return MV_NOT_FOUND;
	*hit_cntr = rule->hit_cntr;
	return MV_OK;
}

MV_STATUS mvNfpCtRuleHitCntrSet(MV_NFP_CT_KEY *key, MV_U32 val)
{
	NFP_RULE_CT *rule;
	MV_U32 ports;

	if (!key || val < 0)
		return MV_BAD_PARAM;
	ports = (MV_16BIT_BE(key->dport) << 16) | MV_16BIT_BE(key->sport);
	rule = mvNfpCtLookupByTuple(key->family, key->src_l3, key->dst_l3, ports, key->proto);
	if (!rule)
		return MV_NOT_FOUND;
	rule->hit_cntr = val;
	return MV_OK;
}

MV_STATUS mvNfpCtRuleInfoGet(MV_NFP_CT_KEY *key, MV_NFP_CT_INFO *ct_info)
{
	NFP_RULE_CT *rule;
	NFP_IF_MAP *ifMap;
	MV_U32 ports;

	if (!key || !ct_info)
		return MV_BAD_PARAM;
	ports = (MV_16BIT_BE(key->dport) << 16) | MV_16BIT_BE(key->sport);
	rule = mvNfpCtLookupByTuple(key->family, key->src_l3, key->dst_l3, ports, key->proto);
	if (!rule)
		return MV_NOT_FOUND;
	ct_info->flags = rule->flags;
	ct_info->new_sip = rule->new_sip;
	ct_info->new_dip = rule->new_dip;
	ct_info->new_sport = rule->new_sport;
	ct_info->new_dport = rule->new_dport;
	if (rule->fib) {
		memcpy(ct_info->sa, (rule->fib)->sa, MV_MAC_ADDR_SIZE);
		memcpy(ct_info->da, (rule->fib)->da, MV_MAC_ADDR_SIZE);
		ifMap = mvNfpIfMapGet((rule->fib)->oif);
		ct_info->out_port = ifMap->port;
	}
#ifdef NFP_CLASSIFY
	ct_info->mh = rule->mh;
	ct_info->txp = rule->txp;
	/* return only global txq/dscp/vprio mapping */
	ct_info->txq = (rule->txq_map[NFP_DSCP_MAP_GLOBAL]).txq;
	if (!(rule->txq_map[NFP_DSCP_MAP_GLOBAL]).valid)
		ct_info->flags = ct_info->flags & ~NFP_F_CT_SET_TXQ;
	ct_info->dscp = (rule->dscp_map[NFP_DSCP_MAP_GLOBAL]).new_dscp;
	if (!(rule->dscp_map[NFP_DSCP_MAP_GLOBAL]).valid)
		ct_info->flags = ct_info->flags & ~NFP_F_CT_SET_DSCP;
	ct_info->vprio = (rule->vpri_map[NFP_VPRI_MAP_GLOBAL]).new_prio;
	if (!(rule->vpri_map[NFP_VPRI_MAP_GLOBAL]).valid)
		ct_info->flags = ct_info->flags & ~NFP_F_CT_SET_VLAN_PRIO;
#endif /* NFP_CLASSIFY */
	return MV_OK;
}

void    mvNfpCtDump(void)
{
	MV_U32 i;
	NFP_RULE_CT *ct;
	MV_LIST_ELEMENT	*curr;

	mvOsPrintf("\n(ct hash)\n");
	for (i = 0; i < NFP_CT_HASH_SIZE; i++) {
		ct = ct_hash[i];

		while (ct) {
			mvOsPrintf(" [%2d] ", i);
			mvNfpCtRulePrint(ct);

			ct = ct->next;
		}
	}
	if (!ct_inv_list) {
		mvOsPrintf("(ct_inv_list) does not exist\n");
		return;
	}
	mvOsPrintf("(ct_inv_list)\n");
	curr = ct_inv_list->next;
	while (curr) {
		ct = (NFP_RULE_CT *)curr->data;
		mvNfpCtRulePrint(ct);
		curr = curr->next;
	}
}

