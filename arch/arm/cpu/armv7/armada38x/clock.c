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
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/arch-mvebu/fdt.h>

u32 soc_tclk_get(void)
{
	return MHZ * 200;
}

u32 soc_cpu_clk_get(void)
{
	return 200000000;
}

u32 soc_ddr_clk_get(void)
{
	return 200000000;
}

u32 soc_l2_clk_get(void)
{
	return 800000000;
}

u32 soc_timer_clk_get(void)
{
	return 800000000;
}

u32 get_fdt_tclk(const void *blob, int node)
{
	u32 tclk;
	void *reg;

	if (node == -1)
		node = fdt_node_offset_by_compatible(blob, -1, "marvell,tclk");

	reg = fdt_get_regs_offs(blob, node, "reg");

	tclk = readl(reg);
	tclk = ((tclk & 0x8000) >> 15);

	switch (tclk) {
	case 0:
		return MHZ * 250;
	case 1:
		return MHZ * 200;
	default:
		return MHZ * 250;
	}
}
