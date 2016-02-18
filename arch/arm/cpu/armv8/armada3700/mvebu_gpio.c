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
#include <asm/arch/gpio.h>

/*
 * reset PCIe device
 */
void mvebu_reset_pcie_dev(void)
{
	u32 reg_val;

	/* Set PCIe reset to 0 */
	reg_val = readl(MVEBU_GPIO_SB_OUTPUT_VAL_REG);
	reg_val &= ~(1 << MVEBU_GPIO_PCIE_RESET_OFF);
	writel(reg_val, MVEBU_GPIO_SB_OUTPUT_VAL_REG);

	/* Set to GPIO output mode */
	reg_val = readl(MVEBU_GPIO_SB_OUTPUT_EN_REG);
	reg_val |= (1 << MVEBU_GPIO_PCIE_RESET_OFF);
	writel(reg_val, MVEBU_GPIO_SB_OUTPUT_EN_REG);

	/* Set PCIe to GPIO mode */
	reg_val = readl(MVEBU_GPIO_SB_SEL_REG);
	reg_val |= (1 << MVEBU_GPIO_PCIE_EN_OFF);
	writel(reg_val, MVEBU_GPIO_SB_SEL_REG);

	/* typical delay for NIC to finish reset from NIC specification */
	udelay(100);

	/* Release PCIe reset */
	reg_val = readl(MVEBU_GPIO_SB_OUTPUT_VAL_REG);
	reg_val |= (1 << MVEBU_GPIO_PCIE_RESET_OFF);
	writel(reg_val, MVEBU_GPIO_SB_OUTPUT_VAL_REG);
}

/*
 * initialize SB SDIO device
 */
void mvebu_init_sdio(void)
{
	u32 reg_val;

	/* Set test pin to SDIO mode */
	reg_val = readl(MVEBU_GPIO_SB_SEL_REG);
	reg_val &= ~(1 << MVEBU_GPIO_SB_SDIO_EN_OFF);
	writel(reg_val, MVEBU_GPIO_SB_SEL_REG);
}

/*
 * mvebu_init_gpio
 *
 * This function sets GPIO configuration for Armada3700
 * by hardcoded value.
 * And this routine (patch) will be removed (revert),
 * and implemented by MPP/PIN-CTRL driver later.
 *
 */
void mvebu_init_gpio(void)
{
	u32 reg_val;

	/*
	 * NETA GPIO
	 * on Palladium, RGMII is used
	 */
	reg_val = readl(MVEBU_GPIO_SB_SEL_REG);
	reg_val = reg_val & (~(1 << MVEBU_GPIO_RGMII_GPIO_EN_OFF));
	writel(reg_val, MVEBU_GPIO_SB_SEL_REG);

	/*
	 * I2C, SPI GPIO
	 */
	reg_val = readl(MVEBU_GPIO_NB_SEL_REG);
	/* enable GPIO for I2C */
	reg_val = reg_val & (~(1 << MVEBU_GPIO_TW1_GPIO_EN_OFF));
	/* enable GPIO for SPI
	 * In A3700 Register Spec, it says that In North bridge GPIO configuration,
	 * bit 18 is for SPI quad mode, but this is not accurate description.
	 * In fact, bit 18 controls HOLD and WP pins for SPI, which is needed for all
	 * SPI mode, single, dual, and quad.
	 */
	reg_val = reg_val & (~(1 << MVEBU_GPIO_SPI_GPIO_EN_OFF));
	writel(reg_val, MVEBU_GPIO_NB_SEL_REG);

	/* set hiden GPIO setting for SPI
	 * in north_bridge_test_pin_out_en register 13804,
	 * bit 28 is the one which enables CS, CLK pin to be
	 * output, need to set it to 1.
	 * normally, it is needed only in UART boot mode,
	 * but after trying all other modes, it is OK to set it.
	 * later, we could read the SAR register, and do not
	 * set it in other boot mode.
	 */
	reg_val = readl(MVEBU_GPIO_NB_OUTPUT_EN_HIGH_REG);
	reg_val = reg_val | (1 << MVEBU_GPIO_NB_OUTPUT_SPI_EN_OFF);
	writel(reg_val, MVEBU_GPIO_NB_OUTPUT_EN_HIGH_REG);

	/* Set test pin to SDIO mode */
	mvebu_init_sdio();

	return;
}
