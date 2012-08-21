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
* mvNfpDefs.h - Header File for Marvell NFP Configuration definitions
*
* DESCRIPTION:
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __mvNfpDefs_h__
#define __mvNfpDefs_h__

#include "mv802_3.h"
#include "gbe/mvNetaRegs.h"
#include "gbe/mvNeta.h"

#ifdef CONFIG_MV_ETH_NFP

#define ETH_NFP

#ifdef CONFIG_MV_ETH_NFP_EXT
# define NFP_EXT
# define NFP_EXT_NUM 	CONFIG_MV_ETH_NFP_EXT_NUM
#else
# define NFP_EXT_NUM 	0
#endif

#define NFP_MAX_PORTS   (MV_ETH_MAX_PORTS + NFP_EXT_NUM)

#if defined(CONFIG_MV_ETH_NFP_FIB_LEARN) || defined(CONFIG_MV_ETH_NFP_VLAN_LEARN) || defined(CONFIG_MV_ETH_NFP_BRIDGE_LEARN)||\
		defined(CONFIG_MV_ETH_NFP_PPP_LEARN) || defined(CONFIG_MV_ETH_NFP_CT_LEARN)
#define NFP_LEARN
#endif

#ifdef CONFIG_MV_ETH_NFP_FIB
#define NFP_FIB
#endif

#ifdef CONFIG_MV_ETH_NFP_FIB_LEARN
#define NFP_FIB_LEARN
#endif

#ifdef CONFIG_MV_ETH_NFP_VLAN
#define NFP_VLAN
#endif

#ifdef CONFIG_MV_ETH_NFP_VLAN_LEARN
#define NFP_VLAN_LEARN
#endif

#ifdef CONFIG_MV_ETH_NFP_PPP
#define NFP_PPP
#endif

#ifdef CONFIG_MV_ETH_NFP_PPP_LEARN
#define NFP_PPP_LEARN
#endif

#ifdef CONFIG_MV_ETH_NFP_CT
#define NFP_CT
#endif

#ifdef CONFIG_MV_ETH_NFP_CT_LEARN
#define NFP_CT_LEARN
#endif

#ifdef CONFIG_MV_ETH_NFP_NAT
#define NFP_NAT
#endif

#ifdef CONFIG_MV_ETH_NFP_LIMIT
#define NFP_LIMIT
#endif

#ifdef CONFIG_MV_ETH_NFP_CLASSIFY
#define NFP_CLASSIFY
#endif

#ifdef CONFIG_MV_ETH_NFP_STATS
#define NFP_STAT
#endif

#ifdef CONFIG_MV_ETH_NFP_BRIDGE
#define NFP_BRIDGE
#endif

#ifdef CONFIG_MV_ETH_NFP_BRIDGE_LEARN
#define NFP_BRIDGE_LEARN
#endif

#ifdef CONFIG_MV_ETH_SWITCH
# define NFP_MAX_SWITCH_GROUPS  CONFIG_MV_ETH_SWITCH_NETDEV_NUM
#else
# define NFP_MAX_SWITCH_GROUPS  1
#endif /* CONFIG_MV_ETH_SWITCH */

typedef struct {
	MV_U16 flags;

	MV_U8  txp;
	MV_U8  txq;

	MV_U8 pkt_ecn;
	MV_U8 pkt_dscp;
	int   ct_dscp;

	int pkt_vlan_prio;
	int bridge_vlan_prio;
	int ct_vlan_prio;

	MV_U16 mh;
} NFP_CLASSIFY_INFO;

typedef struct {
	void   *dev;
	MV_U32 tx_cmd;
	MV_U32 diffL4[2];
	MV_U8  *pWrite;
	MV_U16 flags;
	MV_U16 mtu;
	short  shift;
	MV_U8  txp;
	MV_U8  txq;
	MV_IP_HEADER_INFO ipInfo;
	NFP_CLASSIFY_INFO classifyInfo;
} MV_NFP_RESULT;

#define MV_NFP_RES_TXP_VALID       0x0001
#define MV_NFP_RES_TXQ_VALID       0x0002
#define MV_NFP_RES_IP_INFO_VALID   0x0004
#define MV_NFP_RES_NETDEV_EXT      0x0010
#define MV_NFP_RES_L4_CSUM_NEEDED  0x0020

MV_STATUS mvNfpIfMapPortGet(int ifIdx, int *port);
MV_STATUS mvNfpRx(MV_U32 inPort, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt, MV_NFP_RESULT* pRes);

#endif /* CONFIG_MV_ETH_NFP */

#endif /* __mvNfp_h__ */
