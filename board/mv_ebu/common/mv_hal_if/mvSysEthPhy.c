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
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "eth-phy/mvEthPhy.h"
#ifdef CONFIG_MV_XSMI
#include "eth-phy/mvEthPhyXsmi.h"
#endif
#ifdef CONFIG_MV_ETH_10G
#include "eth-phy/mvEth10gPhy.h"
#endif
#if defined(MV_ETH_LEGACY)
#include "eth/gbe/mvEthRegs.h"
#elif defined(MV_ETH_NETA)
#include "neta/gbe/mvEthRegs.h"
#else
#include "pp2/gbe/mvPp2Gbe.h"
#include "pp2/gmac/mvEthGmacRegs.h"
#endif
#include "mvSysEthPhyApi.h"

/*******************************************************************************
* mvSysEthPhyInit - Initialize the EthPhy subsystem
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
MV_STATUS mvSysEthPhyInit(void)
{
	MV_ETHPHY_HAL_DATA halData;
	MV_U32 port;

	for (port=0; port < mvCtrlEthMaxPortGet(); port++) {
		halData.phyAddr[port] = mvBoardPhyAddrGet(port);
		halData.boardSpecInit = MV_FALSE;
		halData.isSgmii[port] = mvBoardIsPortInSgmii(port);
		halData.QuadPhyPort0[port] = mvBoardQuadPhyAddr0Get(port);
	}
	halData.ethPhySmiReg = ETH_SMI_REG(MV_ETH_SMI_PORT);
	halData.ctrlModel = mvCtrlModelGet();
	halData.ctrlFamily=mvCtrlDevFamilyIdGet(halData.ctrlModel);

	return mvEthPhyHalInit(&halData);
}

#if defined(CONFIG_MV_XSMI)
/*******************************************************************************
* mvSysEthPhyXsmiInit - Initialize the EthPhy XSMI subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*               None
* RETURN:
*       None
*
*******************************************************************************/
MV_STATUS mvSysEthPhyXsmiInit(void)
{
	MV_ETHPHY_XSMI_HAL_DATA halData;

	halData.ctrlModel = 0;
	halData.ethPhyXsmiRegOff = PSS_PORTS_PHYS_BASE + MV_XSMI_REGS_OFFSET;

	return mvEthPhyXsmiHalInit(&halData);
}

#if defined(CONFIG_MV_ETH_10G)
/*******************************************************************************
* mvSysEth10gPhyInit - Initialize the EthPhy subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*               None
* RETURN:
*       None
*
*******************************************************************************/
MV_STATUS mvSysEth10gPhyInit(void)
{
	MV_ETHPHY_HAL_DATA halData;
	MV_U32 port;

	/* Init the XSMI interface, which is required for the 10G PHY */
	mvSysEthPhyXsmiInit();

	for (port=0; port < mvCtrlEthMaxPortGet(); port++) {
		halData.phyAddr[port] = mvBoardPhyAddrGet(port);
		halData.boardSpecInit = MV_FALSE;
		halData.isSgmii[port] = mvBoardIsPortInSgmii(port);
		halData.QuadPhyPort0[port] = mvBoardQuadPhyAddr0Get(port);
	}
	halData.ethPhySmiReg = ETH_SMI_REG(MV_ETH_SMI_PORT);
	halData.ctrlModel = mvCtrlModelGet();
	halData.ctrlFamily = mvCtrlDevFamilyIdGet(halData.ctrlModel);

	return mvEth10gPhyHalInit(&halData);
}
#endif /* CONFIG_MV_ETH_10G */
#endif /* CONFIG_MV_XSMI */
