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

/***********************************************************
* Init the PHY of the board                        *
***********************************************************/
void mvBoardEgigaPhyInit(void)
{
	int i;

	/* Set SMI control to CPU, before initializing phy */
	mvCtrlSmiMasterSet(CPU_SMI_CTRL);

	mvSysEthPhyInit();

#if defined (MV88F66XX)
	return mvBoardALPEgigaPhyInit();
#elif defined (MV88F67XX)
	if (mvBoardIsSwitchConnected())
		mvEthE6171SwitchBasicInit(1);

	if (mvBoardIsSwitchConnected() || mvBoardIsGMIIConnected()) {
		/* Init Only Phy 0 */
		/* writing the PHY address before PHY init */
		unsigned int regData;
		regData = MV_REG_READ(ETH_PHY_ADDR_REG(0));
		regData &= ~ETH_PHY_ADDR_MASK;
		regData |= mvBoardPhyAddrGet(0);
		MV_REG_WRITE(ETH_PHY_ADDR_REG(0), regData);
		mvEthPhyInit(0, MV_FALSE);
	} else
#endif
	{
		for (i = 0; i < mvCtrlEthMaxPortGet(); i++) {
			/* writing the PHY address before PHY init */
#ifdef MV_ETH_PP2
			mvEthPhyAddrSet(i, mvBoardPhyAddrGet(i));
#else
			mvNetaPhyAddrSet(i, mvBoardPhyAddrGet(i));
#endif
			mvEthPhyInit(i, MV_FALSE);
		}
	}

	/* If switch is in use, Set SMI control back to Switch, after initializing phy */
	if (mvBoardIsInternalSwitchConnected(0) || mvBoardIsInternalSwitchConnected(1))
		mvCtrlSmiMasterSet(SWITCH_SMI_CTRL);
}

/***********************************************************
*     Specific Init the PHY of the board for Avanta LP     *
***********************************************************/
#if defined (MV88F66XX)
void mvBoardALPEgigaPhyInit(void)
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();
	MV_U32 portEnabled = 0;
	MV_U32 phyEnabled = 0;

	/* 1st Run switch basic init with enabled ports, then initalize internal Quad PHYs*/
	if (mvBoardIsInternalSwitchConnected(0) || mvBoardIsInternalSwitchConnected(1)) {
		if (ethComplex & (MV_ETHCOMP_GE_MAC0_2_SW_P6 ))
			portEnabled |= BIT6;
		if (ethComplex & MV_ETHCOMP_GE_MAC1_2_SW_P4)
			portEnabled |= BIT4;
		if (ethComplex & MV_ETHCOMP_SW_P0_2_GE_PHY_P0) {
			portEnabled |= BIT0;
			phyEnabled |= BIT0;
		}
		if (ethComplex & MV_ETHCOMP_SW_P3_2_GE_PHY_P3) {
			portEnabled |= BIT3;
			phyEnabled |= BIT3;
		}

		portEnabled |= BIT1 | BIT2;
		phyEnabled |= BIT1 | BIT2;      /* ports 1-2 are connected to switch by default */

		mvEthALPSwitchBasicInit(portEnabled);
		mvEthInternalQuadPhyBasicInit(phyEnabled, MV_FALSE);
	}

	/* if MAC-0 is connected through switch port 6, or directly to RGMII-0 / Quad Phy */
	if (ethComplex & (MV_ETHCOMP_GE_MAC0_2_SW_P6 | MV_ETHCOMP_GE_MAC0_2_RGMII0 | MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0 ))
		mvEthPhyInit(0, MV_FALSE);

	/* if MAC-1 is connected through switch port 4, or directly to Quad Phy */
	if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_SW_P4 | MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3))
		mvEthPhyInit(1, MV_FALSE);

	/* if Switch port 4 is connected to RGMII-0 external phy
	 * (if SW-P4 is not connected to MAC1 or MAC0 is not connected to RGMII-0  */
//	if ( !(ethComplex & (MV_ETH_COMPLEX_GE_MAC1_SW_P4| MV_ETH_COMPLEX_GE_MAC0_RGMII0)) )
//		mvEthPhyInit(???, MV_FALSE);

	/* if MAC-1 is connected to RGMII-1
	 * due to phy address 0x1 conflict, first disable quad phy 0x1 cpu ctrl, and enable externl phy cpu ctrl:
	 *      - force quad phy 0x1 SMI ctrl to switch SMI ctrl
	 *      - Enable external RGMII-1 phy buffer  */
	if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_RGMII1)) {
		/* enable external phy cpu ctrl */
		mvBoardExtPhyBufferSelect(MV_TRUE);

		mvEthPhyInit(1, MV_FALSE);

		/* disable external phy cpu ctrl */
		mvBoardExtPhyBufferSelect(MV_FALSE);

	}

}

MV_VOID mvEthInternalQuadPhyBasicInit(MV_U32 enabledPhys, MV_BOOL eeeEnable)
{
	int i;
	MV_U16 reg;
	MV_U32 port = 0;

	//switchPhyRegWrite 0 1 0x1D 9
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 9);

	//switchPhyRegWrite 0 1 0x1F  bits[6:5] change to 0
	//switchPhyRegWrite 0 2 0x1F  bits[6:5] change to 0
	//switchPhyRegWrite 0 3 0x1F  bits[6:5] change to 0
	for (i = 1; i < 4; i++) {
		mvEthSwitchPhyRegRead(port, i, 0x1F, &reg);
		reg &= ~(BIT6 | BIT5);
		mvEthSwitchPhyRegWrite(port, i, 0x1F, reg);
	}

	//switchPhyRegWrite 0 1 0x1D 10
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0x10);

	//switchPhyRegWrite 0 1 0x1E bit [13] change to 0
	//switchPhyRegWrite 0 2 0x1E bit [13] change to 0
	//switchPhyRegWrite 0 3 0x1E bit [13] change to 0
	for (i = 1; i < 4; i++) {
		mvEthSwitchPhyRegRead(port, i, 0x1E, &reg);
		reg &= ~(BIT13);
		mvEthSwitchPhyRegWrite(port, i, 0x1E, reg);
	}

	//switchPhyRegWrite 0 1 0x1D 0
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0);

	/* Raise falltime configuration. */
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0x8005);
	mvEthSwitchPhyRegWrite(port, 1, 0x1E, 0x2000);
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0x9005);
	mvEthSwitchPhyRegWrite(port, 1, 0x1E, 0x2000);
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0xA005);
	mvEthSwitchPhyRegWrite(port, 1, 0x1E, 0x2000);

	for (i = 1; i < 4; i++) {
		mvEthSwitchPhyRegWrite(port, i, 0x1C, 0xF03);

		/* soft reset the phy */
		mvEthSwitchPhyRegRead(port, i, 0, &reg);
		reg |= BIT15;
		mvEthSwitchPhyRegWrite(port, i, 0, reg);
	}
}

#endif /* MV88F66XX */
