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

#ifndef __ARMADA_LP_H
#define __ARMADA_LP_H

/* Version */
#define CONFIG_BOOTP_VCI_STRING		" U-boot.armv8.armadalp"

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

#define COUNTER_FREQUENCY	(6000)


/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS           1

#ifdef CONFIG_SPL_BUILD
/* SPL */
/* Defines for SPL */
#define CONFIG_SPL_TEXT_BASE	 0xFFE1C048
#define CONFIG_SPL_MAX_SIZE		(0x1ffc0)

#ifndef __ASSEMBLY__
extern char __end_of_spl[];
#endif /* __ASSEMBLY__ */
#define CONFIG_SYS_SPL_MALLOC_START	((ulong)__end_of_spl)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(0x4000)
#endif /* CONFIG_SPL_BUILD */
#define CONFIG_SPL_LDSCRIPT		"arch/arm/cpu/mvebu-common/u-boot-armv8-spl.lds"


/* SPI flash for Palladium */
#ifdef CONFIG_MVEBU_SPI

#define CONFIG_MVEBU_SPI_ARLP

#ifdef CONFIG_PALLADIUM
#define CONFIG_SPI_FLASH_SPANSION
#endif /* CONFIG_PALLADIUM */

#endif /* CONFIG_MVEBU_SPI */

/*
 * Include the generic MVEBU configuration file.
 * The generic file is affected by the defines above.
 */
#include "mvebu-common.h"



/*
 * SOC specific overrides
 * Add here all config values that differ
 * from the generic value in mvebu-common.h
 */
#ifdef CONFIG_PALLADIUM

/* Disable I-CACHE for palladium */
#define CONFIG_SYS_ICACHE_OFF

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY                -1

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE                 4000

#undef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS			"console=ttyS0,5652 earlycon=uart8250,mmio32,0xf0512000 mem=128M"

#undef  CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS	"kernel_addr=0x107ffc0\0"	\
					"fdt_addr=0x1000000\0"		\
					"fdt_high=0xa0000000\0"
#endif /*CONFIG_PALLADIUM*/

#endif /* __ARMADA_LP_H */
