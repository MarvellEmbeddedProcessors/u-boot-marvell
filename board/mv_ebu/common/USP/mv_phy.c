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
#if defined(MV_ETH_NETA)
#include "neta/gbe/mvEthRegs.h"
#include "neta/gbe/mvNeta.h"
#elif defined (MV_ETH_PP2)
#include "pp2/gmac/mvEthGmacRegs.h"
#include "pp2/gmac/mvEthGmacApi.h"
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
			  MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)) {
		mvEthPhyInit(0, MV_FALSE);
	}

	/* for RD-6650 board, RGMII-0 is connected to switch port 4
	 * to avoid address conflicts between 1512 PHY with internal PHY (0x1),
	 * the 1512 PHY is also SMI controlled and initialized by MAC0
	 */
	if (ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY)
		mvEthPhyInit(0, MV_FALSE);

	/* Init PHY connected to MAC1 */
	if (ethComplex & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) {
		mvEthPhyInit(1, MV_FALSE);
	}

	/* if MAC-1 is connected to RGMII-1 && SW_P4 is not connected RGMII-0 */
	if ((ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1) &&
		!(ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY)) {

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
 * Init the PHY of the board                               *
 ***********************************************************/
void mvBoardEgigaPhyInit(void)
{

	/* Prepare HAL data information */
	mvSysEthPhyInit();

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
		/* writing the PHY address before PHY init */
		mvNetaPhyAddrSet(i, phyAddr);
		if (MV_ERROR == mvEthPhyInit(i, MV_FALSE)) {
			mvNetaPhyAddrPollingDisable(i);
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, i, MV_FALSE);
			mvOsOutput("PHY error - shutdown port%d\n", i);
		}
		else if (mvBoardIsPortInMii(i)) {
			/* if port is MII the speed is les the 1Gbps need too change the phy advertisment */
			mvEthPhyAdvertiseSet(phyAddr, MV_PHY_ADVERTISE_100_FULL);
			/* after PHY advertisment change must reset PHY is needed*/
			status = mvEthPhyReset(phyAddr, 1000);
			if (status != MV_OK)
				mvOsPrintf("mvEthPhyReset(port=%d) failed: status=0x%x\n", i, status);
		}
	}

#elif defined (MV88F66XX) /* Avanta-LP: dynamic PPv2 configuration */

	/* Init PHYs according to eth. complex configuration */
	mvAlpBoardEgigaPhyInit();

	if (mvBoardIsInternalSwitchConnected() == MV_TRUE)
		mvAlpBoardSwitchBasicInit(mvBoardSwitchPortsMaskGet(0));

	mvBoardLedMatrixInit();

	/* Disable external SMI control:
	 * If NO external RGMII/PHY are connected to CPU MACs */
	if (!(mvBoardEthComplexConfigGet() & (MV_ETHCOMP_GE_MAC0_2_RGMII0 |
					MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0 |
					MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY |
					MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 |
					MV_ETHCOMP_GE_MAC1_2_RGMII1)))
		mvCtrlSmiMasterSet(NO_SMI_CTRL);

#elif defined(MV88F672X) /* Armada-375: static PPv2 configuration */

	/* MAC0 is GE-PHY#0 on board*/
	/* MAC1 is GE-PHY#3 on board*/
	mvEthPhyInit(0, MV_FALSE);
	mvEthPhyInit(1, MV_FALSE);

	mvBoardLedMatrixInit();
#endif
}
