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

#ifndef __INCmvCtrlEnvRegsh
#define __INCmvCtrlEnvRegsh

#include "mvTypes.h"
#include "mvCtrlEnvSpec.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CV Support */
#define PEX0_MEM0       PEX0_MEM
#define PCI0_MEM0       PEX0_MEM

/* Controller revision info */
#define PCI_CLASS_CODE_AND_REVISION_ID          0x008
#define PCCRIR_REVID_OFFS                       0               /* Revision ID */
#define PCCRIR_REVID_MASK                       (0xff << PCCRIR_REVID_OFFS)

/* Controler environment registers offsets */
#define MV_TDM_IRQ_NUM                          56

/* CIB registers offsets */
#define MV_CIB_CTRL_CFG_REG                     (MV_COHERENCY_FABRIC_OFFSET + 0x80)

/*
 * SoC Device Multiplex Register
 */
#define SOC_DEV_MUX_REG				0x18208

#define SATAHC_LED_CONFIGURATION_REG		(MV_SATA_REGS_OFFSET + 0x2c)

/*
 * Power Management Clock (PMC) Gating Control Register
 */
#define POWER_MNG_CTRL_REG			0x18220

#define PMC_TDM_STOP_CLK_OFFS			25
#define PMC_TDM_STOP_CLK_MASK			(1 << PMC_TDM_STOP_CLK_OFFS)
#define PMC_TDM_STOP_CLK_EN			(1 << PMC_TDM_STOP_CLK_OFFS)
#define PMC_TDM_STOP_CLK_STOP			(0 << PMC_TDM_STOP_CLK_OFFS)

#define PMC_PEX_STOP_CLK_OFFS(port)		((port) + 5)
#define PMC_PEX_STOP_CLK_MASK(port)		(1 << PMC_PEX_STOP_CLK_OFFS(port))
#define PMC_PEX_STOP_CLK_EN(port)		(1 << PMC_PEX_STOP_CLK_OFFS(port))
#define PMC_PEX_STOP_CLK_STOP(port)		(0 << PMC_PEX_STOP_CLK_OFFS(port))

#define PMC_USB_STOP_CLK_OFFS			18
#define PMC_USB_STOP_CLK_MASK			(1 << PMC_USB_STOP_CLK_OFFS)
#define PMC_USB_STOP_CLK_EN			(1 << PMC_USB_STOP_CLK_OFFS)
#define PMC_USB_STOP_CLK_STOP			(0 << PMC_USB_STOP_CLK_OFFS)

#define PMC_SDIO_STOP_CLK_OFFS			17
#define PMC_SDIO_STOP_CLK_MASK			(1 << PMC_SDIO_STOP_CLK_OFFS)
#define PMC_SDIO_STOP_CLK_EN			(1 << PMC_SDIO_STOP_CLK_OFFS)
#define PMC_SDIO_STOP_CLK_STOP			(0 << PMC_SDIO_STOP_CLK_OFFS)

#define PMC_XOR_STOP_CLK_OFFS(xor_id)		(xor_id == 0 ? 22 : 28)
#define PMC_XOR_STOP_CLK_MASK(xor_id)		(1 << PMC_XOR_STOP_CLK_OFFS(xor_id))
#define PMC_XOR_STOP_CLK_EN(xor_id)		(1 << PMC_XOR_STOP_CLK_OFFS(xor_id))
#define PMC_XOR_STOP_CLK_STOP(xor_id)		(0 << PMC_XOR_STOP_CLK_OFFS(xor_id))

#define PMC_SATA_STOP_CLK_OFFS			14
#define PMC_SATA_STOP_CLK_MASK			(3 << PMC_SATA_STOP_CLK_OFFS)
#define PMC_SATA_STOP_CLK_EN			(3 << PMC_SATA_STOP_CLK_OFFS)
#define PMC_SATA_STOP_CLK_STOP			(0 << PMC_SATA_STOP_CLK_OFFS)

#define PMC_CESA_STOP_CLK_OFFS(port)		(port == 0 ? 28 : 29)
#define PMC_CESA_STOP_CLK_MASK(port)		(0x5 << PMC_CESA_STOP_CLK_OFFS(port))
#define PMC_CESA_STOP_CLK_EN(port)		(0x5 << PMC_CESA_STOP_CLK_OFFS(port))
#define PMC_CESA_STOP_CLK_STOP(port)		(0 << PMC_CESASTOPMEM_OFFS)

#define MPP_CONTROL_REG(id)                     (0x18000 + (id * 4))

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)		(0xE8200 + ( id * 0x4 ))
#define SATR_DEVICE_ID_2_0_OFFS		21
#define SATR_DEVICE_ID_2_0_MASK		(3 << SATR_DEVICE_ID_2_0_OFFS)

/* Core Divider Clock Control */
#define CORE_DIV_CLK_CTRL(num)		(0xE8250 + ((num) * 0x4))

#define CORE_DIVCLK_RELOAD_FORCE_OFFS		0
#define CORE_DIVCLK_RELOAD_FORCE_MASK		(0xFF << CORE_DIVCLK_RELOAD_FORCE_OFFS)
#define CORE_DIVCLK_RELOAD_FORCE_VAL		(0x2 << CORE_DIVCLK_RELOAD_FORCE_OFFS)

#define NAND_ECC_DIVCKL_RATIO_OFFS		8
#define NAND_ECC_DIVCKL_RATIO_MASK		(0x3F << NAND_ECC_DIVCKL_RATIO_OFFS)

#define CORE_DIVCLK_RELOAD_RATIO_OFFS		8
#define CORE_DIVCLK_RELOAD_RATIO_MASK		(1 << CORE_DIVCLK_RELOAD_RATIO_OFFS)

#define SERDES_LANE_OFFS(num)		(num <= 1 ? num : (num + 1))
#define SERDES_LANE_MASK(num)		((num == 1 ? 3 : 1) << SERDES_LANE_OFFS(num))

#define DEV_ID_REG			0x18238
#define VENDOR_ID_OFFS			0
#define VENDOR_ID_MASK			0xFFFF
#define DEVICE_ID_OFFS			16
#define DEVICE_ID_MASK			0xFFFF0000
#define DEV_VERSION_ID_REG		0x1823C
#define REVISON_ID_OFFS			8
#define REVISON_ID_MASK			0xF00

/* SYSRSTn Length Counter */
#define SYSRST_LENGTH_COUNTER_REG               0x18250
#define SLCR_COUNT_OFFS                         0
#define SLCR_COUNT_MASK                         (0x1FFFFFFF << SLCR_COUNT_OFFS)
#define SLCR_CLR_OFFS                           31
#define SLCR_CLR_MASK                           (1 << SLCR_CLR_OFFS)

/* Device ID */
#define CHIP_BOND_REG                           0x18238
#define PCKG_OPT_MASK                           0x3

/* LED Matrix Control */
#define LED_MATRIX_CONTROL_REG(unit)		(MV_MISC_REGS_OFFSET + 0xf0 + (unit * 4))

/*****************/
/* PUP registers */
/*****************/
#define PUP_EN_REG                              0x1864C

/* Extract CPU, L2, DDR clocks SAR value from
** SAR bits 24-27
*/

#ifndef MV_ASMLANGUAGE

typedef enum {
	MSAR_0_BOOT_NOR_FLASH,
	MSAR_0_BOOT_NAND_NEW,
	MSAR_0_BOOT_UART,
	MSAR_0_BOOT_SPI_FLASH,
	MSAR_0_BOOT_PEX,
	MSAR_0_BOOT_SATA,
	MSAR_0_BOOT_PROMPT,
	MSAR_0_BOOT_SPI1_FLASH,
	MSAR_0_BOOT_NAND_SPI
} MV_BOARD_BOOT_SRC;

typedef struct _mvSATRBootTable {
	MV_BOARD_BOOT_SRC bootSrc;
	MV_U32 attr1;                           /* Device width/Port */
	MV_U32 attr2;                           /* ALE TIming Parameters/Page Size/Serdes Lane/Address cycles */
	MV_U32 attr3;                           /* Dev_Wen Dev_Oen Muxed/Address cycle/SPI interface */
} MV_SATR_BOOT_TABLE;

#define MV_SATR_TABLE_VAL { \
/*00*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*01*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*02*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*03*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*04*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*05*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*06*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*07*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*08*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0 }, \
/*09*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*10*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0 }, \
/*11*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*12*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 4, 0 }, \
/*13*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*14*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 5, 0 }, \
/*15*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 5, 1 }, \
/*16*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, MSAR_0_NAND_ECC_4BIT }, \
/*17*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, MSAR_0_NAND_ECC_4BIT }, \
/*18*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_4BIT },	\
/*19*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_8BIT },	\
/*20*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_12BIT }, \
/*21*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_16BIT }, \
/*22*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_4BIT },	\
/*23*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_8BIT },	\
/*24*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_12BIT }, \
/*25*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_16BIT }, \
/*26*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_4BIT },	\
/*27*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_8BIT },	\
/*28*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_12BIT }, \
/*29*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_16BIT }, \
/*30*/ { 0,           5,            60,          60           }, \
/*31*/ { 0,           2,            40,          40           }, \
/*32*/ {-1, -1, -1, -1 },\
/*33*/ {-1, -1, -1, -1 },\
/*34*/ {-1, -1, -1, -1 },\
/*35*/ {-1, -1, -1, -1 },\
/*36*/ {-1, -1, -1, -1 },\
/*37*/ {-1, -1, -1, -1 },\
/*38*/ { 0,           1,            2,            2              }, \
/*39*/ { 0,           3,            6,            6              }, \
/*40*/ { 0,           3,            5,            5              }, \
/*41*/ { 0,           2,            6,            3              }, \
/*42*/ { 0,           4,            10,          5              }, \
/*43*/ { 0,           3,            6,            6              }, \
/*44*/ { MSAR_0_BOOT_NAND_SPI,           2,            4,            4              }, \
/*45*/ { 0,           3,            6,            3              }, \
/*46*/ { 0,           2,            5,            5              }, \
/*47*/ { MSAR_0_BOOT_PROMPT,         2,            5,            5              }, \
/*48*/ { MSAR_0_BOOT_UART,               1,            3,            3              }, \
/*49*/ { MSAR_0_BOOT_SATA,                5,            10,          10           }, \
/*50*/ { MSAR_0_BOOT_PEX,   3,            8,            4              }, \
/*51*/ { MSAR_0_BOOT_PEX,   1,            2,            1              }, \
/*52*/ { 0,           3,            6,            3              }, \
/*53*/ { 0,           2,            8,            4              }, \
/*54*/ { 0,           5,            10,          5              }, \
/*55*/ { 0,           1,            20,          20           }, \
/*56*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*57*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*58*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*59*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*60*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*61*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*62*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*63*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 40               } \
}

typedef struct {
	MV_U8 id;
	MV_U32 cpuFreq;
	MV_U32 ddrFreq;
	MV_U32 l2Freq;
} MV_FREQ_MODE;

/* PLL(CPU) input clock */
#define MV_PLL_IN_CLK	_1GHz

#define MV_USER_SAR_FREQ_MODES { \
		{ 6,  400,  400, 200 }, \
		{ 14, 600,  400, 300 }, \
		{ 21, 800,  534, 400 }, \
		{ 25, 1000, 500, 500 }, \
};

/* These macros help units to identify a target Mport Arbiter group */
#define MV_TARGET_IS_DRAM(target)   \
	((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)   \
	((target >= PEX0_MEM) && (target <= PEX0_IO))
#define MV_TARGET_IS_PEX1(target)   \
	((target >= PEX1_MEM) && (target <= PEX1_IO))

#define MV_TARGET_IS_PEX(target)        ((target >= PEX0_MEM) && (target <= PEX1_IO))

#define MV_TARGET_IS_DEVICE(target)     ((target >= DEVICE_CS0) && (target <= DEVICE_CS3))

#define MV_CHANGE_BOOT_CS(target) target

#define START_DEV_CS            DEV_CS0
#define DEV_TO_TARGET(dev)      ((dev) + START_DEV_CS)

#define PCI_IF0_MEM0            PEX0_MEM

/* This enumerator defines the Marvell controller target ID (see Address map) */
typedef enum _mvTargetId {
	DRAM_TARGET_ID   = 0,  /* Port 0  -> DRAM interface             */
	DEV_TARGET_ID    = 1,  /* Port 1  -> Device bus, BootROM, SPI, UART,
				*	     GPIO, MPP, and Miscellaneous */
	USB3_TARGET_ID   = 5,  /* Port 5  -> USB3 Unit,                 */
	PEX_TARGET_ID    = 4,  /* Port 4  -> PCI Express 0 and 1        */
	CRYPT_TARGET_ID  = 9,  /* Port 9  -> Crypto Engine SRAM         */
	PP2_TARGET_ID = 15,    /* Port 12 -> PP2 Unit                   */
	MAX_TARGETS_ID
} MV_TARGET_ID;

/*
        This enum should reflect the units numbers in register
        space which we will need when accessing the HW
 */

typedef enum {
	PEX_0		= 0,
	PEX_1		= 1,
	PEXIF_MAX	= 2
} MV_PEXIF_INDX;

#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
