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
};
