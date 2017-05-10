/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
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
#include <mach/clock.h>
#include <mvebu/mvebu_chip_sar.h>

#define MSS_CLOCK_DIV	6

#ifndef CONFIG_PALLADIUM
#define CONFIG_MSS_FREQUENCY    (200 * 1000000)
#else
#define CONFIG_MSS_FREQUENCY    (384000)
#endif

#ifdef __ASSEMBLY__
#define MVEBU_CP0_REGS_BASE     (0xF2000000)
#else
#define MVEBU_CP0_REGS_BASE     (0xF2000000UL)
#endif

u32 soc_ring_clk_get(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_AP_FABRIC_FREQ , &sar);
	return sar.freq;
}

u32 soc_mss_clk_get(void)
{
	return CONFIG_MSS_FREQUENCY;
}

u32 soc_tclk_get(void)
{
	return soc_mss_clk_get();
}



u32 soc_cpu_clk_get(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_CPU_FREQ , &sar);
	return sar.freq;
}

u32 soc_ddr_clk_get(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_DDR_FREQ, &sar);
	return sar.freq;
}

void soc_print_clock_info(void)
{
	printf("Clock:  CPU     %-4d [MHz]\n", soc_cpu_clk_get() / MHz);
	printf("\tDDR     %-4d [MHz]\n", soc_ddr_clk_get() / MHz);
	printf("\tFABRIC  %-4d [MHz]\n", soc_ring_clk_get() / MHz);
	printf("\tMSS     %-4d [MHz]\n", soc_mss_clk_get() / MHz);
}
