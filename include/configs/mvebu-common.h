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

#ifndef _MVEBU_COMMON_H_
#define _MVEBU_COMMON_H_

#include <asm/arch/regs-base.h>

/* Temp */
#define CONFIG_ADEC

/* Usefull Debug */
/* #define DEBUG */
#define CONFIG_BOOTSTAGE
#define CONFIG_BOOTSTAGE_REPORT
/* #define CONFIG_MVEBU_DEBUG_FUNC_IN_OUT */
/* #define CONFIG_MV_DEBUG_PUTS */

#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_ARMADA_8K_SOC_ID		8022

/* Global definitions */
#ifndef CONFIG_SPL_BUILD
#define CONFIG_SYS_MALLOC_F_LEN		0x5000
#endif
#ifndef CONFIG_SYS_TEXT_BASE
#ifdef CONFIG_PALLADIUM
/* Overcome issue in emulation where writes to address 0x0 might
   sometimes fail. */
#define CONFIG_SYS_TEXT_BASE            0x00001000
#else
#define CONFIG_SYS_TEXT_BASE            0x00000000
#endif
#endif

#define CONFIG_SYS_SDRAM_BASE           0x00000000
#define CONFIG_SYS_RESET_ADDRESS        0xffff0000
#define CONFIG_SYS_MALLOC_BASE          (CONFIG_SYS_TEXT_BASE + (3 << 20))  /* TEXT_BASE + 3M */
#ifdef CONFIG_PALLADIUM
#define CONFIG_SYS_MALLOC_LEN           (1 << 19)	/* Reserve 0.5MB for malloc*/
#else
#define CONFIG_SYS_MALLOC_LEN           (5 << 20)	/* Reserve 5MB for malloc*/
#endif
#define CONFIG_NR_DRAM_BANKS		(2)
#define CONFIG_MVEBU_UBOOT_DFLT_NAME	"u-boot.bin"

/* maybe need to set back to 0x7fff0 */
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_TEXT_BASE + 0xFF0000)   /* End of 16M scrubbed by training in bootrom */
#define CONFIG_SYS_GBL_DATA_SIZE        128	/* Size in bytes reserved for init data */
#define CONFIG_SYS_MAXARGS		32	/* Max number of command argg */
/* The U-Boot size should be aligned with NAND page-size,
 * the environment variable of the u-boot start at CONFIG_UBOOT_SIZE (and it
 * should be aligned with NAND page size)
 */
#define CONFIG_UBOOT_SIZE		0x180000 /* 1.5MB */
#undef  CONFIG_USE_IRQ

/* Memory reserve */
#define CONFIG_UBOOT_MAX_MEM_SIZE	(3ll << 30)	/* Limit u-boot to 3GB */
#define MVEBU_IO_RESERVE_BASE		0xC0000000ll

/* memtest definition */
#define CONFIG_SYS_MEMTEST_START        (CONFIG_SYS_SDRAM_BASE)
#define CONFIG_SYS_MEMTEST_END          (CONFIG_SYS_SDRAM_BASE + 0x10000000)

/* Board init functions */
#define CONFIG_BOARD_EARLY_INIT_F	/* pre relloc board init */
#define CONFIG_MISC_INIT_R              /* post relloc board init */
#define CONFIG_BOARD_LATE_INIT		/* late  board init */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_ENV_OVERWRITE            /* allow to change env parameters */
#undef  CONFIG_WATCHDOG                 /* watchdog disabled */
#define CONFIG_LAST_STAGE_INIT	1	    /* call last_stage_init()	*/

/* SPL */
/* Common defines for SPL */
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_DRIVERS_MISC_SUPPORT

/* Cache */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_CACHELINE_SIZE       32

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(16 << 20)	/* Initial Memory map for Linux */

/*
 * Increase maximum gunzip output size
 * with compressed kernel images.
 * Default in common/bootm.c is 8MB.
 */
#define CONFIG_SYS_BOOTM_LEN	(20 << 20)	/* max Image size */

#define CONFIG_CMDLINE_TAG              1	/* enable passing of ATAGs  */
#define CONFIG_CMDLINE_EDITING          1
#define CONFIG_INITRD_TAG               1       /* enable INITRD tag for ramdisk data */
#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_MARVELL_TAG
#define ATAG_MARVELL                    0x41000403
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_SYS_LOAD_ADDR		0x02000000      /* default load address	*/
#define CONFIG_SYS_MIN_HDR_DEL_SIZE	0x100

/* Recovery */
#ifdef MV_INCLUDE_RCVR
#define CONFIG_CMD_RCVR
#define RCVR_IP_ADDR			"169.254.100.100"
#define RCVR_LOAD_ADDR			"0x02000000"
#endif /* MV_INCLUDE_RCVR */

/* Parser */
#define CONFIG_SILENT_CONSOLE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "
#define CONFIG_SYS_LONGHELP             /* undef to save memory	*/
#define CONFIG_SYS_PROMPT               "Marvell>> "  /* Command Prompt	*/
#define CONFIG_SYS_CBSIZE               1024          /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)  /* Print Buffer Size */
#define CONFIG_LOADS_ECHO		0       /* Echo off for serial download */
#define CONFIG_SYS_CONSOLE_INFO_QUIET		/* Don't print In/Out/Err console assignment. */


/* Pre console buffer */
#ifndef CONFIG_SPL_BUILD
#define CONFIG_PRE_CONSOLE_BUFFER
#define CONFIG_PRE_CON_BUF_ADDR		(0x2000000)
#define CONFIG_PRE_CON_BUF_SZ		(1 << 20)	/* 1MB */
#endif

/* Default Env vars */
#define CONFIG_SYS_BARGSIZE             CONFIG_SYS_CBSIZE       /* Boot Argument Buffer Size */
#define CONFIG_BOOTDELAY                3
#define CONFIG_ROOTPATH                 "/srv/nfs/"             /* Default Dir for NFS */
#define CONFIG_EXTRA_ENV_SETTINGS	"kernel_addr=0x2000000\0"	\
					"initrd_addr=0xa00000\0"	\
					"initrd_size=0x2000000\0"	\
					"fdt_addr=0x1000000\0"		\
					"loadaddr=0x2000000\0"		\
					"fdt_high=0xffffffffffffffff\0"	\
					"hostname=marvell\0"		\
					"ramfs_addr=0x3000000\0"	\
					"ramfs_name=-\0"		\
					"fdt_name=fdt.dtb\0"		\
					"netdev=eth0\0"			\
					"image_name=Image\0"		\
					"get_ramfs=if test \"${ramfs_name}\" != \"-\"; then setenv ramfs_addr "	\
					"0x3000000; tftp $ramfs_addr $ramfs_name; else setenv ramfs_addr -;fi\0"\
					"get_images=tftp $kernel_addr $image_name; tftp $fdt_addr $fdt_name; "	\
						"run get_ramfs\0"						\
					"console=" CONFIG_DEFAULT_CONSOLE "\0"					\
					"root=root=/dev/nfs rw\0"						\
					"set_bootargs=setenv bootargs $console $root "				\
						"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:none "\
						"nfsroot=$serverip:$rootpath $extra_params"

#define CONFIG_BOOTCOMMAND		"run get_images; run set_bootargs; booti $kernel_addr $ramfs_addr $fdt_addr"

/* U-Boot Commands */
/* #define CONFIG_BOOTP_MASK       (CONFIG_BOOTP_DEFAULT | CONFIG_BOOTP_BOOTFILESIZE) */
#define CONFIG_CMD_BSP		/* Enable board specfic commands */
#define CONFIG_CMD_ELF
#define CONFIG_CMD_STAGE_BOOT
#ifdef CONFIG_ARM64
#define CONFIG_CMD_BOOTI
#endif

/* Memory commands / tests. */
#ifdef CONFIG_CMD_MEMTEST
#define CONFIG_SYS_ALT_MEMTEST		/* More comprehensive mtest algorithm */
#define CONFIG_SYS_MEMTEST_SCRATCH	0x10800000
#endif


#if defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#define CONFIG_ENV_IS_IN_SPI_FLASH
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_ENV_IS_IN_MMC
	#define DEFAULT_BUBT_DST "spi"
	#define CONFIG_DOIMAGE_TYPE     "flash"
	#define CONFIG_DOIMAGE_SUFFIX   "dynamic"
	#define CONFIG_SYS_MMC_ENV_DEV	0
#endif  /* CONIFG_ENV_IS_IN_BOOTDEV */


/* Flash env setup */
#if !defined(MV_INCLUDE_NOR) && !defined(CONFIG_MVEBU_NAND_BOOT) &&		\
	!defined(CONFIG_MVEBU_SPI_BOOT) && !defined(CONFIG_MVEBU_MMC_BOOT) &&	\
	!defined(CONFIG_MVEBU_SATA_BOOT) && !defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#undef CONFIG_CMD_FLASH
	#undef CONFIG_CMD_IMLS
	#define CONFIG_ENV_IS_NOWHERE
	#define CONFIG_ENV_SIZE		0x10000  /* environment takes one erase block */
	#define CONFIG_SYS_NO_FLASH
#else
	#define CONFIG_ENV_SECT_SIZE	0x10000
	#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE    /* environment takes one sector */
	#define CONFIG_ENV_OFFSET	CONFIG_UBOOT_SIZE
	#define CONFIG_ENV_ADDR		CONFIG_ENV_OFFSET

	/* TODO - Do we really need this */
	#define CONFIG_SYS_MONITOR_BASE	0
	#define CONFIG_SYS_MONITOR_LEN	0x80000  /* Reserve 512 kB for Monitor */

	#define CONFIG_MVEBU_DOIMAGE
#endif


/* Boot from NAND settings */
#if defined(CONFIG_MVEBU_NAND_BOOT) && !defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_DOIMAGE_TYPE	"nand"
	#define CONFIG_DOIMAGE_SUFFIX	"nand"
#endif /* CONFIG_MVEBU_NAND_BOOT */

/* Boot from SPI settings */
#if defined(CONFIG_MVEBU_SPI_BOOT) && !defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#define CONFIG_ENV_IS_IN_SPI_FLASH
	#define CONFIG_DOIMAGE_TYPE	"flash"
	#define CONFIG_DOIMAGE_SUFFIX	"spi"
#endif /* CONFIG_MVEBU_SPI_BOOT */

/* Boot from eMMC settings */
#if defined(CONFIG_MVEBU_MMC_BOOT) && !defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#define CONFIG_ENV_IS_IN_MMC
	#define CONFIG_SYS_MMC_ENV_DEV	0
	#define CONFIG_DOIMAGE_TYPE	"mmc"
	#define CONFIG_DOIMAGE_SUFFIX	"emmc"
#endif /* CONFIG_MVEBU_MMC_BOOT */

/* Boot from SATA settings */
#if defined(CONFIG_MVEBU_SATA_BOOT) && !defined(CONFIG_ENV_IS_IN_BOOTDEV)
	#define CONFIG_ENV_IS_IN_FAT
	#define FAT_ENV_INTERFACE	"scsi"
	#define FAT_ENV_DEVICE_AND_PART	"0:2"
	#define FAT_ENV_FILE		"uboot.env"
	#define CONFIG_FAT_WRITE
	#define CONFIG_DOIMAGE_SUFFIX	"sata"
#endif /* CONFIG_MVEBU_SATA_BOOT */

/* Generic Interrupt Controller Definitions */
#define GICD_BASE                       MVEBU_GICD_BASE
#define GICC_BASE                       MVEBU_GICC_BASE
#define GICR_BASE                       MVEBU_GICR_BASE

/* Flat Device Tree Definitions */
#define CONFIG_OF_LIBFDT

/* SMP Spin Table Definitions */
#define CPU_RELEASE_ADDR               (CONFIG_SYS_SDRAM_BASE + 0x2000000)

/* Exception level */
/* define CONFIG_ARMV8_SWITCH_TO_EL1 */

/*********  IO Definitions **********/

/* UART */
#ifdef CONFIG_SYS_NS16550
/* We keep the UART configuration in .h file, because the UART driver is basic driver for loading
 * U-Boot, if there any issue in the driver or in FDT getting values or maybe in the values
 * the U-Boot will stuck while loading it. For now we'll keep this defines here,
 * and we can later move them to Kconfig.
 */
	#define CONFIG_SYS_NS16550_SERIAL
	#define CONFIG_SYS_NS16550_MEM32
	#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
	#define CONFIG_SYS_NS16550_COM1		MVEBU_UART_BASE(0)
	#define CONFIG_SYS_NS16550_CLK		200000000
	#define CONFIG_CONS_INDEX		1
	#define CONFIG_SYS_LOADS_BAUD_CHANGE	/* allow baudrate changes */
	/*#define CONFIG_SYS_DUART_CHAN		0*/
	#define CONFIG_DEFAULT_CONSOLE		"console=ttyS0,115200"
#endif /* CONFIG_SYS_NS16550 */

#ifdef CONFIG_MVEBU_A3700_UART
	#define CONFIG_DEFAULT_CONSOLE		"console=ttyMV0,115200 earlycon=ar3700_uart,0xd0012000"
#endif

#if defined(CONFIG_SYS_NS16550) || defined(CONFIG_MVEBU_A3700_UART)
#define CONFIG_BAUDRATE		115200
#endif

/* I2C */
#ifdef CONFIG_MVEBU_I2C
	#define CONFIG_SYS_I2C
	#define CONFIG_I2C_MULTI_BUS
/*	#define CONFIG_CMD_EEPROM
	#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  1
	#define CONFIG_SYS_I2C_MULTI_EEPROMS
	#define CONFIG_SYS_I2C_SLAVE		0x0*/
#endif
#if defined(CONFIG_MVEBU_I2C) || defined(CONFIG_I2C_MV)
#define CONFIG_SYS_I2C_SPEED            100000  /* I2C speed default */
#endif

/* RTC */
#if defined(CONFIG_MVEBU_RTC)
	#define CONFIG_CMD_DATE
	#define CONFIG_SYS_NVRAM_BASE_ADDR DEVICE_CS1_BASE      /* dummy */
#endif

/* Shared USB3.0 and USB2.0 definitions*/
#if defined(CONFIG_USB)
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB
#define CONFIG_EHCI_IS_TDI
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#endif

/* USB 3.0 */
#ifdef CONFIG_USB_XHCI_HCD
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 3
#define CONFIG_USB_MAX_CONTROLLER_COUNT CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS
#endif

/* USB 2.0 */
#ifdef CONFIG_USB_EHCI_HCD
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 3
#define CONFIG_USB_MAX_CONTROLLER_COUNT CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS
#endif

/* MMC-SD */
#if MV_INCLUDE_SDIO
	#define  CONFIG_MMC
	#define  CONFIG_CMD_MMC
	#define  CONFIG_GENERIC_MMC
	#define  CONFIG_MRVL_MMC
	#define  CONFIG_SYS_MMC_BASE
	#define  CONFIG_SYS_MMC_MAX_DEVICE
#endif

/* XENON MMC */
#ifdef CONFIG_XENON_MMC
#define  CONFIG_MMC
#define  CONFIG_CMD_MMC
#define  CONFIG_GENERIC_MMC
#endif /* CONFIG_XENON_MMC */

/* PCI-E */
#if defined(CONFIG_MVEBU_PCI) || defined(CONFIG_MVEBU_DW_PCIE)
	#define CONFIG_PCI
	#define CONFIG_PCI_PNP  /* Enable plug-and-play */
	#ifdef CONFIG_MVEBU_PCI_BURST_RELAX
	#define CONFIG_PCI_FIXUP_DEV
	#endif
	/*#define CONFIG_MVEBU_PCI_EP*/ /* Set PCI host as end point */

	/* Enable PCIE NIC for devel boards */
	#ifdef CONFIG_DEVEL_BOARD
		#define CONFIG_E1000
	#endif

#endif /* CONFIG_MVEBU_PCI */

#ifdef CONFIG_MVEBU_ADVK_PCIE
	#define CONFIG_PCI
	#define CONFIG_PCI_PNP	/* Enable plug-and-play */
	#define CONFIG_PCI_SCAN_SHOW
	#define CONFIG_SYS_PCI_64BIT
	#define CONFIG_PCI_ADDR_PREFIX
	#define CONFIG_PCIE_RC_MODE

	#if defined(CONFIG_PCIE_RC_MODE) && defined(CONFIG_PCI_CHECK_EP_PAYLOAD)
	#define CONFIG_PCI_FIXUP_DEV
	#endif

	/* Enable PCIE NIC for devel boards */
	#ifdef CONFIG_DEVEL_BOARD
		#define CONFIG_E1000
	#endif
#endif /* CONFIG_MVEBU_ADVK_PCIE */

/* Add network parameters when network command is enabled */
#ifdef CONFIG_CMD_NET
	/* Environment */
	#define CONFIG_IPADDR		0.0.0.0	/* In order to cause an error */
	#define CONFIG_SERVERIP		0.0.0.0	/* In order to cause an error */
	#define CONFIG_NETMASK          255.255.255.0
	#define CONFIG_GATEWAYIP	10.4.50.254
	#define CONFIG_ETHADDR          00:00:00:00:51:81
	#define CONFIG_HAS_ETH1
	#define CONFIG_ETH1ADDR		00:00:00:00:51:82
	#define CONFIG_HAS_ETH2
	#define CONFIG_ETH2ADDR		00:00:00:00:51:83
	#define CONFIG_ETHPRIME         "egiga0"
	#define CONFIG_PHY_GIGE			/* Include GbE speed/duplex detection */

#endif /* CONFIG_CMD_NET */

/* SATA AHCI via controller or over PCIe */
#if     defined(CONFIG_MVEBU_PCI) || defined(CONFIG_MVEBU_DW_PCIE_) || \
	defined(CONFIG_MV_INCLUDE_SATA) || defined(CONFIG_SCSI_AHCI_PLAT)

	#define CONFIG_CMD_SCSI
	#define CONFIG_CMD_EXT2
	#define CONFIG_CMD_EXT4
	#define CONFIG_CMD_EXT4_WRITE
	#define CONFIG_CMD_JFFS2
	#define CONFIG_CMD_FAT

	#define CONFIG_EXT4_WRITE
	#define CONFIG_FS_FAT
	#define CONFIG_FS_EXT4
	#define CONFIG_SUPPORT_VFAT

	#undef CONFIG_MAC_PARTITION
	#define CONFIG_DOS_PARTITION
	#define CONFIG_EFI_PARTITION

	#define CONFIG_SYS_64BIT_LBA	/* Support disk over 2TB */
	#define CONFIG_LBA48

	#define __io
	#define CONFIG_LIBATA
	#define CONFIG_SCSI_AHCI
	#define CONFIG_SCSI_MAX_CONTROLLERS	2
	#define CONFIG_SYS_SCSI_MAX_SCSI_ID	(4 * CONFIG_SCSI_MAX_CONTROLLERS)
	#define CONFIG_SYS_SCSI_MAX_LUN		1
	#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)

	/* Add support for Magni 9215 and Magni 9235 */
	#define CONFIG_SCSI_DEV_LIST		{ 0x1B4B, 0x9215 }, { 0x1B4B, 0x9235 }, { 0, 0 }
#endif

/* NAND */
#if defined(CONFIG_MVEBU_NAND) || defined(CONFIG_NAND_PXA3XX)
	#define CONFIG_SYS_MAX_NAND_DEVICE 1
	#define CONFIG_SYS_NAND_MAX_CHIPS 1
	#define CONFIG_SYS_NAND_ONFI_DETECTION
	#define CONFIG_SYS_NAND_USE_FLASH_BBT
#endif

/* SPI Flash */
#ifdef CONFIG_MVEBU_SPI
	#define CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_WINBOND
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_SPI_FLASH_SPANSION
	#define CONFIG_ENV_SPI_MAX_HZ           10000000        /*Max 50Mhz- will sattle on SPI bus max 41.5Mhz */
	#define CONFIG_ENV_SPI_CS               0
	#define CONFIG_ENV_SPI_BUS              0
	#define CONFIG_SPI_FLASH_BAR

	#ifndef CONFIG_SF_DEFAULT_SPEED
	#define CONFIG_SF_DEFAULT_SPEED        1000000
	#endif

	#ifndef CONFIG_SF_DEFAULT_MODE
	#define CONFIG_SF_DEFAULT_MODE         SPI_MODE_3
	#endif


#endif  /* MV_INCLUDE_SPI */


/* NOR Flash */
#ifdef MV_INCLUDE_NOR

	#define DEVICE_CS0_BASE NOR_CS_BASE
	#define DEVICE_CS0_SIZE NOR_CS_SIZE

	#define CONFIG_SYS_MAX_FLASH_BANKS              1

	#define CONFIG_SYS_FLASH_CFI
	#define CONFIG_SYS_FLASH_PROTECTION
	#define CONFIG_FLASH_CFI_DRIVER

	#define CONFIG_SYS_MAX_FLASH_SECT               128
	#define CONFIG_SYS_FLASH_BASE                   NOR_CS_BASE
	#define CONFIG_SYS_FLASH_CFI_WIDTH              FLASH_CFI_8BIT

	#define CONFIG_FLASH_SHOW_PROGRESS              1
	#define CONFIG_SYS_FLASH_EMPTY_INFO
	#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE

	#define CONFIG_CMD_FLASH
	#undef  CONFIG_CMD_IMLS

	#if defined(MV_NOR_BOOT)
	#define CONFIG_ENV_IS_IN_FLASH
	#define CONFIG_ENV_SIZE                 0x10000
	#define CONFIG_ENV_SECT_SIZE            0x10000
	#define CONFIG_ENV_OFFSET               0x60000
	#define CONFIG_ENV_RANGE                CONFIG_ENV_SIZE * 8
	#define CONFIG_ENV_ADDR                 (NOR_CS_BASE + CONFIG_ENV_OFFSET)
	#define MONITOR_HEADER_LEN              0x200
	#define CONFIG_SYS_MONITOR_LEN          0x70000 /* 448 K */
	#define CONFIG_SYS_MONITOR_BASE		(0 + CONFIG_ENV_SECT_SIZE)
	#define CONFIG_SYS_MONITOR_END		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
	#endif /* MV_NOR_BOOT */
#else /* MV_INCLUDE_NOR */
	#define CONFIG_SYS_NO_FLASH
	#undef CONFIG_CMD_FLASH
	#undef CONFIG_CMD_IMLS
#endif


#endif /* _MVEBU_COMMON_H_ */
