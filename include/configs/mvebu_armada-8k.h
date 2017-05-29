/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_MVEBU_ARMADA_8K_H
#define _CONFIG_MVEBU_ARMADA_8K_H

#define CONFIG_DEFAULT_CONSOLE		"console=ttyS0,115200 "\
					"earlycon=uart8250,mmio32,0xf0512000"

#include <configs/mvebu_armada-common.h>

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_SYS_TCLK		250000000	/* 250MHz */

#define CONFIG_NR_DRAM_BANKS	1

#define CONFIG_BOARD_EARLY_INIT_R

#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#define CONFIG_USB_MAX_CONTROLLER_COUNT (3 + 3)

#endif /* _CONFIG_MVEBU_ARMADA_8K_H */
