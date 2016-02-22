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

/* #define DEBUG */
/* #define DEBUG_REG */

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/rfu.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {Addr[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)

DECLARE_GLOBAL_DATA_PTR;

enum rfu_target_ids {
	BOOTROM_TID     = 0x0,
	STM_TID         = 0x1,
	SPI_TID         = 0x2,
	PCIE_REGS_TID   = 0x3,
	IHBPHY_TID      = 0x4,
	PCIE_PORT_TID   = 0x5,
	RFU_MAX_TID
};

u32 rfu_alr_offset_get(u32 trgt_id)
{
	u32 rfu_alr_offset[RFU_MAX_TID] = {0x0, 0x10, 0x20, 0x208, 0x210, 0x218};

	if (trgt_id < 0 || trgt_id > RFU_MAX_TID) {
		error("target ID is wrong");
		return 0;
	}
	return rfu_alr_offset[trgt_id];
}

u32 rfu_ahr_offset_get(u32 trgt_id)
{
	u32 rfu_ahr_offset[RFU_MAX_TID] = {0x0, 0x18, 0x28, 0x0, 0x214, 0x21c};

	if (trgt_id < 0 || trgt_id > RFU_MAX_TID) {
		error("target ID is wrong");
		return 0;
	}
	return rfu_ahr_offset[trgt_id];
}

void dump_rfu(void)
{
	u32 trgt_id;
	u32 alr, ahr;
	u64 start, end;
	const void *blob = gd->fdt_blob;
	void __iomem *rfu_base;
	char *rfu_target_name[RFU_MAX_TID] = {"BootRoom ", "STM      ", "SPI      ",
					"PCIe-reg ", "IHB-Port ", "PCIe-Port"};
	int node;

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_RFU));
	if (node < 0) {
		error("No RFU address decoding node found in FDT blob\n");
		return;
	}

	/* Get the base address of the address decoding MBUS */
	rfu_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Dump all RFU windows */
	printf("bank  target     start              end\n");
	printf("----------------------------------------------------\n");
	for (trgt_id = 0; trgt_id < RFU_MAX_TID; trgt_id++) {
		alr = readl(rfu_base + rfu_alr_offset_get(trgt_id));
		if (alr & WIN_ENABLE_BIT) {
			alr &= ~WIN_ENABLE_BIT;
			if (trgt_id == BOOTROM_TID || trgt_id == PCIE_REGS_TID)
				ahr = alr;
			else
				ahr = readl(rfu_base + rfu_ahr_offset_get(trgt_id));
			start = ((u64)alr << ADDRESS_SHIFT);
			end = (((u64)ahr + 0x10) << ADDRESS_SHIFT);
			printf("rfu   %s  0x%016llx 0x%016llx\n", rfu_target_name[trgt_id], start, end);
		}
	}
	printf("rfu   PIDI-port  - all other IO transactions\n");

	return;
}
