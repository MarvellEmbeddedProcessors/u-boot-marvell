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
#include "mvCommon.h"
#include "mvBoardEnvLib.h"
#include "mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPexRegs.h"

#define ARRSZ(x)                (sizeof(x) / sizeof(x[0]))

MV_BOARD_SATR_INFO boardSatrInfo[] = {
/*{{MV_SATR_TYPE_ID SarID,	Mask	Offset, regNum, isActiveForBoard[]}*/
{ MV_SATR_CPU_DDR_L2_FREQ,	 0x003E0000,	17,	1, {1, 1, 1, 1} },
{ MV_SATR_CORE_CLK_SELECT,	 0x00400000,	22,	1, {1, 0, 0, 0} },
{ MV_SATR_CPU1_ENABLE,		 0x00008000,	15,	0, {1, 0, 0, 0} },
{ MV_SATR_SSCG_DISABLE,		 0x00000002,	1,	0, {1, 0, 0, 0} },
{ MV_SATR_I2C0_SERIAL_ROM,	 0X00000001,	0,	0, {1, 0, 0, 0} },
{ MV_SATR_EXTERNAL_CPU_RESET,	 0X00000000,	0,	0, {1, 0, 0, 0} },
{ MV_SATR_EXTERNAL_CORE_RESET,	 0X00000000,	0,	0, {1, 0, 0, 0} },
{ MV_SATR_BOOT_DEVICE,		 0X000001F8,	3,	0, {1, 1, 1, 1} },
{ MV_SATR_CPU_PLL_XTAL_BYPASS,	 0x00000200,	9,	0, {1, 0, 0, 0} },
{ MV_SATR_PEX0_CLOCK,		 0x00000400,	10,	0, {1, 0, 0, 0} },
{ MV_SATR_PEX1_CLOCK,		 0x00000800,	11,	0, {1, 0, 0, 0} },
{ MV_SATR_REF_CLOCK_ENABLE,	 0x00000004,	2,	0, {1, 0, 0, 0} },
{ MV_SATR_TESTER_OPTIONS,	 0x00080000,	19,	0, {1, 0, 0, 0} },
{ MV_SATR_CPU0_ENDIANESS,	 0x00001000,	12,	0, {1, 0, 0, 0} },
{ MV_SATR_CPU0_NMFI,		 0x00002000,	13,	0, {1, 0, 0, 0} },
{ MV_SATR_CPU0_THUMB,		 0x00004000,	14,	0, {1, 0, 0, 0} },
{ MV_SATR_EFUSE_BYPASS,		 0x00020000,	17,	0, {1, 0, 0, 0} },
{ MV_SATR_POR_BYPASS,		 0x00100000,	20,	0, {1, 0, 0, 0} },
{ MV_SATR_BOARD_ID,		 0x000000F0,	4,	1, {1, 0, 0, 0} },
{ MV_SATR_WRITE_CPU_FREQ,	 0X0000001F,	0,	0, {1, 0, 1, 0} },
{ MV_SATR_WRITE_CORE_CLK_SELECT, 0x00000001,	0,	1, {1, 0, 1, 0} },
{ MV_SATR_WRITE_CPU1_ENABLE,	 0x00000002,	1,	1, {1, 0, 1, 0} },
{ MV_SATR_WRITE_SSCG_DISABLE,	 0x00000004,	2,	1, {1, 0, 1, 0} },
};

MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = {
/* {{MV_CONFIG_TYPE_ID ConfigID, MV_U32 Mask,  Offset, expanderNum,  regNum,    isActiveForBoard[]}} */
	{ MV_CONFIG_MAC0,	       0x3,	0,	 0,		0, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#0, BITS [0:1] */
	{ MV_CONFIG_MAC1,	       0xC,	2,	 0,		0, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#0, BITS [2:3] */
	{ MV_CONFIG_PON_SERDES,	       0x10,	4,	 0,		0, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#0, BITS [4]   */
	{ MV_CONFIG_PON_BEN_POLARITY,  0x20,	5,	 0,		0, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#0, BITS [5]   */
	{ MV_CONFIG_SGMII0_CAPACITY,   0x40,	6,	 0,		0, 	{ 1, 0, 0, 1 } }, /* Exp#0, Reg#0, BITS [6]   */
	{ MV_CONFIG_SGMII1_CAPACITY,   0x80,	7,	 0,		0, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#0, BITS [7]   */
	{ MV_CONFIG_SLIC_TDM_DEVICE,   0x3,	0,	 0,		1, 	{ 1, 0, 1, 1 } }, /* Exp#0, Reg#1, BITS [0:1] */
	{ MV_CONFIG_LANE1,	       0xC,	2,	 0,		1, 	{ 1, 0, 0, 1 } }, /* Exp#0, Reg#1, BITS [2:3] */
	{ MV_CONFIG_LANE2,	       0x10,	4,	 0,		1, 	{ 1, 0, 0, 1 } }, /* Exp#0, Reg#1, BITS [4]   */
	{ MV_CONFIG_LANE3,	       0X20,	5,	 0,		1, 	{ 1, 0, 0, 1 } }, /* Exp#0, Reg#1, BITS [5:6] */
	{ MV_CONFIG_DEVICE_BUS_MODULE, 0x3,	0,	 1,		0, 	{ 1, 0, 0, 1 } }, /* Exp#1, Reg#0, BITS [0:1] */
};

MV_BOARD_IO_EXPANDER_TYPE_INFO db88f6660InfoBoardIOExpanderInfo[] = {
/* {{MV_CONFIG_TYPE_ID ConfigID,      MV_U32 Offset,	 expanderNum,  regNum,   }} */
		/* 1st IO Expander Register*/
	{ MV_IO_EXPANDER_SFP0_TX_DIS,		 0,		 1,	 1},
	{ MV_IO_EXPANDER_SFP0_PRSNT,		 1,		 1,	 1},
	{ MV_IO_EXPANDER_SFP0_TX_FAULT,		 2,		 1,	 1},
	{ MV_IO_EXPANDER_SFP0_LOS,		 3,		 1,	 1},
	{ MV_IO_EXPANDER_USB_VBUS,		 4,		 1,	 1},
	{ MV_IO_EXPANDER_MAC0_RJ45_PORT_LED,	 5,		 1,	 1},
	{ MV_IO_EXPANDER_MAC0_SFP_PORT_LED,	 6,		 1,	 1},
	{ MV_IO_EXPANDER_PON_PORT_LED,		 7,		 1,	 1},
		/* 2nd IO Expander Register*/
	{ MV_IO_EXPANDER_SD_STATUS,		 0,		 2,	 0},
	{ MV_IO_EXPANDER_SD_WRITE_PROTECT,	 1,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_PRSNT,		 2,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_TX_FAULT,		 3,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_LOS,		 4,		 2,	 0},
	{ MV_IO_EXPANDER_JUMPER1,		 6,		 2,	 0},
	{ MV_IO_EXPANDER_JUMPER2_EEPROM_ENABLED, 7,		 2,	 0},
		/* 3rd IO Expander Register*/
	{ MV_IO_EXPANDER_EXT_PHY_SMI_EN,	 0,		 2,	 1},
	{ MV_IO_EXPANDER_SFP1_TX_DIS,		 1,		 2,	 1},
	{ MV_IO_EXPANDER_SPI1_CS_MSB0,		 2,		 2,	 1},
	{ MV_IO_EXPANDER_SPI1_CS_MSB1,		 3,		 2,	 1},
	{ MV_IO_EXPANDER_MAC1_SFP_PORT_LED,	 4,		 2,	 1},
	{ MV_IO_EXPANDER_MAC1_RJ45_PORT_LED,	 5,		 2,	 1},
	{ MV_IO_EXPANDER_INTEG_PHY_PORTS_LED,	 6,		 2,	 1},
	{ MV_IO_EXPANDER_USB_SUPER_SPEED,	 7,		 2,	 1},
};

MV_BOARD_IO_EXPANDER_TYPE_INFO db88f6650InfoBoardIOExpanderInfo[] = {
/* {{MV_CONFIG_TYPE_ID ConfigID,      MV_U32 Offset,	 expanderNum,  regNum,   }} */
		/* 2nd IO Expander Register*/
	{ MV_IO_EXPANDER_USB_VBUS,		 0,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_PRSNT,		 2,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_TX_FAULT,		 3,		 2,	 0},
	{ MV_IO_EXPANDER_SFP1_LOS,		 4,		 2,	 0},
	{ MV_IO_EXPANDER_JUMPER2_EEPROM_ENABLED, 6,		 2,	 0},
	{ MV_IO_EXPANDER_JUMPER3,		 7,		 2,	 0},
		/* 3rd IO Expander Register*/
	{ MV_IO_EXPANDER_EXT_PHY_SMI_EN,	 0,		 2,	 1},
	{ MV_IO_EXPANDER_SFP1_TX_DIS,		 1,		 2,	 1},
	{ MV_IO_EXPANDER_MAC0_RJ45_PORT_LED,	 2,		 2,	 1},
	{ MV_IO_EXPANDER_PON_PORT_LED,		 3,		 2,	 1},
	{ MV_IO_EXPANDER_MAC1_SFP_PORT_LED,	 4,		 2,	 1},
	{ MV_IO_EXPANDER_MAC1_RJ45_PORT_LED,	 5,		 2,	 1},
	{ MV_IO_EXPANDER_INTEG_PHY_PORTS_LED,	 6,		 2,	 1},
};

/*******************************************************************************
 * AvantaLP DB-88F6660 board */
/*******************************************************************************/

MV_BOARD_TWSI_INFO db88f6660InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		1,	0x4E,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_EEPROM,	0,	0x54,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_IO_EXPANDER,	0,	0x21,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_IO_EXPANDER,	1,	0x22,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_IO_EXPANDER,	2,	0x24,	   ADDR7_BIT	},
};
MV_BOARD_MAC_INFO db88f6660InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_1000M, -1									},
	{ BOARD_MAC_SPEED_AUTO, 0x1									},
	{ N_A,			N_A									}
};
MV_BOARD_MPP_TYPE_INFO db88f6660InfoBoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_AUTO,
		.ethSataComplexOpt = (MV_ETHCOMP_GE_MAC0_2_SW_P6 | MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_SW_P0_2_GE_PHY_P0 | \
				MV_ETHCOMP_SW_P3_2_GE_PHY_P3 | MV_ETHCOMP_SW_P4_2_RGMII0),
		.ethPortsMode = 0x0
	}
};

MV_DEV_CS_INFO db88f6660InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 } /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6660InfoBoardMppConfigValue[] = {
	{ {
		  DB_88F6660_MPP0_7,
		  DB_88F6660_MPP8_15,
		  DB_88F6660_MPP16_23,
		  DB_88F6660_MPP24_31,
		  DB_88F6660_MPP32_39,
		  DB_88F6660_MPP40_47,
		  DB_88F6660_MPP48_55,
		  DB_88F6660_MPP56_63,
		  DB_88F6660_MPP64_67,
	 } }
};

MV_BOARD_SWITCH_INFO db88f6660InfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {0, 1, 2, 3, 4, -1, -1},
	 .cpuPort = 6,
	 .connectedPort = {6, -1},
	 .internalQuadPhyAddr = 0,
	 .connectedPortMask= ( BIT0| BIT1| BIT2| BIT3| BIT4| BIT6),
	 .forceLinkMask = 0x0
	 }
};

MV_BOARD_INFO db88f6660_board_info = {
	.boardName			= "DB-88F6660",
	.numBoardMppTypeValue		= ARRSZ(db88f6660InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f6660InfoBoardModTypeInfo,
	.pBoardMppConfigValue		= db88f6660InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f6660InfoBoardDeCsInfo),
	.pDevCsInfo			= db88f6660InfoBoardDeCsInfo,
	.numBoardIoExpanderInfo		= ARRSZ(db88f6660InfoBoardIOExpanderInfo),
	.pBoardIoExpanderInfo		= db88f6660InfoBoardIOExpanderInfo,
	.numBoardTwsiDev		= ARRSZ(db88f6660InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f6660InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f6660InfoBoardMacInfo),
	.pBoardMacInfo			= db88f6660InfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6660_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6660_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6660_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6660_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6660_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6660_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6660_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6660_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6660_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6660InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6660InfoBoardSwitchValue),

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
	.nandFlashControl		= 0,
	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE
};

/*******************************************************************************
 * AvantaLP DB-88F6650 board */
/*******************************************************************************/

MV_BOARD_TWSI_INFO db88f6650InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	ADDR7_BIT },
	{ BOARD_DEV_TWSI_SATR,		1,	0x4E,	ADDR7_BIT },
	{ BOARD_DEV_TWSI_EEPROM,	0,	0x54,	ADDR7_BIT },
	{ BOARD_DEV_TWSI_IO_EXPANDER,	0,	0x21,	ADDR7_BIT },
	{ BOARD_DEV_TWSI_IO_EXPANDER,	2,	0x24,	ADDR7_BIT },
};

MV_BOARD_MAC_INFO db88f6650InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0 },
	{ BOARD_MAC_SPEED_AUTO, 0x3 },
	{ N_A,			N_A },
};

MV_BOARD_MPP_TYPE_INFO db88f6650InfoBoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_AUTO,
		.ethSataComplexOpt = MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0 |
				     MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3,
		.ethPortsMode = 0x0,
	}
};

MV_DEV_CS_INFO db88f6650InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 } /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6650InfoBoardMppConfigValue[] = {
	{ {
		DB_88F6650_MPP0_7,
		DB_88F6650_MPP8_15,
		DB_88F6650_MPP16_23,
		DB_88F6650_MPP24_31,
		DB_88F6650_MPP32_39,
		DB_88F6650_MPP40_47,
		DB_88F6650_MPP48_55,
		DB_88F6650_MPP56_63,
		DB_88F6650_MPP64_67,
	 } }
};

MV_BOARD_SWITCH_INFO db88f6650InfoBoardSwitchValue[] = {
	{
		.switchIrq = 29,	/* set to -1 for timer operation */
		.switchPort = {0, 1, 2, 3, 4, -1, -1},
		.cpuPort = 6,
		.connectedPort = {6, -1},
		.internalQuadPhyAddr = 0,
		 .connectedPortMask = (BIT0 | BIT1 | BIT2 | BIT3 | BIT6),
		.forceLinkMask = 0x0,
	}
};

MV_BOARD_INFO db88f6650_board_info = {
	.boardName			= "DB-88F6650",
	.numBoardMppTypeValue		= ARRSZ(db88f6650InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f6650InfoBoardModTypeInfo,
	.pBoardMppConfigValue		= db88f6650InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f6650InfoBoardDeCsInfo),
	.pDevCsInfo			= db88f6650InfoBoardDeCsInfo,
	.numBoardIoExpanderInfo		= ARRSZ(db88f6650InfoBoardIOExpanderInfo),
	.pBoardIoExpanderInfo		= db88f6650InfoBoardIOExpanderInfo,
	.numBoardTwsiDev		= ARRSZ(db88f6650InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f6650InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f6650InfoBoardMacInfo),
	.pBoardMacInfo			= db88f6650InfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6650_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6650_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6650_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6650_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6650_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6650_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6650_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6650_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6650_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6650InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6650InfoBoardSwitchValue),

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
	.nandFlashControl		= 0,
	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE
};

/*******************************************************************************
 * AvantaLP RD-88F6650 board */
/*******************************************************************************/
MV_BOARD_INFO rd88f6650_board_info = {

};

/*******************************************************************************
 * AvantaLP RD-88F6660 board */
/*******************************************************************************/
MV_BOARD_INFO rd88f6660_board_info = {

};

/*******************************************************************************
 * AvantaLP FPGA board
*******************************************************************************/
MV_BOARD_INFO avanta_lp_fpga_board_info = {
	.boardName			= "CortexA9-FPGA",
	.numBoardMppTypeValue		= 0,
	.pBoardModTypeValue		= NULL,
	.pBoardMppConfigValue		= NULL,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= 0,
	.pDevCsInfo			= NULL,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= 0,
	.pBoardMacInfo			= NULL,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= NULL,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 0,

	/* GPP values */
	.gppOutEnValLow			= 0,
	.gppOutEnValMid			= 0,
	.gppOutEnValHigh		= 0,
	.gppOutValLow			= 0,
	.gppOutValMid			= 0,
	.gppOutValHigh			= 0,
	.gppPolarityValLow		= 0,
	.gppPolarityValMid		= 0,
	.gppPolarityValHigh		= 0,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
	.nandFlashControl		= 0,
};

/*******************************************************************************
* AvanataLP customer board
*******************************************************************************/

#define AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS       0x000C0282
#define AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS      0x00010305
/*NAND care support for small page chips*/
#define AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL           0x01c00543

#define AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS        0x403E07CF
#define AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS       0x000F0F0F

MV_BOARD_MAC_INFO avanta_lp_customerInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x10},
	{ BOARD_MAC_SPEED_AUTO, 0x11},
	{ BOARD_MAC_SPEED_AUTO, 0x12},
	{ BOARD_MAC_SPEED_AUTO, 0x13}
};

MV_BOARD_MPP_TYPE_INFO avanta_lp_customerInfoBoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_AUTO,
		.ethSataComplexOpt = 0x0,
		.ethPortsMode = 0x0
	}
};

MV_BOARD_GPP_INFO avanta_lp_customerInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{ BOARD_GPP_RESET, 21 }
};

MV_DEV_CS_INFO avanta_lp_customerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 } /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO avanta_lp_customerInfoBoardMppConfigValue[] = {
	{ {
		  AVANTA_LP_CUSTOMER_MPP0_7,
		  AVANTA_LP_CUSTOMER_MPP8_15,
		  AVANTA_LP_CUSTOMER_MPP16_23,
		  AVANTA_LP_CUSTOMER_MPP24_31,
		  AVANTA_LP_CUSTOMER_MPP32_39,
		  AVANTA_LP_CUSTOMER_MPP40_47,
		  AVANTA_LP_CUSTOMER_MPP48_55,
		  AVANTA_LP_CUSTOMER_MPP56_63,
		  AVANTA_LP_CUSTOMER_MPP64_67,
	 } }
};

MV_BOARD_INFO avanta_lp_customer_board_info = {
	.boardName				= "AvantaLP-CUSTOMER",
	.numBoardMppTypeValue			= ARRSZ(avanta_lp_customerInfoBoardModTypeInfo),
	.pBoardModTypeValue			= avanta_lp_customerInfoBoardModTypeInfo,
	.pBoardMppConfigValue			= avanta_lp_customerInfoBoardMppConfigValue,
	.intsGppMaskLow				= 0,
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(avanta_lp_customerInfoBoardDeCsInfo),
	.pDevCsInfo				= avanta_lp_customerInfoBoardDeCsInfo,
	.numBoardTwsiDev			= 0,
	.pBoardTwsiDev				= NULL,
	.numBoardMacInfo			= ARRSZ(avanta_lp_customerInfoBoardMacInfo),
	.pBoardMacInfo				= avanta_lp_customerInfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(avanta_lp_customerInfoBoardGppInfo),
	.pBoardGppInfo				= avanta_lp_customerInfoBoardGppInfo,
	.activeLedsNumber			= 0,
	.pLedGppPin				= NULL,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow				= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_LOW,
	.gppOutEnValMid				= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_MID,
	.gppOutEnValHigh			= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_HIGH,
	.gppOutValLow				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_LOW,
	.gppOutValMid				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_MID,
	.gppOutValHigh				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow			= AVANTA_LP_CUSTOMER_GPP_POL_LOW,
	.gppPolarityValMid			= AVANTA_LP_CUSTOMER_GPP_POL_MID,
	.gppPolarityValHigh			= AVANTA_LP_CUSTOMER_GPP_POL_HIGH,

	/* TDM */
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams			= AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams			= AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl			= AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams			= AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams			= AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS
};

/*
 * All supported avanta boards
 */
MV_BOARD_INFO *boardInfoTbl[] = {
	&rd88f6650_board_info,
	&db88f6650_board_info,
	&rd88f6660_board_info,
	&db88f6660_board_info,
	&avanta_lp_fpga_board_info,
	&avanta_lp_customer_board_info,
};
