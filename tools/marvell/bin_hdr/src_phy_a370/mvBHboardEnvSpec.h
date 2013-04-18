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


#ifndef __INCmvBHboardEnvSpech
#define __INCmvBHboardEnvSpech

/* Board specific configuration */
/* ============================ */


/* KW40 */
#define MV_6710_DEV_ID		0x6710

#define MV_6710_Z1_REV		0x0
#define MV_6710_Z1_ID		((MV_6710_DEV_ID << 16) | MV_6710_Z1_REV)
#define MV_6710_Z1_NAME		"MV6710 Z1"

#define MV_6710_A0_REV          0x0
#define MV_6710_A0_ID           ((MV_6710_DEV_ID << 16) | MV_6710_A0_REV)
#define MV_6710_A0_NAME         "MV6710 A0"

#define MV_6710_A1_REV          0x1
#define MV_6710_A1_ID           ((MV_6710_DEV_ID << 16) | MV_6710_A1_REV)
#define MV_6710_A1_NAME         "MV6710 A1"

#define MV_6707_DEV_ID          0x6707
#define MV_6707_A0_REV          0x0
#define MV_6707_A0_ID           ((MV_6707_DEV_ID << 16) | MV_6707_A0_REV)
#define MV_6707_A0_NAME         "MV6707 A0"

#define MV_6707_A1_REV          0x1
#define MV_6707_A1_ID           ((MV_6707_DEV_ID << 16) | MV_6707_A1_REV)
#define MV_6707_A1_NAME         "MV6707 A1"

#define MV_6W11_DEV_ID          0x6711
#define MV_6W11_A0_REV          0x0
#define MV_6W11_A0_ID           ((MV_6W11_DEV_ID << 16) | MV_6W11_A0_REV)
#define MV_6W11_A0_NAME         "MV6W11 A0"

#define MV_6W11_A1_REV          0x1
#define MV_6W11_A1_ID           ((MV_6W11_DEV_ID << 16) | MV_6W11_A1_REV)
#define MV_6W11_A1_NAME         "MV6W11 A1"


/* Armada XP Family */
#define MV_78130_DEV_ID		0x7813
#define MV_78160_DEV_ID		0x7816
#define MV_78230_DEV_ID		0x7823
#define MV_78260_DEV_ID		0x7826
#define MV_78460_DEV_ID		0x7846
#define MV_78000_DEV_ID		0x7888

#define MV_FPGA_DEV_ID		0x2107

#define MV_78XX0_Z1_REV		0x0


/* boards ID numbers */
#define BOARD_ID_BASE				0x0

/* New board ID numbers */
#define DB_88F6710_BP_ID		(BOARD_ID_BASE)
#define DB_88F6710_PCAC_ID		(BOARD_ID_BASE + 1)
#define RD_88F6710_ID			(BOARD_ID_BASE + 2)
#define MV_MAX_BOARD_ID			(BOARD_ID_BASE + 3)
#define INVALID_BAORD_ID		0xFFFFFFFF

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)			(0x18230)

#define MSAR_TCLK_OFFS				20
#define MSAR_TCLK_MASK				(0x1 << MSAR_TCLK_OFFS)

#define AVS_CONTROL2_REG			0x20868
#define AVS_LOW_VDD_LIMIT			0x20860


/* Controler environment registers offsets */
#define GEN_PURP_RES_1_REG			0x182F4
#define GEN_PURP_RES_2_REG			0x182F8

/* registers offsets */
#define MV_GPP_REGS_OFFSET(unit)		(0x18100 + ((unit) * 0x40))

#define MPP_CONTROL_REG(id)			(0x18000 + (id * 4))
#define MV_GPP_REGS_BASE(unit)		(MV_GPP_REGS_OFFSET(unit))
#define MV_GPP_REGS_BASE_0			(MV_GPP_REGS_OFFSET_0)

#define GPP_DATA_OUT_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x00)
#define GPP_DATA_OUT_REG_0			(MV_GPP_REGS_BASE_0 + 0x00)	/* Used in .S files */
#define GPP_DATA_OUT_EN_REG(grp)	(MV_GPP_REGS_BASE(grp) + 0x04)
#define GPP_BLINK_EN_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x08)
#define GPP_DATA_IN_POL_REG(grp)	(MV_GPP_REGS_BASE(grp) + 0x0C)
#define GPP_DATA_IN_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x10)
#define GPP_INT_CAUSE_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x14)
#define GPP_INT_MASK_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x18)
#define GPP_INT_LVL_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x1C)
#define GPP_OUT_SET_REG(grp)		(0x18130 + ((grp) * 0x40))
#define GPP_OUT_CLEAR_REG(grp)		(0x18134 + ((grp) * 0x40))
#define GPP_FUNC_SELECT_REG			(MV_GPP_REGS_BASE(0) + 0x40)

/* This define describes the maximum number of supported PEX Interfaces */
#define MV_PEX_MAX_IF				2
#define MV_PEX_MAX_UNIT				2

#define MV_SERDES_NUM_TO_PEX_NUM(sernum)	(sernum)

#define PEX_PHY_ACCESS_REG(unit)	(0x40000 + ((unit)%2 * 0x40000) + ((unit)/2 * 0x2000) + 0x1B00)

#define SATA_BASE_REG(port)			(0xA2000 + (port)*0x2000)

#define SATA_PWR_PLL_CTRL_REG(port)			(SATA_BASE_REG(port) + 0x804)
#define SATA_DIG_LP_ENA_REG(port)			(SATA_BASE_REG(port) + 0x88C)
#define SATA_REF_CLK_SEL_REG(port)			(SATA_BASE_REG(port) + 0x918)
#define SATA_COMPHY_CTRL_REG(port)			(SATA_BASE_REG(port) + 0x920)
#define SATA_LP_PHY_EXT_CTRL_REG(port)		(SATA_BASE_REG(port) + 0x058)
#define SATA_LP_PHY_EXT_STAT_REG(port)		(SATA_BASE_REG(port) + 0x05C)
#define SATA_IMP_TX_SSC_CTRL_REG(port)		(SATA_BASE_REG(port) + 0x810)
#define SATA_GEN_1_SET_0_REG(port)			(SATA_BASE_REG(port) + 0x834)
#define SATA_GEN_1_SET_1_REG(port)			(SATA_BASE_REG(port) + 0x838)
#define SATA_GEN_2_SET_0_REG(port)			(SATA_BASE_REG(port) + 0x83C)
#define SATA_GEN_2_SET_1_REG(port)			(SATA_BASE_REG(port) + 0x840)

#define MV_ETH_BASE_ADDR				(0x72000)
#define MV_ETH_REGS_OFFSET(port)		(MV_ETH_BASE_ADDR - ((port) / 2) * 0x40000 + ((port) % 2) * 0x4000)
#define MV_ETH_REGS_BASE(port)			MV_ETH_REGS_OFFSET(port)


#define SGMII_PWR_PLL_CTRL_REG(port)	(MV_ETH_REGS_BASE(port) + 0xE04)
#define SGMII_DIG_LP_ENA_REG(port)		(MV_ETH_REGS_BASE(port) + 0xE8C)
#define SGMII_REF_CLK_SEL_REG(port)		(MV_ETH_REGS_BASE(port) + 0xF18)
#define SGMII_SERDES_CFG_REG(port)		(MV_ETH_REGS_BASE(port) + 0x4A0)
#define SGMII_SERDES_STAT_REG(port)		(MV_ETH_REGS_BASE(port) + 0x4A4)
#define SGMII_COMPHY_CTRL_REG(port)		(MV_ETH_REGS_BASE(port) + 0xF20)
#define QSGMII_GEN_1_SETTING_REG(port)	(MV_ETH_REGS_BASE(port) + 0xE38)
#define QSGMII_SERDES_CFG_REG(port)	   	(MV_ETH_REGS_BASE(port) + 0x4a0)

#define SERDES_LINE_MUX_REG_0_3			0x18270

#define MV_MISC_REGS_OFFSET			(0x18200)
#define MV_MISC_REGS_BASE			(MV_MISC_REGS_OFFSET)
#define SOC_CTRL_REG				(MV_MISC_REGS_BASE + 0x4)

/********************************************/
/* PCI Express Control and Status Registers */
/********************************************/
#define MAX_PEX_DEVICES         1
#define MAX_PEX_FUNCS           8
#define MAX_PEX_BUSSES          256

#define PXSR_PEX_BUS_NUM_OFFS			8	/* Bus Number Indication */
#define PXSR_PEX_BUS_NUM_MASK			(0xff << PXSR_PEX_BUS_NUM_OFFS)

#define PXSR_PEX_DEV_NUM_OFFS			16	/* Device Number Indication */
#define PXSR_PEX_DEV_NUM_MASK			(0x1f << PXSR_PEX_DEV_NUM_OFFS)

#define PXSR_DL_DOWN				BIT0	/* DL_Down indication. */
#define PXCAR_CONFIG_EN			BIT31
#define PEX_STATUS_AND_COMMAND						0x004
#define PXSAC_MABORT			BIT29	/* Recieved Master Abort        */

/* PCI Express Configuration Address Register */
/* PEX_CFG_ADDR_REG (PXCAR) */
#define PXCAR_REG_NUM_OFFS		2
#define PXCAR_REG_NUM_MAX		0x3F
#define PXCAR_REG_NUM_MASK		(PXCAR_REG_NUM_MAX << PXCAR_REG_NUM_OFFS)
#define PXCAR_FUNC_NUM_OFFS		8
#define PXCAR_FUNC_NUM_MAX		0x7
#define PXCAR_FUNC_NUM_MASK		(PXCAR_FUNC_NUM_MAX << PXCAR_FUNC_NUM_OFFS)
#define PXCAR_DEVICE_NUM_OFFS		11
#define PXCAR_DEVICE_NUM_MAX		0x1F
#define PXCAR_DEVICE_NUM_MASK		(PXCAR_DEVICE_NUM_MAX << PXCAR_DEVICE_NUM_OFFS)
#define PXCAR_BUS_NUM_OFFS		16
#define PXCAR_BUS_NUM_MAX		0xFF
#define PXCAR_BUS_NUM_MASK		(PXCAR_BUS_NUM_MAX << PXCAR_BUS_NUM_OFFS)
#define PXCAR_EXT_REG_NUM_OFFS		24
#define PXCAR_EXT_REG_NUM_MAX		0xF

#define PXCAR_REAL_EXT_REG_NUM_OFFS     8
#define PXCAR_REAL_EXT_REG_NUM_MASK     (0xF << PXCAR_REAL_EXT_REG_NUM_OFFS)


#define PEX_CAPABILITIES_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x60)
#define PEX_LINK_CAPABILITIES_REG(pexIf)	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x6C)
#define PEX_LINK_CTRL_STATUS_REG(pexIf) 	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x70)
#define PEX_LINK_CTRL_STATUS2_REG(pexIf) 	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x90)
#define PEX_CTRL_REG(pexIf)					((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A00)
#define PEX_STATUS_REG(pexIf)				((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A04)
#define PEX_COMPLT_TMEOUT_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A10)
#define PEX_PWR_MNG_EXT_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A18)
#define PEX_FLOW_CTRL_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A20)
#define PEX_SECONDARY_BUS_REG(pexIf)        ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A2C)
#define PEX_DYNMC_WIDTH_MNG_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A30)
#define PEX_ROOT_CMPLX_SSPL_REG(pexif)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A0C)
#define PEX_RAM_PARITY_CTRL_REG(pexIf) 		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A50)
#define PEX_DBG_CTRL_REG(pexIf) 			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A60)
#define PEX_DBG_STATUS_REG(pexIf)           ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A64)

#define PXLCSR_NEG_LNK_GEN_OFFS			16	/* Negotiated Link GEN */
#define PXLCSR_NEG_LNK_GEN_MASK 		(0xf << PXLCSR_NEG_LNK_GEN_OFFS)
#define PXLCSR_NEG_LNK_GEN_1_1			(0x1 << PXLCSR_NEG_LNK_GEN_OFFS)
#define PXLCSR_NEG_LNK_GEN_2_0			(0x2 << PXLCSR_NEG_LNK_GEN_OFFS)

#define PEX_CFG_ADDR_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18F8)
#define PEX_CFG_DATA_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18FC)
#define PEX_CAUSE_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1900)



#define PEX_CAPABILITY_REG						0x60
#define PEX_DEV_CAPABILITY_REG						0x64
#define PEX_DEV_CTRL_STAT_REG						0x68
#define PEX_LINK_CAPABILITY_REG						0x6C
#define PEX_LINK_CTRL_STAT_REG						0x70
#define PEX_LINK_CTRL_STAT_2_REG					0x90

/* PCI Ecpress Secondary Bus register */
/* PEX_SECONDARY_BUS_REG */
#define SECONDARY_BUS_NUMBER_ENABLE				BIT8
#define SECONDARY_BUS_NUMBER_OFFS				0
#define SECONDARY_BUS_NUMBER_MASK				0xFF

/* EPPROM Modules detection information */

#define MV_BOARD_MODULES_ADDR_TYPE			ADDR7_BIT

#define MV_BOARD_DEVICE_CON_ADDR			0x20
#define MV_BOARD_EEPROM_MODULE_ADDR			0x21
#define MV_BOARD_GIGA_CON_GMII_ADDR			0x22
#define MV_BOARD_GIGA_CON_ADDR				0x26
#define MV_BOARD_SERDES_CON_ADDR			0x27

#define MV_BOARD_TDM_GMII_MODULE_TDM_ID		0x1
#define MV_BOARD_TDM_GMII_MODULE_GMII_ID	0x4
#define MV_BOARD_SWITCH_MODULE_ID			0xE
#define MV_BOARD_I2S_SPDIF_MODULE_ID		0x2
#define MV_BOARD_NAND_SDIO_MODULE_ID		0xF
#define MV_BOARD_MODULE_ID_MASK				0xF

typedef enum {
	LANE0 = 0x1,
	LANE1 = 0x2,
	LANE2 = 0x4,
	LANE3 = 0x8
} MV_SERDES_LANES;

#define MV_BOARD_CFG_SATA0_MODE(cfg)	(cfg & 0x3)
#define MV_BOARD_CFG_SATA1_MODE(cfg)	((cfg >> 2) & 0x1)
#define MV_BOARD_CFG_PCIE_MODE(cfg)		((cfg >> 3) & 0x3)
#define MV_BOARD_CFG_SDIO_MODE(cfg)		((cfg >> 5) & 0x1)


#endif /* __INCmvBHboardEnvSpech */
