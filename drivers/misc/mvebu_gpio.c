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

/*
 * mvebu_a3700_gpio
 *
 * This function sets GPIO configuration for Armada3700
 * by hardcoded value.
 * And this routine (patch) will be removed (revert),
 * and implemented by MPP/PIN-CTRL driver later.
 *
 */
void mvebu_a3700_gpio(void)
{
	u32 reg_val;

	/*
	  * NETA GPIO
	  * on Palladium, RGMII is used
	  */
	reg_val = readl(MVEBU_A3700_GPIO_SB_SEL);
	reg_val = reg_val & (~(1 << MVEBU_A3700_GPIO_RGMII_GPIO_EN_OFF));
	writel(reg_val, MVEBU_A3700_GPIO_SB_SEL);

	/*
	  * I2C GPIO
	  */
	reg_val = readl(MVEBU_A3700_GPIO_NB_SEL);
	/* enable GPIO for I2C */
	reg_val = reg_val & (~(1 << MVEBU_A3700_GPIO_TW1_GPIO_EN_OFF));
	writel(reg_val, MVEBU_A3700_GPIO_NB_SEL);

	return;
}

