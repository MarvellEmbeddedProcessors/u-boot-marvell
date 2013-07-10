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

#ifndef __INCmvSysHwConfigh
#define __INCmvSysHwConfigh

#include <config.h>

/****************************************/
/* Soc supporetd Units definitions	*/
/****************************************/
#ifndef CONFIG_MACH_AVANTA_LP_FPGA
#define MV_INCLUDE_PEX
#define MV_INCLUDE_GIG_ETH
#define OLD_CESA_HAL
#define MV_INCLUDE_USB
#define MV_INCLUDE_TWSI
#define MV_INCLUDE_NAND
#define MV_INCLUDE_UART
#define MV_INCLUDE_SPI
#define MV_INCLUDE_TDM
#define MV_INCLUDE_XOR
#define MV_INCLUDE_SATA
#define MV_INCLUDE_SDIO
#define MV_INCLUDE_RTC
#define MV_INCLUDE_INTEG_SATA
#else
#undef  MV_INCLUDE_PEX /* PEX should be enabled for PCI-IF HAL integration (kostaz) */
#define MV_INCLUDE_PCI
#define MV_INCLUDE_GIG_ETH
#undef  MV_INCLUDE_CESA
#define OLD_CESA_HAL
#undef  MV_INCLUDE_USB
#undef  MV_INCLUDE_TWSI
#undef  MV_INCLUDE_NAND
#define MV_INCLUDE_UART
#undef  MV_INCLUDE_SPI
#undef  MV_INCLUDE_TDM
#undef  MV_INCLUDE_XOR
#undef  MV_INCLUDE_SATA
#undef  MV_INCLUDE_TS
#undef  MV_INCLUDE_AUDIO
#undef  MV_INCLUDE_INTEG_SATA
#undef  MV_INCLUDE_RTC
#undef  MV_INCLUDE_SDIO
#endif

#define MV_INCLUDE_CLK_PWR_CNTRL

/*
 * Board devices
 */
/* On-Board NAND Flash support */
#define MV_INC_BOARD_NAND_FLASH
/* On-Board SPI Flash support */
#define MV_INC_BOARD_SPI_FLASH
/* On-Board RTC */
#define MV_INC_BOARD_RTC

/*
 * U-Boot
 */

/*
 * DRAM
 */
#undef MV_INC_BOARD_DDIM

#if !defined(MV_BOOTROM) || defined(CONFIG_MACH_AVANTA_LP_FPGA)
#define MV_STATIC_DRAM_ON_BOARD
#endif

/*
 *  System memory mapping
 */

#define DRAM_IO_RESERVE_BASE		0xC0000000ll
#ifdef  DRAM_IO_RESERVE_BASE
#define MV_DRAM_IO_RESERVE_BASE		DRAM_IO_RESERVE_BASE
#endif

/* SDRAM: actual mapping is auto detected */
#define SDRAM_CS0_BASE  0x00000000
#define SDRAM_CS0_SIZE  _256M

#define SDRAM_CS1_BASE  0x10000000
#define SDRAM_CS1_SIZE  _256M

#define SDRAM_CS2_BASE  0x20000000
#define SDRAM_CS2_SIZE  _256M

#define SDRAM_CS3_BASE  0x30000000
#define SDRAM_CS3_SIZE  _256M

/*
 * PEX Address Decoding
 */
#define PEX0_MEM_BASE			0xC0000000
#define PEX0_MEM_SIZE			_64M
#define PEX1_MEM_BASE			0xC4000000
#define PEX1_MEM_SIZE			_64M


#define PEX0_IO_BASE			0xF1100000
#define PEX0_IO_SIZE			_1M
#define PEX1_IO_BASE			0xF1200000
#define PEX1_IO_SIZE			_1M

/* Device: CS0 - NOR or NAND, CS1 - SPI, CS2 - Boot ROM, CS3 - Boot device */
/* NOR and NAND are configure to CS0 but it is posible to load
   NOR as BOOT_CS and NAND ad CS0 otherwise only NOR or NAND it posible */
/* Internal registers: size is defined in Controller environment */

#define INTER_REGS_BASE			0xF1000000
#define ARMADAXP_SB_REGS_PHYS_BASE	INTER_REGS_BASE

#define NFLASH_CS_BASE 0xfd000000 /* not relevant for the new controller */
#define NFLASH_CS_SIZE _2M

#define NOR_CS_BASE 0xfd000000
#define NOR_CS_SIZE _16M

#define SPI_CS_BASE 0xf4000000 /*0xf8000000*/
#define SPI_CS_SIZE _8M

#define PP2_CPU0_PHYS_BASE 0xf2100000 /*0xF5000000*/
#define PP2_SIZE	 _64K

/* Important for MP - Do not disable/change this window, used by BOOTROM when booting other cores */
#define CRYPT_ENG_BASE	 0xc8010000
#define CRYPT_ENG_SIZE	 _64K

#define DEVICE_SPI_BASE SPI_CS_BASE

#ifdef MV_INCLUDE_NOR
#define DEVICE_CS0_BASE NOR_CS_BASE
#define DEVICE_CS0_SIZE NOR_CS_SIZE
#else
#define DEVICE_CS0_BASE NFLASH_CS_BASE
#define DEVICE_CS0_SIZE NFLASH_CS_SIZE
#endif

#define DEVICE_CS1_BASE SPI_CS_BASE
#define DEVICE_CS1_SIZE _16M

#define DEVICE_CS2_BASE 0xf2000000 /* 0xf4000000 */
#define DEVICE_CS2_SIZE _1M

#define DEVICE_CS3_BASE BOOTDEV_CS_BASE
#define DEVICE_CS3_SIZE BOOTDEV_CS_SIZE

#if !defined(MV_BOOTROM) && defined(MV_NAND_BOOT)
#define CONFIG_SYS_NAND_BASE 	BOOTDEV_CS_BASE
#else
#define CONFIG_SYS_NAND_BASE 	DEVICE_CS0_BASE
#endif

#if defined (MV_INCLUDE_PEX)
#define PCI_IF0_MEM0_BASE 	PEX0_MEM_BASE
#define PCI_IF0_MEM0_SIZE 	PEX0_MEM_SIZE
#define PCI_IF0_IO_BASE 	PEX0_IO_BASE
#define PCI_IF0_IO_SIZE 	PEX0_IO_SIZE
#endif

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE

#define PCI_ARBITER_CTRL    /* Use/unuse the Marvell integrated PCI arbiter	*/
#undef	PCI_ARBITER_BOARD	/* Use/unuse the PCI arbiter on board			*/

/* Check macro validity */
#if defined(PCI_ARBITER_CTRL) && defined (PCI_ARBITER_BOARD)
	#error "Please select either integrated PCI arbiter or board arbiter"
#endif

/* Board clock detection */
#define TCLK_AUTO_DETECT    /* Use Tclk auto detection */
#define SYSCLK_AUTO_DETECT	/* Use SysClk auto detection */
#define PCLCK_AUTO_DETECT  /* Use PClk auto detection */
#define L2CLK_AUTO_DETECT  /* Use L2 Clk auto detection */
#define MV_FPGA_SYS_CLK 		25000000
#define MV_FPGA_CORE_CLK		12500000

/************* Ethernet driver configuration ********************/

/*#define ETH_JUMBO_SUPPORT*/
/* HW cache coherency configuration */
#define DMA_RAM_COHER	    NO_COHERENCY
#define ETHER_DRAM_COHER    MV_UNCACHED
#define INTEG_SRAM_COHER    MV_UNCACHED  /* Where integrated SRAM available */

#define ETH_DESCR_IN_SDRAM
#undef  ETH_DESCR_IN_SRAM

#if (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WB)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WB"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WT)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WT"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_SW)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_SW"
#elif (ETHER_DRAM_COHER == MV_UNCACHED)
#   define ETH_SDRAM_CONFIG_STR      "MV_UNCACHED"
#else
#   error "Unexpected ETHER_DRAM_COHER value"
#endif /* ETHER_DRAM_COHER */

/*
 * CPU address decode table
 * Note: that table entry number must match its winNum enumerator.
 * For example, table entry '4' must describe Device CS0 winNum
 * which is represented by DEVICE_CS0 enumerator (4).
 */
#define MV_CPU_IF_ADDR_WIN_MAP_88F6660_TBL {									\
	/* base low        base high    size       		WinNum       enable */			\
	{{SDRAM_CS0_BASE,	0,	SDRAM_CS0_SIZE	},	0xFFFFFFFF,	DIS},	/*  0 SDRAM_CS0 */ \
	{{SDRAM_CS1_BASE,	0,	SDRAM_CS1_SIZE	},	0xFFFFFFFF,	DIS},	/*  1 SDRAM_CS1 */ \
	{{SDRAM_CS2_BASE,	0,	SDRAM_CS2_SIZE	},	0xFFFFFFFF,	DIS},	/*  2 SDRAM_CS2 */ \
	{{SDRAM_CS3_BASE,	0,	SDRAM_CS3_SIZE	},	0xFFFFFFFF,	DIS},	/*  3 SDRAM_CS3 */ \
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		10,	DIS},	/*  4 DEVICE_CS0 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  5 DEVICE_CS1 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  6 DEVICE_CS2 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  7 DEVICE_CS3 */\
	{{PEX0_MEM_BASE,	0,	PEX0_MEM_SIZE	},		 0,	 EN},	/*  8 PEX0_MEM */  \
	{{PEX0_IO_BASE,		0,	PEX0_IO_SIZE	},	TBL_UNUSED,	DIS},	/*  9 PEX0_IO */   \
	{{PEX1_MEM_BASE,	0,	PEX1_MEM_SIZE	},		 1,	 EN},	/* 10 PEX1_MEM */  \
	{{PEX1_IO_BASE,		0,	PEX1_IO_SIZE	},	TBL_UNUSED,	DIS},	/* 11 PEX1_IO */   \
	{{INTER_REGS_BASE,	0,	INTER_REGS_SIZE	},MV_AHB_TO_MBUS_INTREG_WIN, EN},	/* 12 INTER_REGS */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 13 DMA_UART   */\
	{{SPI_CS_BASE,		0,	SPI_CS_SIZE	},		 8,	DIS},	/* 14 SPI_CS0 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 15 SPI_CS1 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 16 SPI_CS2 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 17 SPI_CS3 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 18 SPI_CS4 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 19 SPI_CS5 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 20 SPI_CS6 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 21 SPI_CS7 */   \
	{{0xf8000000,		0,		_1M	},		13,	DIS},	/* 22 BOOT_ROM_CS */\
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		 9,	 EN},	/* 23 DEV_BOOCS */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 24 USB3	 */\
	{{CRYPT_ENG_BASE,	0,	CRYPT_ENG_SIZE	},		10,	DIS},	/* 25 CRYPT_ENG */ \
	{{PP2_CPU0_PHYS_BASE,	0,	PP2_SIZE	},		12,	 EN},	/* 26 PP2 */    \
	{{TBL_TERM,		TBL_TERM, TBL_TERM	},	TBL_TERM,	TBL_TERM}               \
};
#define MV_CPU_IF_ADDR_WIN_MAP_88F6650_TBL {									\
	/* base low        base high    size       		WinNum       enable */			\
	{{SDRAM_CS0_BASE,	0,	SDRAM_CS0_SIZE	},	0xFFFFFFFF,	DIS},	/*  0 SDRAM_CS0 */ \
	{{SDRAM_CS1_BASE,	0,	SDRAM_CS1_SIZE	},	0xFFFFFFFF,	DIS},	/*  1 SDRAM_CS1 */ \
	{{SDRAM_CS2_BASE,	0,	SDRAM_CS2_SIZE	},	0xFFFFFFFF,	DIS},	/*  2 SDRAM_CS2 */ \
	{{SDRAM_CS3_BASE,	0,	SDRAM_CS3_SIZE	},	0xFFFFFFFF,	DIS},	/*  3 SDRAM_CS3 */ \
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		10,	DIS},	/*  4 DEVICE_CS0 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  5 DEVICE_CS1 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  6 DEVICE_CS2 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  7 DEVICE_CS3 */\
	{{PEX0_MEM_BASE,	0,	PEX0_MEM_SIZE	},		 0,	EN},	/*  8 PEX0_MEM */  \
	{{PEX0_IO_BASE,		0,	PEX0_IO_SIZE	},	TBL_UNUSED,	DIS},	/*  9 PEX0_IO */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED,	},		 1,	DIS},	/* 10 PEX1_MEM */  \
	{{TBL_UNUSED,		0,	TBL_UNUSED,	},	TBL_UNUSED,	DIS},	/* 11 PEX1_IO */   \
	{{INTER_REGS_BASE,	0,	INTER_REGS_SIZE	},MV_AHB_TO_MBUS_INTREG_WIN, EN},	/* 12 INTER_REGS */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 13 DMA_UART   */\
	{{SPI_CS_BASE,		0,	SPI_CS_SIZE	},		8,	DIS},	/* 14 SPI_CS0 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 15 SPI_CS1 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 16 SPI_CS2 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 17 SPI_CS3 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 18 SPI_CS4 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 19 SPI_CS5 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 20 SPI_CS6 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 21 SPI_CS7 */   \
	{{0xf8000000,		0,	_1M		},		13,	DIS},	/* 22 BOOT_ROM_CS */\
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		 9,	 EN},	/* 23 DEV_BOOCS */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 24 USB3	 */\
	{{CRYPT_ENG_BASE,	0,	CRYPT_ENG_SIZE	},		10,	DIS},	/* 25 CRYPT_ENG */ \
	{{PP2_CPU0_PHYS_BASE,	0,	PP2_SIZE	},		12,	 EN},	/* 26 PP2 */    \
	{{TBL_TERM,		TBL_TERM, TBL_TERM	},	TBL_TERM,  TBL_TERM}               \
};

#define MV_CPU_IF_ADDR_WIN_MAP_88F6610_TBL {									\
	/* base low        base high    size       		WinNum       enable */			\
	{{SDRAM_CS0_BASE,	0,	SDRAM_CS0_SIZE	},	0xFFFFFFFF,	DIS},	/*  0 SDRAM_CS0 */ \
	{{SDRAM_CS1_BASE,	0,	SDRAM_CS1_SIZE	},	0xFFFFFFFF,	DIS},	/*  1 SDRAM_CS1 */ \
	{{SDRAM_CS2_BASE,	0,	SDRAM_CS2_SIZE	},	0xFFFFFFFF,	DIS},	/*  2 SDRAM_CS2 */ \
	{{SDRAM_CS3_BASE,	0,	SDRAM_CS3_SIZE	},	0xFFFFFFFF,	DIS},	/*  3 SDRAM_CS3 */ \
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		10,	DIS},	/*  4 DEVICE_CS0 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  5 DEVICE_CS1 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  6 DEVICE_CS2 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  7 DEVICE_CS3 */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},		 0,	DIS},	/*  8 PEX0_MEM */  \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/*  9 PEX0_IO */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED,	},		 1,	DIS},	/* 10 PEX1_MEM */  \
	{{TBL_UNUSED,		0,	TBL_UNUSED,	},	TBL_UNUSED,	DIS},	/* 11 PEX1_IO */   \
	{{INTER_REGS_BASE,	0,	INTER_REGS_SIZE	},MV_AHB_TO_MBUS_INTREG_WIN, EN},	/* 12 INTER_REGS */\
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 13 DMA_UART   */\
	{{SPI_CS_BASE,		0,	SPI_CS_SIZE	},		 8,	DIS},	/* 14 SPI_CS0 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 15 SPI_CS1 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 16 SPI_CS2 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 17 SPI_CS3 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 18 SPI_CS4 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 19 SPI_CS5 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 20 SPI_CS6 */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 21 SPI_CS7 */   \
	{{0xf8000000,		0,		_1M	},		13,	DIS},	/* 22 BOOT_ROM_CS */\
	{{NOR_CS_BASE,		0,	NOR_CS_SIZE	},		 9,	 EN},	/* 23 DEV_BOOCS */   \
	{{TBL_UNUSED,		0,	TBL_UNUSED	},	TBL_UNUSED,	DIS},	/* 24 USB3	 */\
	{{CRYPT_ENG_BASE,	0,CRYPT_ENG_SIZE	},		10,	DIS},	/* 25 CRYPT_ENG */ \
	{{PP2_CPU0_PHYS_BASE,	0,	PP2_SIZE	},		12,	 EN},	/* 26 PP2 */    \
	{{TBL_TERM,		TBL_TERM, TBL_TERM	},	TBL_TERM,  TBL_TERM}               \
};
#define mvCpuAddrWinMapGet(x)   ((x == MV_6660_DEV_ID)? 	\
				mvCpuAddrWinMap_88F6660:	\
				(x == MV_6650_DEV_ID)? 	\
				mvCpuAddrWinMap_88F6650: \
				mvCpuAddrWinMap_88F6610)

#define mvCpuAddrWinMap		mvCpuAddrWinMapGet(mvCtrlModelGet())

#define MV_CACHEABLE(address) ((address) | 0x80000000)

#define _1K         0x00000400
#define _4K         0x00001000
#define _8K         0x00002000
#define _16K        0x00004000
#define _32K        0x00008000
#define _64K        0x00010000
#define _128K       0x00020000
#define _256K       0x00040000
#define _512K       0x00080000

#define _1M         0x00100000
#define _2M         0x00200000
#define _4M         0x00400000
#define _8M         0x00800000
#define _16M        0x01000000
#define _32M        0x02000000
#define _64M        0x04000000
#define _128M       0x08000000
#define _256M       0x10000000
#define _512M       0x20000000

#define _1G         0x40000000
#define _2G         0x80000000

#if defined(MV_BOOTSIZE_256K)

#define BOOTDEV_CS_SIZE _256K

#elif defined(MV_BOOTSIZE_512K)

#define BOOTDEV_CS_SIZE _512K

#elif defined(MV_BOOTSIZE_4M)

#define BOOTDEV_CS_SIZE _4M

#elif defined(MV_BOOTSIZE_8M)

#define BOOTDEV_CS_SIZE _8M

#elif defined(MV_BOOTSIZE_16M)

#define BOOTDEV_CS_SIZE _16M

#elif defined(MV_BOOTSIZE_32M)

#define BOOTDEV_CS_SIZE _32M

#elif defined(MV_BOOTSIZE_64M)

#define BOOTDEV_CS_SIZE _64M

#elif defined(MV_NAND_BOOT)

#define BOOTDEV_CS_SIZE _512K

#else

#define Error MV_BOOTSIZE undefined

#endif

#define BOOTDEV_CS_BASE	((0xFFFFFFFF - BOOTDEV_CS_SIZE) + 1)

/*
 * Ethernet
 */
#define ETH_DEF_TXQ		0
#define ETH_DEF_RXQ		0
#define MV_ETH_TX_Q_NUM		1
#define MV_ETH_RX_Q_NUM		1
#define ETH_NUM_OF_RX_DESCR	64
#define ETH_NUM_OF_TX_DESCR	(ETH_NUM_OF_RX_DESCR * 2)

/*
 * CESA (crypto)
 */
#define MV_CESA_MAX_BUF_SIZE	1600

#endif /* __INCmvSysHwConfigh */
