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

#ifndef __INCmvCtrlEthCompLibh
#define __INCmvCtrlEthCompLibh

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysEthConfig.h"

/*******************************************************************************
 * Ports Group Control and Status */
#define MV_ETHCOMP_GOP_CTRL_STAT_REG			MV_ETH_COMPLEX_BASE

#define     ETHCGCS_PORT_DP_CLK_SRC_OFFSET(port)	((port == 0) ? 13 : 14)
#define     ETHCGCS_PORT_DP_CLK_SRC_MASK(port)		(0x1 << ETHCGCS_PORT_DP_CLK_SRC_OFFSET(port))

#define     ETHCGCS_GOP_ENABLE_DEV_OFFSET		19
#define     ETHCGCS_GOP_ENABLE_DEV_MASK			(0x1 << ETHCGCS_GOP_ENABLE_DEV_OFFSET)

/*******************************************************************************
* Ports Group Control and Status */
#define MV_ETHCOMP_GBE_PHY_CLOCK_CTRL_REG		(MV_ETH_COMPLEX_BASE + 0x1C)

#define     ETHCC_GBE_PHY_MPP_TO_SW_P4_EDGE_OFFSET	22
#define     ETHCC_GBE_PHY_MPP_TO_SW_P4_EDGE_MASK        (0x1 << ETHCC_GBE_PHY_MPP_TO_SW_P4_EDGE_OFFSET)

#define     ETHCC_GBE_PHY_GBE_P0_TO_MPP_EDGE_OFFSET	28
#define     ETHCC_GBE_PHY_GBE_P0_TO_MPP_EDGE_MASK	(0x1 << ETHCC_GBE_PHY_MPP_TO_SW_P4_EDGE_OFFSET)

/*******************************************************************************
 * Ethernet Complex Control 0 */
#define MV_ETHCOMP_CTRL_REG				(MV_ETH_COMPLEX_BASE + 0x10)

#define     ETHCC_SW_PORT_SRC_OFFSET(port)		(port == 0 ? 4 : (port == 3 ? 5 : \
							(port == 4 ? 6 : (port == 6 ? 7 : 4))))
#define     ETHCC_SW_PORT_SRC_MASK(port)		(0x1 << ETHCC_SW_PORT_SRC_OFFSET(port))

enum mvSwPortSrc {
	ETHC_SW_PORT_SRC_NC,
	ETHC_SW_PORT_SRC_GBE_MAC,
	ETHC_SW_PORT_SRC_MPP,
	ETHC_SW_PORT_SRC_GBE_PHY,
};

#define     ETHCC_GBE_MAC_SRC_OFFSET(port)		(port == 0 ? 10 : (port == 1 ? 12 : 10))
#define     ETHCC_GBE_MAC_SRC_MASK(port)		(0x3 << ETHCC_GBE_MAC_SRC_OFFSET(port))

#define     ETHCC_GBE_PHY_PORT_SMI_SRC_OFFSET(phy)	((phy >= 0 && phy <= 3) ? 14 + phy : 14)
#define     ETHCC_GBE_PHY_PORT_SMI_SRC_MASK(phy)	(0x1 << ETHCC_GBE_PHY_PORT_SMI_SRC_OFFSET(phy))

#define     ETHCC_GE_MAC0_SW_PORT_6_SPEED_OFFSET	18
#define     ETHCC_GE_MAC0_SW_PORT_6_SPEED_MASK		(0x1 << ETHCC_GE_MAC0_SW_PORT_6_SPEED_OFFSET)

#define     ETHCC_LOOPBACK_PORT_SPEED_OFFSET		19
#define     ETHCC_LOOPBACK_PORT_SPEED_MASK		(0x1 << ETHCC_LOOPBACK_PORT_SPEED_OFFSET)

/*******************************************************************************
 * Switch Configuration and Reset Control
 */
#define MV_ETHCOMP_SW_CONFIG_RESET_CTRL			(MV_ETH_COMPLEX_BASE + 0x30)

#define     ETHSCRC_SWITCH_RESET_OFFSET			0
#define     ETHSCRC_SWITCH_RESET_MASK			(0x1 << ETHSCRC_SWITCH_RESET_OFFSET)

#define     ETHSCRC_PORT_2G_SELECT_OFFSET		14
#define     ETHSCRC_PORT_2G_SELECT_MASK			(0x1 << ETHSCRC_PORT_2G_SELECT_OFFSET)

/*******************************************************************************
 * GbE PHY <<%n>> Control 0.
 * 0x000189A0 + n*4: where n (0-3) represents GPHY_Num.
 */
#define MV_ETHCOMP_GBE_PHY_CTRL0_REG(phy)		(MV_ETH_COMPLEX_BASE + 0xA0 + (phy * 0x4))

#define     ETHCGPC0_PCS_PHY_ADDR_OFFSET		13
#define     ETHCGPC0_PCS_PHY_ADDR_MASK			(0x1F << ETHCGPC0_PCS_PHY_ADDR_OFFSET)

/*******************************************************************************
 * GbE PHY <<%n>> Control 1.
 * 0x000189B0 + n*4: where n (0-3) represents GPHY_Num.
 */
#define MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy)		(MV_ETH_COMPLEX_BASE + 0xB0 + (phy * 0x4))

#define     ETHCGPC1_PHY_POWER_DOWN_OFFSET		10
#define     ETHCGPC1_PHY_POWER_DOWN_MASK		(0x3 << ETHCGPC1_PHY_POWER_DOWN_OFFSET)

#define     ETHCGPC1_PS_ENA_XCS_OFFSET			12
#define     ETHCGPC1_PS_ENA_XCS_MASK			(0x3 << ETHCGPC1_PS_ENA_XCS_OFFSET)

#define     ETHCGPC1_PD_CFG_EDED_A_OFFSET		14
#define     ETHCGPC1_PD_CFG_EDED_A_MASK			(0x7 << ETHCGPC1_PD_CFG_EDED_A_OFFSET)

/*******************************************************************************
 * Quad GbE PHY Common Control and Status
 */
#define MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG		(MV_ETH_COMPLEX_BASE + 0xD0)

#define     ETHQPCS_RESET_OFFSET			0
#define     ETHQPCS_RESET_MASK				(0x1 << ETHQPCS_DPLL_RESET_OFFSET)

#define     ETHQPCS_DPLL_RESET_OFFSET			4
#define     ETHQPCS_DPLL_RESET_MASK			(0x1 << ETHQPCS_DPLL_RESET_OFFSET)

#ifdef CONFIG_MV_ETH_PP2
MV_STATUS mvEthComplexInit(MV_U32 ethCompConfig);
#else
MV_STATUS mvEthComplexInit(MV_U32 ethCompConfig) { /* empty */ }
#endif

#endif /* __INCmvCtrlEthCompLibh */
