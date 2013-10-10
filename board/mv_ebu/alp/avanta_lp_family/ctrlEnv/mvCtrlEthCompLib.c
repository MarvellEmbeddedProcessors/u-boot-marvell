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

    * 	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *	Neither the name of Marvell nor the names of its contributors may be
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

#include <mvCommon.h>
#include <mvOs.h>
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvCtrlEthCompLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "pp2/gmac/mvEthGmacRegs.h"
#include "pp2/gbe/mvPp2Gbe.h"

static void mvEthComplexGbePhySrcSet(MV_U32 phy, MV_U32 src)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);
	reg &= ~ETHCC_GBE_PHY_PORT_SMI_SRC_MASK(phy);

	src <<= ETHCC_GBE_PHY_PORT_SMI_SRC_OFFSET(phy);
	src &= ETHCC_GBE_PHY_PORT_SMI_SRC_MASK(phy);

	reg |= src;

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);
}

static MV_U32 mvEthComplexSwPortSrcCalc(MV_U32 swPort, enum mvSwPortSrc src)
{
	MV_U32 val = 0x0;

	if (swPort != 0 && swPort != 3 && swPort != 4 && swPort != 6) {
		mvOsPrintf("%s: Wrong switch port (%d)\n",
			   __func__, swPort);
		return 0x0;
	}

	switch (swPort) {
	case 0:
	case 3:
		if (src == ETHC_SW_PORT_SRC_GBE_PHY)
			val = 0x1;
		else if (src == ETHC_SW_PORT_SRC_NC)
			val = 0x0;
		else {
			mvOsPrintf("%s: Wrong src (%d) for switch port (%d)\n",
				   __func__, src, swPort);
		}
		break;
	case 4:
		if (src == ETHC_SW_PORT_SRC_MPP)
			val = 0x1;
		else if (src == ETHC_SW_PORT_SRC_GBE_MAC)
			val = 0x0;
		else {
			mvOsPrintf("%s: Wrong src (%d) for switch port (%d)\n",
				   __func__, src, swPort);
		}
		break;
	case 6:
		if (src == ETHC_SW_PORT_SRC_GBE_MAC)
			val = 0x1;
		else if (src == ETHC_SW_PORT_SRC_NC)
			val = 0x0;
		else {
			mvOsPrintf("%s: Wrong src (%d) for switch port (%d)\n",
				   __func__, src, swPort);
		}
		break;
	}

	return val;
}

static void mvEthComplexSwPortSrcSet(MV_U32 swPort, MV_U32 src)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);
	reg &= ~ETHCC_SW_PORT_SRC_MASK(swPort);

	src <<= ETHCC_SW_PORT_SRC_OFFSET(swPort);
	src &= ETHCC_SW_PORT_SRC_MASK(swPort);

	reg |= src;

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);
}

static void mvEthComplexGbePortSrcSet(MV_U32 port, MV_U32 src)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);
	reg &= ~ETHCC_GBE_MAC_SRC_MASK(port);

	src <<= ETHCC_GBE_MAC_SRC_OFFSET(port);
	src &= ETHCC_GBE_MAC_SRC_MASK(port);

	reg |= src;

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);
}

/*
 * Set speed Gbe Port 0 when it is connected to switch port 6
 */
static void mvEthComplexGbeToSwitchSpeedSet(MV_BOARD_MAC_SPEED speed)
{
	MV_U32 reg;

	if (speed != BOARD_MAC_SPEED_1000M && speed != BOARD_MAC_SPEED_2000M) {
		mvOsPrintf("%s: wrong speed (%d)\n", __func__, speed);
		return;
	}

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);
	reg &= ~ETHCC_GE_MAC0_SW_PORT_6_SPEED_MASK;

	if (speed == BOARD_MAC_SPEED_2000M)
		reg |= (0x1 << ETHCC_GE_MAC0_SW_PORT_6_SPEED_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);

	if (speed == BOARD_MAC_SPEED_2000M) {
		reg = MV_REG_READ(MV_ETHCOMP_SW_CONFIG_RESET_CTRL);
		reg &= ~ETHSCRC_PORT_2G_SELECT_MASK;
		reg |= (0x1 << ETHSCRC_PORT_2G_SELECT_OFFSET);
		MV_REG_WRITE(MV_ETHCOMP_SW_CONFIG_RESET_CTRL, reg);
	}
}

static void mvEthComplexPortDpClkSrcSet(MV_U32 port, MV_U32 src)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_GOP_CTRL_STAT_REG);
	reg &= ~ETHCGCS_PORT_DP_CLK_SRC_MASK(port);

	src <<= ETHCGCS_PORT_DP_CLK_SRC_OFFSET(port);
	src &= ETHCGCS_PORT_DP_CLK_SRC_MASK(port);

	reg |= src;

	MV_REG_WRITE(MV_ETHCOMP_GOP_CTRL_STAT_REG, reg);
}

static void mvEthComplexGopDevEnable(void)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_GOP_CTRL_STAT_REG);
	reg &= ~ETHCGCS_GOP_ENABLE_DEV_MASK;
	reg |= (0x1 << ETHCGCS_GOP_ENABLE_DEV_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_GOP_CTRL_STAT_REG, reg);
}

static void mvEthComplexSwResetSet(MV_BOOL setReset)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_SW_CONFIG_RESET_CTRL);
	reg &= ~ETHSCRC_SWITCH_RESET_MASK;

	if (setReset == MV_TRUE)
		reg |= (0x0 << ETHSCRC_SWITCH_RESET_OFFSET);
	else
		reg |= (0x1 << ETHSCRC_SWITCH_RESET_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_SW_CONFIG_RESET_CTRL, reg);
}

static void mvEthComplexGbePhyPowerSet(MV_U32 phy, MV_BOOL setPowerUp)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy));
	reg &= ~ETHCGPC1_PHY_POWER_DOWN_MASK;

	if (setPowerUp == MV_TRUE)
		reg |= (0x0 << ETHCGPC1_PHY_POWER_DOWN_OFFSET);
	else
		reg |= (0x3 << ETHCGPC1_PHY_POWER_DOWN_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy), reg);
}

static void mvEthComplexGbePhyPowerCycle(MV_U32 phy)
{
	mvEthComplexGbePhyPowerSet(phy, MV_FALSE);
	mvOsDelay(100);
	mvEthComplexGbePhyPowerSet(phy, MV_TRUE);
}

static void mvEthComplexGbePhyPdConfigEdetASet(MV_U32 phy, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy));
	reg &= ~ETHCGPC1_PD_CFG_EDED_A_MASK;

	val <<= ETHCGPC1_PD_CFG_EDED_A_OFFSET;
	val &= ETHCGPC1_PD_CFG_EDED_A_MASK;

	reg |= val;

	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy), reg);
}

static void mvEthComplexGbePhyPsEnaXcSSet(MV_U32 phy, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy));
	reg &= ~ETHCGPC1_PS_ENA_XCS_MASK;

	val <<= ETHCGPC1_PS_ENA_XCS_OFFSET;
	val &= ETHCGPC1_PS_ENA_XCS_MASK;

	reg |= val;

	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy), reg);
}

static void mvEthComplexGbePhyResetSet(MV_BOOL setReset)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG);

	reg &= ~ETHQPCS_DPLL_RESET_MASK;
	reg &= ~ETHQPCS_RESET_MASK;

	if (setReset == MV_TRUE) {
		reg |= (0x0 << ETHQPCS_RESET_OFFSET);
		reg |= (0x0 << ETHQPCS_DPLL_RESET_OFFSET);
	} else {
		reg |= (0x1 << ETHQPCS_RESET_OFFSET);
		reg |= (0x1 << ETHQPCS_DPLL_RESET_OFFSET);
	}

	MV_REG_WRITE(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG, reg);
}

static void mvEthComplexMacToSwPort(MV_U32 port, MV_U32 swPort,
				   MV_BOARD_MAC_SPEED speed)
{
	MV_U32 src;

	mvEthComplexSwResetSet(MV_FALSE);
	mvOsDelay(200);

	src = mvEthComplexSwPortSrcCalc(swPort, ETHC_SW_PORT_SRC_GBE_MAC);
	mvEthComplexSwPortSrcSet(swPort, src);
	mvEthComplexGbePortSrcSet(port, 0x1);

	/* GE MAC #0 - Switch Port6 2G Speed */
	if (port == 0 && swPort == 6 && speed == BOARD_MAC_SPEED_2000M) {
		mvEthComplexGbeToSwitchSpeedSet(speed);
		mvEthComplexPortDpClkSrcSet(port, 0x0);
	} else
		mvEthComplexPortDpClkSrcSet(port, 0x1);
}

static void mvEthComplexSwPortToRgmii(MV_U32 swPort, MV_U32 port)
{
	MV_U32 src;

	src = mvEthComplexSwPortSrcCalc(swPort, ETHC_SW_PORT_SRC_MPP);
	mvEthComplexSwPortSrcSet(swPort, src);
}

static void mvEthComplexXponMacToPonSerdes(void)
{
	/* Not implemented */
}

static void mvEthComplexMacToGbePhy(MV_U32 port, MV_U32 phy, MV_U32 phyAddr)
{
	mvEthComplexGbePhySrcSet(phy, 0x0);
	mvEthComplexGbePortSrcSet(port, 0x2);

	if (port == 0)
		mvEthComplexSwPortSrcSet(6, 0x0);

	mvEthComplexPortDpClkSrcSet(port, 0x1);
	mvEthComplexGbePhyPdConfigEdetASet(phy, 0x0);
	mvEthComplexGbePhyPsEnaXcSSet(phy, 0x0);
	mvEthComplexGbePhyResetSet(MV_FALSE);
}

static void mvEthComplexMacToComPhy(MV_U32 port, MV_U32 comPhy)
{
	/* Not implemented */
}

static void mvEthComplexMac1ToPonSerdes(MV_U32 port)
{
	/* Not implemented */
}

static void mvEthComplexMacToRgmii(MV_U32 port, MV_U32 phy)
{
	mvEthComplexGbePortSrcSet(port, 0x0);
	mvEthComplexPortDpClkSrcSet(port, 0x1);
	mvEthComplexGopDevEnable();
}

static void mvEthComplexSwPortToGbePhy(MV_U32 swPort, MV_U32 phy)
{
	mvEthComplexSwPortSrcSet(swPort, 0x1);
	mvEthComplexGbePhySrcSet(phy, 0x1);
	mvEthComplexGbePhyResetSet(MV_FALSE);
	mvEthComplexGbePhyPowerCycle(phy);
}

MV_STATUS mvEthComplexInit(MV_U32 ethCompConfig)
{
	MV_U32 c = ethCompConfig;

	mvEthComplexGopDevEnable();

	if (c & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		mvEthComplexMacToSwPort(0, 6, mvBoardMacSpeedGet(0));

	if (c & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)
		mvEthComplexMacToGbePhy(0, 0, mvBoardPhyAddrGet(0));

	if (c & MV_ETHCOMP_GE_MAC0_2_RGMII0)
		mvEthComplexMacToRgmii(0, mvBoardPhyAddrGet(0));

	if (c & MV_ETHCOMP_GE_MAC0_2_COMPHY_1)
		mvEthComplexMacToComPhy(0, 1);

	if (c & MV_ETHCOMP_GE_MAC0_2_COMPHY_2)
		mvEthComplexMacToComPhy(0, 2);

	if (c & MV_ETHCOMP_GE_MAC0_2_COMPHY_3)
		mvEthComplexMacToComPhy(0, 3);

	if (c & MV_ETHCOMP_GE_MAC1_2_SW_P4)
		mvEthComplexMacToSwPort(1, 4, mvBoardMacSpeedGet(1));

	if (c & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)
		mvEthComplexMacToGbePhy(1, 3, mvBoardPhyAddrGet(1));

	if (c & MV_ETHCOMP_GE_MAC1_2_RGMII1)
		mvEthComplexMacToRgmii(1, mvBoardPhyAddrGet(1));

	if (c & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)
		mvEthComplexMac1ToPonSerdes(1);

	if (c & MV_ETHCOMP_SW_P0_2_GE_PHY_P0)
		mvEthComplexSwPortToGbePhy(0, 0);

	if (c & MV_ETHCOMP_SW_P1_2_GE_PHY_P1)
		mvEthComplexSwPortToGbePhy(0, 0);

	if (c & MV_ETHCOMP_SW_P2_2_GE_PHY_P2)
		mvEthComplexSwPortToGbePhy(0, 0);

	if (c & MV_ETHCOMP_SW_P3_2_GE_PHY_P3)
		mvEthComplexSwPortToGbePhy(3, 3);

	if (c & MV_ETHCOMP_SW_P4_2_RGMII0)
		mvEthComplexSwPortToRgmii(4, 0);

	if (c & MV_ETHCOMP_P2P_MAC_2_PON_ETH_SERDES)
		mvEthComplexXponMacToPonSerdes();

	return MV_OK;
}
