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
#include <asm/arch-mvebu/clock.h>
#include <asm/arch/clock.h>

u32 soc_tclk_get(void)
{
	return 200000000;
}

u32 soc_cpu_clk_get(void)
{
	return get_cpu_clk() * 1000000;
}

u32 soc_ddr_clk_get(void)
{
	return get_ddr_clk() * 1000000;
}

u32 soc_l2_clk_get(void)
{
	return 800000000;
}

u32 soc_timer_clk_get(void)
{
	return 800000000;
}

void soc_print_clock_info(void)
{
	printf("       CPU    @ %d [MHz]\n", soc_cpu_clk_get() / 1000000);
	printf("       L2     @ %d [MHz]\n", soc_l2_clk_get() / 1000000);
	printf("       TClock @ %d [MHz]\n", soc_tclk_get() / 1000000);
	printf("       DDR    @ %d [MHz]\n", soc_ddr_clk_get() / 1000000);
}
