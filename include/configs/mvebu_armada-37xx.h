/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 */

#ifndef _CONFIG_MVEBU_ARMADA_37XX_H
#define _CONFIG_MVEBU_ARMADA_37XX_H

#define DEFAULT_CONSOLE		"console=ttyMV0,115200 "	\
	"earlycon=ar3700_uart,0xd0012000"

#include <configs/mvebu_armada-common.h>

/*
 * I2C
 */
#define CONFIG_I2C_MV
#define CONFIG_SYS_I2C_SLAVE		0x0

#define CONFIG_MTD_PARTITIONS		/* required for UBI partition support */

/*
 * Ethernet Driver configuration
 */
#define CONFIG_ARP_TIMEOUT	200
#define CONFIG_NET_RETRY_COUNT	50

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 0) \
	func(USB, usb, 0) \
	func(SCSI, scsi, 0) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

/* fdt_addr and kernel_addr are needed for existing distribution boot scripts */
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"scriptaddr=0x6d00000\0"	\
	"pxefile_addr_r=0x6e00000\0"	\
	"fdt_addr=0x6f00000\0"		\
	"fdt_addr_r=0x6f00000\0"	\
	"kernel_addr=0x7000000\0"	\
	"kernel_addr_r=0x7000000\0"	\
	"ramdisk_addr_r=0xa000000\0"	\
	BOOTENV

#endif /* _CONFIG_MVEBU_ARMADA_37XX_H */
