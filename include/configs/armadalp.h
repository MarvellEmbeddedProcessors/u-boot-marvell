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
#define CONFIG_IDENT_STRING		" Marvell version: 2015_T2.0"
#define CONFIG_BOOTP_VCI_STRING		" U-boot.armv7.armadalp"

/* Platform */
#define CONFIG_MVEBU_CA9
#define CONFIG_MVEBU_TIMER

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

/* Clock */
#ifndef __ASSEMBLY__
	#define CONFIG_SYS_HZ                   1000
	#define CONFIG_SYS_TCLK                 soc_tclk_get()
	#define CONFIG_SYS_BUS_CLK              soc_sys_clk_get()
#endif /* __ASSEMBLY__ */

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS           1

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
