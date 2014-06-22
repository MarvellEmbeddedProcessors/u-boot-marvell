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

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"
#include "neta/gbe/mvNeta.h"


/*******************************************************************************
* mvSysNetaInit - Initialize the Eth subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*		None
* RETURN:
*       None
*
*******************************************************************************/
void 	mvSysNetaInit(MV_U32 portMask, MV_U32 cpuMask)
{
	MV_NETA_HAL_DATA halData;
	MV_U32 port;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;
	int i;

	memset(&halData, 0, sizeof(halData));
	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if (status != MV_OK)
		return;

	for (i = 0; i < MAX_TARGETS; i++) {
		if (addrWinMap[i].enable == MV_FALSE)
			continue;

#ifdef CONFIG_MV_SUPPORT_L2_DEPOSIT
		/* Setting DRAM windows attribute to :
		   0x3 - Shared transaction + L2 write allocate (L2 Deposit) */
		if (MV_TARGET_IS_DRAM(i)) {
			addrWinMap[i].attrib &= ~(0x30);
			addrWinMap[i].attrib |= 0x30;
		}
#endif
	#ifdef DEBUG
		mvOsPrintf("%d - Base 0x%08x , Size = 0x%08llx.\n", i,
						addrWinMap[i].addrWin.baseLow, addrWinMap[i].addrWin.size);
	#endif

	}

	halData.portMask = portMask;
	halData.cpuMask  = cpuMask;
	halData.maxPort = mvCtrlEthMaxPortGet();
	halData.pClk = mvCpuPclkGet();
	halData.tClk = mvBoardTclkGet();
	halData.maxCPUs = mvCtrlEthMaxCPUsGet();
	halData.iocc = MV_FALSE;
	halData.ctrlModel = mvCtrlModelGet();
	halData.ctrlRev = mvCtrlRevGet();
#ifdef CONFIG_MV_ETH_BM
	halData.bmPhysBase = PNC_BM_PHYS_BASE;
	halData.bmVirtBase = (MV_U8 *)ioremap(PNC_BM_PHYS_BASE, PNC_BM_SIZE);
#endif /* CONFIG_MV_ETH_BM */

#ifdef CONFIG_MV_ETH_PNC
	halData.pncPhysBase = PNC_BM_PHYS_BASE;
	halData.pncVirtBase = (MV_U8 *)ioremap(PNC_BM_PHYS_BASE, PNC_BM_SIZE);
#endif /* CONFIG_MV_ETH_PNC */

	for (port = 0; port < halData.maxPort; port++) {
		if (!(MV_BIT_CHECK(portMask, port)))
			continue;

		if (mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port) == MV_FALSE)
			continue;

		mvNetaPortPowerUp(port,
						  mvBoardIsPortInSgmii(port),
						  !mvBoardIsPortInGmii(port),
						  (mvBoardPhyAddrGet(port) == 0xFF));
		status = mvNetaWinInit(port, addrWinMap);
		if (status != MV_OK)
			continue;
	}
	mvNetaHalInit(&halData);

	return;
}

