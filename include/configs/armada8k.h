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
#define CONFIG_IDENT_STRING             " Armada8k"
#define CONFIG_BOOTP_VCI_STRING         "U-boot.armv8.armada8k"

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

/* Clock Definitions */
#define COUNTER_FREQUENCY               (1008000)     /* 24MHz */

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
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY                -1

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE                 63000

#undef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS			"console=ttyS0,63000 earlyprintk root=/dev/ram0"

#endif /* __ARMADA_8K_H */
