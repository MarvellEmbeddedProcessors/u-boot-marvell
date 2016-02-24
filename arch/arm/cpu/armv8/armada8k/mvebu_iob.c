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
#include <asm/arch-mvebu/iob.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)

/* IOB registers */
#define IOB_MAX_WIN_NUM			(24)

#define IOB_WIN_CR_OFFSET(base, win)	(base + 0x0 + (0x20 * win))
#define IOB_TARGET_ID_OFFSET		(8)
#define IOB_TARGET_ID_MASK		(0xF)

#define IOB_WIN_ALR_OFFSET(base, win)	(base + 0x8 + (0x20 * win))
#define IOB_WIN_AHR_OFFSET(base, win)	(base + 0xC + (0x20 * win))

DECLARE_GLOBAL_DATA_PTR;

enum target_ids_iob {
	INTERNAL_TID	= 0x0,
	IHB0_TID	= 0x1,
	PEX1_TID	= 0x2,
	PEX2_TID	= 0x3,
	PEX0_TID	= 0x4,
	NAND_TID	= 0x5,
	RUNIT_TID	= 0x6,
	IHB1_TID	= 0x7,
	IOB_MAX_TID
};

void dump_iob(void)
{
	u32 win_id, win_cr, alr, ahr;
	u8 target_id;
	u64 start, end;
	const void *blob = gd->fdt_blob;
	void __iomem *iob_base;
	u32 max_win, node;
	char *iob_target_name[IOB_MAX_TID] = {"CONFIG", "IHB0  ", "PEX1  ", "PEX2  ",
					      "PEX0  ", "NAND  ", "RUNIT ", "IHB1  "};

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_IOB));
	if (node < 0) {
		error("No IOB address decoding node found in FDT blob\n");
		return;
	}
	/* Get the base address of the address decoding MBUS */
	iob_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Get the maximum number of iob windows supported */
	max_win = fdtdec_get_int(blob, node, "max-win", 0);
	if (max_win == 0) {
		max_win = IOB_MAX_WIN_NUM;
		error("failed reading max windows number\n");
	}

	/* Dump all IOB windows */
	printf("bank  id target  start		    end\n");
	printf("----------------------------------------------------\n");
	for (win_id = 0; win_id < max_win; win_id++) {
		win_cr = readl(IOB_WIN_CR_OFFSET(iob_base, win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> IOB_TARGET_ID_OFFSET) & IOB_TARGET_ID_MASK;
			alr = readl(IOB_WIN_ALR_OFFSET(iob_base, win_id));
			start = ((u64)alr << ADDRESS_SHIFT);
			if (win_id != 0) {
				ahr = readl(IOB_WIN_AHR_OFFSET(iob_base, win_id));
				end = (((u64)ahr + 0x10) << ADDRESS_SHIFT);
			} else {
				/* Window #0 size is hardcoded to 16MB, as it's
				** reserved for CP configuration space. */
				end = start + (16 << 20);
			}
			printf("iob   %02d %s  0x%016llx 0x%016llx\n"
					, win_id, iob_target_name[target_id], start, end);
		}
	}

	return;
}

