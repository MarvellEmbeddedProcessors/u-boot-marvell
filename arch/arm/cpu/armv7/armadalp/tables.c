/*
* ***************************************************************************
* Copyright (C) 2015 Marvell International Ltd.
* ***************************************************************************
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#ifndef CONFIG_PALLADIUM
	{PEX_0_TARGET_ID,	MVEBU_PCIE_MEM_BASE(0),	MVEBU_PCIE_MEM_SIZE(0), PCI_0_MEM_ATTR, 1, 1},
	{DEV_TARGET_ID,		NOR_CS_BASE,		NOR_CS_SIZE,	 BOOT_CS_ATTR,   0, 1},
#endif
	{TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM, TBL_TERM},
};

/* List the maximum amount of units for all devices and boards */
u16 base_unit_mask_table[MAX_UNIT_ID] = {
	/* the following entries are temporary examples only -  referenced from A38x */
	[PCIE_UNIT_ID]		=  cnt_to_mask(1),
	[SATA_UNIT_ID]		=  cnt_to_mask(1),
	[ETH_GIG_UNIT_ID]	=  cnt_to_mask(2),
	[UART_UNIT_ID]		=  cnt_to_mask(1),
	[USB_UNIT_ID]		=  cnt_to_mask(1),
	[USB3_UNIT_ID]		=  cnt_to_mask(1),
	[SPI_UNIT_ID]		=  cnt_to_mask(1),
	[XOR_UNIT_ID]		=  cnt_to_mask(1),
	[SDIO_UNIT_ID]		=  cnt_to_mask(1),
	[NAND_UNIT_ID]		=  cnt_to_mask(1)
};

/* Per device unit info. List which units are disabled for this device */
u16 armadalp_A_unit_disable[MAX_UNIT_ID] = {
	[ETH_GIG_UNIT_ID]	=  id_to_mask(2)
};

u16 armadalp_B_unit_disable[MAX_UNIT_ID] = {
	[ETH_GIG_UNIT_ID]	= id_to_mask(1),
	[USB3_UNIT_ID]		= id_to_mask(1)
};

/* MPP description table. Informative only */
char *armadalp_mpp_desc[MAX_MPP_ID + 1][MAX_MPP_OPTS] = {
/*	0x0        0x1         0x2              0x3             0x4             0x5             0x6 */
	/* the following entries are temporary examples only -  referenced from A38x */
/* 0 */	{"GPIO_0", "NF_IO_2", "SPI_0_CSn",	"SPI_1_CSn",	"NA",           "ND_IO_2", "NA"},	/* 0  */
/* 1 */	{"GPIO_1", "NF_IO_3", "SPI_0_MOSI",	"SPI_1_MOSI",	"NA",	        "ND_IO_3", "NA"},	 /* 1  */
/* 2 */	{"GPIO_2", "NF_IO_4", "PTP_EVENT_REQ",  "C0_LED",	"AU_I2S_DI",	"ND_IO_4", "SPI_1_MOSI"}, /* 2  */
/* 3 */	{"GPIO_3", "NF_IO_5", "PTP_TRIG_GEN",	"P3_LED",	"AU_I2S_MCLK",	"ND_IO_5", "SPI_1_MISO"}, /* 1  */
};

struct mvebu_soc_info armadalp_soc_info[] = {
	/* Armada LP A */
	{
		.name = "MV-ARMADALP_A", /* temp SOC name and SOC ID */
		.id = 0x9991,
		.unit_disable = armadalp_A_unit_disable,
		.memory_map = memory_map
	},
	/* Armada LP B */
	{
		.name = "MV-ARMADALP_B", /* temp SOC name and SOC ID */
		.id = 0x9992,
		.unit_disable = armadalp_B_unit_disable,
		.memory_map = memory_map
	},
	/* Delimiter */
	{.id = 0}
};

struct mvebu_soc_family armadalp_family_info = {
	.name = "MV-99xx", /* temp SOC name and SOC ID */
	.id   = 9990,
	.rev_name = {"Z1", "A0"},
	.base_unit_info = base_unit_mask_table,
	.soc_table = armadalp_soc_info,
	.mpp_desc  = (char **)armadalp_mpp_desc,
};
