/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/mpp.h>

struct adec_win memory_map[] = {
	{PEX_0_TARGET_ID,	MVEBU_PCIE_MEM_BASE(0),	MVEBU_PCIE_MEM_SIZE(0), PCI_0_MEM_ATTR, 1, 1},
	{PEX_1_3_TARGET_ID,	MVEBU_PCIE_MEM_BASE(1),	MVEBU_PCIE_MEM_SIZE(1), PCI_1_MEM_ATTR, 1, 1},
	{PEX_1_3_TARGET_ID,	MVEBU_PCIE_MEM_BASE(2),	MVEBU_PCIE_MEM_SIZE(2), PCI_2_MEM_ATTR, 1, 1},
	{PEX_1_3_TARGET_ID,	MVEBU_PCIE_MEM_BASE(3),	MVEBU_PCIE_MEM_SIZE(3), PCI_3_MEM_ATTR, 1, 1},
	{DEV_TARGET_ID,		NOR_CS_BASE,		NOR_CS_SIZE,	 BOOT_CS_ATTR,   0, 1},
	{TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM},
};

/* List the maximum amount of units for all devices and boards */
u16 base_unit_mask_table[MAX_UNIT_ID] = {
	[PCIE_UNIT_ID]		=  cnt_to_mask(4),
	[SATA_UNIT_ID]		=  cnt_to_mask(4),
	[ETH_GIG_UNIT_ID]	=  cnt_to_mask(4),
	[UART_UNIT_ID]		=  cnt_to_mask(2),
	[USB_UNIT_ID]		=  cnt_to_mask(1),
	[USB3_UNIT_ID]		=  cnt_to_mask(2),
	[SPI_UNIT_ID]		=  cnt_to_mask(2),
	[XOR_UNIT_ID]		=  cnt_to_mask(1),
	[CESA_UNIT_ID]		=  cnt_to_mask(1),
	[SDIO_UNIT_ID]		=  cnt_to_mask(1),
	[NAND_UNIT_ID]		=  cnt_to_mask(1)
};

/* Per device unit info. List which units are disabled for this device */
u16 armada6280_unit_disable[MAX_UNIT_ID] = {
	[SATA_UNIT_ID]		=  id_to_mask(2) | id_to_mask(3)
};

u16 armada6285_unit_disable[MAX_UNIT_ID] = {
	[ETH_GIG_UNIT_ID]	= id_to_mask(3),
	[USB3_UNIT_ID]		= id_to_mask(1)
};

/* MPP description table. Informative only */
char *a38x_mpp_desc[MAX_MPP_ID + 1][MAX_MPP_OPTS] = {
/*	0x0        0x1         0x2              0x3             0x4             0x5             0x6 */
/* 0 */ { "GPIO[0]", "UA0_RXD", "NA", "NA", "NA", "NA", "NA"},
/* 1 */ { "GPIO[1]", "UA0_TXD", "NA", "NA", "NA", "NA", "NA"},
/* 2 */ { "GPIO[2]", "I2C0_SCK", "NA", "NA", "NA", "NA", "NA"},
/* 3 */ { "GPIO[3]", "I2C0_SDA", "NA", "NA", "NA", "NA", "NA"},
/* 4 */ { "GPIO[4]", "GE_MDC", "UA1_TXD", "UA0_RTS", "NA", "NA", "NA"},
/* 5 */ { "GPIO[5]", "GE_MDIO", "UA1_RXD", "UA0_CTS", "NA", "NA", "NA"},
/* 6 */ { "GPIO[6]", "GE0_TXCLKOUT", "GE0_CRS", "NA", "NA", "DEV_CSn[3]", "NA"},
/* 7 */ { "GPIO[7]", "GE0_TXD[0]", "NA", "NA", "NA", "DEV_AD[9]", "NA"},
/* 8 */ { "GPIO[8]", "GE0_TXD[1]", "NA", "NA", "NA", "DEV_AD[10]", "NA"},
/* 9 */ { "GPIO[9]", "GE0_TXD[2]", "NA", "NA", "NA", "DEV_AD[11]", "NA"},
/* 10 */ { "GPIO[10]", "GE0_TXD[3]", "NA", "NA", "NA", "DEV_AD[12]", "NA"},
/* 11 */ { "GPIO[11]", "GE0_TXCTL", "NA", "NA", "NA", "DEV_AD[13]", "NA"},
/* 12 */ { "GPIO[12]", "GE0_RXD[0]", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "SPI0_CSn[1]", "DEV_AD[14]", "PCIe3_CLKREQ"},
/* 13 */ { "GPIO[13]", "GE0_RXD[1]", "PCIe0_CLKREQ", "PCIe1_CLKREQ", "SPI0_CSn[2]", "DEV_AD[15]", "PCIe2_CLKREQ"},
/* 14 */ { "GPIO[14]", "GE0_RXD[2]", "PTP_CLK", "M_VTT_CTRL", "SPI0_CSn[3]", "DEV_WEn[1]", "PCIe3_CLKREQ"},
/* 15 */ { "GPIO[15]", "GE0_RXD[3]", "GE_MDC Slave", "PCIe0_RSTOUTn", "SPI0_MOSI", "PCIe1_RSTOUTn", "PCIe2_RSTOUTn"},
/* 16 */ { "GPIO[16]", "GE0_RXCTL", "GE_MDIO Slave", "M_DECC_ERR", "SPI0_MISO", "PCIe0_CLKREQ", "PCIe1_CLKREQ"},
/* 17 */ { "GPIO[17]", "GE0_RXCLK", "PTP_CLK", "UA1_RXD", "SPI0_SCK", "SATA1_PRESENT_ACTIVEn", "SATA0_PRESENT_ACTIVEn"},
/* 18 */ { "GPIO[18]", "GE0_RXERR", "PTP_TRIG_GEN", "UA1_TXD", "SPI0_CSn[0]", "PCIe1_RSTOUTn", "PCIe2_RSTOUTn"},
/* 19 */ { "GPIO[19]", "GE0_COL", "PTP_EVENT_REQ", "PCIe0_CLKREQ", "SATA1_PRESENT_ACTIVEn", "UA0_CTS", "UA1_RXD"},
/* 20 */ { "GPIO[20]", "GE0_TXCLK", "PTP_CLK", "PCIe1_RSTOUTn", "SATA0_PRESENT_ACTIVEn", "UA0_RTS", "UA1_TXD"},
/* 21 */ { "GPIO[21]", "SPI0_CSn[1]", "GE1_RXD[0]", "SATA0_PRESENT_ACTIVEn", "SD0_CMD", "DEV_BOOTCSn", "SATA1_PRESENT_ACTIVEn"},
/* 22 */ { "GPIO[22]", "SPI0_MOSI", "NA", "NA", "NA", "DEV_AD[0]", "NA"},
/* 23 */ { "GPIO[23]", "SPI0_SCK", "NA", "NA", "NA", "DEV_AD[2]", "NA"},
/* 24 */ { "GPIO[24]", "SPI0_MISO", "UA0_CTS", "UA1_RXD", "SD0_D[4]", "DEV_READYn", "NA"},
/* 25 */ { "GPIO[25]", "SPI0_CSn[0]", "UA0_RTS", "UA1_TXD", "SD0_D[5]", "DEV_CSn[0]", "NA"},
/* 26 */ { "GPIO[26]", "SPI0_CSn[2]", "NA", "I2C1_SCK", "SD0_D[6]", "DEV_CSn[1]", "NA"},
/* 27 */ { "GPIO[27]", "SPI0_CSn[3]", "GE1_TXCLKOUT", "I2C1_SDA", "SD0_D[7]", "DEV_CSn[2]", "NA"},
/* 28 */ { "GPIO[28]", "NA", "GE1_TXD[0]", "NA", "SD0_CLK", "DEV_AD[5]", "NA"},
/* 29 */ { "GPIO[29]", "NA", "GE1_TXD[1]", "NA", "NA", "DEV_ALE[0]", "NA"},
/* 30 */ { "GPIO[30]", "NA", "GE1_TXD[2]", "NA", "NA", "DEV_OEn", "NA"},
/* 31 */ { "GPIO[31]", "NA", "GE1_TXD[3]", "NA", "NA", "DEV_ALE[1]", "NA"},
/* 32 */ { "GPIO[32]", "NA", "GE1_TXCTL", "NA", "NA", "DEV_WEn[0]", "NA"},
/* 33 */ { "GPIO[33]", "M_DECC_ERR", "NA", "NA", "NA", "DEV_AD[3]", "NA"},
/* 34 */ { "GPIO[34]", "NA", "NA", "NA", "NA", "DEV_AD[1]", "NA"},
/* 35 */ { "GPIO[35]", "REF_CLK_OUT[1]", "NA", "NA", "NA", "DEV_A[1]", "NA"},
/* 36 */ { "GPIO[36]", "PTP_TRIG_GEN", "NA", "NA", "NA", "DEV_A[0]", "NA"},
/* 37 */ { "GPIO[37]", "PTP_CLK", "GE1_RXCLK", "NA", "SD0_D[3]", "DEV_AD[8]", "NA"},
/* 38 */ { "GPIO[38]", "PTP_EVENT_REQ", "GE1_RXD[1]", "REF_CLK_OUT[0]", "SD0_D[0]", "DEV_AD[4]", "NA"},
/* 39 */ { "GPIO[39]", "I2C1_SCK", "GE1_RXD[2]", "UA0_CTS", "SD0_D[1]", "DEV_A[2]", "NA"},
/* 40 */ { "GPIO[40]", "I2C1_SDA", "GE1_RXD[3]", "UA0_RTS", "SD0_D[2]", "DEV_AD[6]", "NA"},
/* 41 */ { "GPIO[41]", "UA1_RXD", "GE1_RXCTL", "UA0_CTS", "SPI1_CSn[3]", "DEV_BURSTnLASTn", "ND_RBn[0]"},
/* 42 */ { "GPIO[42]", "UA1_TXD", "NA", "UA0_RTS", "NA", "DEV_AD[7]", "NA"},
/* 43 */ { "GPIO[43]", "PCIe0_CLKREQ", "M_VTT_CTRL", "M_DECC_ERR", "PCIe0_RSTOUTn", "DEV_CLKOUT", "ND_RBn[1]"},
/* 44 */ { "GPIO[44]", "SATA0_PRESENT_ACTIVEn", "SATA1_PRESENT_ACTIVEn", "NA", "NA", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn"},
/* 45 */ { "GPIO[45]", "REF_CLK_OUT[0]", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "PCIe2_RSTOUTn", "PCIe3_RSTOUTn", "UA1_RXD"},
/* 46 */ { "GPIO[46]", "REF_CLK_OUT[1]", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "PCIe2_RSTOUTn", "PCIe3_RSTOUTn", "UA1_TXD"},
/* 47 */ { "GPIO[47]", "SATA0_PRESENT_ACTIVEn", "SATA1_PRESENT_ACTIVEn", "NA", "SPI1_CSn[2]", "NA", "PCIe2_RSTOUTn"},
/* 48 */ { "GPIO[48]", "SATA0_PRESENT_ACTIVEn", "M_VTT_CTRL", "TDM2C_PCLK", "AU_I2SMCLKSPDIFRMCLK", "SD0_D[4]", "PCIe0_CLKREQ"},
/* 49 */ { "GPIO[49]", "NA", "NA", "TDM2C_FSYNC", "AU_I2SLRCLK", "SD0_D[5]", "PCIe1_CLKREQ"},
/* 50 */ { "GPIO[50]", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "TDM2C_DRX", "AU_EXTCLK", "SD0_CMD", "PCIe2_RSTOUTn"},
/* 51 */ { "GPIO[51]", "NA", "NA", "TDM2C_DTX", "AU_I2SDOAU_SPDIFO", "M_DECC_ERR", "PTP_TRIG_GEN"},
/* 52 */ { "GPIO[52]", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "TDM2C_INTn", "AU_I2SDI", "SD0_D[6]", "PTP_CLK"},
/* 53 */ { "GPIO[53]", "SATA1_PRESENT_ACTIVEn", "SATA0_PRESENT_ACTIVEn", "TDM2C_RSTn", "AU_I2SBCLK", "SD0_D[7]", "PTP_EVENT_REQ"},
/* 54 */ { "GPIO[54]", "SATA0_PRESENT_ACTIVEn", "SATA1_PRESENT_ACTIVEn", "PCIe0_RSTOUTn", "PCIe1_RSTOUTn", "SD0_D[3]", "PCIe2_RSTOUTn"},
/* 55 */ { "GPIO[55]", "UA1_CTS", "GE_MDIO", "PCIe1_CLKREQ", "SPI1_CSn[1]", "SD0_D[0]", "UA1_RXD"},
/* 56 */ { "GPIO[56]", "UA1_RTS", "GE_MDC", "M_DECC_ERR", "SPI1_MOSI", "NA", "UA1_TXD"},
/* 57 */ { "GPIO[57]", "NA", "NA", "NA", "SPI1_SCK", "SD0_CLK", "UA1_TXD"},
/* 58 */ { "GPIO[58]", "PCIe1_CLKREQ", "I2C1_SCK", "PCIe2_CLKREQ", "SPI1_MISO", "SD0_D[1]", "UA1_RXD"},
/* 59 */ { "GPIO[59]", "PCIe0_RSTOUTn", "I2C1_SDA", "PCIe1_RSTOUTn", "SPI1_CSn[0]", "SD0_D[2]", "PCIe2_RSTOUTn"},
};

/* Describe the available MPP buses */
struct mpp_bus a38x_mpp_buses[MAX_MPP_BUS] = {
	[UART_0_MPP_BUS] = {"UART 0", 2, 1, {{{0, 0x1 }, {0, 0x1} } } },
	[UART_1_MPP_BUS] = {"UART 1", 2, 2, {{{4, 0x2}, {5, 0x2} }, {{19, 0x6}, {20, 0x6} } } },
	[I2C_0_MPP_BUS] = {"I2C 0", 2, 1, {{{2, 0x1}, {3, 0x1} } } },
	[GE_0_MPP_BUS] = {"GE 0", 14, 1, {{{4, 0x1}, {5, 0x1}, {6, 0x1}, {7, 0x1}, {8, 0x1}, {9, 0x1}, {10, 0x1}, {11, 0x1}, \
										{12, 0x1}, {13, 0x1}, {14, 0x1}, {15, 0x1}, {16, 0x1}, {17, 0x1} } } },
	[GE_1_MPP_BUS] = {"GE 1", 12, 1, {{{21, 0x2}, {27, 0x2}, {28, 0x2}, {29, 0x2}, {30, 0x2}, {31, 0x2}, {32, 0x2}, {37, 0x2}, {38, 0x2}, {39, 0x2}, {40, 0x2}, {41, 0x2} } } },
	[SPI_0_MPP_BUS]  = {"SPI 0", 4, 1, {{{22, 0x1}, {23, 0x1}, {24, 0x1}, {25, 0x1} } } },
	[M_VTT_0_MPP_BUS]  = {"M_VTT_CTRL", 1, 1, {{{43, 0x2} } } },
	[SDIO_0_MPP_BUS]  = {"SDIO 0", 10, 1, {{{48, 0x5}, {49, 0x5}, {50, 0x5}, {52, 0x5}, {53, 0x5}, {54, 0x5}, {55, 0x5}, {57, 0x5}, {58, 0x5}, {59, 0x5} } } },
};

struct mvebu_soc_info a38x_soc_info[] = {
	/* Armda 380 */
	{
		.name = "MV-6280",
		.id = 0x6820,
		.unit_disable = armada6280_unit_disable,
		.memory_map = memory_map
	},
	/* Armda 385 */
	{
		.name = "MV-6285",
		.id = 0x6825,
		.unit_disable = armada6285_unit_disable,
		.memory_map = memory_map
	},
	/* Armda 388 */
	{
		.name = "MV-6288",
		.id = 0x6828,
		.unit_disable = armada6280_unit_disable,
		.memory_map = memory_map
	},
	/* Delimiter */
	{.id = 0}
};

struct mvebu_soc_family a38x_family_info = {
	.name = "MV-80xx",
	.id   = 8000,
	.rev_name = {"Z1", "A0"},
	.base_unit_info = base_unit_mask_table,
	.soc_table = a38x_soc_info,
	.mpp_desc  = (char **)a38x_mpp_desc,
	.mpp_buses = a38x_mpp_buses,
};
