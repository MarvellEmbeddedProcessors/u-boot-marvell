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

#define MVEBU_FDT_SIZE 0x1C00

/* Plaform */
#define CONFIG_MARVELL

/* Clock Definitions */
#ifdef CONFIG_PALLADIUM
#define COUNTER_FREQUENCY	(6000)
#else
/*
 * The counter on A3700 always fed from reference clock (XTAL)
 * However minimal CPU counter prescaler is 2, so the counter
 * frequency will be divided by 2
 */
#define COUNTER_FREQUENCY	(12500000)
#endif

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS		1

#ifdef CONFIG_SPL_BUILD
/* SPL */
/* Defines for SPL */
/*#define CONFIG_SPL_TEXT_BASE	0xF0000000 - CM3 SRAM, 0x01000000 - DRAM */
/* SRAM is 128K in total:      0xF0000000 - 0xF001FFFF
   SPL code occupies addresses 0xF0000000 - 0xF000FFFF
   SPL data can use addresses  0xF0010000 - 0xF0014000
   WTMI starts at address      0xF00140F0
   CM3 BootROM uses adresses   0xF0016000 - 0xF001FFFF
   CM3 mapping for SRAM is     0x1FFF0000 - 0x20010000
 */
#define CONFIG_SPL_TEXT_BASE		0x01000000

#if (CONFIG_SPL_TEXT_BASE != 0xF0000000)
#define SPL_IS_IN_DRAM
#define CONFIG_SPL_MAX_SIZE		0x00020000	/* 128K */
#else
#undef SPL_IS_IN_DRAM
#define CONFIG_SPL_MAX_SIZE		0x00010000	/* 64K */
#endif


#ifndef __ASSEMBLY__
extern char __end_of_spl[];
#endif /* __ASSEMBLY__ */
#define CONFIG_SYS_SPL_MALLOC_START	((ulong)__end_of_spl)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(0x4000)	/* 16K */
#endif /* CONFIG_SPL_BUILD */
#define CONFIG_SPL_LDSCRIPT		"arch/arm/cpu/mvebu-common/u-boot-armv8-spl.lds"

/* SPI flash for Palladium */
#ifdef CONFIG_MVEBU_SPI

#define CONFIG_MVEBU_SPI_A3700

#ifdef CONFIG_PALLADIUM
#define CONFIG_SPI_FLASH_SPANSION
#endif /* CONFIG_PALLADIUM */

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
#ifdef CONFIG_PALLADIUM
#define CONFIG_BAUDRATE                 300
#else
#define CONFIG_BAUDRATE                 4000
#endif

#undef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS			"console=ttyS0,300 earlycon=ar3700_uart,0xd0012000 swiotlb=4096"

#undef  CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS	"kernel_addr=0x107ffc0\0"	\
					"fdt_addr=0x1000000\0"		\
					"fdt_high=0xa0000000\0"
#endif /*CONFIG_PALLADIUM*/

#endif /* __ARMADA_3700_H */
