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

#define CORE_DIV_CLK_CTRL(x)			(MVEBU_REGS_BASE + 0xe4250 + (x * 4))
#define CORE_DIVCLK_RELOAD_FORCE_OFFS           0
#define CORE_DIVCLK_RELOAD_FORCE_MASK           (0xFF << CORE_DIVCLK_RELOAD_FORCE_OFFS)
#define CORE_DIVCLK_RELOAD_FORCE_VAL            (0x2 << CORE_DIVCLK_RELOAD_FORCE_OFFS)

#define NAND_ECC_DIVCKL_RATIO_OFFS              8
#define NAND_ECC_DIVCKL_RATIO_MASK              (0x3F << NAND_ECC_DIVCKL_RATIO_OFFS)

#define CORE_DIVCLK_RELOAD_RATIO_OFFS           8
#define CORE_DIVCLK_RELOAD_RATIO_MASK           (1 << CORE_DIVCLK_RELOAD_RATIO_OFFS)

#define PUP_EN_REG                              (MVEBU_REGS_BASE + 0x1864C)
#define SOC_DEV_MUX_REG                         (MVEBU_REGS_BASE + 0x18208)

#define NAND_ECC_REF_CLOCK			(1000000000UL)	/* 1GHz */
#define NAND_ECC_CLOCK_DIV			(4)	/* 100MHz */

void nand_clock_init(void)
{
	int divider = NAND_ECC_CLOCK_DIV;
	u32 val;

	/* Enable NAND Flash PUP */
	val = readl(PUP_EN_REG);
	val |= 0x10;
	writel(val, PUP_EN_REG);

	val = readl(SOC_DEV_MUX_REG);
	/* NAND flash enable */
	val |= 0x1;
	/* Enable arbitration between device and NAND */
	val |= (1 << 27);
	writel(val, SOC_DEV_MUX_REG);

	/* Set the division ratio of ECC Clock 0x00018748[13:8] (by default it's double of core clock) */
	val = readl(CORE_DIV_CLK_CTRL(1));

	val &= ~(NAND_ECC_DIVCKL_RATIO_MASK);
	val |= (divider << NAND_ECC_DIVCKL_RATIO_OFFS);
	writel(val, CORE_DIV_CLK_CTRL(1));

	/* Set reload force of ECC clock 0x00018740[7:0] to 0x2 (meaning you will force only the ECC clock) */
	val = readl(CORE_DIV_CLK_CTRL(0));
	val &= ~(CORE_DIVCLK_RELOAD_FORCE_MASK);
	val |= CORE_DIVCLK_RELOAD_FORCE_VAL;
	writel(val, CORE_DIV_CLK_CTRL(0));

	/* Set reload ratio bit 0x00018740[8] to 1'b1 */
	val = readl(CORE_DIV_CLK_CTRL(0));
	val |= CORE_DIVCLK_RELOAD_RATIO_MASK;
	writel(val, CORE_DIV_CLK_CTRL(0));
	mdelay(1); /*  msec */

	/* Set reload ratio bit 0x00018740[8] to 0'b1 */
	val = readl(CORE_DIV_CLK_CTRL(0));
	val &= ~CORE_DIVCLK_RELOAD_RATIO_MASK;
	writel(val, CORE_DIV_CLK_CTRL(0));
}

#ifdef CONFIG_NAND_PXA3XX
/* Needed by pxa3xx_nand driver */
unsigned long mvebu_get_nand_clock(void)
{
	int divider = NAND_ECC_CLOCK_DIV;

	/* Return calculated nand clock frequency */
	return NAND_ECC_REF_CLOCK/(2 * divider);
}
#endif /* CONFIG_NAND_PXA3XX */


