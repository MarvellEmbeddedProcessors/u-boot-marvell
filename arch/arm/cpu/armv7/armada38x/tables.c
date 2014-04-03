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
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/mpp.h>

struct adec_win memory_map[] = {
	{0x0,     0x40000000, DRAM_0_TID, 0},
	{0x40000000, 0x20000000, IO_0_TID, 0},
	/* Delimiter */
	{0x0, 0x0, INVALID_TID, 0}
};

/* List the maximum amount of units for all devices and boards */
u16 base_unit_mask_table[MAX_UNIT_ID] = {
	[PEX_UNIT_ID]		=  cnt_to_mask(3),
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
	[PEX_UNIT_ID]		=  id_to_mask(0) | id_to_mask(2),
	[SATA_UNIT_ID]		=  id_to_mask(2) | id_to_mask(3)
};

u16 armada6285_unit_disable[MAX_UNIT_ID] = {
	[ETH_GIG_UNIT_ID]	= id_to_mask(3),
	[USB3_UNIT_ID]		= id_to_mask(1)
};

/* MPP description table. Informative only */
char *a38x_mpp_desc[MAX_MPP_ID + 1][MAX_MPP_OPTS] = {
/*	0x0        0x1         0x2              0x3             0x4             0x5             0x6 */
/* 0 */	{"GPIO_0", "UA_0_RXD", "NA",		"NA",		"NA",           "NA",		"NA"},
/* 1 */	{"GPIO_1", "UA_0_TXD", "NA",		"NA",		"NA",		"NA",		"NA"},
/* 2 */	{"GPIO_2", "I2C_0_SCK", "NA",		"NA",		"NA",		"NA",		"NA"},
/* 3 */	{"GPIO_3", "I2C_0_SDA", "NA",		"NA",		"NA",		"NA",		"NA"},
/* 4 */	{"GPIO_4", "GE_MDC",    "UA_1_RXD",	"UA_0_RTS",	"NA",		"NA",		"NA"},
/* 5 */	{"GPIO_5", "GE_MDIO",   "UA_1_TXD",	"UA_0_CTS",	"NA",		"NA",		"NA"},
};

/* Describe the available MPP buses */
struct mpp_bus a38x_mpp_buses[MAX_MPP_BUS] = {
	[UART_0_MPP_BUS] = {"UART 0", 2, 1, {{{0, 0x1}, {1, 0x1} } } },
	[UART_1_MPP_BUS] = {"UART 1", 2, 1, {{{4, 0x2}, {5, 0x2} } } },
	[SPI_0_MPP_BUS]  = {"SPI 0", 4, 1, {{{0, 0x1}, {1, 0x1} } } },
	[SPI_1_MPP_BUS]  = {"SPI 1", 4, 1, {{{0, 0x1}, {1, 0x1} } } },
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
	.adec_type = ADEC_MBUS,
	.mpp_buses = a38x_mpp_buses,
};
