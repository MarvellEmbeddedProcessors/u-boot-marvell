/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvCtrlEnvSpech
#define __INCmvCtrlEnvSpech

#include "mvDeviceId.h"
#include "mvSysHwConfig.h"

#include "ctrlEnv/sys/mvCpuIfRegs.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MV_ARM_SOC
#define SOC_NAME_PREFIX				"Bobcat2"

/*
 * Bobcat2 Units Address decoding
 */
#define MV_DRAM_REGS_OFFSET			(0x0)
#define MV_AURORA_L2_REGS_OFFSET		(0x8000)
#define MV_RTC_REGS_OFFSET			(0x10300)
#define MV_DEV_BUS_REGS_OFFSET			(0x10400)
#define MV_SPI_REGS_OFFSET(unit)		(0x10600 + (unit * 0x80))
#define MV_TWSI_SLAVE_REGS_OFFSET(chanNum)	(0x11000 + (chanNum * 0x100))

#define MV_UART_REGS_OFFSET(chanNum)		(0x12000 + (chanNum * 0x100))

#define MV_MPP_REGS_OFFSET			(0x18000)
#define MV_GPP_REGS_OFFSET(unit)		(0x18100 + ((unit) * 0x80))

#define MV_MISC_REGS_OFFSET			(0x18200)
#define MV_MBUS_REGS_OFFSET			(0x20000)
#define MV_COHERENCY_FABRIC_OFFSET		(0x20200)
#define MV_COHERENCY_FABRIC_CTRL_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x0)
#define MV_COHERENCY_FABRIC_CFG_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x4)
#define MV_CIB_CTRL_STATUS_OFFSET		(0x20280)
#define MV_CNTMR_REGS_OFFSET			(0x20300)
#define MV_CPUIF_LOCAL_REGS_OFFSET		(0x21000)
#define MV_CPUIF_REGS_OFFSET(cpu)		(0x21800 + (cpu) * 0x100)
#define MV_PMU_NFABRIC_UNIT_SERV_OFFSET		(0x22000)
#define MV_CPU_PMU_UNIT_SERV_OFFSET(cpu)	(0x22100 + (cpu) * 0x100)
#define MV_ETH_BASE_ADDR			(0x70000)
#define MV_ETH_REGS_OFFSET(port)		(MV_ETH_BASE_ADDR + (port)* 0x40000 )
#define MV_PEX_IF_REGS_OFFSET(pexIf)\
			(pexIf < 8 ? (0x40000 + ((pexIf) / 4) * 0x40000 + ((pexIf) % 4) * 0x4000)\
	: (0X42000 + ((pexIf) % 8) * 0x40000))
#define MV_USB_REGS_OFFSET(dev)			(0x50000 + (dev * 0x1000))
#define MV_XOR_REGS_OFFSET(unit)		(0xF0000)
#if defined(MV_INCLUDE_IDMA)
#define MV_IDMA_REGS_OFFSET			(0x60800)
#endif
#define MV_SATA_REGS_OFFSET			(0xA0000)
#define MV_COMM_UNIT_REGS_OFFSET		(0xB0000)
#define MV_NFC_REGS_OFFSET			(0xD0000)
#define MV_BM_REGS_OFFSET			(0xC0000)
#define MV_PNC_REGS_OFFSET			(0xC8000)
#define MV_SDMMC_REGS_OFFSET			(0xD4000)


#define MV_ETH_SMI_PORT   0
/*
 * Miscellanuous Controller Configurations
 */

#define AVS_CONTROL2_REG			0x20868
#define AVS_LOW_VDD_LIMIT			0x20860

#define INTER_REGS_SIZE				_1M

/* This define describes the TWSI interrupt bit and location */
#define TWSI_CPU_MAIN_INT_CAUSE_REG(cpu)	CPU_MAIN_INT_CAUSE_REG(1, (cpu))
#define TWSI0_CPU_MAIN_INT_BIT(ch)		((ch) + 3)
#define TWSI_SPEED				100000

#define MV_GPP_MAX_PINS				33
#define MV_GPP_MAX_GROUP    			2 	/* group == configuration register? */
#define MV_CNTMR_MAX_COUNTER 			8 	/* 4 global + 1 global WD + 2 current private CPU + 1 private CPU WD*/

#define MV_UART_MAX_CHAN			2

#define MV_XOR_MAX_UNIT				1 /* XOR unit == XOR engine */
#define MV_XOR_MAX_CHAN         		2 /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT		2 /* channels for units */

#define MV_MPP_MAX_GROUP			5

#define MV_DRAM_MAX_CS				4
#define MV_SPI_MAX_CS				8
/* This define describes the maximum number of supported PCI\PCIX Interfaces */
#ifdef MV_INCLUDE_PCI
 #define MV_PCI_MAX_IF				1
 #define MV_PCI_START_IF			0
 #define PCI_HOST_BUS_NUM(pciIf)               (pciIf)
 #define PCI_HOST_DEV_NUM(pciIf)               0
#else
 #define MV_PCI_MAX_IF				0
 #define MV_PCI_START_IF			0
#endif

/* This define describes the maximum number of supported PEX Interfaces */
#define MV_PEX_MAX_IF				1
#define MV_PEX_MAX_UNIT				1
#ifdef MV_INCLUDE_PEX
#define MV_INCLUDE_PEX0
#define MV_DISABLE_PEX_DEVICE_BAR

#define MV_PEX_START_IF				MV_PCI_MAX_IF
 #define PEX_HOST_BUS_NUM(pciIf)               (pciIf)
 #define PEX_HOST_DEV_NUM(pciIf)               0
#else
 #undef MV_INCLUDE_PEX0
#endif

#define PCI_IO(pciIf)				(PEX0_IO + 2 * (pciIf))
#define PCI_MEM(pciIf, memNum)			(PEX0_MEM0 + 2 * (pciIf))
/* This define describes the maximum number of supported PCI Interfaces 	*/
#define BC2_NAND				1
#define BC2_SDIO				1
#define MV_DEVICE_MAX_CS      			4


/* This define describes the maximum number of supported Ethernet ports */
/* TODO - verify all these numbers */
#define MV_ETH_VERSION 				4 /* for Legacy mode */
#define MV_NETA_VERSION				1 /* for NETA mode */
#define MV_ETH_MAX_PORTS			2
#define MV_ETH_MAX_RXQ              		8
#define MV_ETH_MAX_TXQ              		8
#define MV_ETH_TX_CSUM_MAX_SIZE 		9800
#define BOARD_ETH_SWITCH_PORT_NUM		2

/* New GMAC module is used */
#define MV_ETH_GMAC_NEW
/* New WRR/EJP module is used */
#define MV_ETH_WRR_NEW
/* IPv6 parsing support for Legacy parser */
#define MV_ETH_LEGACY_PARSER_IPV6

#define MV_MV_98DX_ETH_MAX_PORT			2

#define MV_SPI_VERSION				2

#define MV_INCLUDE_SDRAM_CS0
#define MV_INCLUDE_SDRAM_CS1
#define MV_INCLUDE_SDRAM_CS2
#define MV_INCLUDE_SDRAM_CS3

#define MV_INCLUDE_DEVICE_CS0
#define MV_INCLUDE_DEVICE_CS1
#define MV_INCLUDE_DEVICE_CS2
#define MV_INCLUDE_DEVICE_CS3

#ifndef MV_ASMLANGUAGE

#define TBL_UNUSED	0	/* Used to mark unused entry */


/* This enumerator defines the Marvell Units ID      */
typedef enum _mvUnitId {
	DRAM_UNIT_ID,
	PEX_UNIT_ID,
	ETH_GIG_UNIT_ID,
	XOR_UNIT_ID,
	UART_UNIT_ID,
	SPI_UNIT_ID,
	SDIO_UNIT_ID,
	I2C_UNIT_ID,
	MAX_UNITS_ID
} MV_UNIT_ID;

/* This enumerator describes the Marvell controller possible devices that   */
/* can be connected to its device interface.                                */
typedef enum _mvDevice {
#if defined(MV_INCLUDE_DEVICE_CS0)
	DEV_CS0 = 0,    /* Device connected to dev CS[0]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS1)
	DEV_CS1 = 1,        /* Device connected to dev CS[1]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS2)
	DEV_CS2 = 2,        /* Device connected to dev CS[2]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS3)
	DEV_CS3 = 3,        /* Device connected to dev CS[2]    */
#endif
	BOOT_CS,        /* Device connected to BOOT dev    */
	MV_DEV_MAX_CS = MV_DEVICE_MAX_CS
} MV_DEVICE;

/* This enumerator described the possible Controller paripheral targets.    */
/* Controller peripherals are designated memory/IO address spaces that the  */
/* controller can access. They are also refered as "targets"                */
typedef enum _mvTarget {
	TBL_TERM = -1, 	/* none valid target, used as targets list terminator*/
	SDRAM_CS0,	/*  0 SDRAM chip select 0	*/
	SDRAM_CS1,	/*  1 SDRAM chip select 1	*/
	SDRAM_CS2,	/*  2 SDRAM chip select 2	*/
	SDRAM_CS3,	/*  3 SDRAM chip select 3	*/
	DEVICE_CS0,	/*  4 Device chip select 0	*/
	DEVICE_CS1,	/*  5 Device chip select 1	*/
	DEVICE_CS2,	/*  6 Device chip select 2	*/
	DEVICE_CS3,	/*  7 Device chip select 3	*/
	PEX0_MEM,	/*  8 PCI Express 0 Memory	*/
	PEX0_IO,	/*  9 PCI Express 0 IO		*/
	INTER_REGS,	/* 10 Internal registers	*/
	DFX_REGS,	/* 11 DFX Internal registers	*/
	DMA_UART,	/* 12 DMA based UART request	*/
	SPI_CS0,	/* 13 SPI_CS0			*/
	SPI_CS1,	/* 14 SPI_CS1			*/
	SPI_CS2,	/* 15 SPI_CS2			*/
	SPI_CS3,	/* 16 SPI_CS3			*/
	SPI_CS4,	/* 17 SPI_CS4			*/
	SPI_CS5,	/* 18 SPI_CS5			*/
	SPI_CS6,	/* 19 SPI_CS6			*/
	SPI_CS7,	/* 20 SPI_CS7			*/
	BOOT_ROM_CS, 	/* 21 BOOT_ROM_CS		*/
	DEV_BOOCS,	/* 22 DEV_BOOCS			*/
	MAX_TARGETS
} MV_TARGET;

#ifdef AURORA_IO_CACHE_COHERENCY
#define DRAM_CS0_ATTR		0x1E
#define DRAM_CS1_ATTR		0x1D
#define DRAM_CS2_ATTR		0x1B
#define DRAM_CS3_ATTR		0x17
#else
#define DRAM_CS0_ATTR		0x0E
#define DRAM_CS1_ATTR		0x0D
#define DRAM_CS2_ATTR		0x0B
#define DRAM_CS3_ATTR		0x07
#endif

#define TARGETS_DEF_ARRAY	{			\
	{DRAM_CS0_ATTR, DRAM_TARGET_ID   },	/*  0 SDRAM_CS0 */	\
	{DRAM_CS1_ATTR, DRAM_TARGET_ID   },	/*  1 SDRAM_CS1 */	\
	{DRAM_CS2_ATTR, DRAM_TARGET_ID   },	/*  2 SDRAM_CS0 */	\
	{DRAM_CS3_ATTR, DRAM_TARGET_ID   },	/*  3 SDRAM_CS1 */	\
	{0x3E, DEV_TARGET_ID    },		/*  4 DEVICE_CS0 */	\
	{0x3D, DEV_TARGET_ID    },		/*  5 DEVICE_CS1 */	\
	{0x3B, DEV_TARGET_ID    },		/*  6 DEVICE_CS2 */	\
	{0x37, DEV_TARGET_ID    },		/*  7 DEVICE_CS3 */	\
	{0xE8, PEX0_TARGET_ID	},		/*  8 PEX0_LANE0_MEM */	\
	{0xE0, PEX0_TARGET_ID	},		/*  9 PEX0_LANE0_IO */	\
	{0xFF, 0xFF             },		/* 10 INTER_REGS */	\
	{0x00, DFX_TARGET_ID	},		/* 11 DFX_INTER_REGS */	\
	{0x01, DEV_TARGET_ID    },		/* 12 DMA_UART */	\
	{0x1E, DEV_TARGET_ID    },		/* 13 SPI_CS0 */	\
	{0x5E, DEV_TARGET_ID    },		/* 14 SPI_CS1 */	\
	{0x9E, DEV_TARGET_ID    },		/* 15 SPI_CS2 */	\
	{0xDE, DEV_TARGET_ID    },		/* 16 SPI_CS3 */	\
	{0x1F, DEV_TARGET_ID    },		/* 17 SPI_CS4 */	\
	{0x5F, DEV_TARGET_ID    },		/* 18 SPI_CS5 */	\
	{0x9F, DEV_TARGET_ID    },		/* 19 SPI_CS6 */	\
	{0xDF, DEV_TARGET_ID    },		/* 20 SPI_CS7 */	\
	{0x1D, DEV_TARGET_ID    },		/* 21 BOOT_ROM_CS (Main Boot device )*/	\
	{0x2F, DEV_TARGET_ID    },		/* 22 DEV_BOOT_CS (Secondary Boot device,)*/	\
}

#define TARGETS_NAME_ARRAY	{			\
	"SDRAM_CS0",    	/*  0 SDRAM_CS0 */	\
	"SDRAM_CS1",    	/*  1 SDRAM_CS1 */	\
	"SDRAM_CS2",    	/*  2 SDRAM_CS1 */	\
	"SDRAM_CS3",    	/*  3 SDRAM_CS1 */	\
	"DEVICE_CS0",		/*  4 DEVICE_CS0 */	\
	"DEVICE_CS1",		/*  5 DEVICE_CS1 */	\
	"DEVICE_CS2",		/*  6 DEVICE_CS2 */	\
	"DEVICE_CS3",		/*  7 DEVICE_CS3 */	\
	"PEX0_MEM",		/*  8 PEX0_MEM */	\
	"PEX0_IO",		/*  9 PEX0_IO */		\
	"INTER_REGS",		/* 10 INTER_REGS */	\
	"DFX_INTER_REGS",	/* 11 INTER_REGS */	\
	"DMA_UART",		/* 12 DMA_UART */	\
	"SPI_CS0",		/* 13 SPI_CS0 */	\
	"SPI_CS1",		/* 14 SPI_CS1 */	\
	"SPI_CS2",		/* 15 SPI_CS2 */	\
	"SPI_CS3",		/* 16 SPI_CS3 */	\
	"SPI_CS4",		/* 17 SPI_CS4 */	\
	"SPI_CS5",		/* 18 SPI_CS5 */	\
	"SPI_CS6",		/* 19 SPI_CS6 */	\
	"SPI_CS7",		/* 20 SPI_CS7 */	\
	"BOOT_ROM_CS",		/* 21 BOOT_ROM_CS */	\
	"DEV_BOOTCS",		/* 22 DEV_BOOCS */	\
}




#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvCtrlEnvSpech */
