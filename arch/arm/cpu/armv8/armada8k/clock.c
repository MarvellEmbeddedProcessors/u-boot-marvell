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
#include <asm/arch-mvebu/clock.h>
#include <mvebu_chip_sar.h>

#define MSS_CLOCK_DIV	6


u32 soc_ring_clk_get(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_AP_FABRIC_FREQ , &sar);
	return sar.freq;
}

u32 soc_mss_clk_get(void)
{
#ifdef CONFIG_AP806_Z_SUPPORT
	return soc_ring_clk_get() / MSS_CLOCK_DIV;
#else
	return CONFIG_MSS_FREQUENCY;
#endif
}

u32 soc_tclk_get(void)
{
	return soc_mss_clk_get();
}


#ifdef CONFIG_NAND_PXA3XX
#define NAND_FLASH_CLK_CTRL (MVEBU_CP0_REGS_BASE + 0x440700)
#define NF_CLOCK_SEL_MASK	(0x1)
unsigned long mvebu_get_nand_clock(void)
{
	u32 reg;
	reg = readl(NAND_FLASH_CLK_CTRL);
	if (reg & NF_CLOCK_SEL_MASK)
		return 400 * MHz;
	else
		return 250 * MHz;
}
#endif

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
