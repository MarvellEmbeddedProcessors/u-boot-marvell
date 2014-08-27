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

#ifndef _MVEBU_COMMON_H_
#define _MVEBU_COMMON_H_

#include <asm/arch/regs-base.h>
#include <config_cmd_default.h>

/* High Level Configuration Options */
#define CONFIG_MVEBU	/* an mvebu generic machine */

/* Temp */
#define CONFIG_ADEC

/* Usefull Debug */
/* #define DEBUG */
#define CONFIG_BOOTSTAGE
#define CONFIG_BOOTSTAGE_REPORT
#define CONFIG_MVEBU_DEBUG_FUNC_IN_OUT
/* #define CONFIG_MV_DEBUG_PUTS */

/* Global definitions */
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE            0x00000000
#endif
#define CONFIG_SYS_SDRAM_BASE           0x00000000
#define CONFIG_SYS_RESET_ADDRESS        0xffff0000
#define CONFIG_SYS_MALLOC_BASE          (CONFIG_SYS_TEXT_BASE + (3 << 20))  /* TEXT_BASE + 3M */
#define CONFIG_SYS_MALLOC_LEN           (1 << 20)    /* Reserve 1MB for malloc*/
#define CONFIG_NR_DRAM_BANKS		(4)
/* maybe need to set back to 0x7fff0 */
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_TEXT_BASE + 0xFF0000)   /* End of 16M scrubbed by training in bootrom */
#define CONFIG_SYS_GBL_DATA_SIZE        128          /* Size in bytes reserved for init data */
#define CONFIG_SYS_MAXARGS		32      /* Max number of command argg */
#define CONFIG_UBOOT_SIZE		0x100000
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

/* Cache */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_CACHELINE_SIZE       32

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(16 << 20)	/* Initial Memory map for Linux */
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
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "
#define CONFIG_SYS_LONGHELP             /* undef to save memory	*/
#define CONFIG_SYS_PROMPT               "Marvell>> "  /* Command Prompt	*/
#define CONFIG_SYS_CBSIZE               1024          /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)  /* Print Buffer Size */
#define CONFIG_LOADS_ECHO		0       /* Echo off for serial download */
#define CONFIG_SYS_CONSOLE_INFO_QUIET		/* Don't print In/Out/Err console assignment. */


/* Default Env vars */
#define CONFIG_SYS_BARGSIZE             CONFIG_SYS_CBSIZE       /* Boot Argument Buffer Size */
#define CONFIG_BOOTDELAY                3
#define CONFIG_ROOTPATH                 "/srv/nfs/"             /* Default Dir for NFS */
#define CONFIG_EXTRA_ENV_SETTINGS	"kernel_addr=0x200000\0"	\
					"initrd_addr=0xa00000\0"	\
					"initrd_size=0x2000000\0"	\
					"fdt_addr=0x100000\0"		\
					"fdt_high=0xa0000000\0"

#define CONFIG_BOOTARGS			"console=ttyS0,115200 earlyprintk root=/dev/ram0"
#define CONFIG_BOOTCOMMAND		"bootm $kernel_addr - $fdt_addr"

/* Marvell specific env*/
#define MV_BOOTARGS_END			":10.4.50.254:255.255.255.0:Marvell:eth0:none"
#define MV_BOOTARGS_END_SWITCH		":::Marvell:eth0:none"

/* U-Boot Commands */
/* #define CONFIG_BOOTP_MASK       (CONFIG_BOOTP_DEFAULT | CONFIG_BOOTP_BOOTFILESIZE) */
#define CONFIG_CMD_BSP		/* Enable board specfic commands */
#define CONFIG_CMD_ELF
#define CONFIG_CMD_STAGE_BOOT

/* MVEBU Generic commands */
#define CONFIG_CMD_MVEBU_MAP
#define CONFIG_CMD_MVEBU_MPP
#define CONFIG_CMD_MVEBU_IR


/* No flash setup */
#if !defined(MV_INCLUDE_NOR) && !defined(MV_INCLUDE_NAND) && !defined(MV_SPI_BOOT)
	#undef CONFIG_CMD_FLASH
	#undef CONFIG_CMD_IMLS
	#define CONFIG_ENV_IS_NOWHERE
	#define CONFIG_ENV_SIZE		0x10000  /* environment takes one erase block */
	#define CONFIG_SYS_NO_FLASH
#endif

/* Generic Interrupt Controller Definitions */
#define GICD_BASE                       MVEBU_GICD_BASE
#define GICC_BASE                       MVEBU_GICC_BASE

/* Flat Device Tree Definitions */
#define CONFIG_OF_LIBFDT

/* SMP Spin Table Definitions */
#define CPU_RELEASE_ADDR               (CONFIG_SYS_SDRAM_BASE + 0x7fff0)

/* Exception level */
/* define CONFIG_ARMV8_SWITCH_TO_EL1 */

/*********  IO Definitions **********/

/* UART */
#ifdef CONFIG_MVEBU_UART
	#define CONFIG_SYS_NS16550
	#define CONFIG_SYS_NS16550_SERIAL
	#define CONFIG_SYS_NS16550_MEM32
	#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
	#define CONFIG_SYS_NS16550_COM1		MVEBU_UART_BASE(0)
	#define CONFIG_SYS_NS16550_CLK		200000000
	#define CONFIG_CONS_INDEX		1
	#define CONFIG_BAUDRATE			115200
	#define CONFIG_SYS_LOADS_BAUD_CHANGE	/* allow baudrate changes */
	/*#define CONFIG_SYS_DUART_CHAN		0*/
#endif /* CONFIG_MVEBU_UART */


/* GPIO */
#ifdef MV_INCLUDE_GPIO
	#define CONFIG_KIRKWOOD_GPIO
#endif /* MV_INCLUDE_GPIO */

/* I2C */
#if defined(MV_INCLUDE_I2C)
	#ifdef CONFIG_MVEBU_DEVEL_BOARD
		#define CONFIG_CMD_MVEBU_SAR
		#define CONFIG_CMD_MVEBU_CONFIG
	#endif
	#define CONFIG_CMD_EEPROM
	#define CONFIG_CMD_I2C
	#define CONFIG_I2C_MVEBU
	#define CONFIG_I2C_MULTI_BUS
	#define CONFIG_SYS_MAX_I2C_BUS          2
	#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  1
	#define CONFIG_SYS_I2C_MULTI_EEPROMS
	#define CONFIG_SYS_I2C_SLAVE		0x0
	#define CONFIG_SYS_I2C_SPEED            100000  /* I2C speed default */
#endif

/* RTC */
#if defined(MV_INCLUDE_RTC)
	#define CONFIG_CMD_DATE
	#define CONFIG_RTC_MVEBU
	#define CONFIG_SYS_NVRAM_SIZE  0x00                     /* dummy */
	#define CONFIG_SYS_NVRAM_BASE_ADDR DEVICE_CS1_BASE      /* dummy */
#endif

/* USB */
#ifdef MV_INCLUDE_USB
	#define CONFIG_CMD_USB
	#define CONFIG_USB_STORAGE
	#define CONFIG_USB_EHCI
	#define CONFIG_USB_EHCI_MARVELL
	#define CONFIG_EHCI_IS_TDI
	#define CONFIG_DOS_PARTITION
	#define CONFIG_ISO_PARTITION
	#define ENV_USB0_MODE   "host"
	#define ENV_USB1_MODE   "host"
	#define ENV_USB2_MODE   "device"
	#define ENV_USB_ACTIVE  "0"
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

/* PCI-E */
#ifdef CONFIG_MVEBU_PCI
	#define CONFIG_PCI
	#define CONFIG_CMD_PCI
	#define CONFIG_PCI_HOST PCI_HOST_FORCE  /* select pci host function     */
	#define CONFIG_PCI_PNP                  /* do pci plug-and-play         */

	/* PnP PCI Network cards */
	#define CONFIG_EEPRO100 /* Support for Intel 82557/82559/82559ER chips */
	#define CONFIG_E1000
	/*#define CONFIG_SK98
	#define YUK_ETHADDR                     "00:00:00:EE:51:81"*/

	#undef MV_MEM_OVER_PEX_WA

	#define PCI_HOST_ADAPTER 0      /* configure ar pci adapter     */
	#define PCI_HOST_FORCE   1      /* configure as pci host        */
	#define PCI_HOST_AUTO    2      /* detected via arbiter enable  */

#endif /* CONFIG_MVEBU_PCI */


/* Network */
#ifdef CONFIG_MV_ETH_NETA
	#define CONFIG_CMD_NET
	#define CONFIG_CMD_DHCP
	#define CONFIG_CMD_PING
	#define MV_INCLUDE_GIG_ETH
	#define CONFIG_MV_INCLUDE_GIG_ETH
	#define MV_ETH_NETA
	#define CONFIG_NET_MULTI
	#define CONFIG_IPADDR           10.4.50.154
	#define CONFIG_SERVERIP         10.4.50.3
	#define CONFIG_NETMASK          255.255.255.0
	#define ETHADDR                 "00:00:00:00:51:81"
	#define ENV_ETH_PRIME           "egiga0"
#else
	#undef CONFIG_CMD_NET
#endif /* CONFIG_MV_ETH_NETA */

/* IDE / SATA */
#ifdef MV_INCLUDE_SATA
	#define __io

	#define CONFIG_CMD_SCSI
	#define CONFIG_CMD_EXT2
	#define CONFIG_CMD_EXT4
	#define CONFIG_FS_EXT4
	#define CONFIG_CMD_EXT4_WRITE
	#define CONFIG_EXT4_WRITE
	#define CONFIG_CMD_JFFS2
	#define CONFIG_CMD_FAT
	#define CONFIG_FS_FAT
	#define CONFIG_SUPPORT_VFAT
	#define CONFIG_CMD_IDE

	#define CONFIG_SYS_ATA_BASE_ADDR        0x20000000
	#define CONFIG_SYS_ATA_REG_OFFSET       0x0000          /* Offset for normal register accesses*/
	#define CONFIG_SYS_ATA_DATA_OFFSET      0x0000          /* Offset for data I/O */

	#undef CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card	Adapter	*/

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
	/* #define CONFIG_SCSI_AHCI */
	#ifdef CONFIG_SCSI_AHCI
		#define CONFIG_SATA_6121
		#define CONFIG_SYS_SCSI_MAX_SCSI_ID	4
		#define CONFIG_SYS_SCSI_MAX_LUN	1
		#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
	#endif /* CONFIG_SCSI_AHCI */

	#define CONFIG_SCSI_MV94XX
	#ifdef CONFIG_SCSI_MV94XX
		#define CONFIG_SYS_SCSI_MAX_SCSI_ID	40 /*8 PM * 5 sata port*/
		#define CONFIG_SYS_SCSI_MAX_LUN	1
		#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
	#endif /* CONFIG_SCSI_AHCI */
#endif /* MV_INCLUDE_SATA */

/* NAND */
#ifdef MV_INCLUDE_NAND
	#define NFLASH_CS_BASE 0xfd000000 /* not relevant for the new controller */
	#define NFLASH_CS_SIZE _2M
	#define DEVICE_CS0_BASE		NFLASH_CS_BASE
	#define DEVICE_CS0_SIZE		NFLASH_CS_SIZE
	#define CONFIG_SYS_NAND_BASE	DEVICE_CS0_BASE

	#ifndef RD_78460_SERVER_REV2
		#define CONFIG_CMD_NAND
		#define CONFIG_CMD_NAND_TRIMFFS  /*  add this line to support "nand write.trimffs" command */
	#endif
	#define CONFIG_SYS_MAX_NAND_DEVICE 1

	#if defined(CONFIG_MTD_NAND_LNC)
		/* Use Software BCH */
		#define CONFIG_BCH
		#define CONFIG_NAND_ECC_BCH
		#define MV_NAND_1CS_MODE
		/* Allow to use both NAND and NOR at once */
		#define MTD_NAND_LNC_WITH_NOR
	#elif defined(CONFIG_MTD_NAND_NFC)
		/* #define CONFIG_NAND_RS_ECC_SUPPORT */
		#define CONFIG_MV_MTD_GANG_SUPPORT
		#define CONFIG_MV_MTD_MLC_NAND_SUPPORT
		#define CONFIG_SYS_64BIT_VSPRINTF
		#define CONFIG_SKIP_BAD_BLOCK
		#undef MV_NFC_DBG

		#define MV_NAND_PIO_MODE
		#define MV_NAND_1CS_MODE
	#ifndef MV_NAND_READ_OOB
		#define MV_NAND_4BIT_MODE
	#endif
		#define MTD_NAND_NFC_INIT_RESET
	#endif /* CONFIG_MTD_NAND_NFC */

	#if defined(MV_NAND_2CS_MODE)
		#define CONFIG_SYS_NAND_MAX_CHIPS 2
	#elif defined(MV_NAND_1CS_MODE)
		#define CONFIG_SYS_NAND_MAX_CHIPS 1
	#endif

	/* Boot from NAND settings */
	#if defined(MV_NAND_BOOT)
		#define CONFIG_ENV_IS_IN_NAND

		#define CONFIG_ENV_SIZE                 0x80000  /* environment takes one erase block */
		#define CONFIG_ENV_OFFSET               nand_get_env_offs() /* environment starts here  */
		#define CONFIG_ENV_ADDR                 CONFIG_ENV_OFFSET
		#define MONITOR_HEADER_LEN              0x200
		#define CONFIG_SYS_MONITOR_BASE         0
		#define CONFIG_SYS_MONITOR_LEN          0x80000           /* Reserve 512 kB for Monitor */
		#define CONFIG_ENV_RANGE                CONFIG_ENV_SIZE * 8

		#define MV_NBOOT_BASE                   0
		#define MV_NBOOT_LEN                    (4 << 10)       /* Reserved 4KB for boot strap */
	#endif /* MV_NAND_BOOT */
#endif /* MV_INCLUDE_NAND */

/* SPI Flash */
#ifdef CONFIG_MVEBU_SPI
	#define CONFIG_CMD_SPI
	#define CONFIG_CMD_SF
	#define CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_WINBOND
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_ENV_SPI_MAX_HZ           10000000        /*Max 50Mhz- will sattle on SPI bus max 41.5Mhz */
	#define CONFIG_ENV_SPI_CS               0
	#define CONFIG_ENV_SPI_BUS              0

	#ifndef CONFIG_SF_DEFAULT_SPEED
	#define CONFIG_SF_DEFAULT_SPEED        1000000
	#endif

	#ifndef CONFIG_SF_DEFAULT_MODE
	#define CONFIG_SF_DEFAULT_MODE         SPI_MODE_3
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
	#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE    /* environment takes one sector */
	#define CONFIG_ENV_OFFSET	0x100000    /* (1MB For Image) environment starts here  */
	#define CONFIG_ENV_ADDR		CONFIG_ENV_OFFSET
	#define MONITOR_HEADER_LEN	0x200
	#define CONFIG_SYS_MONITOR_BASE	0
	#define CONFIG_SYS_MONITOR_LEN	0x80000        /*(512 << 10) Reserve 512 kB for Monitor */

	#ifndef MV_INCLUDE_NOR
	#ifdef MV_BOOTROM
	#define CONFIG_SYS_FLASH_BASE           DEVICE_SPI_BASE
	#define CONFIG_SYS_FLASH_SIZE           _16M
	#else
	#define CONFIG_SYS_FLASH_BASE           BOOTDEV_CS_BASE
	#define CONFIG_SYS_FLASH_SIZE           BOOTDEV_CS_SIZE
	#endif  /* MV_BOOTROM */
	#endif  /* MV_INCLUDE_NOR */
	#endif  /* MV_SPI_BOOT */
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
