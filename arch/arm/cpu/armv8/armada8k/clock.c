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
#include <asm/arch/sar.h>

#define DDR_PLL_ID	0
#define RING_PLL_ID	1
#define CPU_PLL_ID	2
#define PIDI_AP_PLL_ID	3
#define PIDI_CP_PLL_ID	4

#define MSS_CLOCK_DIV	6

const u32 pll_freq_tbl[7][5] = {
	/* DDR */   /* Ring */ /* CPU */ /* PIDI-AP */  /* PIDI-CP */
	{1.2  * GHz, 2.0 * GHz, 2.5 * GHz, 2 * GHz,	2 * GHz},
	{1.05 * GHz, 1.8 * GHz, 2.2 * GHz, 1 * GHz,	1 * GHz},
	{900  * MHz, 1.6 * GHz, 2.0 * GHz, 1 * GHz,	1 * GHz},
	{800  * MHz, 1.4 * GHz, 1.7 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.6 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.2 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.2 * GHz,    0,	   0}
};

u32 soc_ring_clk_get(void)
{
	u32 pll_idx = (readl(SAMPLE_AT_RESET_REG_1) & SAR1_PLL1_MASK) >> SAR1_PLL1_OFFSET;
	return pll_freq_tbl[pll_idx][RING_PLL_ID];
}

u32 soc_mss_clk_get(void)
{
	return soc_ring_clk_get() / MSS_CLOCK_DIV;
}

u32 soc_tclk_get(void)
{
#ifndef CONFIG_PALLADIUM
	return soc_mss_clk_get();
#else
	return CONFIG_MSS_FREQUENCY;
#endif
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
	u32 pll_idx = (readl(SAMPLE_AT_RESET_REG_1) & SAR1_PLL2_MASK) >> SAR1_PLL2_OFFSET;
	return pll_freq_tbl[pll_idx][CPU_PLL_ID];
}

u32 soc_ddr_clk_get(void)
{
	u32 pll_idx = (readl(SAMPLE_AT_RESET_REG_1) & SAR1_PLL0_MASK) >> SAR1_PLL0_OFFSET;
	return pll_freq_tbl[pll_idx][DDR_PLL_ID];
}

void soc_print_clock_info(void)
{
	printf("Clock:  CPU     %-4d [MHz]\n", soc_cpu_clk_get() / MHz);
	printf("\tDDR     %-4d [MHz]\n", soc_ddr_clk_get() / MHz);
	printf("\tFABRIC  %-4d [MHz]\n", soc_ring_clk_get() / MHz);
	printf("\tMSS     %-4d [MHz]\n", soc_mss_clk_get() / MHz);
}
