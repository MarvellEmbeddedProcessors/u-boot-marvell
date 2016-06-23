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

#ifndef _ARMADA3700_CLOCK_H_
#define _ARMADA3700_CLOCK_H_

/****************/
/* North Bridge */
/****************/
#define MVEBU_NORTH_BRG_PLL_BASE		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x200)
#define MVEBU_NORTH_BRG_TBG_CFG			(MVEBU_NORTH_BRG_PLL_BASE + 0x0)
#define MVEBU_NORTH_BRG_TBG_CTRL0		(MVEBU_NORTH_BRG_PLL_BASE + 0x4)
#define MVEBU_NORTH_BRG_TBG_CTRL1		(MVEBU_NORTH_BRG_PLL_BASE + 0x8)
#define MVEBU_NORTH_BRG_TBG_CTRL7		(MVEBU_NORTH_BRG_PLL_BASE + 0x20)
#define MVEBU_NORTH_BRG_TBG_CTRL8		(MVEBU_NORTH_BRG_PLL_BASE + 0x30)

#define MVEBU_NORTH_CLOCK_TBG_SELECT_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x0)

/* north bridge clock divider select registers */
#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x4)

/* north bridge clock source register */
#define MVEBU_NORTH_CLOCK_SELECT_REG		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x10)

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

/*****************************************************************************
* get_ref_clk
*
* return: reference clock in MHz (25 or 40)
******************************************************************************/
u32 get_ref_clk(void);

/*****************************************************************************
* get_cpu_clk
*
* return: current CPU clock in Mhz
******************************************************************************/
u32 get_cpu_clk(void);

/*****************************************************************************
* get_ddr_clk
*
* return: current DDR clock in Mhz
******************************************************************************/
u32 get_ddr_clk(void);

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
int get_cpu_clk_src_div(u32 *cpu_clk_sel, u32 *cpu_clk_prscl);

#endif /* _ARMADA3700_CLOCK_H_ */
