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

#include "mvebu-common.h"

#undef CONFIG_SYS_INIT_SP_ADDR
#undef CONFIG_SYS_MALLOC_LEN
#undef CONFIG_SYS_LOAD_ADDR
#undef CONFIG_SYS_SDRAM_BASE
#undef CONFIG_ENV_IS_NOWHERE

#define CONFIG_AURORA_2_TIMER

#define DEBUG

#define CONFIG_REMAKE_ELF

/*#define CONFIG_ARMV8_SWITCH_TO_EL1*/

/*#define CONFIG_SYS_GENERIC_BOARD*/

/* Has ADEC address decode unit */
#define CONFIG_ADEC

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SUPPORT_RAW_INITRD

/* Cache Definitions */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_ICACHE_OFF

#define CONFIG_IDENT_STRING		" armada8021-pxp"
#define CONFIG_BOOTP_VCI_STRING		"U-boot.armv8.armada8021-pxp"

/* Link Definitions */
#define CONFIG_SYS_TEXT_BASE		0x80000000
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_SDRAM_BASE + 0x7fff0)

/* Flat Device Tree Definitions */
#define CONFIG_OF_LIBFDT

#define CONFIG_DEFAULT_DEVICE_TREE	vexpress64

/* SMP Spin Table Definitions */
#define CPU_RELEASE_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x7fff0)

/* CS register bases for the original memory map. */
#define V2M_PA_CS0			0x00000000
#define V2M_PA_CS1			0x14000000
#define V2M_PA_CS2			0x18000000
#define V2M_PA_CS3			0x1c000000
#define V2M_PA_CS4			0x0c000000
#define V2M_PA_CS5			0x10000000

#define V2M_PERIPH_OFFSET(x)		(x << 16)
#define V2M_SYSREGS			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(1))
#define V2M_SYSCTL			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(2))
#define V2M_SERIAL_BUS_PCI		(V2M_PA_CS3 + V2M_PERIPH_OFFSET(3))

#define V2M_BASE			0x80000000

/*
 * Physical addresses, offset from V2M_PA_CS0-3
 */
#define V2M_NOR0			(V2M_PA_CS0)
#define V2M_NOR1			(V2M_PA_CS4)
#define V2M_SRAM			(V2M_PA_CS1)

/* Common peripherals relative to CS7. */
#define V2M_AACI			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(4))
#define V2M_MMCI			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(5))
#define V2M_KMI0			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(6))
#define V2M_KMI1			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(7))

#define V2M_UART0			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(9))
#define V2M_UART1			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(10))
#define V2M_UART2			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(11))
#define V2M_UART3			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(12))

#define V2M_WDT				(V2M_PA_CS3 + V2M_PERIPH_OFFSET(15))

#define V2M_TIMER01			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(17))
#define V2M_TIMER23			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(18))

#define V2M_SERIAL_BUS_DVI		(V2M_PA_CS3 + V2M_PERIPH_OFFSET(22))
#define V2M_RTC				(V2M_PA_CS3 + V2M_PERIPH_OFFSET(23))

#define V2M_CF				(V2M_PA_CS3 + V2M_PERIPH_OFFSET(26))

#define V2M_CLCD			(V2M_PA_CS3 + V2M_PERIPH_OFFSET(31))

/* System register offsets. */
#define V2M_SYS_CFGDATA			(V2M_SYSREGS + 0x0a0)
#define V2M_SYS_CFGCTRL			(V2M_SYSREGS + 0x0a4)
#define V2M_SYS_CFGSTAT			(V2M_SYSREGS + 0x0a8)

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY		(0x1800000)	/* 24MHz */

/* Generic Interrupt Controller Definitions */
#define GICD_BASE			(0x2C001000)
#define GICC_BASE			(0x2C002000)

#define CONFIG_SYS_MEMTEST_START	V2M_BASE
#define CONFIG_SYS_MEMTEST_END		(V2M_BASE + 0x80000000)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128 * 1024)

/* SMSC91C111 Ethernet Configuration */
/*#define CONFIG_SMC91111			1
#define CONFIG_SMC91111_BASE		(0x01A000000)*/

/* PL011 Serial Configuration */
#define CONFIG_PL011_SERIAL
#define CONFIG_PL011_CLOCK		24000000
#define CONFIG_PL01x_PORTS		{(void *)CONFIG_SYS_SERIAL0, \
					 (void *)CONFIG_SYS_SERIAL1}
#define CONFIG_CONS_INDEX		0

#define CONFIG_BAUDRATE			38400
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_SERIAL0		V2M_UART0
#define CONFIG_SYS_SERIAL1		V2M_UART1

/* Command line configuration */
#define CONFIG_MENU
/*#define CONFIG_MENU_SHOW*/
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_BDI
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PXE
#define CONFIG_CMD_ENV
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_IMI
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_RUN
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_SOURCE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_PXE
#define CONFIG_BOOTP_PXE_CLIENTARCH	0x100

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR		(V2M_BASE + 0x10000000)

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			(V2M_BASE)	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE		0x80000000	/* 2048 MB */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

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

/* Do not preserve environment */
#define CONFIG_ENV_IS_NOWHERE		1

/* Monitor Command Prompt */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING		1

#endif /* __ARMADA_8K_H */
