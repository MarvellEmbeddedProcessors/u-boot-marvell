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
#include <asm/arch/cpu.h>
#include <asm/io.h>

#define NB_CLOCK_REGS_BASE	(MVEBU_REGISTER(0x13000))
#define NB_PLL_BASE		(NB_CLOCK_REGS_BASE + 0x200)
#define NB_TBG_CTRL0		(NB_PLL_BASE + 0x4)
 #define NB_TBG_CTRL0_TBG_A_FBDIV_OFFSET		2
 #define NB_TBG_CTRL0_TBG_A_FBDIV_MASK			0x1FFUL
 #define NB_TBG_CTRL0_TBG_B_FBDIV_OFFSET		18
 #define NB_TBG_CTRL0_TBG_B_FBDIV_MASK			0x1FFUL

#define NB_TBG_CTRL1		(NB_PLL_BASE + 0x8)
 #define NB_TBG_CTRL1_TBG_B_VCODIV_SEL_SE_OFFSET	16
 #define NB_TBG_CTRL1_TBG_B_VCODIV_SEL_SE_MASK		0x1FFUL
 #define NB_TBG_CTRL1_TBG_A_VCODIV_SEL_SE_MASK		0x1FFUL

#define NB_TBG_CTRL7		(NB_PLL_BASE + 0x20)
 #define NB_TBG_CTRL7_TBG_B_REFDIV_OFFSET		16
 #define NB_TBG_CTRL7_TBG_B_REFDIV_MASK			0x1FFUL
 #define NB_TBG_CTRL7_TBG_A_REFDIV_MASK			0x1FFUL

#define NB_TBG_CTRL8		(NB_PLL_BASE + 0x30)
 #define NB_TBG_CTRL8_TBG_A_VCODIV_SEL_DIFF_OFFSET	1
 #define NB_TBG_CTRL8_TBG_A_VCODIV_SEL_DIFF_MASK	0x1FFUL
 #define NB_TBG_CTRL8_TBG_B_VCODIV_SEL_DIFF_OFFSET	17
 #define NB_TBG_CTRL8_TBG_B_VCODIV_SEL_DIFF_MASK	0x1FFUL

#define NB_CLOCK_TBG_SELECT_REG	NB_CLOCK_REGS_BASE
 #define NB_CLOCK_TBG_SEL_A53_CPU_PCLK_OFFSET		22
 #define NB_CLOCK_TBG_SEL_A53_CPU_PCLK_MASK		0x3

/* north bridge clock divider select registers */
#define NB_CLOCK_DIV_SEL0_REG	(NB_CLOCK_REGS_BASE + 0x4)
 #define NB_CLOCK_DIV_SEL0_A53_CPU_CLK_PRSCL_OFFSET	28
 #define NB_CLOCK_DIV_SEL0_A53_CPU_CLK_PRSCL_MASK	0x7

#define NB_CLOCK_DIV_SEL2_REG	(NB_CLOCK_REGS_BASE + 0xC)
 #define NB_CLOCK_DIV_SEL2_WC_AHB_DIV_SEL_OFFSET	0
 #define NB_CLOCK_DIV_SEL2_WC_AHB_DIV_SEL_MASK		0x7

/* north bridge clock source register */
#define NB_CLOCK_SELECT_REG	(NB_CLOCK_REGS_BASE + 0x10)
 #define NB_CLOCK_SEL_DDR_PHY_CLK_SEL_OFFSET		10
 #define NB_CLOCK_SEL_DDR_PHY_CLK_SEL_MASK		0x1
 #define NB_CLOCK_SEL_A53_CPU_CLK_OFFSET		15
 #define NB_CLOCK_SEL_A53_CPU_CLK_MASK			0x1

/* south bridge clock registers */
#define SB_CLOCK_REGS_BASE	(MVEBU_REGISTER(0x18000))
#define SB_CLOCK_TBG_SELECT_REG	SB_CLOCK_REGS_BASE
 #define SB_CLOCK_TBG_SEL_SB_AXI_PCLK_OFFSET		20
 #define SB_CLOCK_TBG_SEL_SB_AXI_PCLK_MASK		0x3

/* south bridge clock divider select registers */
#define SB_CLOCK_DIV_SEL0_REG	(SB_CLOCK_REGS_BASE + 0x4)
 #define SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL1_OFFSET	24
 #define SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL1_MASK	0x7
 #define SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL2_OFFSET     21
 #define SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL2_MASK       0x7


#define TBG_A_REFDIV_GET(reg_val)	((reg_val >> 0) &\
					NB_TBG_CTRL7_TBG_A_REFDIV_MASK)
#define TBG_B_REFDIV_GET(reg_val)	((reg_val >>\
					NB_TBG_CTRL7_TBG_B_REFDIV_OFFSET) &\
					NB_TBG_CTRL7_TBG_B_REFDIV_MASK)
#define	TBG_A_FBDIV_GET(reg_val)	((reg_val >>\
					NB_TBG_CTRL0_TBG_A_FBDIV_OFFSET) &\
					NB_TBG_CTRL0_TBG_A_FBDIV_MASK)
#define TBG_B_FBDIV_GET(reg_val)	((reg_val >>\
					NB_TBG_CTRL0_TBG_B_FBDIV_OFFSET) &\
					NB_TBG_CTRL0_TBG_B_FBDIV_MASK)
#define TBG_A_VCODIV_SEL_SE_GET(reg_val)	((reg_val >> 0) &\
					NB_TBG_CTRL1_TBG_A_VCODIV_SEL_SE_MASK)
#define TBG_B_VCODIV_SEL_SE_GET(reg_val)	((reg_val >>\
				NB_TBG_CTRL1_TBG_B_VCODIV_SEL_SE_OFFSET) &\
				NB_TBG_CTRL1_TBG_B_VCODIV_SEL_SE_MASK)
#define TBG_A_VCODIV_SEL_DIFF_GET(reg_val)	((reg_val >>\
				NB_TBG_CTRL8_TBG_A_VCODIV_SEL_DIFF_OFFSET) &\
				NB_TBG_CTRL8_TBG_A_VCODIV_SEL_DIFF_MASK)
#define TBG_B_VCODIV_SEL_DIFF_GET(reg_val)	((reg_val >>\
				NB_TBG_CTRL8_TBG_B_VCODIV_SEL_DIFF_OFFSET) &\
				NB_TBG_CTRL8_TBG_B_VCODIV_SEL_DIFF_MASK)
#define A53_CPU_CLK_SEL_GET(reg_val)	((reg_val >>\
					NB_CLOCK_SEL_A53_CPU_CLK_OFFSET) &\
					NB_CLOCK_SEL_A53_CPU_CLK_MASK)
#define A53_CPU_PCLK_SEL_GET(reg_val)	((reg_val >>\
					NB_CLOCK_TBG_SEL_A53_CPU_PCLK_OFFSET) &\
					NB_CLOCK_TBG_SEL_A53_CPU_PCLK_MASK)
#define A53_CPU_CLK_PRSCL_GET(reg_val)	((reg_val >>\
				NB_CLOCK_DIV_SEL0_A53_CPU_CLK_PRSCL_OFFSET) &\
				NB_CLOCK_DIV_SEL0_A53_CPU_CLK_PRSCL_MASK)
#define DDR_PHY_CLK_SEL_GET(reg_val)	((reg_val >>\
					NB_CLOCK_SEL_DDR_PHY_CLK_SEL_OFFSET) &\
					NB_CLOCK_SEL_DDR_PHY_CLK_SEL_MASK)
#define WC_AHB_DIV_SEL_GET(reg_val)	((reg_val >>\
				NB_CLOCK_DIV_SEL2_WC_AHB_DIV_SEL_OFFSET) &\
				NB_CLOCK_DIV_SEL2_WC_AHB_DIV_SEL_MASK)
#define SB_AXI_PCLK_SEL_GET(reg_val)	((reg_val >>\
					SB_CLOCK_TBG_SEL_SB_AXI_PCLK_OFFSET) &\
					SB_CLOCK_TBG_SEL_SB_AXI_PCLK_MASK)
#define SB_AXI_CLK_PRSCL1_GET(reg_val)	((reg_val >>\
				SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL1_OFFSET) &\
				SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL1_MASK)
#define SB_AXI_CLK_PRSCL2_GET(reg_val)	((reg_val >>\
				SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL2_OFFSET) &\
				SB_CLOCK_DIV_SEL0_SB_AXI_CLK_PRSCL1_MASK)

enum a3700_clock_line {
	TBG_A_P = 0,
	TBG_B_P = 1,
	TBG_A_S = 2,
	TBG_B_S = 3
};

/* Clock source selection */
enum a3700_clk_select {
	CLK_SEL_OSC = 0,
	CLK_SEL_TBG,
};

/* TBG divider */
enum a3700_tbg_divider {
	TBG_DIVIDER_1 = 1,
	TBG_DIVIDER_2,
	TBG_DIVIDER_3,
	TBG_DIVIDER_4,
	TBG_DIVIDER_5,
	TBG_DIVIDER_6,
	TBG_DIVIDER_NUM
};

static u32 get_tbg_clk(enum a3700_clock_line tbg_typ)
{
	u32 tbg_M, tbg_N, vco_div;
	u32 ref, reg_val;

	/* get ref clock */
	ref = get_ref_clk();

	/* get M, N */
	reg_val = readl(NB_TBG_CTRL7);
	tbg_M = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		TBG_A_REFDIV_GET(reg_val) : TBG_B_REFDIV_GET(reg_val);
	tbg_M = (tbg_M == 0) ? 1 : tbg_M;

	reg_val = readl(NB_TBG_CTRL0);
	tbg_N = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		TBG_A_FBDIV_GET(reg_val) : TBG_B_FBDIV_GET(reg_val);

	if ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_B_S)) {
		/* get SE VCODIV */
		reg_val = readl(NB_TBG_CTRL1);
		reg_val = (tbg_typ == TBG_A_S) ?
			  TBG_A_VCODIV_SEL_SE_GET(reg_val) :
			  TBG_B_VCODIV_SEL_SE_GET(reg_val);
	} else {
		/* get DIFF VCODIV */
		reg_val = readl(NB_TBG_CTRL8);
		reg_val = (tbg_typ == TBG_A_P) ?
			  TBG_A_VCODIV_SEL_DIFF_GET(reg_val) :
			  TBG_B_VCODIV_SEL_DIFF_GET(reg_val);
	}
	if (reg_val > 7)
		return 0; /*invalid*/

	vco_div = 0x1 << reg_val;

	return ((tbg_N * ref) << 2)/(tbg_M * vco_div);
}

u32 soc_cpu_clk_get(void)
{
	u32 tbg, cpu_prscl;
	enum a3700_clock_line tbg_typ;

	/* 1. check cpu clock select */
	if (!A53_CPU_CLK_SEL_GET(readl(NB_CLOCK_SELECT_REG)))
		return 0; /* CPU clock is using XTAL output*/

	/* 2. get TBG select */
	tbg_typ = A53_CPU_PCLK_SEL_GET(readl(NB_CLOCK_TBG_SELECT_REG));

	/* 3. get TBG clock */
	tbg = get_tbg_clk(tbg_typ);
	if (tbg == 0)
		return 0;

	/* 4. get CPU clk divider */
	cpu_prscl = A53_CPU_CLK_PRSCL_GET(readl(NB_CLOCK_DIV_SEL0_REG));
	if (cpu_prscl == 7)
		return 0; /* divider value error */

	return tbg/cpu_prscl;
}

u32 soc_ddr_clk_get(void)
{
	u32 tbg;

	/* 1. check DDR clock select */
	if (!DDR_PHY_CLK_SEL_GET(readl(NB_CLOCK_SELECT_REG)))
		return 0; /* DDR clock is using XTAL output*/

	/* 2. get TBG_A clock */
	tbg = get_tbg_clk(TBG_A_S);
	if (tbg == 0)
		return 0;

	return tbg>>1;
}

/******************************************************************************
* Name: get_cpu_clk_src_div
*
* Description: Get CPU clock source selection and prescaling divider
*
* Input:	None
* Output:	cpu_clk_sel: CPU clock source selection
*		cpu_clk_prscl: CPU clock prescaling divider
* Return:	Non-zero if failed to get the CPU clock selection and prescaling
******************************************************************************/
int get_cpu_clk_src_div(u32 *cpu_clk_sel, u32 *cpu_clk_prscl)
{
	/* 1. check cpu clock select */
	if (!A53_CPU_CLK_SEL_GET(readl(NB_CLOCK_SELECT_REG)))
		return -1; /* CPU clock is using XTAL output*/

	/* 2. get TBG select */
	*cpu_clk_sel = A53_CPU_PCLK_SEL_GET(readl(NB_CLOCK_TBG_SELECT_REG));

	/* 3. get CPU clk divider */
	*cpu_clk_prscl = A53_CPU_CLK_PRSCL_GET(readl(NB_CLOCK_DIV_SEL0_REG));

	return 0;
}

u32 soc_nb_axi_clk_get(void)
{
	u32 nb_axi_div;

	nb_axi_div = WC_AHB_DIV_SEL_GET(readl(NB_CLOCK_DIV_SEL2_REG));
	if (nb_axi_div == 0)
		return 0;

	return soc_cpu_clk_get()/nb_axi_div;
}

u32 soc_sb_axi_clk_get(void)
{
	u32 tbg, sb_axi_prscl1, sb_axi_prscl2;
	enum a3700_clock_line tbg_typ;

	/* 1. get TBG select */
	tbg_typ = SB_AXI_PCLK_SEL_GET(readl(SB_CLOCK_TBG_SELECT_REG));

	/* 2. get TBG clock */
	tbg = get_tbg_clk(tbg_typ);
	if (tbg == 0)
		return 0;

	sb_axi_prscl1 = SB_AXI_CLK_PRSCL1_GET(readl(SB_CLOCK_DIV_SEL0_REG));
	if (sb_axi_prscl1 == 0 || sb_axi_prscl1 == 7)
		return 0;

	sb_axi_prscl2 = SB_AXI_CLK_PRSCL2_GET(readl(SB_CLOCK_DIV_SEL0_REG));
	if (sb_axi_prscl2 == 0 || sb_axi_prscl2 == 7)
		return 0;

	return tbg/(sb_axi_prscl1*sb_axi_prscl2);
}

void soc_print_clock_info(void)
{
	printf("       CPU     %d [MHz]\n", soc_cpu_clk_get());
	printf("       L2      %d [MHz]\n", soc_cpu_clk_get());
	printf("       NB AXI  %d [MHz]\n", soc_nb_axi_clk_get());
	printf("       SB AXI  %d [MHz]\n", soc_sb_axi_clk_get());
	printf("       DDR     %d [MHz]\n", soc_ddr_clk_get());
}
