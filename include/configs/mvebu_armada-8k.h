/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 */

#ifndef _CONFIG_MVEBU_ARMADA_8K_H
#define _CONFIG_MVEBU_ARMADA_8K_H

#define DEFAULT_CONSOLE		"console=ttyS0,115200 "	\
	"earlycon=uart8250,mmio32,0xf0512000"

#include <configs/mvebu_armada-common.h>

/* When runtime detection fails this is the default */

#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_SYS_NAND_ONFI_DETECTION

/*
 * PCI configuration
 */
#ifdef CONFIG_PCIE_DW_MVEBU
#define CONFIG_E1000
#endif

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 0) \
	func(USB, usb, 0) \
	func(SCSI, scsi, 0) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)

#endif /* _CONFIG_MVEBU_ARMADA_8K_H */
