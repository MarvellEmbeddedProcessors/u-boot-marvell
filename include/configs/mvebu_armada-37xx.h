/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_MVEBU_ARMADA_37XX_H
#define _CONFIG_MVEBU_ARMADA_37XX_H

#define CONFIG_DEFAULT_CONSOLE		"console=ttyMV0,115200 "\
					"earlycon=ar3700_uart,0xd0012000"

/* USB 2.0 */
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 1

/* USB 3.0 */
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 2

#include <configs/mvebu_armada-common.h>

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_NR_DRAM_BANKS	4

/*
 * I2C
 */
#define CONFIG_I2C_MV
#define CONFIG_SYS_I2C_SLAVE		0x0

/* Environment in MMC */
#define CONFIG_SYS_MMC_ENV_DEV		1 /* 0 means sdio, 1 means mmc. */

/*
 * Ethernet Driver configuration
 */
#define CONFIG_MVNETA		/* Enable Marvell Gbe Controller Driver */
#define CONFIG_PHY_MARVELL


/*
 * PCI configuration
 */
#ifdef CONFIG_PCIE_ADVK
#define CONFIG_E1000
#define CONFIG_CMD_PCI
#endif

#endif /* _CONFIG_MVEBU_ARMADA_37XX_H */
