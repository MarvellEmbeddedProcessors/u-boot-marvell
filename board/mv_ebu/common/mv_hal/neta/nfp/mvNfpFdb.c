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
* mvNfpFdb.c - Marvell Fast Network Processing
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

NFP_RULE_FDB **nfp_fdb_hash = NULL;

MV_STATUS _INIT mvNfpFdbInit(void)
{
	MV_U32 bytes = sizeof(NFP_RULE_FDB *) * NFP_BRIDGE_HASH_SIZE;

	nfp_fdb_hash = (NFP_RULE_FDB **)mvOsMalloc(bytes);
	if (nfp_fdb_hash == NULL) {
		mvOsPrintf("NFP (FDB hash): not enough memory\n");
		return MV_NO_RESOURCE;
	}
	mvOsMemset(nfp_fdb_hash, 0, bytes);

	mvOsPrintf("NFP (FDB) init %d entries, %d bytes\n", NFP_BRIDGE_HASH_SIZE, bytes);

	return MV_OK;
}

/* Clear FDB Rule Database */
MV_STATUS mvNfpFdbClear(void)
{
	int	i;
	NFP_RULE_FDB	*rule, *tmp;

	if (nfp_fdb_hash == NULL)
		return MV_NOT_INITIALIZED;

	for (i = 0; i < NFP_BRIDGE_HASH_SIZE; i++) {

		rule = nfp_fdb_hash[i];
		while (rule) {
			tmp = rule;
			rule = rule->next;
			mvOsFree(tmp);
		}
		nfp_fdb_hash[i] = NULL;
	}
	return MV_OK;
}

void mvNfpFdbDestroy(void)
{
	if (nfp_fdb_hash != NULL)
		mvOsFree(nfp_fdb_hash);
}

MV_STATUS mvNfpFdbRuleAdd(NFP_RULE_FDB *rule2)
{
	MV_U32 hash;
	NFP_RULE_FDB	*rule;
	NFP_RULE_FIB	*fib;
	NFP_IF_MAP      *outIfMap;
	MV_LIST_ELEMENT	*curr, *tmp;

	hash = mvNfpFdbRuleHash(rule2->bridgeIf, rule2->mac);

	rule = nfp_fdb_hash[hash];
	while (rule) {
		if (mvNfpFdbRuleCmp(rule2->bridgeIf, rule2->mac, rule)) {
			MV_U32 age = rule->age;

			/* Update rule, but save age */
			mvOsMemcpy(rule, rule2, sizeof(NFP_RULE_FDB));
			rule->age = age;
			goto out;
		}
		rule = rule->next;
	}

	rule = (NFP_RULE_FDB *)mvOsMalloc(sizeof(NFP_RULE_FDB));
	if (rule == NULL) {
		mvOsPrintf("%s: NFP (FDB rule) - OOM\n", __func__);
		return MV_FAIL;
	}

	mvOsMemcpy(rule, rule2, sizeof(NFP_RULE_FDB));

	rule->next = nfp_fdb_hash[hash];
	nfp_fdb_hash[hash] = rule;

	outIfMap = mvNfpIfMapGet(rule->if_index);

	/* Update incomplete FIB entires */
	curr = fib_inv_list->next;
	while (curr) {
		fib = (NFP_RULE_FIB *)curr->data;
		tmp = curr->next;
		if ((rule->bridgeIf == fib->oif) &&
			(!memcmp(rule->mac, fib->da, MV_MAC_ADDR_SIZE))) {

			/* Found incomplete FIB entry */
			fib->oif = outIfMap->ifIdx;
			fib->mh = outIfMap->txMh;
			fib->flags &= ~NFP_F_FIB_BRIDGE_INV;

			mvNfpFibRuleValid(fib, curr);
		}
		curr = tmp;
	}
out:
	NFP_DBG("NFP (FDB) add %p\n", rule);

	return MV_OK;
}

MV_STATUS mvNfpFdbRuleDel(NFP_RULE_FDB *rule2)
{
	MV_U32 hash;
	NFP_RULE_FDB	*rule, *prev;
	NFP_RULE_FIB	*fib, *fib_prev;
	int i;

	hash = mvNfpFdbRuleHash(rule2->bridgeIf, rule2->mac);

	rule = nfp_fdb_hash[hash];
	prev = NULL;

	while (rule) {
		if (mvNfpFdbRuleCmp(rule2->bridgeIf, rule2->mac, rule)) {
			/* Found: delete rule */
			if (prev)
				prev->next = rule->next;
			else
				nfp_fdb_hash[hash] = rule->next;

			NFP_DBG("NFP (FDB) del %p\n", rule);
			rule2->if_index = rule->if_index;
			mvOsFree(rule);
			break;
		}
		prev = rule;
		rule = rule->next;
	}
	if (rule == NULL)
		return MV_NOT_FOUND;

	/* Invalidate relevant FIB rules */
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		fib_prev = NULL;
		while (fib) {
			if ((rule2->if_index == fib->oif) &&
				(!memcmp(rule2->mac, fib->da, MV_MAC_ADDR_SIZE))) {
				/* Invalidate FIB route. FIXME - update HWF rule */
				fib->flags |= NFP_F_FIB_BRIDGE_INV;
				fib->oif = rule2->bridgeIf;

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

MV_STATUS mvNfpFdbRuleAge(NFP_RULE_FDB *rule2)
{
	NFP_RULE_FDB *rule;

	rule = mvNfpFdbLookup(rule2->bridgeIf, rule2->mac);
	if (rule) {
		rule2->age = rule->age;
		rule->age = 0;
		return MV_OK;
	}
	rule2->age = 0;
	return MV_NOT_FOUND;
}

static void mvNfpFdbRulePrint(NFP_RULE_FDB *rule)
{
	if (rule->status == NFP_BRIDGE_INV)
		mvOsPrintf("INVALID  : ");
	else if (rule->status == NFP_BRIDGE_LOCAL)
		mvOsPrintf("LOCAL    : ");
	else
		mvOsPrintf("NON_LOCAL: ");

	mvOsPrintf(" bridgeIf=%d, if_index=%u, MAC=" MV_MACQUAD_FMT " age=%u\n",
		rule->bridgeIf, rule->if_index, MV_MACQUAD(rule->mac), rule->age);
}

void mvNfpFdbDump(void)
{
	int             i;
	NFP_RULE_FDB *rule;

	mvOsPrintf("\n(fdb)\n");
	for (i = 0; i < NFP_BRIDGE_HASH_SIZE; i++) {
		rule = nfp_fdb_hash[i];

		while (rule) {
			mvOsPrintf(" [%5d] ", i);
			mvNfpFdbRulePrint(rule);
			rule = rule->next;
		}
	}
}

void mvNfpFdbFlushBridge(int ifindex)
{
	int i;

	NFP_RULE_FDB *rule;

	for (i = 0; i < NFP_BRIDGE_HASH_SIZE; i++) {
		rule = nfp_fdb_hash[i];

		while (rule) {
			if ((rule->if_index == ifindex) || (ifindex == -1))
				mvNfpFdbRuleDel(rule);
			rule = rule->next;
		}
	}
}

