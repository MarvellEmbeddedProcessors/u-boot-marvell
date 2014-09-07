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

#include "boardEnv/mvBoardEnvLib.h"
#include "mvCtrlNetCompLib.h"

static MV_VOID mvNetComplexNssSelect(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_FUNCTION_ENABLE_CTRL_1);
	reg &= ~NETC_PACKET_PROCESS_MASK;

	val <<= NETC_PACKET_PROCESS_OFFSET;
	val &= NETC_PACKET_PROCESS_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_FUNCTION_ENABLE_CTRL_1, reg);
}

static MV_VOID mvNetComplexActivePort(MV_U32 port, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_PORTS_CONTROL_1);
	reg &= ~NETC_PORTS_ACTIVE_MASK(port);

	val <<= NETC_PORTS_ACTIVE_OFFSET(port);
	val &= NETC_PORTS_ACTIVE_MASK(port);

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_PORTS_CONTROL_1, reg);
}

static MV_VOID mvNetComplexXauiEnable(MV_U32 port, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_CONTROL_0);
	reg &= ~NETC_CTRL_ENA_XAUI_MASK;

	val <<= NETC_CTRL_ENA_XAUI_OFFSET;
	val &= NETC_CTRL_ENA_XAUI_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_CONTROL_0, reg);
}

static MV_VOID mvNetComplexRxauiEnable(MV_U32 port, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_CONTROL_0);
	reg &= ~NETC_CTRL_ENA_RXAUI_MASK;

	val <<= NETC_CTRL_ENA_RXAUI_OFFSET;
	val &= NETC_CTRL_ENA_RXAUI_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_CONTROL_0, reg);
}

static MV_VOID mvNetComplexGopReset(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_SYSTEM_SOFT_RESET);
	reg &= ~NETC_GOP_SOFT_RESET_MASK;

	val <<= NETC_GOP_SOFT_RESET_OFFSET;
	val &= NETC_GOP_SOFT_RESET_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_SYSTEM_SOFT_RESET, reg);
}

static MV_VOID mvNetComplexGopClockLogicSet(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_PORTS_CONTROL_0);
	reg &= ~NETC_CLK_DIV_PHASE_MASK;

	val <<= NETC_CLK_DIV_PHASE_OFFSET;
	val &= NETC_CLK_DIV_PHASE_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_PORTS_CONTROL_0, reg);
}

static MV_VOID mvNetComplexPortRfReset(MV_U32 port, MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_PORTS_CONTROL_1);
	reg &= ~NETC_PORT_GIG_RF_RESET_MASK(port);

	val <<= NETC_PORT_GIG_RF_RESET_OFFSET(port);
	val &= NETC_PORT_GIG_RF_RESET_MASK(port);

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_PORTS_CONTROL_1, reg);
}

static MV_VOID mvNetComplexGbeModeSelect(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_CONTROL_0);
	reg &= ~NETC_GBE_PORT1_MODE_MASK;

	val <<= NETC_GBE_PORT1_MODE_OFFSET;
	val &= NETC_GBE_PORT1_MODE_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_CONTROL_0, reg);
}

static MV_VOID mvNetComplexBusWidthSelect(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_PORTS_CONTROL_0);
	reg &= ~NETC_BUS_WIDTH_SELECT_MASK;

	val <<= NETC_BUS_WIDTH_SELECT_OFFSET;
	val &= NETC_BUS_WIDTH_SELECT_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_PORTS_CONTROL_0, reg);
}

static MV_VOID mvNetComplexSampleStagesTiming(MV_U32 val)
{
	MV_U32 reg;

	reg = MV_REG_READ(MV_NETCOMP_PORTS_CONTROL_0);
	reg &= ~NETC_GIG_RX_DATA_SAMPLE_MASK;

	val <<= NETC_GIG_RX_DATA_SAMPLE_OFFSET;
	val &= NETC_GIG_RX_DATA_SAMPLE_MASK;

	reg |= val;

	MV_REG_WRITE(MV_NETCOMP_PORTS_CONTROL_0, reg);
}

static MV_VOID mvNetComplexComPhySelectorConfig(MV_U32 netComplex)
{
	MV_U32 selector = MV_REG_READ(COMMON_PHYS_SELECTORS_REG);
	switch (netComplex) {
	case MV_NETCOMP_GE_MAC0_2_SGMII_L0:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(0);
		selector |= 0x4 << COMMON_PHYS_SELECTOR_LANE_OFFSET(0);
		break;
	case MV_NETCOMP_GE_MAC0_2_SGMII_L1:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(1);
		selector |= 0x8 << COMMON_PHYS_SELECTOR_LANE_OFFSET(1);
		break;
	case MV_NETCOMP_GE_MAC1_2_SGMII_L1:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(1);
		selector |= 0x9 << COMMON_PHYS_SELECTOR_LANE_OFFSET(1);
		break;
	case MV_NETCOMP_GE_MAC1_2_SGMII_L2:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(2);
		selector |= 0x5 << COMMON_PHYS_SELECTOR_LANE_OFFSET(2);
		break;
	case MV_NETCOMP_GE_MAC2_2_SGMII_L3:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(3);
		selector |= 0x7 << COMMON_PHYS_SELECTOR_LANE_OFFSET(3);
		break;
	case MV_NETCOMP_GE_MAC1_2_SGMII_L4:
		/* in serdes lane 4 can't connect MAC1->SGMII using LANE4 with V3*/
		break;
	case MV_NETCOMP_GE_MAC3_2_SGMII_L4:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(4);
		selector |= 0x8 << COMMON_PHYS_SELECTOR_LANE_OFFSET(4);
		break;
	case MV_NETCOMP_GE_MAC2_2_SGMII_L5:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(5);
		selector |= 0x6 << COMMON_PHYS_SELECTOR_LANE_OFFSET(5);
		break;
	case MV_NETCOMP_GE_MAC3_2_SGMII_L6:
		selector &= ~COMMON_PHYS_SELECTOR_LANE_MASK(6);
		selector |= 0x2 << COMMON_PHYS_SELECTOR_LANE_OFFSET(6);
		break;
	}
	MV_REG_WRITE(COMMON_PHYS_SELECTORS_REG, selector);
}

static MV_VOID mvNetComplexQsgmiiCtrlConfig(MV_VOID)
{
	MV_U32 reg;
	/* Reset the QSGMII controller */
	reg = (MV_REG_READ(MV_NETCOMP_QSGMII_CTRL_1) & (~NETC_QSGMII_CTRL_RSTN_MASK));
	reg |= 0 << NETC_QSGMII_CTRL_RSTN_OFFSET;
	MV_REG_WRITE(MV_NETCOMP_QSGMII_CTRL_1, reg);
	/* Set the QSGMII controller to work with NSS */
	reg = (MV_REG_READ(MV_NETCOMP_QSGMII_CTRL_1) & (~NETC_QSGMII_CTRL_VERSION_MASK));
	reg |= 1 << NETC_QSGMII_CTRL_VERSION_OFFSET;
	MV_REG_WRITE(MV_NETCOMP_QSGMII_CTRL_1, reg);
	/* Enable the QSGMII Serdes-GOP path */
	reg = (MV_REG_READ(MV_NETCOMP_QSGMII_CTRL_1) & (~NETC_QSGMII_CTRL_V3ACTIVE_MASK));
	reg |= 0 << NETC_QSGMII_CTRL_V3ACTIVE_OFFSET;
	MV_REG_WRITE(MV_NETCOMP_QSGMII_CTRL_1, reg);
	/* De-assert the QSGMII controller */
	reg = (MV_REG_READ(MV_NETCOMP_QSGMII_CTRL_1) & (~NETC_QSGMII_CTRL_RSTN_MASK));
	reg |= 1 << NETC_QSGMII_CTRL_RSTN_OFFSET;
	MV_REG_WRITE(MV_NETCOMP_QSGMII_CTRL_1, reg);
}

static MV_VOID mvNetComplexMacToRgmii(MV_U32 port, MV_NETC_PHASE_CFG phase)
{
	switch (phase) {
	case MV_NETC_FIRST_PHASE:
		/* Set Bus Width to HB mode = 1 */
		mvNetComplexBusWidthSelect(1);
		/* Select RGMII mode */
		mvNetComplexGbeModeSelect(1);
		break;
	case MV_NETC_SECOND_PHASE:
		/* De-assert the relevant port HB reset */
		mvNetComplexPortRfReset(port, 1);
		break;
	}
}

static MV_VOID mvNetComplexMacToQsgmii(MV_U32 port, MV_NETC_PHASE_CFG phase)
{
	switch (phase) {
	case MV_NETC_FIRST_PHASE:
		/* Set Bus Width to FB mode = 0 */
		mvNetComplexBusWidthSelect(0);
		/* Select SGMII mode */
		mvNetComplexGbeModeSelect(0);
		/* Configure the sample stages */
		mvNetComplexSampleStagesTiming(0);
		/* config QSGMII */
		mvNetComplexQsgmiiCtrlConfig();
		break;
	case MV_NETC_SECOND_PHASE:
		/* De-assert the relevant port HB reset */
		mvNetComplexPortRfReset(port, 1);
		break;
	}
}

static MV_VOID mvNetComplexMacToSgmii(MV_U32 port, MV_NETC_PHASE_CFG phase, MV_U32 netComplex)
{
	switch (phase) {
	case MV_NETC_FIRST_PHASE:
		/* Set Bus Width to HB mode = 1 */
		mvNetComplexBusWidthSelect(1);
		/* Select SGMII mode */
		mvNetComplexGbeModeSelect(0);
		/* Configure the sample stages */
		mvNetComplexSampleStagesTiming(0);
		/* Configure the ComPhy Selector */
		mvNetComplexComPhySelectorConfig(netComplex);
		break;
	case MV_NETC_SECOND_PHASE:
		/* De-assert the relevant port HB reset */
		mvNetComplexPortRfReset(port, 1);
		break;
	}
}

static MV_VOID mvNetComplexMacToRxaui(MV_U32 port, MV_NETC_PHASE_CFG phase)
{
	switch (phase) {
	case MV_NETC_FIRST_PHASE:
		/* RXAUI Serdes/s Clock alignment */
		mvNetComplexRxauiEnable(port, 1);
		break;
	case MV_NETC_SECOND_PHASE:
		/* De-assert the relevant port HB reset */
		mvNetComplexPortRfReset(port, 1);
		break;
	}
}

static MV_VOID mvNetComplexMacToXaui(MV_U32 port, MV_NETC_PHASE_CFG phase)
{
	switch (phase) {
	case MV_NETC_FIRST_PHASE:
		/* RXAUI Serdes/s Clock alignment */
		mvNetComplexXauiEnable(port, 1);
		break;
	case MV_NETC_SECOND_PHASE:
		/* De-assert the relevant port HB reset */
		mvNetComplexPortRfReset(port, 1);
		break;
	}
}

MV_STATUS mvNetComplexInit(MV_U32 netCompConfig, MV_NETC_PHASE_CFG phase)
{
	MV_U32 reg;
	MV_U32 c = netCompConfig, i;

	if (phase == MV_NETC_FIRST_PHASE) {
		/* fix the base address for transactions from the AXI to MBUS */
		reg = (MV_REG_READ(MV_NETCOMP_AMB_ACCESS_CTRL_0) & (~NETC_AMB_ACCESS_CTRL_MASK));
		reg |= (INTER_REGS_BASE & NETC_AMB_ACCESS_CTRL_MASK);
		MV_REG_WRITE(MV_NETCOMP_AMB_ACCESS_CTRL_0, reg);

		/* Reset the GOP unit */
		mvNetComplexGopReset(0);
		/* Active the GOP 4 ports */
		for (i = 0; i < 4; i++)
			mvNetComplexActivePort(i, 1);
	}

	if (c & MV_NETCOMP_GE_MAC0_2_RXAUI)
		mvNetComplexMacToRxaui(0, phase);

	if (c & MV_NETCOMP_GE_MAC0_2_XAUI)
		mvNetComplexMacToXaui(0, phase);

	if (c & (MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1))
		mvNetComplexMacToSgmii(0, phase, c);

	if (c & MV_NETCOMP_GE_MAC0_2_QSGMII)
		mvNetComplexMacToQsgmii(0, phase);

	if (c & (MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_SGMII_L2 |
			MV_NETCOMP_GE_MAC1_2_SGMII_L4))
		mvNetComplexMacToSgmii(1, phase, c);

	if (c & MV_NETCOMP_GE_MAC1_2_QSGMII)
		mvNetComplexMacToQsgmii(1, phase);

	if (c & MV_NETCOMP_GE_MAC1_2_RGMII1)
		mvNetComplexMacToRgmii(1, phase);

	if (c & (MV_NETCOMP_GE_MAC2_2_SGMII_L3 | MV_NETCOMP_GE_MAC2_2_SGMII_L5))
		mvNetComplexMacToSgmii(2, phase, c);

	if (c & MV_NETCOMP_GE_MAC2_2_QSGMII)
		mvNetComplexMacToQsgmii(2, phase);

	if (c & (MV_NETCOMP_GE_MAC3_2_SGMII_L4 | MV_NETCOMP_GE_MAC3_2_SGMII_L6))
		mvNetComplexMacToSgmii(3, phase, c);

	if (c & MV_NETCOMP_GE_MAC3_2_QSGMII)
		mvNetComplexMacToQsgmii(3, phase);

	if (phase == MV_NETC_FIRST_PHASE)
		/* Enable the NSS (PPv3) instead of the NetA (PPv1) */
		mvNetComplexNssSelect(1);
	else if (phase == MV_NETC_SECOND_PHASE) {
		/* Enable the GOP internal clock logic */
		mvNetComplexGopClockLogicSet(1);
		/* De-assert GOP unit reset */
		mvNetComplexGopReset(1);
	}

	return MV_OK;
}
