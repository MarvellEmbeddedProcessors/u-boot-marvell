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
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/io.h>

/* north bridge clock source register */
#define MVEBU_NORTH_CLOCK_SELEC_REG	0x10

#define WCPU_CLK_SEL		(1 << 15)
#define COUNTER_CLK_SEL		(1 << 13)
#define TRACE_CLK_SEL		(1 << 12)
#define DDR_FCLK_CLK_SEL	(1 << 11)
#define DDR_PHY_CLK_SEL		(1 << 10)
#define EIP97_CLK_SEL		(1 << 9)
#define PWM_CLK_SEL		(1 << 8)
#define SQF_CLK_SEL		(1 << 7)
#define AVS_CLK_SEL		(1 << 6)
#define SETM_TMX_CLK_SEL	(1 << 5)
#define TSECM_CLK_SEL		(1 << 4)
#define SEC_DAP_CLK_SEL		(1 << 3)
#define SEC_AT_CLK_SEL		(1 << 2)
#define SATA_HOST_CLK_SEL	(1 << 1)
#define MMC_CLK_SEL		(1 << 0)
/* change all clock source to PLL */
#define NB_PLL_CLK_SOURCES	(WCPU_CLK_SEL | COUNTER_CLK_SEL | TRACE_CLK_SEL | DDR_FCLK_CLK_SEL |\
				DDR_PHY_CLK_SEL | EIP97_CLK_SEL | PWM_CLK_SEL | SQF_CLK_SEL |\
				AVS_CLK_SEL | SETM_TMX_CLK_SEL | TSECM_CLK_SEL | SEC_DAP_CLK_SEL |\
				SEC_AT_CLK_SEL | SATA_HOST_CLK_SEL | MMC_CLK_SEL)

/* north bridge clock enable register */
#define MVEBU_NORTH_CLOCK_ENABLE_REG	0x14
#define SB_AXI_CLK_EN		(0 << 27)
#define EIP97_CLK_EN		(0 << 26)
#define SATA_HOST_AXI_CLK_EN	(0 << 25)
#define ATB_CLK_EN		(0 << 24)
#define COUNTER_CLK_EN		(0 << 23)
#define TRACE_CLK_EN		(0 << 22)
#define DDR_FCLK_CLK_EN		(0 << 21)
#define DDR_PHY_MCK_CLK_EN	(0 << 19)
#define DDR_AXI_CLK_EN		(0 << 18)
#define TWSI_1_CLK_EN		(0 << 17)
#define TWSI_2_CLK_EN		(0 << 16)
#define SEC_AXI_CLK_EN		(0 << 15)
#define PWM_CLK_EN		(0 << 13)
#define SQF_CLK_EN		(0 << 12)
#define AVS_CLK_EN		(0 << 11)
#define SETM_TMX_CLK_EN		(0 << 10)
#define DMA_CLK_EN		(0 << 9)
#define TSECM_CLK_EN		(0 << 8)
#define SEC_DAP_CLK_EN		(0 << 7)
#define SEC_AT_CLK_EN		(0 << 6)
#define SEC_XTAL_CLK_EN		(0 << 5)
#define CPU_CNT_EN		(0 << 4)
#define SATA_HOST_CLK_EN	(0 << 3)
#define MMC_CLK_CLK_EN		(0 << 2)
#define MMC_SYSBUS_CLK_EN	(0 << 1)
#define MMC_DEBOUNCE_CLK_EN	(0 << 0)

/* enable all the north bridge clocks by default */
#define NB_CLK_ENABLE		(SB_AXI_CLK_EN | EIP97_CLK_EN | SATA_HOST_AXI_CLK_EN | ATB_CLK_EN |\
				COUNTER_CLK_EN | TRACE_CLK_EN | DDR_FCLK_CLK_EN | DDR_PHY_MCK_CLK_EN |\
				DDR_AXI_CLK_EN | TWSI_1_CLK_EN | TWSI_2_CLK_EN | SEC_AXI_CLK_EN |\
				PWM_CLK_EN | SQF_CLK_EN | AVS_CLK_EN | SETM_TMX_CLK_EN |\
				DMA_CLK_EN | TSECM_CLK_EN | SEC_DAP_CLK_EN | SEC_AT_CLK_EN |\
				SEC_XTAL_CLK_EN | CPU_CNT_EN | SATA_HOST_CLK_EN | MMC_CLK_CLK_EN |\
				MMC_SYSBUS_CLK_EN | TSECM_CLK_EN)

/* south bridge clock source register */
#define MVEBU_SOUTH_CLOCK_SELEC_REG	0x10
#define SB_AXI_CLK_SEL		(1 << 10)
#define USB32_SS_SYS_CLK_SEL	(1 << 9)
#define USB32_USB2_SYS_CLK_SEL	(1 << 8)
#define SDIO_CLK_SEL		(1 << 7)
#define GBE_CORE_CLK_SEL	(1 << 5)
#define GBE_125_CLK_SEL		(1 << 3)
#define GBE_50_CLK_SEL		(1 << 1)

#define SB_PLL_CLK_SOURCES	(SB_AXI_CLK_SEL | USB32_SS_SYS_CLK_SEL | USB32_USB2_SYS_CLK_SEL |\
				SDIO_CLK_SEL | GBE_CORE_CLK_SEL | GBE_125_CLK_SEL | GBE_50_CLK_SEL)

/* south bridge clock enable register */
#define MVEBU_SOUTH_CLOCK_ENABLE_REG	0x14
#define GBE0_PWR_UP_CLK_EN	(1 << 20)
#define GBE1_PWR_UP_CLK_EN	(1 << 19)
#define USB32_AXI_CLK_EN	(0 << 18)
#define USB32_SS_SYS_CLK_EN	(0 << 17)
#define USB32_USB2_SYS_CLK_EN	(0 << 16)
#define USB2_HOST_CLK_EN	(0 << 15)
#define PCIE_CLKK_EN		(0 << 14)
#define USB32_REF_CLK_EN	(0 << 13)
#define USB2_HOST_REF_CLK_EN	(0 << 12)
#define SDIO_CLK_EN		(0 << 11)
#define SDIO_SYSBUS_CLK_EN	(0 << 10)
#define GBE_BM_CORE_CLK_EN	(0 << 9)
#define GBE_BM_AXI_CLK_EN	(0 << 8)
#define GBE0_AXI_CLK_EN		(0 << 7)
#define GBE1_AXI_CLK_EN		(0 << 6)
#define GBE0_CORE_CLK_EN	(0 << 5)
#define GBE1_CORE_CLK_EN	(0 << 4)
#define GBE0_125_CLK_EN		(0 << 3)
#define GBE1_125_CLK_EN		(0 << 2)
#define GBE0_50_CLK_DIS_EN	(0 << 1)
#define GBE1_50_CLK_DIS_EN	(0 << 0)
/* enable all the south bridge clocks by default */
#define SB_CLK_ENABLE		(GBE0_PWR_UP_CLK_EN | GBE1_PWR_UP_CLK_EN | USB32_AXI_CLK_EN | USB32_SS_SYS_CLK_EN |\
				USB32_USB2_SYS_CLK_EN | USB2_HOST_CLK_EN | PCIE_CLKK_EN | USB32_REF_CLK_EN |\
				USB2_HOST_REF_CLK_EN | SDIO_CLK_EN | SDIO_SYSBUS_CLK_EN | GBE_BM_CORE_CLK_EN |\
				GBE_BM_AXI_CLK_EN | GBE0_AXI_CLK_EN | GBE1_AXI_CLK_EN | GBE0_CORE_CLK_EN |\
				GBE1_CORE_CLK_EN | GBE0_125_CLK_EN | GBE1_125_CLK_EN | GBE0_50_CLK_DIS_EN |\
				GBE1_50_CLK_DIS_EN)

int init_a3700_clock(void)
{
	int ret = 0;

	debug_enter();

	/* Both of north and south bridge clocks will be enabled by default.
	 * But most of the clock sources will be from XTAL by default.
	 * need to change clock sources from XTAL to PLL and enable all the clocks again.
	 */
	/* change north bridge clock source from XTAL to clock divider */
	writel(NB_PLL_CLK_SOURCES, MVEBU_NORTH_CLOCK_REGS_BASE + MVEBU_NORTH_CLOCK_SELEC_REG);
	debug("North bridge clock source is set to 0x%x\n",
	      readl(MVEBU_NORTH_CLOCK_REGS_BASE + MVEBU_NORTH_CLOCK_SELEC_REG));

	/* enable north bridge clocks */
	writel(NB_CLK_ENABLE, MVEBU_NORTH_CLOCK_REGS_BASE + MVEBU_NORTH_CLOCK_ENABLE_REG);
	debug("North bridge clocks are enabled 0x%x\n",
	      readl(MVEBU_NORTH_CLOCK_REGS_BASE + MVEBU_NORTH_CLOCK_ENABLE_REG));

	/* change south bridge clock source from XTAL to clock divider */
	writel(SB_PLL_CLK_SOURCES, MVEBU_SOUTH_CLOCK_REGS_BASE + MVEBU_SOUTH_CLOCK_SELEC_REG);
	debug("South bridge clock source is set to 0x%x\n",
	      readl(MVEBU_SOUTH_CLOCK_REGS_BASE + MVEBU_SOUTH_CLOCK_SELEC_REG));

	/* enable south bridge clocks */
	writel(SB_CLK_ENABLE, MVEBU_SOUTH_CLOCK_REGS_BASE + MVEBU_SOUTH_CLOCK_ENABLE_REG);
	debug("South bridge clocks are enabled 0x%x\n",
	      readl(MVEBU_SOUTH_CLOCK_REGS_BASE + MVEBU_SOUTH_CLOCK_ENABLE_REG));

	debug_exit();
	return ret;
}

