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

#ifndef __CONFIG_H
#define __CONFIG_H

/* Platform */
#define CONFIG_MVEBU_CA9
#define CONFIG_MVEBU_TIMER

/*
#define MV_INCLUDE_SDIO
#define MV_INCLUDE_INTEG_SATA
#define MV_INCLUDE_CLK_PWR_CNTRL

#define MV_INCLUDE_RCVR
#define MV_INCLUDE_NAND
#define MV_INCLUDE_XOR
#define MV_INCLUDE_SATA
#define MV_INCLUDE_USB
#define CONFIG_MV_ETH_NETA
#define MV_INCLUDE_GPIO
#define MV_INCLUDE_I2C
#define MV_INCLUDE_RTC
#define MV_INCLUDE_PEX
*/

/* For Integration only */
#define DB_784MP_GP
#define MV_CPU_LE
#define MV88F78X60
#define MV88F672X
#define MV_SEC_64K
#define MV_BOOTSIZE_512K
#define MV_LARGE_PAGE
#define MV_DDR_64BIT
#define MV_BOOTROM

/* Plaform */
#define CONFIG_MARVELL

/* FDT Support */
/*
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1
*/

/* Define NAND chip parameters */
#if defined(CONFIG_MVEBU_NAND_BOOT) || defined(CONFIG_ENV_IS_IN_BOOTDEV)
#define CONFIG_MVEBU_NAND_PAGE_SIZE	4096
#endif

/* Clock */
#ifndef __ASSEMBLY__
	#define CONFIG_SYS_HZ                   1000
	#define CONFIG_SYS_TCLK                 soc_tclk_get()
	#define CONFIG_SYS_BUS_CLK              soc_sys_clk_get()
#endif /* __ASSEMBLY__ */

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS           1

/* SPL */
#ifdef CONFIG_SPL_BUILD
/* Defines for SPL */
#define CONFIG_SPL_TEXT_BASE		0x40000040
#define CONFIG_SPL_MAX_SIZE		(0x1ffc0)

#ifndef __ASSEMBLY__
extern char __end_of_spl[];
#endif /* __ASSEMBLY__ */
#define CONFIG_SYS_SPL_MALLOC_START	((ulong)__end_of_spl)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(0x4000)
#endif /* CONFIG_SPL_BUILD */
#define CONFIG_SPL_LDSCRIPT		"arch/arm/cpu/mvebu-common/u-boot-armv7-spl.lds"

/* Include AFTER since it is affected by defines above */
#include "mvebu-common.h"

#endif /* __CONFIG_H */
