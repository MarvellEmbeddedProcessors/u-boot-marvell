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
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/arch-mvebu/fdt.h>

u32 soc_tclk_get(void)
{
	return MHz * 200;
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
		return MHz * 250;
	case 1:
		return MHz * 200;
	default:
		return MHz * 250;
	}
}

void soc_print_clock_info(void)
{
	printf("       CPU    @ %d [MHz]\n", soc_cpu_clk_get() / 1000000);
	printf("       L2     @ %d [MHz]\n", soc_l2_clk_get() / 1000000);
	printf("       TClock @ %d [MHz]\n", soc_tclk_get() / 1000000);
	printf("       DDR    @ %d [MHz]\n", soc_ddr_clk_get() / 1000000);
}
