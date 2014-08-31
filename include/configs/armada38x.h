/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
   Marvell GPL License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File in accordance with the terms and conditions of the General
   Public License Version 2, June 1991 (the "GPL License"), a copy of which is
   available along with the File in the license.txt file or by writing to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
   on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

   THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
   WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
   DISCLAIMED.  The GPL License provides additional details about this warranty
   disclaimer.
*******************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

/* Version */
#define CONFIG_IDENT_STRING	" Marvell version: 2014_T2.0"

/* Configurable boards */
#define CONFIG_MVEBU_DEVEL_BOARD      /* Turn this off for Customer boards */

/* Platform */
#define CONFIG_MVEBU_CA9
#define CONFIG_AURORA_TIMER

/* Enable IO drivers */
#define CONFIG_MVEBU_UART
#define CONFIG_MVEBU_SPI
#define CONFIG_MVEBU_PCI
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

#ifdef CONFIG_MVEBU_SPI
#define CONFIG_MVEBU_SPI_BOOT
#endif

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

/* Clock */
#ifndef __ASSEMBLY__
	#define CONFIG_SYS_HZ                   1000
	#define CONFIG_SYS_TCLK                 soc_tclk_get()
	#define CONFIG_SYS_BUS_CLK              soc_sys_clk_get()
#endif /* __ASSEMBLY__ */

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE
#define CONFIG_SYS_DRAM_BANKS           1

/* Include AFTER since it is affected by defines above */
#include "mvebu-common.h"

#endif /* __CONFIG_H */
