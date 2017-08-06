/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_MVEBU_ARMADA_37XX_H
#define _CONFIG_MVEBU_ARMADA_37XX_H

#define CONFIG_DEFAULT_CONSOLE		"console=ttyMV0,115200 "\
					"earlycon=ar3700_uart,0xd0012000"

#include <configs/mvebu_armada-common.h>

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_NR_DRAM_BANKS	1

/*
 * I2C
 */
#define CONFIG_I2C_MV
#define CONFIG_SYS_I2C_SLAVE		0x0

/*
 * Ethernet Driver configuration
 */
#define CONFIG_PHY_MARVELL

#define CONFIG_USB_MAX_CONTROLLER_COUNT (1 + 2)

#endif /* _CONFIG_MVEBU_ARMADA_37XX_H */
