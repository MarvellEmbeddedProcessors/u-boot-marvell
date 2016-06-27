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

#ifndef __ARMADA_3700_H
#define __ARMADA_3700_H

/* Version */
#define CONFIG_BOOTP_VCI_STRING		" U-boot.armv8.armada3700"

/* Platform */
/* Support GICv3 */
#undef CONFIG_GICV2
#define CONFIG_GICV3

/*
#define MV_INCLUDE_SDIO
#define MV_INCLUDE_INTEG_SATA
#define MV_INCLUDE_CLK_PWR_CNTRL

#define MV_INCLUDE_RCVR
#define MV_INCLUDE_NAND
#define MV_INCLUDE_SPI
#define MV_INCLUDE_XOR
#define MV_INCLUDE_SATA
#define MV_INCLUDE_USB
#define CONFIG_MV_ETH_NETA
#define MV_INCLUDE_GPIO
#define MV_INCLUDE_I2C
#define MV_INCLUDE_RTC
#define MV_INCLUDE_PEX
*/

/* Plaform */
#define CONFIG_MARVELL

/*
 * The counter on A3700 always fed from reference clock (XTAL)
 * However minimal CPU counter prescaler is 2, so the counter
 * frequency will be divided by 2
 */
#define COUNTER_FREQUENCY	(12500000)

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS		1

/* SPI flash for Palladium */
#ifdef CONFIG_MVEBU_SPI

#define CONFIG_MVEBU_SPI_A3700

#endif /* CONFIG_MVEBU_SPI */

/* The MAX_PAYLOAD_SIZE of A3700 should be aligned with the capability
   of the EndPoint.
 */
#define CONFIG_PCI_CHECK_EP_PAYLOAD

/* The EEPROM ST M24C64 has 32 byte page write mode and takes up to 10 msec.
 */
#define CONFIG_MVEBU_EEPROM_PAGE_WRITE_DELAY_MS	10

/*
 * Include the generic MVEBU configuration file.
 * The generic file is affected by the defines above.
 */
#include "mvebu-common.h"

/* need board_early_init_f to invoke init_mbus, before dram_init */
#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_SYS_PCI_64BIT	/* enable 64-bit PCI resources */

#endif /* __ARMADA_3700_H */
