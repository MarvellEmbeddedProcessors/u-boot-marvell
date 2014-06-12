/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#ifndef __ARMADA_8K_H
#define __ARMADA_8K_H

/* Version */
#define CONFIG_IDENT_STRING             " armada8021-pxp"
#define CONFIG_BOOTP_VCI_STRING         "U-boot.armv8.armada8021-pxp"

/* Enable IO drivers */
#define MV_INCLUDE_UART
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

#define CONFIG_AURORA_2_TIMER
#define CONFIG_REMAKE_ELF
#define CONFIG_SYS_ICACHE_OFF
/* Generic Timer Definitions */
#define COUNTER_FREQUENCY               (1008000)     /* 24MHz */

#include "mvebu-common.h"

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY                -1

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE                 63000

/* Initial environment variables */
#define CONFIG_EXTRA_ENV_SETTINGS	\
					"kernel_addr=0x200000\0"	\
					"initrd_addr=0xa00000\0"	\
					"initrd_size=0x2000000\0"	\
					"fdt_addr=0x100000\0"		\
					"fdt_high=0xa0000000\0"

#define CONFIG_BOOTARGS			"console=ttyAMA0 root=/dev/ram0"
#define CONFIG_BOOTCOMMAND		"bootm $kernel_addr " \
					"$initrd_addr:$initrd_size $fdt_addr"

#endif /* __ARMADA_8K_H */
