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
/*#define DEBUG*/
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/clock.h>
#include <asm/arch/soc-info.h>	/* XTAL and Boot mode */

/***************************************************************************************************
  * get_ref_clk
  *
  * return: reference clock in MHz (25 or 40)
 ***************************************************************************************************/
u32 get_ref_clk(void)
{
	u32 regval;

	regval = (readl(MVEBU_TEST_PIN_LATCH_N) & MVEBU_XTAL_MODE_MASK) >> MVEBU_XTAL_MODE_OFFS;

	if (regval == MVEBU_XTAL_CLOCK_25MHZ)
		return 25;
	else
		return 40;
}

static u32 get_tbg_clk(enum a3700_clock_line tbg_typ)
{
	u32 tbg_M, tbg_N, vco_div;
	u32 ref, reg_val;

	/* get ref clock */
	ref = get_ref_clk();

	/* get M, N */
	reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL7);
	tbg_M = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		(reg_val & 0x1FFUL) : ((reg_val >> 16) & 0x1FFUL);
	tbg_M = (tbg_M == 0) ? 1 : tbg_M;

	reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	tbg_N = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		((reg_val >> 2) & 0x1FFUL) : ((reg_val >> 18) & 0x1FFUL);

	if ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_B_S)) {
		/* get SE VCODIV */
		reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
		reg_val = (tbg_typ == TBG_A_S) ?
			  (reg_val & 0x1FFUL) : ((reg_val >> 16) & 0x1FFUL);
	} else {
		/* get DIFF VCODIV */
		reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL8);
		reg_val = (tbg_typ == TBG_A_P) ?
			  ((reg_val >> 1) & 0x1FFUL) : ((reg_val >> 17) & 0x1FFUL);
	}
	if (reg_val > 7)
		return 0; /*invalid*/

	vco_div = 0x1 << reg_val;

	return ((tbg_N * ref) << 2)/(tbg_M * vco_div);
}

u32 get_cpu_clk(void)
{
	u32 tbg, cpu_prscl;
	enum a3700_clock_line tbg_typ;

	/* 1. check cpu clock select */
	if (!((readl(MVEBU_NORTH_CLOCK_SELECT_REG) >> 15) & 0x1))
		return 0; /* CPU clock is using XTAL output*/

	/* 2. get TBG select */
	tbg_typ = (readl(MVEBU_NORTH_CLOCK_TBG_SELECT_REG) >> 22) & 0x3;

	/* 3. get TBG clock */
	tbg = get_tbg_clk(tbg_typ);
	if (tbg == 0)
		return 0;

	/* 4. get CPU clk divider */
	cpu_prscl = (readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG) >> 28) & 0x7;
	if (cpu_prscl == 7)
		return 0; /* divider value error */

	return tbg/cpu_prscl;
}

u32 get_ddr_clk(void)
{
	u32 tbg;

	/* 1. check DDR clock select */
	if (!((readl(MVEBU_NORTH_CLOCK_SELECT_REG) >> 10) & 0x1))
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
	if (!((readl(MVEBU_NORTH_CLOCK_SELECT_REG) >> 15) & 0x1))
		return -1; /* CPU clock is using XTAL output*/

	/* 2. get TBG select */
	*cpu_clk_sel = (readl(MVEBU_NORTH_CLOCK_TBG_SELECT_REG) >> 22) & 0x3;

	/* 3. get CPU clk divider */
	*cpu_clk_prscl = (readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG) >> 28) & 0x7;

	return 0;
}
