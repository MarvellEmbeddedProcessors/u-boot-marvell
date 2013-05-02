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
#include "ctrlEnv/mvCtrlEthCompLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "pp2/gmac/mvEthGmacRegs.h"
#include "pp2/gbe/mvPp2Gbe.h"

static MV_BOOL gEthComplexSkipInit = MV_FALSE;

/******************************************************************************
* mvEthCompSkipInitSet
*
* DESCRIPTION:
*	Configure the eth-complex to skip initialization.
*
* INPUT:
*	skip - MV_TRUE to skip initialization.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
void mvEthCompSkipInitSet(MV_BOOL skip)
{
	gEthComplexSkipInit = skip;
	return;
}

/******************************************************************************
* mvEthCompMac2SwitchConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to switch ports 4/6 mode.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*	muxCfgOnly - MV_TRUE: Configure only the ethernet complex mux'es and
*		     skip other switch reset configurations.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac2SwitchConfig(MV_U32 ethCompCfg, MV_BOOL muxCfgOnly)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompSwitchReset
*
* DESCRIPTION:
*	Reset switch device after being configured by ethernet complex functions.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwitchReset(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompMac2RgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac2RgmiiConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompSwP56ToRgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch port 5 or 6 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwP56ToRgmiiConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}


/******************************************************************************
* mvEthCompSataConfig
*
* DESCRIPTION:
*	Configure ethernet complex for sata port output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSataConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompShutdownIf
*
* DESCRIPTION:
*	Shutdown ethernet complex interfaces.
*
* INPUT:
*	integSwitch	- MV_TRUE to shutdown the integrated switch.
*	gePhy		- MV_TRUE to shutdown the GE-PHY
*	fePhy		- MV_TRUE to shutdown the 3xFE PHY.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompShutdownIf(MV_BOOL integSwitch, MV_BOOL gePhy, MV_BOOL fePhy)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompGopPhySmiAutoPollSet
*
* DESCRIPTION:
* 	Disables hardware PHY polling mode.
* 	Relevant for SMI Interface0.
* 	Used for Auto-Negotiation and PHY configuration of the PHY devices
* 	connected to ports 0 through 11. Stops the Auto-Negotiation process
* 	on SMI Interface0. When the CPU accesses a PHY via SMI Interface0,
* 	this bit must be set to 1 when a 88E1112 is connected to one of the ports.
* 	NOTE: Although the device ignores the information read from the
* 	PHY registers, it keeps polling these registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
#if 0 /* This bit does not work !!! */
static void mvEthCompGopPhySmiAutoPollSet(MV_BOOL enable)
{
	if (enable == MV_TRUE)
		MV_REG_BIT_RESET(ETH_PHY_AN_CFG0_REG(0),
				 ETH_PHY_AN_CFG0_STOP_AN_SMI0_MASK);
	else
		MV_REG_BIT_SET  (ETH_PHY_AN_CFG0_REG(0),
				 ETH_PHY_AN_CFG0_STOP_AN_SMI0_MASK);
}
#endif

/******************************************************************************
* mvEthCompGopPhySmiAddrSet
*
* DESCRIPTION:
*	Perform basic setup that is needed before configuring the eth-complex
*	registers.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompGopPhySmiAddrSet(MV_U32 port, MV_U32 smiAddr)
{
	MV_U32 reg;

	/*
	 * Set PHY SMI address: set PcsPhyAddress to PHY SMI address.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL0_REG(port));
	reg &= ~ETHCGPC0_PCS_PHY_ADDR_MASK;
	smiAddr &= ETHCGPC0_PCS_PHY_ADDR_MASK;
	reg |= (smiAddr << ETHCGPC0_PCS_PHY_ADDR_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL0_REG(port), reg);
}

/******************************************************************************
* mvEthCompMacSetReset
*
* DESCRIPTION:
* 	None.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
void mvEthCompMacSetReset(MV_U32 port, MV_BOOL enable)
{
	MV_U32 reg, mask;

	if (enable == MV_TRUE)
		mask = 0x1;
	else
		mask = 0x0;

	/*
	 * Set 'PortMACReset' in Port MAC Control Register 2 to 'Port_MAC_Not_reset'.
	 */
	reg = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	reg &= ~ETH_GMAC_PORT_RESET_MASK;
	reg |= (mask << ETH_GMAC_PORT_RESET_BIT);
	MV_REG_WRITE(ETH_GMAC_CTRL_2_REG(port), reg);
}

/******************************************************************************
* mvEthCompPreInit
*
* DESCRIPTION:
*	Perform basic setup that is needed before configuring the eth-complex
*	registers.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompPreInit(MV_U32 ethCompCfg, MV_U32 smiAddr)
{
#if 0
	mvEthCompMacSetReset(GBE_PORT(0), MV_TRUE);
	mvEthCompMacSetReset(GBE_PORT(1), MV_TRUE);
#endif

	/* mvEthCompGopPhySmiAutoPollSet(MV_FALSE); <--- does not works according to Eran Maor !!! */

	mvEthCompGopPhySmiAddrSet(GBE_PORT(0), smiAddr);
	mvEthCompGopPhySmiAddrSet(GBE_PORT(1), smiAddr);
}

/******************************************************************************
* mvEthCompPostInit
*
* DESCRIPTION:
*	Perform basic setup that is needed after configuring the eth-complex
*	registers.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompPostInit(MV_U32 ethCompCfg)
{
	/* mvEthCompGopPhySmiAutoPollSet(MV_FALSE); <--- does not works according to Eran Maor !!! */
}

/******************************************************************************
* mvEthCompMacSetAutoNegMode
*
* DESCRIPTION:
* 	None.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompMacSetAutoNegMode(MV_U32 macId)
{
	MV_U32 reg;

	/*
	 * When working on AN mode, set the following:
	 * 'AnFcEn'		=> 'AnFcEnable'
	 * 'PauseAdv'		=> 'Support_Flow_Control'
	 * 'AnSpeedEn'		=> 'Enable_Update'
	 * 'AnDuplexEn'		=> 'AnDuplexEnable'
	 * 'SetFullDuplex'	=> 'Full_duplex'
	 */

	reg = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(macId));

	/* 'AnFcEn'		=> 'AnFcEnable' */
	reg &= ~ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
	reg |= (0x1 << ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_BIT);

	/* 'PauseAdv'		=> 'Support_Flow_Control' */
	reg &= ~ETH_FLOW_CONTROL_ADVERTISE_MASK;
	reg |= (0x1 << ETH_FLOW_CONTROL_ADVERTISE_BIT);

	/* 'AnSpeedEn'		=> 'Enable_Update' */
	reg &= ~ETH_ENABLE_SPEED_AUTO_NEG_MASK;
	reg |= (0x1 << ETH_ENABLE_SPEED_AUTO_NEG_BIT);

	/* 'AnDuplexEn'		=> 'AnDuplexEnable' */
	reg &= ~ETH_ENABLE_DUPLEX_AUTO_NEG_MASK;
	reg |= (0x1 << ETH_ENABLE_DUPLEX_AUTO_NEG_BIT);

	/* 'SetFullDuplex'	=> 'Full_duplex' */
	reg &= ~ETH_SET_FULL_DUPLEX_MASK;
	reg |= (0x1 << ETH_SET_FULL_DUPLEX_BIT);

	MV_REG_WRITE(ETH_GMAC_AN_CTRL_REG(macId), reg);
}

/******************************************************************************
* mvEthCompMac0ToGePhyConfig
*
* DESCRIPTION:
*	This flow assumes Auto Negotionation (AN) between MAC and GE PHY.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompMacToGePhyConfig(MV_U32 port, MV_U32 phy, MV_U32 ethCompCfg)
{
	MV_U32 reg;

	if ((ethCompCfg & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) == 0)
		return;

	/*----------------------------------------------------------------------
	 * GE MAC 0 to GE PHY Port 0
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);

	/* Set 'GbEPhyPort0Source' field in Ethernet Complex Control 0 reg to 0x0 */
	reg &= ~ETHCC_GBE_PHY_PORT_0_SRC_MASK;
	reg |= (0x0 << ETHCC_GBE_PHY_PORT_0_SRC_OFFSET);

	/* Set 'GeMAC0Source' field in Ethernet Complex Control 0 reg to 0x2 */
	reg &= ~ETHCC_GBE_MAC0_SRC_MASK;
	reg |= (0x2 << ETHCC_GBE_MAC0_SRC_OFFSET);

	/* Set 'SwitchPort6Source' field in Ethernet Complex Control 0 to 0x0 'UC' */
	reg &= ~ETHCC_SW_PORT_6_SRC_MASK;
	reg |= (0x0 << ETHCC_SW_PORT_6_SRC_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);

	/*----------------------------------------------------------------------
	 * Set 'Port[0/1]DpClkSource' field in Ports Group Control and Status to 0x1
	 */
	reg = MV_REG_READ(MV_ETHCOMP_GOP_CTRL_STAT_REG);
	reg &= ~ETHCGCS_PORT_DP_CLK_SRC_MASK(port);
	reg |= (0x1 << ETHCGCS_PORT_DP_CLK_SRC_OFFSET(port));
	MV_REG_WRITE(MV_ETHCOMP_GOP_CTRL_STAT_REG, reg);

	/*----------------------------------------------------------------------
	 * GE PHY configuration
	 */

	/* Set PHY SMI address: set PcsPhyAddress to PHY SMI address.
	 * Note: it is set in pre-init function.
	 */

	/* Set PdConfigEdetA field */
	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy));
	reg &= ~ETHCGPC1_PD_CFG_EDED_A_MASK;
	reg |= (0x0 << ETHCGPC1_PD_CFG_EDED_A_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy), reg);

	/* Set PsEnaXcS field */
	reg = MV_REG_READ(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy));
	reg &= ~ETHCGPC1_PS_ENA_XCS_MASK;
	reg |= (0x0 << ETHCGPC1_PS_ENA_XCS_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_GBE_PHY_CTRL1_REG(phy), reg);

	mvOsDelay(200);

	/* Set 'DPLLReset' field in Quad Gbe PHY Common Control and Status
	 * register to 'Normal Operation'.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG);
	reg &= ~ETHQPCS_DPLL_RESET_MASK;
	reg |= (0x1 << ETHQPCS_DPLL_RESET_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG, reg);

	mvOsDelay(20);

	/* Set 'Reset' field in Quad Gbe PHY Common Control and Status
	 * register to 'Normal'.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG);
	reg &= ~ETHQPCS_RESET_MASK;
	reg |= (0x1 << ETHQPCS_RESET_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_QUAD_GBE_PHY_CTRL_STAT_REG, reg);

	/*----------------------------------------------------------------------
	 * GOP init
	 */

	/* GOP works NOT in SGMII, ==> set 'PcsEn' field in
	 * Port MAC Control Register2 to 'Not_working'.
	 */
	reg = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	reg &= ETH_GMAC_PCS_ENABLE_MASK;
	reg |= (0 << ETH_GMAC_PCS_ENABLE_BIT);
	MV_REG_WRITE(ETH_GMAC_CTRL_2_REG(port), reg);

	/*
	 * When working with GbE PHY:
	 * Set 'PeriodicXonEn' field in Port MAC Control Register 1 to 'Disabled'.
	 */
	reg = MV_REG_READ(ETH_GMAC_CTRL_1_REG(port));
	reg &= ~ETH_GMAC_PERIODIC_XON_EN_MASK;
	reg |= (0x0 << ETH_GMAC_PERIODIC_XON_EN_BIT);
	MV_REG_WRITE(ETH_GMAC_CTRL_1_REG(port), reg);

	/*
	 * Working in Auto Negotiation (AN) mode
	 */
	mvEthCompMacSetAutoNegMode(port);

	/*
	 * Set 'PortMACReset' in Port MAC Control Register 2 to 'Port_MAC_Not_reset'.
	 */
	mvEthCompMacSetReset(port, MV_FALSE);
}

/******************************************************************************
* mvEthCompMacRgmii1Config
*
* DESCRIPTION:
* 	None.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompMacRgmii1Config(MV_U32 port, MV_U32 ethCompCfg)
{
	MV_U32 reg;

	if ((ethCompCfg & MV_ETHCOMP_GE_MAC1_2_RGMII1) == 0)
		return;

	/*----------------------------------------------------------------------
	 * Connect GE MAC 1 to RGMII 1
	 */

	/*
	 * Set 'GeMAC1Source' field in Ethernet Complex Control 0 reg to 0x0, ==>
	 * GbE MAC 1 is connected to an MPP (RGMII).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG);
	reg &= ~ETHCC_GBE_MAC1_SRC_MASK;
	reg |= (0x0 << ETHCC_GBE_MAC1_SRC_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG, reg);

	/*
	 * Set MPP[35:24] to option 0x2.
	 */

	/* Read MPP[31:24] and config MPP[31:24] to option 0x2 */
	reg = MV_REG_READ(MPP_CONTROL_REG(3));
	reg &= ~0xffffffff; /* each MPP stands for 4 bits */
	reg |= 0x22222222;
	MV_REG_WRITE(MPP_CONTROL_REG(3), reg);

	/* Read MPP[32:39] and config MPP[35:32] to option 0x2 */
	reg = MV_REG_READ(MPP_CONTROL_REG(4));
	reg &= ~0xffff; /* each MPP stands for 4 bits */
	reg |= 0x2222;
	MV_REG_WRITE(MPP_CONTROL_REG(4), reg);

	/*----------------------------------------------------------------------
	 * Set 'Port[0/1]DpClkSource' field in Ports Group Control and Status to 0x1
	 */
	reg = MV_REG_READ(MV_ETHCOMP_GOP_CTRL_STAT_REG);

	reg &= ~ETHCGCS_PORT_DP_CLK_SRC_MASK(port);
	reg |= (0x1 << ETHCGCS_PORT_DP_CLK_SRC_OFFSET(port));

	reg &= ~ETHCGCS_GOP_ENABLE_DEV_MASK;
	reg |= (0x1 << ETHCGCS_GOP_ENABLE_DEV_OFFSET);

	MV_REG_WRITE(MV_ETHCOMP_GOP_CTRL_STAT_REG, reg);

	/*----------------------------------------------------------------------
	 * GOP init
	 */

	/* GOP works NOT in SGMII, ==> set 'PcsEn' field in
	 * Port MAC Control Register2 to 'Not_working'.
	 */
	reg = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	reg &= ETH_GMAC_PCS_ENABLE_MASK;
	reg |= (0 << ETH_GMAC_PCS_ENABLE_BIT);
	MV_REG_WRITE(ETH_GMAC_CTRL_2_REG(port), reg);

	/*
	 * Working in Auto Negotiation (AN) mode
	 */
	mvEthCompMacSetAutoNegMode(port);

	/*
	 * Set 'PortMACReset' in Port MAC Control Register 2 to 'Port_MAC_Not_reset'.
	 */
	mvEthCompMacSetReset(port, MV_FALSE);
}

/******************************************************************************
* mvEthCompInit
*
* DESCRIPTION:
*	Initialize the ethernet complex according to the boardEnv setup.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompInit(void)
{
	MV_U32 ethCompCfg = mvBoardEthComplexConfigGet();

	if (gEthComplexSkipInit == MV_TRUE)
		return MV_OK;

	mvEthCompPreInit(ethCompCfg, mvBoardPhyAddrGet(GBE_PORT(0)));

	/* MAC1 to RGMII1 */
	mvEthCompMacRgmii1Config(GBE_PORT(1), ethCompCfg);

	/* MAC0 to GE PHY Port0 */
	mvEthCompMacToGePhyConfig(GBE_PORT(0), GE_PHY(0), ethCompCfg);

	/*  Reset the switch after all configurations are done. */
	mvEthCompSwitchReset(ethCompCfg);

	mvEthCompPostInit(ethCompCfg);

	return MV_OK;
}

/******************************************************************************
* mvEthCompChangeMode
*
* DESCRIPTION:
*	Change the ethernet complex configuration at runtime.
*	Meanwhile the function supports only the following mode changes:
*		- Moving the switch between MAC0 & MAC1.
*		- Connect / Disconnect GE-PHY to MAC1.
*		- Connect / Disconnect RGMII-B to MAC0.
*
* INPUT:
*	oldCfg	- The old ethernet complex configuration.
*	newCfg	- The new ethernet complex configuration to switch to.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
static MV_STATUS mvEthCompChangeMode(MV_U32 oldCfg, MV_U32 newCfg)
{
	return MV_OK;
}
