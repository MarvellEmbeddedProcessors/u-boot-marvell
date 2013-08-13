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

#include <config.h>

/*
 * Plaform
 */
/* kostaz: Are there any platform defines ??? */

/*
 * Machine
 */
#ifdef CONFIG_AVANTA_LP_FPGA
	#define CONFIG_MACH_AVANTA_LP_FPGA
#endif

#include "../../board/mv_ebu/alp/mvSysHwConfig.h"

/*
 * Version
 */
#define CONFIG_IDENT_STRING	" Marvell version: 2013_Q3.0.eng_dropv6"

/* Version number passed to kernel */
#define VER_NUM 0x11120000              /* 2011.12 */

/*
 * General
 */
#define MV88F66XX
#define MV_SEC_64K
#define MV_BOOTSIZE_512K
#define MV_LARGE_PAGE
#define MV_DDR_64BIT
#define MV_BOOTROM

#ifndef CONFIG_MACH_AVANTA_LP_FPGA
/* USB currently disabled due to instability
#define MV_USB
*/
#define MV_FS
#define CONFIG_CMD_DATE
#endif

/*
 * Debug
 */
#define MV_RT_DEBUG
#define CONFIG_SYS_INIT_SP_ADDR         0x00FF0000      /* end of 16M scrubbed by training in bootrom */
#define CONFIG_SYS_ATA_BASE_ADDR        0x20000000
#define CONFIG_SYS_ATA_REG_OFFSET       0x0000          /* Offset for normal register accesses*/
#define CONFIG_SYS_ATA_DATA_OFFSET      0x0000          /* Offset for data I/O */

/*
 * Marvell Monitor Extension
 */
#define enaMonExt()                     MV_FALSE

/*
 * Clock
 */
#ifndef __ASSEMBLY__
extern unsigned int mvSysClkGet(void);
extern unsigned int mvTclkGet(void);
	#define UBOOT_CNTR              0               /* counter to use for uboot timer */
	#define MV_TCLK_CNTR            1               /* counter to use for uboot timer */
	#define MV_REF_CLK_DEV_BIT      1000            /* Number of cycle to eanble timer */
	#define MV_REF_CLK_BIT_RATE     100000          /* Ref clock frequency */
	#define MV_REF_CLK_INPUT_GPP    6               /* Ref clock frequency input */

	#define CONFIG_SYS_HZ                   1000    /*800*/
	#define CONFIG_SYS_TCLK                 mvTclkGet()
	#define CONFIG_SYS_BUS_HZ               mvSysClkGet()
	#define CONFIG_SYS_BUS_CLK              CONFIG_SYS_BUS_HZ
	#define CONFIG_SYS_FPGA_DRAM_SIZE       _256M
#endif /* __ASSEMBLY__ */

#define CONFIG_DISPLAY_CPUINFO

/********************/
/* Page Table settings */
/********************/
#define MV_PT

#ifdef MV_PT
	#define MV_PT_BASE(cpu)  (CONFIG_SYS_MALLOC_BASE - 0x20000 - (cpu * 0x20000))
#endif /* MV_PT */

/********************/
/* AMP settings	    */
/********************/
#define CONFIG_AMP_SUPPORT		1
#ifdef CONFIG_AMP_SUPPORT
	#define MAX_AMP_GROUPS		2
#endif

/*
 * High Level Configuration Options
 */
#define CONFIG_MARVELL
#define CONFIG_API

/*
 * Commands
 */
#define CONFIG_BOOTP_MASK       (CONFIG_BOOTP_DEFAULT | CONFIG_BOOTP_BOOTFILESIZE)
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_I2C
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_BSP
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_RUN
#define CONFIG_CMD_MISC
#define CONFIG_CMD_IDE
#define CONFIG_CMD_SCSI
#define CONFIG_CMD_SAR
#define CONFIG_CMD_STAGE_BOOT
#define CONFIG_CMD_RCVR

#if defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_PCI)
	#define CONFIG_PCI
	#define CONFIG_CMD_PCI
#endif

#ifdef MV_FS
/* FS supported */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_FS_EXT4
#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_EXT4_WRITE
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_FAT
#define CONFIG_FS_FAT
#define CONFIG_SUPPORT_VFAT
#endif /* MV_FS */
/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <config_cmd_default.h>

#define	CONFIG_SYS_MAXARGS	32	/* max number of command args	*/

/*-----------------------------------------------------------------------
 * IDE/ATA/SATA stuff (Supports IDE harddisk on PCMCIA Adapter)
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card	Adapter	*/

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE    not supported	*/
#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CONFIG_SYS_IDE_MAXBUS		4				/* max. 1 IDE bus		*/
#define CONFIG_SYS_IDE_MAXDEVICE	CONFIG_SYS_IDE_MAXBUS * 8	/* max. 1 drive per IDE bus	*/

#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0000

#undef CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION
#define CONFIG_EFI_PARTITION


#define CONFIG_SYS_64BIT_LBA			/*    Support disk over 2TB        */

#define CONFIG_LBA48
//#define CONFIG_SCSI_AHCI
#ifdef  CONFIG_SCSI_AHCI
	#define CONFIG_SATA_6121
	#define CONFIG_SYS_SCSI_MAX_SCSI_ID	4
	#define CONFIG_SYS_SCSI_MAX_LUN	1
	#define CONFIG_SYS_SCSI_MAX_DEVICE 	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
#endif /* CONFIG_SCSI_AHCI */

#define CONFIG_SCSI_MV94XX
#ifdef  CONFIG_SCSI_MV94XX
	//#define CONFIG_SATA_6121
	#define CONFIG_SYS_SCSI_MAX_SCSI_ID	40 /*8 PM * 5 sata port*/
	#define CONFIG_SYS_SCSI_MAX_LUN	1
	#define CONFIG_SYS_SCSI_MAX_DEVICE 	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
#endif /* CONFIG_SCSI_AHCI */

/*
 * U-Boot
 */
#define CONFIG_SYS_MAXARGS      32      /* max number of command argg */

/* SPI Flash configuration   */
/*****************************/
#if defined(MV_INCLUDE_SPI)
	#define CONFIG_CMD_SPI
	#define CONFIG_CMD_SF
	#define CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_ENV_SPI_MAX_HZ           10000000        /*Max 50Mhz- will sattle on SPI bus max 41.5Mhz */
	#define CONFIG_ENV_SPI_CS               0
	#define CONFIG_ENV_SPI_BUS              0

#ifndef CONFIG_SF_DEFAULT_SPEED
# define CONFIG_SF_DEFAULT_SPEED        1000000
#endif
#ifndef CONFIG_SF_DEFAULT_MODE
# define CONFIG_SF_DEFAULT_MODE         SPI_MODE_3
#endif

/* Boot from SPI settings */
	#if defined(MV_SPI_BOOT)
		#define CONFIG_ENV_IS_IN_SPI_FLASH

		#if defined(MV_SEC_64K)
			#define CONFIG_ENV_SECT_SIZE            0x10000
		#elif defined(MV_SEC_128K)
			#define CONFIG_ENV_SECT_SIZE            0x20000
		#elif defined(MV_SEC_256K)
			#define CONFIG_ENV_SECT_SIZE            0x40000
		#endif

		#define CONFIG_ENV_SIZE                         CONFIG_ENV_SECT_SIZE    /* environment takes one sector */
		#define CONFIG_ENV_OFFSET                       0x100000                /* (1MB For Image) environment starts here  */
		#define CONFIG_ENV_ADDR                         CONFIG_ENV_OFFSET
		#define MONITOR_HEADER_LEN                      0x200
		#define CONFIG_SYS_MONITOR_BASE                 0
		#define CONFIG_SYS_MONITOR_LEN                  0x80000                 /*(512 << 10) Reserve 512 kB for Monitor */

		#ifndef MV_INCLUDE_NOR
			#ifdef MV_BOOTROM
				#define CONFIG_SYS_FLASH_BASE           DEVICE_SPI_BASE
				#define CONFIG_SYS_FLASH_SIZE           _16M
			#else
				#define CONFIG_SYS_FLASH_BASE           BOOTDEV_CS_BASE
				#define CONFIG_SYS_FLASH_SIZE           BOOTDEV_CS_SIZE
			#endif  /* ifdef MV_BOOTROM */
		#endif
	#endif                  //#if defined(MV_SPI_BOOT)
#endif                          //#if defined(MV_SPI)

#define CONFIG_SYS_NAND_MAX_CHIPS 1     /* pass compilation for non NAND board configuration */

/* NAND-FLASH stuff     */
/************************/
#ifdef MV_NAND
	#define MV_NAND_PIO_MODE
	#define MV_NAND_1CS_MODE
	#define MV_NAND_4BIT_MODE
	#define MTD_NAND_NFC_INIT_RESET

	#define CONFIG_SYS_MAX_NAND_DEVICE 1
	#define CONFIG_CMD_NAND
	#define CONFIG_MV_MTD_GANG_SUPPORT
	#define CONFIG_MV_MTD_MLC_NAND_SUPPORT
	#define CONFIG_SYS_64BIT_VSPRINTF
	#define CONFIG_SKIP_BAD_BLOCK
	#undef MV_NFC_DBG

/* Boot from NAND settings */
	#if defined(MV_NAND_BOOT)
		#define CONFIG_ENV_IS_IN_NAND

		#define CONFIG_ENV_SIZE                 0x10000			/* environment takes one erase block */
		#define CONFIG_ENV_OFFSET               nand_get_env_offs()     /* environment starts here  */
		#define CONFIG_ENV_ADDR                 CONFIG_ENV_OFFSET
		#define MONITOR_HEADER_LEN              0x200
		#define CONFIG_SYS_MONITOR_BASE         0
		#define CONFIG_SYS_MONITOR_LEN          0x80000           /* Reserve 512 kB for Monitor */
		#define CONFIG_ENV_RANGE                nand_get_env_range()

		#define MV_NBOOT_BASE                   0
		#define MV_NBOOT_LEN                    (4 << 10)       /* Reserved 4KB for boot strap */
	#endif /* MV_NAND_BOOT */
#endif /* MV_NAND */
/*
 * Board init
 */
/* which initialization functions to call for this board */
#define CONFIG_MISC_INIT_R              /* after relloc initialization*/
#define CONFIG_ENV_OVERWRITE            /* allow to change env parameters */
#undef  CONFIG_WATCHDOG                 /* watchdog disabled */

/*
 * Cache
 */
#define CONFIG_SYS_CACHELINE_SIZE       32

/*
 * Global definitions
 */
#define CONFIG_SYS_TEXT_BASE            0x00000000
#define CONFIG_SYS_SDRAM_BASE           0x00000000
#define CONFIG_SYS_RESET_ADDRESS        0xffff0000
#define CONFIG_SYS_MALLOC_BASE          (CONFIG_SYS_TEXT_BASE + (3 << 20))      /* TEXT_BASE + 3M */

#define CONFIG_SYS_MALLOC_LEN           (5 << 20)                               /* Reserve 5MB for malloc*/
#define CONFIG_SYS_GBL_DATA_SIZE        128                                     /* size in bytes reserved for init data */

/*
 * DRAM
 */
#define CONFIG_SYS_DRAM_BANKS           1
#define CONFIG_NR_DRAM_BANKS            2

#define CONFIG_SYS_MEMTEST_START        0x00400000
#define CONFIG_SYS_MEMTEST_END          0x007fffff

/*
 * RTC
 */
#if defined(CONFIG_CMD_DATE)
	#define CONFIG_SYS_NVRAM_SIZE  0x00                     /* dummy */
	#define CONFIG_SYS_NVRAM_BASE_ADDR DEVICE_CS1_BASE      /* dummy */
#endif

/*
 * Serial + Parser
 */
#ifdef CONFIG_AVANTA_LP_FPGA
	#define CONFIG_BAUDRATE                 57600 /* Workaround uart clock divisor issue. */ /* 115200 */
#else
	#define CONFIG_BAUDRATE                 115200
#endif
#define CONFIG_SYS_BAUDRATE_TABLE       { 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }
#define CONFIG_SYS_DUART_CHAN           0

#define CONFIG_LOADS_ECHO               0       /* echo off for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE            /* allow baudrate changes       */

#define CONFIG_SYS_CONSOLE_INFO_QUIET           /* don't print In/Out/Err console assignment. */

/*
 * Parser
 */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_AUTO_COMPLETE

#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "

#define CONFIG_SYS_LONGHELP                                                                     /* undef to save memory		*/
#define CONFIG_SYS_PROMPT               "Marvell>> "                                            /* Monitor Command Prompt	*/
#define CONFIG_SYS_CBSIZE               1024                                                    /* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)    /* Print Buffer Size */

/*
 * Ethernet
 */
#define MV_ETH_PP2
#define CONFIG_MV_ETH_PP2
#define CONFIG_NET_MULTI
#define CONFIG_IPADDR           10.4.50.120
#define CONFIG_SERVERIP         10.4.50.1
#define CONFIG_NETMASK          255.255.255.0
#define ETHADDR                 "00:00:00:00:51:81"
#define ENV_ETH_PRIME           "egiga0"

/*
 * PCI and PCIe
 */

/*
 * USB
 */
#ifdef MV_USB
	#define MV_INCLUDE_USB
	#define CONFIG_CMD_USB
	#define CONFIG_USB_STORAGE
	#define CONFIG_USB_EHCI
/*  FIX-ME : disabled CONFIG_USB_EHCI_MARVELL : break compilation
 #define CONFIG_USB_EHCI_MARVELL */
	#define CONFIG_EHCI_IS_TDI
	#define CONFIG_DOS_PARTITION
	#define CONFIG_ISO_PARTITION
	#define ENV_USB0_MODE   "host"
	#define ENV_USB1_MODE   "host"
	#define ENV_USB_ACTIVE          "0"
#else
	#undef MV_INCLUDE_USB
	#undef CONFIG_CMD_USB
	#undef CONFIG_USB_STORAGE
#endif /* MV_USB */

/*
 * SDIO and MMC
 */
#undef  CONFIG_MMC
#undef  CONFIG_CMD_MMC
#undef  CONFIG_GENERIC_MMC
#undef  CONFIG_MRVL_MMC
#undef  CONFIG_SYS_MMC_BASE
#undef  CONFIG_SYS_MMC_MAX_DEVICE

/*
 * Linux boot and other
 */
#define MV_BOOTARGS_END         ":10.4.50.254:255.255.255.0:AvantaLP:eth0:none"
#define MV_BOOTARGS_END_SWITCH  ":::AvantaLP:eth0:none"
#define RCVR_IP_ADDR            "169.254.100.100"
	#define RCVR_LOAD_ADDR          "0x02000000"

#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_SYS_LOAD_ADDR    0x02000000      /* default load address	*/
#define CONFIG_SYS_MIN_HDR_DEL_SIZE 0x100

#undef  CONFIG_BOOTARGS

/*
 * Auto boot
 */
#define CONFIG_BOOTDELAY                3
#define CONFIG_ROOTPATH                 "/srv/nfs/"             /* Default Dir for NFS */
#define CONFIG_SYS_BARGSIZE             CONFIG_SYS_CBSIZE       /* Boot Argument Buffer Size */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ    (16 << 20)      /* Initial Memory map for Linux */

#define BRIDGE_REG_BASE_BOOTM   0xfbe00000      /* this paramaters are used when booting the linux kernel */

#define CONFIG_CMDLINE_TAG              1       /* enable passing of ATAGs  */
#define CONFIG_CMDLINE_EDITING          1
#define CONFIG_INITRD_TAG               1       /* enable INITRD tag for ramdisk data */
#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_MARVELL_TAG
#define ATAG_MARVELL                    0x41000403

/*
 * I2C
 */
#if defined(CONFIG_CMD_I2C)
	#define CONFIG_I2C_MULTI_BUS
	#define CONFIG_SYS_MAX_I2C_BUS          2
	#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  1
	#define CONFIG_SYS_I2C_MULTI_EEPROMS
	#define CONFIG_SYS_I2C_SPEED            100000  /* I2C speed default */
#endif

/*
 * PCI
 */
#ifdef CONFIG_PCI
	#define CONFIG_PCI_HOST PCI_HOST_FORCE  /* select pci host function     */
	#define CONFIG_PCI_PNP                  /* do pci plug-and-play         */

/* PnP PCI Network cards */
	#define CONFIG_EEPRO100 /* Support for Intel 82557/82559/82559ER chips */
	#define CONFIG_E1000
	#define CONFIG_SK98
	#define YUK_ETHADDR                     "00:00:00:EE:51:81"
#endif

#define PCI_HOST_ADAPTER 0              /* configure ar pci adapter     */
#define PCI_HOST_FORCE   1              /* configure as pci host        */
#define PCI_HOST_AUTO    2              /* detected via arbiter enable  */


#define CONFIG_UBOOT_SIZE                       0xE0000
/*
 * NOR Flash
 */
#if defined(MV_INCLUDE_NOR)
	#define CONFIG_SYS_MAX_FLASH_BANKS              1

	#define CONFIG_SYS_FLASH_CFI
	#define CONFIG_SYS_FLASH_PROTECTION
	#define CONFIG_FLASH_CFI_DRIVER

	#ifdef CONFIG_MACH_AVANTA_LP_FPGA
		#define CONFIG_FLASH_CFI_LEGACY
		#define CONFIG_SYS_FLASH_LEGACY_512Kx8
	#endif

	#define CONFIG_SYS_MAX_FLASH_SECT               128
	#define CONFIG_SYS_FLASH_BASE                   NOR_CS_BASE
	#define CONFIG_SYS_FLASH_CFI_WIDTH              FLASH_CFI_8BIT

	#define CONFIG_FLASH_SHOW_PROGRESS              1
	#define CONFIG_SYS_FLASH_EMPTY_INFO
	#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
		#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
	#endif

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
		#define CONFIG_SYS_MONITOR_BASE (0 + CONFIG_ENV_SECT_SIZE)
		#define CONFIG_SYS_MONITOR_END  (CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
	#endif /* MV_NOR_BOOT */
#else
	#define CONFIG_SYS_NO_FLASH
	#undef CONFIG_CMD_FLASH
	#undef CONFIG_CMD_IMLS
#endif /* MV_INCLUDE_NOR */

/*
 * Other
 */
#define MV_DFL_REGS             0xf1000000      /* boot time MV_REGS */
#define MV_REGS                 INTER_REGS_BASE /* MV Registers will be mapped here */
#define BOARD_LATE_INIT
#define CONFIG_BOARD_LATE_INIT
#undef  CONFIG_USE_IRQ

#define CONFIG_STACKSIZE        (1 << 20)       /* regular stack - up to 4M (in case of exception)*/

#endif /* __CONFIG_H */
