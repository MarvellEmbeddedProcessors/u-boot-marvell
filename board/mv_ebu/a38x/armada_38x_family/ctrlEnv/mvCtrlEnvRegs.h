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

/*
 * Power Management Clock (PMC) Gating Control Register
 */
#define POWER_MNG_CTRL_REG			0x18220

#define PMC_TDM_STOP_CLK_OFFS			25
#define PMC_TDM_STOP_CLK_MASK			(1 << PMC_TDM_STOP_CLK_OFFS)
#define PMC_TDM_STOP_CLK_EN			(1 << PMC_TDM_STOP_CLK_OFFS)
#define PMC_TDM_STOP_CLK_STOP			(0 << PMC_TDM_STOP_CLK_OFFS)

#define PMC_PEX_STOP_CLK_OFFS(port)		(((port) == 0) ? 8 : (4 + port))
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

#ifdef CONFIG_ARMADA_38X
#define PMC_SATA_STOP_CLK_OFFS(index)		(index == 0 ? 15 : 30)	/* bit 15 - sata0, bit 30 - sata1 */
#elif CONFIG_ARMADA_39X
#define PMC_SATA_STOP_CLK_OFFS(index)           (index < 2 ? 15 : 30)	/* bit 15 - sata0/1, bit 30 - sata2/3 */
#endif

#define PMC_SATA_STOP_CLK_MASK(index)		(1 << PMC_SATA_STOP_CLK_OFFS(index))
#define PMC_SATA_STOP_CLK_EN(index)		(1 << PMC_SATA_STOP_CLK_OFFS(index))
#define PMC_SATA_STOP_CLK_STOP(index)		(0 << PMC_SATA_STOP_CLK_OFFS(index))


#define MPP_CONTROL_REG(id)                     (0x18000 + (id * 4))

#define SGMII_PWR_PLL_CTRL_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0xE04)
#define SGMII_GEN_1_SET_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0xE34)
#define SGMII_DIG_LP_ENA_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0xE8C)
#define SGMII_REF_CLK_SEL_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0xF18)
#define SGMII_PHY_CTRL_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0xF20)
#define SGMII_SERDES_CFG_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0x4A0)
#define SGMII_SERDES_STAT_REG(port)		(MV_ETH_SGMII_PHY_REGS_BASE(port) + 0x4A4)

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET		(0x18600)
#define SATR_BOOT_SRC_OFFS		4
#define SATR_BOOT_SRC_MASK		(0x3F << SATR_BOOT_SRC_OFFS)
#define SATR_CPU_FREQ_OFFS		10
#define SATR_CPU_FREQ_MASK		(0x1F << SATR_CPU_FREQ_OFFS)
#define SATR_CPU1_ENABLE_OFFS		19
#define SATR_CPU1_ENABLE_MASK		(1 << SATR_CPU1_ENABLE_OFFS)

/* Core Divider Clock Control */
#define CORE_DIV_CLK_CTRL(num)		(0xE4250 + ((num) * 0x4))

#define CORE_DIVCLK_RELOAD_FORCE_OFFS		0
#define CORE_DIVCLK_RELOAD_FORCE_MASK		(0xFF << CORE_DIVCLK_RELOAD_FORCE_OFFS)
#define CORE_DIVCLK_RELOAD_FORCE_VAL		(0x2 << CORE_DIVCLK_RELOAD_FORCE_OFFS)

#define NAND_ECC_DIVCKL_RATIO_OFFS		8
#define NAND_ECC_DIVCKL_RATIO_MASK		(0x3F << NAND_ECC_DIVCKL_RATIO_OFFS)

#define CORE_DIVCLK_RELOAD_RATIO_OFFS		8
#define CORE_DIVCLK_RELOAD_RATIO_MASK		(1 << CORE_DIVCLK_RELOAD_RATIO_OFFS)


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

/* common PHys Selectors register */
#define COMM_PHY_SELECTOR_REG			0x183fc

#ifdef CONFIG_ARMADA_38X
#define COMPHY_SELECT_OFFS(x)			(x * 3)
#define COMPHY_SELECT_MASK(x)			(0x07 << COMPHY_SELECT_OFFS(x))
#define	PCIE0_X4_EN_OFFS			18
#define	PCIE0_X4_EN_MASK			(1 << PCIE0_X4_EN_OFFS)
#elif defined CONFIG_ARMADA_39X
#define COMPHY_SELECT_OFFS(x)                   (x * 4)
#define COMPHY_SELECT_MASK(x)                   (0x0F << COMPHY_SELECT_OFFS(x))
#define PCIE0_X4_EN_OFFS                        31
#define PCIE0_X4_EN_MASK                        (1 << PCIE0_X4_EN_OFFS)
#endif


/*****************/
/* PUP registers */
/*****************/
#define PUP_EN_REG                              0x1864C
/*****************/
/* General Purpose registers */
/*****************/
#define GENERAL_PURPOSE_RESERVED1_REG		0x182E4
#define GENERAL_PURPOSE_RESERVED1_DEFAULT_VALUE	(~BIT17)

/* USB3 registers */
#define MV_USB3_WIN_BASE(dev)		(MV_USB3_REGS_BASE(dev) + 0x4000)

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
	MSAR_0_BOOT_I2C,
	MSAR_0_BOOT_SDIO
} MV_BOARD_BOOT_SRC;

typedef struct _mvSATRBootTable {
	MV_BOARD_BOOT_SRC bootSrc;
	MV_U32 attr1;                           /* Device width/Port */
	MV_U32 attr2;                           /* ALE TIming Parameters/Page Size/Serdes Lane/Address cycles */
	MV_U32 attr3;                           /* Dev_Wen Dev_Oen Muxed/Address cycle/SPI interface */
	char name[80];
} MV_SATR_BOOT_TABLE;

#define BOOT_SRC_TABLE_SIZE 0x3A
#define MV_SATR_BOOT_SRC_TABLE_VAL { \
/*00*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,  0, 0, "NOR 8 bit bus"}, \
/*01*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,  0, 1, \
		"NOR 8 bit bus, WEN/OEN MUXed with ADDR[16:15]"}, \
/*02*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,  0, 0, "NOR 8 bit bus, BootROM Disabled"}, \
/*03*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,  0, 1, \
		"NOR 8 bit bus, BootROM Disabled, WEN/OEN MUXed with ADDR[16:15]"}, \
/*04*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0, "NOR 16 bit bus"}, \
/*05*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 1, \
		"NOR 16 bit bus, WEN/OEN MUXed with ADDR[16:15]"}, \
/*06*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0, "NOR 16 bit bus, BootROM Disabled"}, \
/*07*/ { MSAR_0_BOOT_NOR_FLASH, MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 1, \
		"NOR 16 bit bus, BootROM Disabled, WEN/OEN MUXed with ADDR[16:15]"}, \
/*08*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, 3, \
		"NAND 8 bit bus, Page size 512B, 3 address cycles, 1 ECC bits per page"}, \
/*09*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, 4, \
		"NAND 8 bit bus, Page size 512B, 4 address cycles, 1 ECC bits per page"}, \
/*0A*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 8 bit bus, Page size 2KB,  4 ECC bits per page"}, \
/*0B*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 8 bit bus, Page size 2KB,  8 ECC bits per page "}, \
/*0C*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 8 bit bus, Page size 2KB, 12 ECC bits per page"}, \
/*0D*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 8 bit bus, Page size 2KB, 16 ECC bits per page"}, \
/*0E*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 8 bit bus, Page size 4KB,  4 ECC bits per page"}, \
/*0F*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 8 bit bus, Page size 4KB,  8 ECC bits per page"}, \
/*10*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 8 bit bus, Page size 4KB, 12 ECC bits per page"}, \
/*11*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 8 bit bus, Page size 4KB, 16 ECC bits per page"}, \
/*12*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 8 bit bus, Page size 8KB,  4 ECC bits per page"}, \
/*13*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 8 bit bus, Page size 8KB,  8 ECC bits per page"}, \
/*14*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 8 bit bus, Page size 8KB, 12 ECC bits per page"}, \
/*15*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 8 bit bus, Page size 8KB, 16 ECC bits per page"}, \
/*16*/ { -1, -1, -1, -1},	\
/*17*/ { -1, -1, -1, -1}, \
/*18*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_512B, 3		       , \
		"NAND 16 bit bus, Page size 512B, 3 address cycles, 1 ECC bit per page"}, \
/*19*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_512B, 4		       , \
		"NAND 16 bit bus, Page size 512B, 4 address cycles, 1 ECC bits per page"}, \
/*1A*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 16 bit bus, Page size 2KB,  4 ECC bits per page"}, \
/*1B*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 16 bit bus, Page size 2KB,  8 ECC bits per page"}, \
/*1C*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 16 bit bus, Page size 2KB, 12 ECC bits per page"}, \
/*1D*/ { MSAR_0_BOOT_NAND_NEW,	MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 16 bit bus, Page size 2KB, 16 ECC bits per page"}, \
/*1E*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 16 bit bus, Page size 4KB,  4 ECC bits per page"}, \
/*1F*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 16 bit bus, Page size 4KB,  8 ECC bits per page"}, \
/*20*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 16 bit bus, Page size 4KB, 12 ECC bits per page"},\
/*21*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 16 bit bus, Page size 4KB, 16 ECC bits per page"},\
/*22*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_4BIT , \
		"NAND 16 bit bus, Page size 8KB,  4 ECC bits per page"},\
/*23*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_8BIT , \
		"NAND 16 bit bus, Page size 8KB,  8 ECC bits per page"},\
/*24*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_12BIT, \
		"NAND 16 bit bus, Page size 8KB, 12 ECC bits per page"},\
/*25*/ { MSAR_0_BOOT_NAND_NEW,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_16BIT, \
		"NAND 16 bit bus, Page size 8KB, 16 ECC bits per page"},\
/*26*/ { -1, -1, -1, -1}, \
/*27*/ { -1, -1, -1, -1}, \
/*28*/ { MSAR_0_BOOT_UART, 0,  0,  1, "UART, Port 0, MPP[1:0]"}, /* port, mpp, mpp */ \
/*29*/ { MSAR_0_BOOT_UART, 1, 56, 55, "UART, Port 1, MPP[56:55]"}, /* port, mpp, mpp */  \
/*2A*/ { MSAR_0_BOOT_SATA, 0,  0, -1, "SATA 0, Lane 0 "}, /* port, lane */  \
/*2B*/ { MSAR_0_BOOT_SATA, 0,  1, -1, "SATA 0, Lane 1 "}, /* port, lane */ \
/*2C*/ { MSAR_0_BOOT_PEX,  0,  0, -1, "PCIe 0, Lane 0 "}, /* port, lane */ \
/*2D*/ { MSAR_0_BOOT_PEX,  0,  1, -1, "PCIe 0, Lane 1 "}, /* port, lane */ \
/*2E*/ { MSAR_0_BOOT_I2C,  0,  3,  2, "I2C, MPP[3:2]"}, /* port, mpp, mpp */ \
/*2F*/ { -1, -1, -1, -1}, \
/*30*/ { MSAR_0_BOOT_SDIO,		0,		59,		54		, \
		"SDIO, Port 0, MPP[59:54]"}, /* port, mpp, mpp */ \
/*31*/ { MSAR_0_BOOT_SDIO,		0,		40,		37		, \
		"SDIO, Port 0, MPP[40:37] and MPP[28:24]]"}, /* port, mpp, mpp */ \
/*32*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24BIT, 25, \
		"SPI channel 0, 24 bit address, MPP[25:22]"}, \
/*33*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 25, \
		"SPI channel 0, 32 bit address, MPP[25:22]"}, \
/*34*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24BIT, 59, \
		"SPI channel 1, 24 bit address, MPP[59:56]"}, \
/*35*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 59, \
		"SPI channel 1, 32 bit address, MPP[59:56]"}, \
/*36*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24BIT, 25, \
		"SPI channel 0, 24 bit address, MPP[25:22], BootROM Disabled"}, \
/*37*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 25, \
		"SPI channel 0, 32 bit address, MPP[25:22], BootROM Disabled"}, \
/*38*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24BIT, 25, \
		"SPI channel 1, 24 bit address, MPP[59:56], BootROM Disabled"}, \
/*39*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 25, \
		"SPI channel 1, 32 bit address, MPP[59:56], BootROM Disabled"} \
}

typedef struct {
	MV_U8 id;
	MV_U32 cpuFreq;
	MV_U32 ddrFreq;
	MV_U32 l2Freq;
	MV_BOOL	isDisplay;
} MV_FREQ_MODE;

/* End of Table indicator - Should be in the last line of the SAR Table */
#define MV_SAR_FREQ_MODES_EOT		0xFF

#define MV_SAR_FREQ_MODES { \
		{ 0x4,   1066, 533, 533, MV_TRUE  }, \
		{ 0x6,   1200, 600, 600, MV_TRUE  }, \
		{ 0x8,   1332, 666, 666, MV_TRUE  }, \
		{ 0xC,   1600, 800, 800, MV_TRUE  }, \
		{ MV_SAR_FREQ_MODES_EOT,  0,    0,   0, MV_FALSE } \
};

/* These macros help units to identify a target Mport Arbiter group */
#define MV_TARGET_IS_DRAM(target)   \
	((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)   \
	((target >= PEX0_MEM) && (target <= PEX0_IO))
#define MV_TARGET_IS_PEX1(target)   \
	((target >= PEX1_MEM) && (target <= PEX1_IO))
#define MV_TARGET_IS_PEX2(target)   \
		((target >= PEX2_MEM) && (target <= PEX2_IO))
#define MV_TARGET_IS_PEX3(target)   \
		((target >= PEX3_MEM) && (target <= PEX3_IO))

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
	PEX_TARGET_ID_123  = 4,  /* Port 4  -> PCI Express 0 and 1        */
	PEX_TARGET_ID_0   = 8,  /* Port 4  -> PCI Express 0 and 1        */
	CRYPT_TARGET_ID  = 9,  /* Port 9  -> Crypto Engine SRAM         */
	PSS_PORTS_TARGET_ID = 0xB,   /* Port 11 -> PSS Ports (A39x)     */
	PNC_BM_NSS_TARGET_ID = 0xC, /* Port 12 -> PP2 Unit For A38x
					or NSS for A39x */
	MAX_TARGETS_ID
} MV_TARGET_ID;



#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
