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

#ifndef __ARMADA_8K_H
#define __ARMADA_8K_H

/* Version */
#define CONFIG_BOOTP_VCI_STRING         "U-boot.armv8.armada8k"

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
#define MV_INCLUDE_PEX
*/
#define CONFIG_GZIP_COMPRESSED

#undef CONFIG_GICV3
#define CONFIG_GICV2

#define CONFIG_GENERIC_TIMER
#define CONFIG_REMAKE_ELF

/* Clock Definitions */
#ifndef CONFIG_PALLADIUM
#define COUNTER_FREQUENCY	(25 * 1000000)
#define CONFIG_MSS_FREQUENCY	(200 * 1000000)
#else
#define COUNTER_FREQUENCY	(48000)
#define CONFIG_MSS_FREQUENCY	(384000)
#endif
#if defined(CONFIG_MVEBU_RTC)
#define ERRATA_FE_3124064
#endif

#ifdef CONFIG_SPL_BUILD
/* SPL */
/* Defines for SPL */
#define CONFIG_SPL_TEXT_BASE		0xFFE1C048
#define CONFIG_SPL_MAX_SIZE		(0x27000)

#ifndef __ASSEMBLY__
extern char __end_of_spl[];
#endif /* __ASSEMBLY__ */
#define CONFIG_SYS_SPL_MALLOC_START	((ulong)__end_of_spl)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(0x4000)
#endif /* CONFIG_SPL_BUILD */
#define CONFIG_SPL_LDSCRIPT		"arch/arm/cpu/mvebu-common/u-boot-armv8-spl.lds"

/*
 * Include the generic MVEBU configuration file.
 * The generic file is affected by the defines above.
 */
#include "mvebu-common.h"

#undef CONFIG_MVEBU_UBOOT_DFLT_NAME
#define CONFIG_MVEBU_UBOOT_DFLT_NAME	"flash-image.bin"

#define CONFIG_SYS_PCI_64BIT	/* enable 64-bit PCI resources */

/*
 * SOC specific overrides
 * Add here all config values that differ
 * from the generic value in mvebu-common.h
 */

/* Emulation specific setting */
#ifdef CONFIG_PALLADIUM

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY                -1

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE                 24000

#undef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS		"console=ttyS0,24000 earlycon=uart8250,mmio32,0xf0512000 " \
				"mem=256M init=/bin/sh root=/dev/ram0 rw"

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS	"kernel_addr=0x180000\0"	\
					"fdt_addr=0x1000000\0"		\
					"ramfs_addr=0x3000000\0"	\
					"fdt_high=0xa0000000\0"		\
					"initrd_high=0xffffffffffffffff\0"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		"booti $kernel_addr $ramfs_addr $fdt_addr"

#endif /*CONFIG_PALLADIUM*/


#endif /* __ARMADA_8K_H */
