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
#include <asm/arch-mvebu/amb_adec.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)

/* AMB registers */
#define AMB_MAX_WIN_ID			(7)

#define AMB_WIN_CR_OFFSET(base, win)	(base + 0x0 + (0x8 * win))
#define AMB_ATTR_OFFSET			(8)
#define AMB_ATTR_MASK			(0xFF)

#define AMB_WIN_BASE_OFFSET(base, win)	(base + 0x4 + (0x8 * win))
#define AMB_BASE_OFFSET			(16)

DECLARE_GLOBAL_DATA_PTR;

void dump_amb_adec(void)
{
	const void *blob = gd->fdt_blob;
	void __iomem *amb_base;
	u32 ctrl, base, win_id, attr, node;

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_AMB_ADEC));
	if (node < 0) {
		error("No AMB address decoding node found in FDT blob\n");
		return;
	}
	/* Get the base address of the address decoding MBUS */
	amb_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Dump all AMB windows */
	printf("bank  attribute     base\n");
	printf("------------------------------\n");
	for (win_id = 0; win_id < AMB_MAX_WIN_ID; win_id++) {
		ctrl = readl(AMB_WIN_CR_OFFSET(amb_base, win_id));
		if (ctrl & WIN_ENABLE_BIT) {
			base = readl(AMB_WIN_BASE_OFFSET(amb_base, win_id));
			attr = (ctrl >> AMB_ATTR_OFFSET) & AMB_ATTR_MASK;
			printf("amb   0x%04x        0x%08x\n", attr, base);
		}
	}

	return;
}
