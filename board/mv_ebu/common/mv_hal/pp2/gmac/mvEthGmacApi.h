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
#ifndef __mvEthGmac_h__
#define __mvEthGmac_h__

#include "mvEthGmacRegs.h"

#include "mvTypes.h"
#include "mvCommon.h"
#include "mvOs.h"

typedef enum {
	MV_ETH_SPEED_AN,
	MV_ETH_SPEED_10,
	MV_ETH_SPEED_100,
	MV_ETH_SPEED_1000
} MV_ETH_PORT_SPEED;

typedef enum {
	MV_ETH_DUPLEX_AN,
	MV_ETH_DUPLEX_HALF,
	MV_ETH_DUPLEX_FULL
} MV_ETH_PORT_DUPLEX;

typedef enum {
	MV_ETH_FC_AN_NO,
	MV_ETH_FC_AN_SYM,
	MV_ETH_FC_AN_ASYM,
	MV_ETH_FC_DISABLE,
	MV_ETH_FC_ENABLE,
	MV_ETH_FC_ACTIVE

} MV_ETH_PORT_FC;

typedef struct eth_link_status {
	MV_BOOL			linkup;
	MV_ETH_PORT_SPEED	speed;
	MV_ETH_PORT_DUPLEX	duplex;
	MV_ETH_PORT_FC		rxFc;
	MV_ETH_PORT_FC		txFc;

} MV_ETH_PORT_STATUS;

void mvEthPortEnable(int port);
void mvEthPortDisable(int port);
MV_BOOL mvEthPortIsLinkUp(int port);
MV_STATUS mvEthLinkStatus(int port, MV_ETH_PORT_STATUS *pStatus);
void mvEthPortLbSet(int port, int isGmii, int isPcsEn);
void mvEthPortPowerUp(int port, MV_BOOL isSgmii, MV_BOOL isRgmii);
void mvEthPortPowerDown(int port);

/******************************************************************************/
/*                          Port Configuration functions                      */
/******************************************************************************/
void mvEthMaxRxSizeSet(int port, int maxRxSize);
MV_STATUS mvEthForceLinkModeSet(int portNo, MV_BOOL force_link_up, MV_BOOL force_link_down);
MV_STATUS mvEthSpeedDuplexSet(int portNo, MV_ETH_PORT_SPEED speed, MV_ETH_PORT_DUPLEX duplex);
MV_STATUS mvEthSpeedDuplexGet(int portNo, MV_ETH_PORT_SPEED *speed, MV_ETH_PORT_DUPLEX *duplex);
MV_STATUS mvEthFlowCtrlSet(int port, MV_ETH_PORT_FC flowControl);
MV_STATUS mvEthFlowCtrlGet(int port, MV_ETH_PORT_FC *pFlowCntrl);

/******************************************************************************/
/*                         PHY Control Functions                              */
/******************************************************************************/
void mvEthPhyAddrSet(int port, int phyAddr);
int mvEthPhyAddrGet(int port);

/****************************************/
/*        MIB counters		       	*/
/****************************************/
MV_U32 mvEthMibCounterRead(int port, unsigned int mibOffset, MV_U32 *pHigh32);
void mvEthMibCountersClear(int port);
void mvEthMibCountersShow(int port);

#endif /* __mvEthGmac_h__ */
