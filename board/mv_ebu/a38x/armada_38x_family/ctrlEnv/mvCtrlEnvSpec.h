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
#define SOC_NAME_PREFIX                         "MV88F"

/*
 * Armada-38x Units Address decoding
 */
#define MV_DRAM_REGS_OFFSET                     (0x0)
#define MV_AURORA_L2_REGS_OFFSET                (0x8000)
#define MV_RTC_REGS_OFFSET                      (0x10300)
#define MV_DEV_BUS_REGS_OFFSET                  (0x10400)
#define MV_SPI_REGS_OFFSET(unit)                (0x10600 + (unit * 0x80))
#define MV_TWSI_SLAVE_REGS_OFFSET(chanNum)      (0x11000 + (chanNum * 0x100))
#define MV_UART_REGS_OFFSET(chanNum)            (0x12000 + (chanNum * 0x100))
#define MV_MPP_REGS_OFFSET                      (0x18000)
#define MV_GPP_REGS_OFFSET(unit)                (0x18100 + ((unit) * 0x40))
#define MV_MISC_REGS_OFFSET                     (0x18200)
#define MV_COMMON_PHY_REGS_OFFSET               (0x18300)
#define MV_IP_CONFIG_REGS_OFFSET                (0x18400)
#define MV_CLK_CMPLX_REGS_OFFSET        	(0x18700)
#define MV_NET_COMPLEX_OFFSET			(0x18A00)
#define MV_MBUS_REGS_OFFSET                     (0x20000)
#define CPU_GLOBAL_BASE                         (MV_MBUS_REGS_OFFSET)
#define MV_COHERENCY_FABRIC_OFFSET              (0x20200) /* ??? IO Error Control Register = Offset: 0x00020200 */
#define MV_CIB_CTRL_STATUS_OFFSET               (0x20280)
#define MV_CNTMR_REGS_OFFSET                    (0x20300)

#define MV_XSMI_REGS_OFFSET			(0x30000) /* Access for XSMI registers using PSS window only */

/*
 * CPU virtual/banked registers: 0x21000 to 0x21100 (64 registers).
 * Physical registers:
 * 	CPU0: 0x21800 - 0x21900
 * 	CPU1: 0x21900 - 0x21a00
 */
#define MV_CPUIF_LOCAL_REGS_OFFSET              (0x21000)
#define MV_CPUIF_REGS_OFFSET(cpu)               (0x21800 + (cpu) * 0x100)
#define MV_CPU_HW_SEM_OFFSET                    (0x20500)

#define MV_ETH_BASE_ADDR_PORT1_2		(0x00030000)	/* Port1 = 0x30000   port1 = 0x34000 */
#define MV_ETH_BASE_ADDR_PORT0			(0x00070000)
#define MV_ETH_REGS_OFFSET(p)			(((p) == 0) ? MV_ETH_BASE_ADDR_PORT0 : \
						(MV_ETH_BASE_ADDR_PORT1_2 + (((p) - 1) * 0x4000)))
#define MV_ETH_SGMII_PHY_REGS_OFFSET(p)		(MV_ETH_REGS_OFFSET(p)+0x2000)

#define MV_PEX_IF_REGS_OFFSET(pexIf)            (((pexIf) == 0) ? 0x80000 : (0x40000 + ((pexIf-1) * 0x4000)))
#define MV_USB_REGS_OFFSET(dev)                 (0x58000)
#define MV_USB3_REGS_OFFSET(dev)                (0xF0000 + (dev * 0x8000))
#define MV_XOR_REGS_OFFSET(unit)                (0x60800 + (unit)*0x100)
#define MV_CESA_TDMA_REGS_OFFSET(chanNum)       (0x90000 + (chanNum * 0x2000))
#define MV_CESA_REGS_OFFSET(chanNum)            (0x9D000 + (chanNum * 0x2000))
#define MV_SATA_REGS_OFFSET                     (0xA0000)
#define MV_SATA3_REGS_OFFSET(u)			((u == 0) ? 0xA8000 : 0xE0000) /* sata register per unit*/
#define MV_COMM_UNIT_REGS_OFFSET                (0xB0000)
#define MV_BM_REGS_OFFSET                       (0xC8000)
#define MV_NFC_REGS_OFFSET                      (0xD0000)
#define MV_SDMMC_REGS_OFFSET			(0xD8000)
#define MV_SDMMC_WINDOWS_REGS_OFFSET		(0xDC000)

#define SATA3_NUM_OF_PORTS			2
#define SATA3_MAX_PORTS_PER_UNIT		2
#define MV_ETH_SMI_PORT   0

/* USB3 registers */
#define USB3_REGS_PHYS_BASE(dev)		(INTER_REGS_BASE + MV_USB3_REGS_BASE(dev))
#define MV_USB2_CAPLENGTH_OFFSET(index)		(INTER_REGS_BASE + MV_USB_REGS_OFFSET(index) + 0x100)

/*
 * Miscellanuous Controller Configurations
 */

#define INTER_REGS_SIZE                         _1M

#ifdef CONFIG_ARMADA_38X
#define MV_SERDES_MAX_LANES			6
#define MV_SERDES_CFG_OPTIONS_CNT		11
#elif defined CONFIG_ARMADA_39X
#define MV_SERDES_MAX_LANES                     7
#define MV_SERDES_CFG_OPTIONS_CNT		11
#endif
#define MV_SERDES_MAX_LANES_6810		5

/* This define describes the TWSI interrupt bit and location */
#define TWSI_CPU_MAIN_INT_CAUSE_REG(cpu)        CPU_MAIN_INT_CAUSE_REG(1, (cpu))
#define TWSI0_CPU_MAIN_INT_BIT(ch)              ((ch) + 3)
#define TWSI_SPEED                              100000

#define MV_GPP_MAX_PINS                         68
#define MV_GPP_MAX_GROUP                        3       /* group == configuration register? */
#define MV_CNTMR_MAX_COUNTER            17              /* 4 global + 1 global WD + 2 per CPU + 4 CPU WD*/

#define MV_IO_EXP_MAX_REGS			3

#define MV_UART_MAX_CHAN                        4

#define MV_XOR_MAX_UNIT                         2       /* XOR unit == XOR engine */
#define MV_XOR_MAX_CHAN                         4       /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT                2       /* channels for units */

#define MV_SATA_MAX_UNIT                        2
#define MV_SATA_MAX_CHAN                        4	/* total channels (ports) for all units together */
#define MV_SATA_MAX_CHAN_PER_UNIT               2       /* channels (ports) per unit */

#define MV_MPP_MAX_GROUP                        8

#define MV_DRAM_MAX_CS                          4
#define MV_SPI_MAX_CS                           8
/* This define describes the maximum number of supported PCI\PCIX Interfaces */
#ifdef MV_INCLUDE_PCI
	#define MV_PCI_MAX_IF                   1
	#define MV_PCI_START_IF                 0
	#define PCI_HOST_BUS_NUM(pciIf)         (pciIf)
	#define PCI_HOST_DEV_NUM(pciIf)         0
#else
	#define MV_PCI_MAX_IF                   0
	#define MV_PCI_START_IF                 0
#endif

/* This define describes the maximum number of supported PEX Interfaces */

#define MV_PEX_MAX_IF                           4
#define MV_PEX_MAX_UNIT                         4
#define MV_PEX_MAX_UNIT_6810                    3
#ifdef MV_INCLUDE_PEX
#define MV_INCLUDE_PEX0
#define MV_DISABLE_PEX_DEVICE_BAR

#define MV_PEX_START_IF                         MV_PCI_MAX_IF
 #define PEX_HOST_BUS_NUM(pciIf)               (pciIf)
 #define PEX_HOST_DEV_NUM(pciIf)               0
#else
 #undef MV_INCLUDE_PEX0
#endif

#define PCI_IO(pciIf)                           (PEX0_IO + 2 * (pciIf))
#define PCI_MEM(pciIf, memNum)                  (PEX0_MEM0 + 2 * (pciIf))
/* This define describes the maximum number of supported PCI Interfaces         */
#define MV_IDMA_MAX_CHAN                        0
#define MV_DEVICE_MAX_CS                        4

#ifndef MV_USB_MAX_PORTS
#define MV_USB_MAX_PORTS			1
#endif
#define MV_USB3_MAX_PORTS			3
#define MV_USB3_MAX_PORTS_6810			2

/* CESA version #3: One channel, 2KB SRAM, TDMA, CHAIN Mode support */
#define MV_CESA_VERSION                         3 /*TODO verify */
#define MV_CESA_SRAM_SIZE                       (2 * 1024)

/* This define describes the maximum number of supported Ethernet ports */
/* TODO - verify all these numbers */
/* This define describes the maximum number of supported Ethernet ports */
#ifdef CONFIG_ARMADA_38X
#define MV_ETH_MAX_PORTS			3
#else
#define MV_ETH_MAX_PORTS                        4
#endif
#define MV_ETH_MAX_ON_BOARD_PORTS		2
#define MV_ETH_MAX_PORTS_6810			2
#define MV_ETH_VERSION				4 /* for Legacy mode */
#define MV_NETA_VERSION				1 /* for NETA mode */
#define MV_ETH_MAX_RXQ				8
#define MV_ETH_MAX_TXQ				8
#define MV_ETH_TX_CSUM_MAX_SIZE			9800
#define MV_ETH_TX_CSUM_MAX_SIZE_SMALL		2048
#define MV_PNC_TCAM_LINES			1024	/* TCAM num of entries */

/* New GMAC module is used */
#define MV_ETH_GMAC_NEW
/* New WRR/EJP module is used */
#define MV_ETH_WRR_NEW
/* IPv6 parsing support for Legacy parser */
#define MV_ETH_LEGACY_PARSER_IPV6

/* This define describes the the support of USB */
#define MV_USB_VERSION                          1

#define MV_SPI_VERSION                          2

#define MV_INCLUDE_SDRAM_CS0
#define MV_INCLUDE_SDRAM_CS1
#undef  MV_INCLUDE_SDRAM_CS2
#undef  MV_INCLUDE_SDRAM_CS3

#define MV_INCLUDE_DEVICE_CS0
#define MV_INCLUDE_DEVICE_CS1
#define MV_INCLUDE_DEVICE_CS2
#define MV_INCLUDE_DEVICE_CS3

#ifndef MV_ASMLANGUAGE

#define TBL_UNUSED      0       /* Used to mark unused entry */


/* This enumerator defines the Marvell Units ID      */
typedef enum {
	SLIC_EXTERNAL_ID,
	SLIC_ZARLINK_ID,
	SLIC_SILABS_ID,
	SLIC_LANTIQ_ID
} MV_SLIC_UNIT_TYPE;

typedef enum {
	TDM_UNIT_2CH,
	TDM_UNIT_8CH
} MV_TDM_UNIT_TYPE;

typedef enum _mvUnitId {
	DRAM_UNIT_ID,
	PEX_UNIT_ID,
	ETH_GIG_UNIT_ID,
	USB_UNIT_ID,
	USB3_UNIT_ID,	/* USB3.0 SerDes lanes */
	IDMA_UNIT_ID,
	XOR_UNIT_ID,
	SATA_UNIT_ID,
	TDM_UNIT_ID,
	UART_UNIT_ID,
	CESA_UNIT_ID,
	SPI_UNIT_ID,
	AUDIO_UNIT_ID,
	SDIO_UNIT_ID,
	TS_UNIT_ID,
	XPON_UNIT_ID,
	BM_UNIT_ID,
	PNC_UNIT_ID,
	I2C_UNIT_ID,
	QSGMII_UNIT_ID,
	XAUI_UNIT_ID,
	USB3_HOST_UNIT_ID, /* USB3.0 HOST ports */
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
	DEV_CS3 = 3,    /* Device connected to dev CS[2]    */
#endif
	BOOT_CS,        /* Device connected to BOOT dev    */
	MV_DEV_MAX_CS = MV_DEVICE_MAX_CS
} MV_DEVICE;

/* This enumerator described the possible Controller paripheral targets.    */
/* Controller peripherals are designated memory/IO address spaces that the  */
/* controller can access. They are also refered as "targets"                */
typedef enum _mvTarget {
	TBL_TERM = -1,	/* none valid target, used as targets list terminator*/
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
	PEX1_MEM,	/* 10 PCI Express 1 Memory	*/
	PEX1_IO,	/* 11 PCI Express 1 IO		*/
	PEX2_MEM,	/* 12 PCI Express 2 Memory		*/
	PEX2_IO,	/* 13 PCI Express 2 IO		*/
	PEX3_MEM,	/* 14 PCI Express 3 Memory		*/
	PEX3_IO,	/* 15 PCI Express 3 IO		*/
	INTER_REGS,	/* 16 Internal registers	*/
	DMA_UART,	/* 17 DMA based UART request	*/
	SPI0_CS0,	/* 18 SPI0_CS0			*/
	SPI0_CS1,	/* 19 SPI0_CS1			*/
	SPI0_CS2,	/* 20 SPI0_CS2			*/
	SPI0_CS3,	/* 21 SPI0_CS3			*/
	SPI1_CS0,	/* 22 SPI1_CS0			*/
	SPI1_CS1,	/* 23 SPI1_CS1			*/
	SPI1_CS2,	/* 24 SPI1_CS2			*/
	SPI1_CS3,	/* 25 SPI1_CS3			*/
	BOOT_ROM_CS,	/* 26 BOOT_ROM_CS		*/
	DEV_BOOCS,	/* 27 DEV_BOOCS			*/
	CRYPT0_ENG,	/* 28 Crypto0 Engine		*/
	CRYPT1_ENG,	/* 29 Crypto1 Engine		*/
	PNC_BM,		/* 30 PNC + BM (A38x)		*/
	PSS_PORT,	/* 31 PSS Ports	(A39x)		*/
	NSS_MAC_NIC,	/* 32 NSS MAC NIC (A39x)	*/
	NSS_MGMT,	/* 33 NSS Managment (A39x)	*/
	MAX_TARGETS
} MV_TARGET;

#ifdef AURORA_IO_CACHE_COHERENCY
#define DRAM_CS0_ATTR           0x1E
#define DRAM_CS1_ATTR           0x1D
#define DRAM_CS2_ATTR           0x1B
#define DRAM_CS3_ATTR           0x17
#else
#define DRAM_CS0_ATTR           0x0E
#define DRAM_CS1_ATTR           0x0D
#define DRAM_CS2_ATTR           0x0B
#define DRAM_CS3_ATTR           0x07
#endif

#define MAIN_BOOT_ATTR         0x1D    /* BootROM */
#define SEC_BOOT_ATTR          0x2F    /* Boot Device CS */

#define TARGETS_DEF_ARRAY {                                                 \
	{ DRAM_CS0_ATTR, DRAM_TARGET_ID },	/* SDRAM_CS0             */ \
	{ DRAM_CS1_ATTR, DRAM_TARGET_ID },	/* SDRAM_CS1             */ \
	{ DRAM_CS2_ATTR, DRAM_TARGET_ID },	/* SDRAM_CS0             */ \
	{ DRAM_CS3_ATTR, DRAM_TARGET_ID },	/* SDRAM_CS1             */ \
	{ 0x3E, DEV_TARGET_ID	},		/* DEVICE_CS0            */ \
	{ 0x3D, DEV_TARGET_ID	},		/* DEVICE_CS1            */ \
	{ 0x3B, DEV_TARGET_ID	},		/* DEVICE_CS2            */ \
	{ 0x37, DEV_TARGET_ID	},		/* DEVICE_CS3            */ \
	{ 0xE8, PEX_TARGET_ID_0	},		/* PEX0_MEM              */ \
	{ 0xE0, PEX_TARGET_ID_0	},		/* PEX0_IO               */ \
	{ 0xE8, PEX_TARGET_ID_123 },		/* PEX1_MEM              */ \
	{ 0xE0, PEX_TARGET_ID_123 },		/* PEX1_IO               */ \
	{ 0xD8, PEX_TARGET_ID_123 },		/* PEX2_MEM              */ \
	{ 0xD0, PEX_TARGET_ID_123 },		/* PEX2_IO               */ \
	{ 0xB8, PEX_TARGET_ID_123 },		/* PEX3_MEM              */ \
	{ 0xB0, PEX_TARGET_ID_123 },		/* PEX3_IO               */ \
	{ 0xFF, 0xFF		},		/* INTER_REGS            */ \
	{ 0x01, DEV_TARGET_ID	},		/* DMA_UART              */ \
	{ 0x1E, DEV_TARGET_ID	},		/* SPI0_CS0              */ \
	{ 0x5E, DEV_TARGET_ID	},		/* SPI0_CS1              */ \
	{ 0x9E, DEV_TARGET_ID	},		/* SPI0_CS2              */ \
	{ 0xDE, DEV_TARGET_ID	},		/* SPI0_CS3              */ \
	{ 0x1A, DEV_TARGET_ID	},		/* SPI1_CS0              */ \
	{ 0x5A, DEV_TARGET_ID	},		/* SPI1_CS1              */ \
	{ 0x9A, DEV_TARGET_ID	},		/* SPI1_CS2              */ \
	{ 0xDA, DEV_TARGET_ID	},		/* SPI1_CS3              */ \
	{ MAIN_BOOT_ATTR, DEV_TARGET_ID },	/* Main Boot device      */ \
	{ SEC_BOOT_ATTR, DEV_TARGET_ID  },	/* Secondary Boot device */ \
	{ 0x09, CRYPT_TARGET_ID	},		/* CRYPT_ENG0            */ \
	{ 0x05, CRYPT_TARGET_ID	},		/* CRYPT_ENG1            */ \
	{ 0x04, PNC_BM_NSS_TARGET_ID },		/* PNC_BM (A38x)	 */ \
	{ 0x04, PSS_PORTS_TARGET_ID },		/* PSS Ports (A39x)	 */ \
	{ 0x05, PNC_BM_NSS_TARGET_ID },		/* NSS MAC_NIC (A39x)	 */ \
	{ 0x04, PNC_BM_NSS_TARGET_ID }		/* NSS MGMT (A39x)	 */ \
}

#define CESA_TARGET_NAME_DEF    ("CRYPT_ENG0", "CRYPT_ENG1")
#define TARGETS_NAME_ARRAY      {			\
	"SDRAM_CS0",		/* SDRAM_CS0 */		\
	"SDRAM_CS1",		/* SDRAM_CS1 */		\
	"SDRAM_CS2",		/* SDRAM_CS1 */		\
	"SDRAM_CS3",		/* SDRAM_CS1 */		\
	"DEVICE_CS0",		/* DEVICE_CS0 */	\
	"DEVICE_CS1",		/* DEVICE_CS1 */	\
	"DEVICE_CS2",		/* DEVICE_CS2 */	\
	"DEVICE_CS3",		/* DEVICE_CS3 */	\
	"PEX0_MEM",		/* PEX0_MEM */		\
	"PEX0_IO",		/* PEX0_IO */		\
	"PEX1_MEM",		/* PEX1_MEM */		\
	"PEX1_IO",		/* PEX1_IO */		\
	"PEX2_MEM",		/* PEX2_MEM */		\
	"PEX2_IO",		/* PEX2_IO */		\
	"PEX3_MEM",		/* PEX3_MEM */		\
	"PEX3_IO",		/* PEX3_IO */		\
	"INTER_REGS",		/* INTER_REGS */	\
	"DMA_UART",		/* DMA_UART */		\
	"SPI0_CS0",		/* SPI0_CS0 */		\
	"SPI0_CS1",		/* SPI0_CS1 */		\
	"SPI0_CS2",		/* SPI0_CS2 */		\
	"SPI0_CS3",		/* SPI0_CS3 */		\
	"SPI1_CS0",		/* SPI1_CS0 */		\
	"SPI1_CS1",		/* SPI1_CS1 */		\
	"SPI1_CS2",		/* SPI1_CS2 */		\
	"SPI1_CS3",		/* SPI1_CS3 */		\
	"BOOT_ROM_CS",		/* BOOT_ROM_CS */	\
	"DEV_BOOTCS",		/* DEV_BOOCS */		\
	"CRYPT1_ENG",		/* CRYPT1_ENG */	\
	"CRYPT2_ENG",		/* CRYPT2_ENG */	\
	"PNC_BM",		/* PNC_BM */		\
	"PSS_PORTS",		/* PSS Ports */		\
	"NSS_MAC_NIC",		/* NSS Managment */	\
	"NSS_MGMT"		/* NSS Managment */	\
}

#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __INCmvCtrlEnvSpech */
