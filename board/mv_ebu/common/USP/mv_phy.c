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

#include "mvBoardEnvSpec.h"
#include "mvBoardEnvLib.h"
#include "mv_phy.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#ifdef CONFIG_ARMADA_39X
#include "ctrlEnv/mvCtrlNetCompLib.h"
#endif
#if defined(MV_ETH_NETA)
#include "neta/gbe/mvEthRegs.h"
#include "neta/gbe/mvNeta.h"
#elif defined (MV_ETH_PP2)
#include "pp2/gmac/mvEthGmacRegs.h"
#include "pp2/gmac/mvEthGmacApi.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"
#else
#include "eth/gbe/mvEthRegs.h"
#endif
#include "mvSysEthPhyApi.h"
#include "ethSwitch/mvSwitch.h"

#if defined (MV88F66XX)
static void mvAlpBoardEgigaPhyInit(void)
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();

	/* Set SMI control to CPU, before initializing PHY */
	mvCtrlSmiMasterSet(CPU_SMI_CTRL);

	/* Init PHY connected to MAC0 */
	if (ethComplex & (MV_ETHCOMP_GE_MAC0_2_RGMII0 |
			  MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0))
		mvEthPhyInit(0, MV_FALSE);

	/* Initialize PHY through MAC0, even though that PHY is not connected
	 * to MAC1. This external PHY is connected to the switch, but the
	 * external SMI control is granted to the CPU, and that why MAC0 is
	 * initializing this PHY, even though it's connected to the switch */
	if (ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY)
		mvEthPhyInit(0, MV_FALSE);

	/* Init PHY connected to MAC1 */
	if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC1_2_RGMII0))
		mvEthPhyInit(1, MV_FALSE);

	/* if MAC-1 is connected to RGMII-1 or PON serdes via SGMII,
	 * and SW_P4 is not connected RGMII-0,
	 * and MAC-1 is not connected to PON serdes via SFP, no need to init the PHY */
	if ((ethComplex & (MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)) &&
		!(ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY) &&
		!(ethComplex & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP)) {

		/* enable external phy cpu ctrl - MAC1 is polling this phy */
		mvBoardExtPhyBufferSelect(MV_TRUE);

		mvEthPhyInit(1, MV_FALSE);

	}

#if 0 /* the following configurations were not fully implemented yet */
	/* if Switch is connected to RGMII-0, it has to auto-poll the phy */
	if (ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0) {
		mvCtrlSmiMasterSet(SWITCH_SMI_CTRL);
		/* disable external phy cpu ctrl - Switch is SMI master */
		mvBoardExtPhyBufferSelect(MV_FALSE);

		/* implement differentiation between internal Phy#1 to external Phy#1
		 * Solve phy address conflicts */
	}

/*
 * Due to phy address 0x1 conflict, the following configurations demands :
 *      1. 'Disconnect' quad phy 0x1 from switch (EthComplex connectivity)
 *      2. Enable SMI Switch control, using mvCtrlSmiMasterSet(SWITCH_SMI_CTRL);
 *      3. use mvBoardExtPhyBufferSelect(MV_FALSE); to toggle external phy buffer limit
 */

	/* if SW_P4 is connected RGMII-0 && MAC-1 is NOT connected to RGMII-1  */
	/*
	if ((ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0) &&
		!(ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1 )) {
		mvEthPhyInit(???, MV_FALSE);
	*/

	/* if SW_P4 is connected RGMII-0 && MAC-1 is connected to RGMII-1  */
	/*
	 * if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_RGMII1 |
			  MV_ETHCOMP_SW_P4_2_RGMII0)) {
	*/
#endif
}
#endif /* MV88F66XX */

#if defined (MV88F66XX) || defined (MV88F672X)
void mvBoardPhyShutDown(MV_U16 phyAddr)
{
	MV_U16  phyRegData;

	/* Set GE-PHY SMI source as CPU MAC (CPU is shutting down the PHY) */
	mvEthComplexGphyPortSmiSrcSet(phyAddr, 0);

	mvEthPhyRegRead(phyAddr, 0x0, &phyRegData);
	mvEthPhyRegWrite(phyAddr, 0x0, phyRegData | BIT11);
	mvEthPhyRegRead(phyAddr, 0x10, &phyRegData);
	mvEthPhyRegWrite(phyAddr, 0x10, phyRegData | BIT2);

	/* set GE-PHY SMI source as switch
	 * (avoid future conflicts when CPU access external PHYs with same SMI address) */
	mvEthComplexGphyPortSmiSrcSet(phyAddr, 1);
}

void mvBoardLedMatrixPhyInit(MV_U16 smiAddr, MV_BOOL internalPhy)
{
	MV_U16 value = (internalPhy == MV_TRUE ? 0x1791 : 0x1771);

	mvEthPhyRegWrite(smiAddr, 0x16, 0x3);
	mvOsDelay(10);
	mvEthPhyRegWrite(smiAddr, 0x10, value);
	mvOsDelay(10);
	mvEthPhyRegWrite(smiAddr, 0x11, 0x8801);
	mvOsDelay(10);
	mvEthPhyRegWrite(smiAddr, 0x16, 0x0);
	mvOsDelay(10);
}

/*******************************************************************************
* switchPhyRegWrite 0 0 16 0 - Initialize LEDS Matrix
*******************************************************************************/
void mvBoardLedMatrixInit(void)
{
	MV_U8 i;

	if (mvCtrlDevFamilyIdGet(0) == MV_88F67X0)
		/* Led matrix mode in 7Bit */
		MV_REG_WRITE(LED_MATRIX_CONTROL_REG(0), BIT0 | BIT1);
	else {
		/* Led matrix mode in 12Bit */
		/* Enable Matrix, Set Mode B signals assignment, invert C1-C3 in Matrix*/
		MV_REG_WRITE(LED_MATRIX_CONTROL_REG(0), BIT0 | BIT2 | BIT5);
		/* initialize LEDS general configuration */
		MV_REG_WRITE(LED_MATRIX_CONTROL_REG(1), 0x2db6db6d);
		/* Use an internal device signal to drive the LED Matrix Control */
		MV_REG_WRITE(LED_MATRIX_CONTROL_REG(2), BIT0 | BIT2 | BIT3 | BIT5);

		/* initialize internal PHYs controlled by switch */
		if (mvBoardIsInternalSwitchConnected() == MV_TRUE) {
			for (i = 0; i < 4; i++) {
				mvOsDelay(10);
				mvEthSwitchPhyRegWrite(0x0, i, 0x16, 0x3);
				mvOsDelay(10);
				mvEthSwitchPhyRegWrite(0x0, i, 0x10, 0x1791);
				mvOsDelay(10);
				mvEthSwitchPhyRegWrite(0x0, i, 0x11, 0x8801);
				mvOsDelay(10);
				mvEthSwitchPhyRegWrite(0x0, i, 0x16, 0x0);
			}
		}
	}

	/* initialize External RGMII-0 PHY (SMI controlled by MAC0 @address 0x1) */
	if(mvBoardEthComplexConfigGet() & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY)
		mvBoardLedMatrixPhyInit(0x1, MV_FALSE);
        if(mvBoardEthComplexConfigGet() & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)
		mvBoardLedMatrixPhyInit(0x0, MV_TRUE);
	if(mvBoardEthComplexConfigGet() & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)
		mvBoardLedMatrixPhyInit(0x3, MV_TRUE);

}
#endif /* MV88F66XX || MV88F672X*/

/***********************************************************
 * Init the 10G PHY of the board                           *
 ***********************************************************/
MV_STATUS mvBoard10GPhyInit(MV_U32 port)
{
#ifdef CONFIG_MV_ETH_10G
	MV_U32 portType;

	portType = mvBoardPortTypeGet(port);
	if ((portType == MV_PORT_TYPE_RXAUI) || (portType == MV_PORT_TYPE_XAUI)) {
		mvNetComplexNssSelect(1);
		if (MV_ERROR == mvEth10gPhyInit(port, MV_FALSE)) {
			mvNetComplexNssSelect(0);
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, port, MV_FALSE);
			mvOsPrintf("PHY error - Failed to initialize 10G PHY (port %d).\n", port);
			return MV_FAIL;
		}
		mvNetComplexNssSelect(0);
		return MV_OK;
	}
#endif	/* CONFIG_MV_ETH_10G */
	return MV_NOT_SUPPORTED;
}

/***********************************************************
 * Init the PHY of the board                               *
 ***********************************************************/
void mvBoardEgigaPhyInit(void)
{

	/* Prepare HAL data information */
	mvSysEthPhyInit();
#if defined(CONFIG_MV_ETH_10G) && defined(CONFIG_MV_XSMI)
	mvSysEth10gPhyInit();
#endif

#if defined(MV_ETH_NETA)
	int i;
	MV_STATUS   status;
	MV_U32 phyAddr;

	for (i = 0; i < mvCtrlEthMaxPortGet(); i++) {
		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, i))
			continue;
		if (MV_FALSE ==  mvBoardIsGbEPortConnected(i))
			continue;
		phyAddr = mvBoardPhyAddrGet(i);
		if (phyAddr != -1) {
			/* writing the PHY address before PHY init */
			mvNetaPhyAddrSet(i, phyAddr);
			/* Initiaze 10G PHYs */
			status = mvBoard10GPhyInit(i);
			if (status == MV_OK)
				continue;
			if ((status != MV_OK) && (status != MV_NOT_SUPPORTED)) {
				mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, i, MV_FALSE);
				mvOsOutput("PHY error - shutdown port%d\n", i);
			} else if (MV_ERROR == mvEthPhyInit(i, MV_FALSE)) {
				mvNetaPhyAddrPollingDisable(i);
				mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, i, MV_FALSE);
				mvOsOutput("PHY error - shutdown port%d\n", i);
			} else if (mvBoardIsPortInMii(i)) {
				/* if port is MII the speed is les the 1Gbps need too change the phy advertisment */
				mvEthPhyAdvertiseSet(phyAddr, MV_PHY_ADVERTISE_100_FULL);
				/* after PHY advertisment change must reset PHY is needed*/
				status = mvEthPhyReset(phyAddr, 1000);
				if (status != MV_OK)
					mvOsPrintf("mvEthPhyReset(port=%d) failed: status=0x%x\n", i, status);
			}
		} else {
			/* Inband PHY control - disable PHY polling */
			 mvNetaPhyAddrPollingDisable(i);
		}
	}

#elif defined (MV88F66XX) /* Avanta-LP: dynamic PPv2 configuration */
        MV_U32 ethComplex = mvBoardEthComplexConfigGet();
	/* Init PHYs according to eth. complex configuration */
	if (ethComplex & (MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0 | MV_ETHCOMP_GE_MAC0_2_RGMII0 |
				MV_ETHCOMP_GE_MAC0_2_COMPHY_1 | MV_ETHCOMP_GE_MAC0_2_COMPHY_2 |
				MV_ETHCOMP_GE_MAC0_2_COMPHY_3 | MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 |
				MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES |
				MV_ETHCOMP_GE_MAC1_2_RGMII0 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP |
				MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY))
		mvAlpBoardEgigaPhyInit();

	if (mvBoardIsInternalSwitchConnected() == MV_TRUE)
		mvAlpBoardSwitchBasicInit(mvBoardSwitchPortsMaskGet(0));

	/* Close unnecessary internal phys */
	if(!(ethComplex & (MV_ETHCOMP_SW_P0_2_GE_PHY_P0 | MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)))
		mvBoardPhyShutDown(0x0);
	/* Shutdown GE-PHY#1:
	 * 1. RGMII-1 enabled & GE-PHY#1 disabled (both use same SMI address = 0x1)
		- set NO external SMI (to avoid mismatch when accessing SMI address 0x1)
		- shutdown GE-PHY#1 (CPU SMI access)
		- restore external SMI to CPU control */
	if((!(ethComplex & MV_ETHCOMP_SW_P1_2_GE_PHY_P1)) && (ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1)) {
		mvCtrlSmiMasterSet(NO_SMI_CTRL);
		mvBoardPhyShutDown(0x1);
		udelay(30);
		mvCtrlSmiMasterSet(CPU_SMI_CTRL);
	}
	/* 2. RGMII-1/GE-PHY#1/PON Serdes is not connected
		- shutdown GE-PHY#1 (CPU SMI access) */
	if(!(ethComplex & (MV_ETHCOMP_SW_P1_2_GE_PHY_P1 | MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)))
		mvBoardPhyShutDown(0x1);
	if(!(ethComplex & MV_ETHCOMP_SW_P2_2_GE_PHY_P2))
		mvBoardPhyShutDown(0x2);
	if(!(ethComplex & (MV_ETHCOMP_SW_P3_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)))
		mvBoardPhyShutDown(0x3);

	mvBoardLedMatrixInit();

	/* Disable external SMI control:
	 * If NO external RGMII/PHY are connected to CPU MACs */
	if (!(mvBoardEthComplexConfigGet() & (MV_ETHCOMP_GE_MAC0_2_RGMII0 |
					MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY |
					MV_ETHCOMP_GE_MAC1_2_RGMII1 |
					MV_ETHCOMP_GE_MAC1_2_RGMII0 |
					MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)))
		mvCtrlSmiMasterSet(NO_SMI_CTRL);

#elif defined(MV88F672X) /* Armada-375: static PPv2 configuration */
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();

	/* Set external CPU SMI control */
	mvCtrlSmiMasterSet(CPU_SMI_CTRL);

	/* Init PHY connected to MAC0 */
	if (ethComplex & (MV_ETHCOMP_GE_MAC0_2_RGMII0 | MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0))
		mvEthPhyInit(0, MV_FALSE);

	/* Init PHY connected to MAC1 */
	if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC1_2_RGMII1))
		mvEthPhyInit(1, MV_FALSE);

	/* Shutdown GE-PHYs:
	 * - disable external CPU SMI control
	 *   (avoid mismatch with equal SMI addresses between internal PHY and external RGMII PHY)
	 * - shutdown GE-PHY (CPU SMI access)
	 * - enable external CPU SMI control
	 */
	mvCtrlSmiMasterSet(NO_SMI_CTRL);

	if(!(ethComplex & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0))
		mvBoardPhyShutDown(0x0);

	/* Internal GE-PHY#1,2 are not used - close unnecessary internal phys */
	mvBoardPhyShutDown(0x1);
	mvBoardPhyShutDown(0x2);

	if(!(ethComplex & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3))
		mvBoardPhyShutDown(0x3);

	/* Restore external CPU SMI control */
	udelay(30);
	mvCtrlSmiMasterSet(CPU_SMI_CTRL);

	mvBoardLedMatrixInit();
#endif
}
