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

#ifndef _MVEBU_GPIO_H_
#define _MVEBU_GPIO_H_
#include <asm/arch-mvebu/gpio.h>

#define MVEBU_GPIO_NB_OUTPUT_EN_REG		(MVEBU_GPIO_NB_REG_BASE)
#define MVEBU_GPIO_NB_OUTPUT_EN_HIGH_REG	(MVEBU_GPIO_NB_REG_BASE + 0x4)
#define MVEBU_GPIO_NB_OUTPUT_SPI_EN_OFF		(28)/* This is hidden bit which is reserved in function spec */
#define MVEBU_GPIO_NB_OUTPUT_VAL_REG		(MVEBU_GPIO_NB_REG_BASE + 0x18)
#define MVEBU_GPIO_NB_SEL_REG			(MVEBU_GPIO_NB_REG_BASE + 0x30)
#define MVEBU_GPIO_TW1_GPIO_EN_OFF		(10)
#define MVEBU_GPIO_SPI_GPIO_EN_OFF		(18)

#define MVEBU_GPIO_SB_OUTPUT_EN_REG		(MVEBU_GPIO_SB_REG_BASE)
#define MVEBU_GPIO_SB_OUTPUT_VAL_REG		(MVEBU_GPIO_SB_REG_BASE + 0x18)
#define MVEBU_GPIO_PCIE_RESET_OFF		(3)
#define MVEBU_GPIO_SDIO_VOLTAGE_3_3V		(0)
#define MVEBU_GPIO_SDIO_VOLTAGE_1_8V		(1)
#define MVEBU_GPIO_SDIO_VOLTAGE_OFF		(22)
#define MVEBU_GPIO_SB_SEL_REG			(MVEBU_GPIO_SB_REG_BASE + 0x30)
#define MVEBU_GPIO_SB_SDIO_EN_OFF		(2)
#define MVEBU_GPIO_RGMII_GPIO_EN_OFF		(3)
#define MVEBU_GPIO_PCIE_EN_OFF			(4)

#define MVEBU_GPIO_MAX_PORTS	2	/* max number of ports per bank */
#define MVEBU_GPIO_BANKS	2	/* number of banks */
#define MVEBU_GPIOS_PER_PORT	32	/* number of gpios per port */

void mvebu_set_sdio(int voltage);
void mvebu_reset_pcie_dev(void);
void mvebu_init_gpio(void);

#endif /* _MVEBU_GPIO_H_ */
