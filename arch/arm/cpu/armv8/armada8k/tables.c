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
u16 armada8021_unit_disable[MAX_UNIT_ID] = {
	[PEX_UNIT_ID]		=  id_to_mask(0) | id_to_mask(2),
	[SATA_UNIT_ID]		=  id_to_mask(2) | id_to_mask(3)
};

u16 armada8022_unit_disable[MAX_UNIT_ID] = {
	[ETH_GIG_UNIT_ID]	= id_to_mask(3),
	[USB3_UNIT_ID]		= id_to_mask(1)
};

/* MPP description table. Informative only */
char *a8k_mpp_desc[MAX_MPP_ID + 1][MAX_MPP_OPTS] = {
	{"GPIO_0", "NF_IO_2", "SPI_0_CSn",	"SPI_1_CSn",	"NA",           "ND_IO_2", "NA"},	/* 0  */
	{"GPIO_1", "NF_IO_3", "SPI_0_MOSI",	"SPI_1_MOSI",	"NA",	        "ND_IO_3", "NA"},	 /* 1  */
	{"GPIO_2", "NF_IO_4", "PTP_EVENT_REQ",  "C0_LED",	"AU_I2S_DI",	"ND_IO_4", "SPI_1_MOSI"}, /* 2  */
	{"GPIO_3", "NF_IO_5", "PTP_TRIG_GEN",	"P3_LED",	"AU_I2S_MCLK",	"ND_IO_5", "SPI_1_MISO"}, /* 1  */
};


struct mvebu_soc_info a8k_soc_info[] = {
	/* Armda 8021 */
	{
		.name = "MV-8021",
		.id = 8021,
		.unit_disable = armada8021_unit_disable,
		.memory_map = memory_map
	},
	/* Armda 8022 */
	{
		.name = "MV-8022",
		.id = 8022,
		.unit_disable = armada8022_unit_disable,
		.memory_map = memory_map
	},
	/* Delimiter */
	{.id = 0}
};

struct mvebu_soc_family a8k_family_info = {
	.name = "MV-80xx",
	.id   = 8000,
	.base_unit_info = base_unit_mask_table,
	.soc_table = a8k_soc_info,
	.mpp_desc  = (char **)a8k_mpp_desc,
};
