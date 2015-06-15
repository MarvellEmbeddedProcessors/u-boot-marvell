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
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/soc.h>

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
	},
	/* Armda 385 */
	{
		.name = "MV-6285",
		.id = 0x6825,
		.unit_disable = armada6285_unit_disable,
	},
	/* Armda 388 */
	{
		.name = "MV-6288",
		.id = 0x6828,
		.unit_disable = armada6280_unit_disable,
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
