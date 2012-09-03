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

#define ARRSZ(x)	(sizeof(x)/sizeof(x[0]))

/**********************/
/* ARMADA-XP DB BOARD */
/**********************/
#define DB_88F78XX0_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_88F78XX0_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define DB_88F78XX0_BOARD_NAND_CONTROL		0x01c00543

#define DB_88F78XX0_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F78XX0_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8 mvDbDisableModuleDetection = 0;

MV_U8	db88f6781InfoBoardDebugLedIf[] = {26, 27, 48};

MV_BOARD_TWSI_INFO	db88f78XX0InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT}
};

MV_BOARD_MAC_INFO db88f78XX0InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x19,0x800},  /* Port 1 */
	{BOARD_MAC_SPEED_AUTO, 0x1B,0x1800}  /* Port 3 */
};

MV_BOARD_MODULE_TYPE_INFO db88f78XX0InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO db88f78XX0InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24}, /* from MPP map */
	{BOARD_GPP_RESET,       47},
};

MV_DEV_CS_INFO db88f78XX0InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO db88f78XX0InfoBoardMppConfigValue[] = {
	{ {
	DB_88F78XX0_MPP0_7,
	DB_88F78XX0_MPP8_15,
	DB_88F78XX0_MPP16_23,
	DB_88F78XX0_MPP24_31,
	DB_88F78XX0_MPP32_39,
	DB_88F78XX0_MPP40_47,
	DB_88F78XX0_MPP48_55,
	DB_88F78XX0_MPP56_63,
	DB_88F78XX0_MPP64_67,
	} },
	{ { /* MV_BOARD_TDM_32CH */
		DB_88F78XX0_MPP0_7,
		DB_88F78XX0_MPP8_15,
		DB_88F78XX0_MPP16_23,
		(DB_88F78XX0_MPP24_31 & 0x00000000) | 0x33333333,
		(DB_88F78XX0_MPP32_39 & 0xFFFF0000) | 0x00003333,
		(DB_88F78XX0_MPP40_47 & 0xFFFFF0FF) | 0x00000300,
		DB_88F78XX0_MPP48_55,
		DB_88F78XX0_MPP56_63,
		DB_88F78XX0_MPP64_67,
	} },
	{ { /* MV_BOARD_LCD_DVI */
		(DB_88F78XX0_MPP0_7   & 0x00000000) | 0x44444444,
		(DB_88F78XX0_MPP8_15  & 0x00000000) | 0x44444444,
		(DB_88F78XX0_MPP16_23 & 0x00000000) | 0x44444444,
		(DB_88F78XX0_MPP24_31 & 0xFFFF0000) | 0x00004444,
		DB_88F78XX0_MPP32_39,
		(DB_88F78XX0_MPP40_47 & 0xFFFFFF00) | 0x00000044,
		DB_88F78XX0_MPP48_55,
		DB_88F78XX0_MPP56_63,
		DB_88F78XX0_MPP64_67,
	} },
	{ { /* MV_BOARD_MII_GMII */
		(DB_88F78XX0_MPP0_7 & 0x00000000) | 0x11111111,
		(DB_88F78XX0_MPP8_15 & 0x00000000) | 0x11111111,
		(DB_88F78XX0_MPP16_23 & 0x000000FF) | 0x11111100,
		DB_88F78XX0_MPP24_31,
		DB_88F78XX0_MPP32_39,
		DB_88F78XX0_MPP40_47,
		DB_88F78XX0_MPP48_55,
		DB_88F78XX0_MPP56_63,
		DB_88F78XX0_MPP64_67,
	} },
	{ { /* MV_BOARD_OTHER */
		DB_88F78XX0_MPP0_7,
		DB_88F78XX0_MPP8_15,
		DB_88F78XX0_MPP16_23,
		DB_88F78XX0_MPP24_31,
		DB_88F78XX0_MPP32_39,
		DB_88F78XX0_MPP40_47,
		DB_88F78XX0_MPP48_55,
		DB_88F78XX0_MPP56_63,
		DB_88F78XX0_MPP64_67,
	} },
};

MV_SERDES_CFG db88f78XX0InfoBoardSerdesConfigValue[] = {
	/* Z1B */
	{MV_TRUE, 0x32221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{MV_TRUE, 0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* PEX module */
	/* Z1A */
	{MV_TRUE, 0x32220000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000030},	/* Default - Z1A */
	{MV_TRUE, 0x31210000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_MODE_X1,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000030}	/* PEX module - Z1A */
};

MV_BOARD_TDM_INFO	db88f78XX0Tdm880[]	= { {1}, {2} };
MV_BOARD_TDM_INFO	db88f78XX0Tdm792[]	= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	db88f78XX0Tdm3215[]	= { {1} };

MV_BOARD_INFO db88f78XX0Info = {
	.boardName			= "DB-78460-BP",
	.numBoardMppTypeValue		= ARRSZ(db88f78XX0InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f78XX0InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f78XX0InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f78XX0InfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(db88f78XX0InfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= db88f78XX0InfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f78XX0InfoBoardDeCsInfo),
	.pDevCsInfo			= db88f78XX0InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db88f78XX0InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f78XX0InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f78XX0InfoBoardMacInfo),
	.pBoardMacInfo			= db88f78XX0InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db88f78XX0InfoBoardGppInfo),
	.pBoardGppInfo			= db88f78XX0InfoBoardGppInfo,
	.activeLedsNumber		= ARRSZ(db88f6781InfoBoardDebugLedIf),
	.pLedGppPin			= db88f6781InfoBoardDebugLedIf,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F78XX0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F78XX0_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F78XX0_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F78XX0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F78XX0_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F78XX0_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F78XX0_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F78XX0_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F78XX0_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= {2, 6, 1},
	.pBoardTdmInt2CsInfo		= {db88f78XX0Tdm880,
					   db88f78XX0Tdm792,
					   db88f78XX0Tdm3215},
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F78XX0_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F78XX0_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F78XX0_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= DB_88F78XX0_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F78XX0_BOARD_NOR_WRITE_PARAMS
};

/***************************/
/* ARMADA-XP DB REV2 BOARD */
/***************************/
#define DB_88F78XX0_REV2_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_88F78XX0_REV2_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define DB_88F78XX0_REV2_BOARD_NAND_CONTROL		0x01c00543

#define DB_88F78XX0_REV2_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F78XX0_REV2_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8 mvDbDisableModuleDetection_rev2 = 0;

MV_U8	db88f6781InfoBoardDebugLedIf_rev2[] = {26, 27, 48};

MV_BOARD_TWSI_INFO	db88f78XX0rev2InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT}
};

MV_BOARD_MAC_INFO db88f78XX0rev2InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x19,0x800},  /* Port 1 */
	{BOARD_MAC_SPEED_AUTO, 0x1B,0x1800}  /* Port 3 */
};

MV_BOARD_MODULE_TYPE_INFO db88f78XX0rev2InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO db88f78XX0rev2InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
	/*{BOARD_GPP_RESET,       47},*/
};

MV_DEV_CS_INFO db88f78XX0rev2InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO db88f78XX0rev2InfoBoardMppConfigValue[] = {
	{ {
	DB_88F78XX0_REV2_MPP0_7,
	DB_88F78XX0_REV2_MPP8_15,
	DB_88F78XX0_REV2_MPP16_23,
	DB_88F78XX0_REV2_MPP24_31,
	DB_88F78XX0_REV2_MPP32_39,
	DB_88F78XX0_REV2_MPP40_47,
	DB_88F78XX0_REV2_MPP48_55,
	DB_88F78XX0_REV2_MPP56_63,
	DB_88F78XX0_REV2_MPP64_67,
	} },
	{ { /* MV_BOARD_TDM_32CH */
		DB_88F78XX0_REV2_MPP0_7,
		DB_88F78XX0_REV2_MPP8_15,
		DB_88F78XX0_REV2_MPP16_23,
		(DB_88F78XX0_REV2_MPP24_31 & 0x00000000) | 0x33333333,
		(DB_88F78XX0_REV2_MPP32_39 & 0xFFFF0000) | 0x00003333,
		(DB_88F78XX0_REV2_MPP40_47 & 0xFFFFF0FF) | 0x00000300,
		DB_88F78XX0_REV2_MPP48_55,
		DB_88F78XX0_REV2_MPP56_63,
		DB_88F78XX0_REV2_MPP64_67,
	} },
	{ { /* MV_BOARD_LCD_DVI */
		(DB_88F78XX0_REV2_MPP0_7   & 0x00000000) | 0x44444444,
		(DB_88F78XX0_REV2_MPP8_15  & 0x00000000) | 0x44444444,
		(DB_88F78XX0_REV2_MPP16_23 & 0x00000000) | 0x44444444,
		(DB_88F78XX0_REV2_MPP24_31 & 0xFFFF0000) | 0x00004444,
		DB_88F78XX0_REV2_MPP32_39,
		(DB_88F78XX0_REV2_MPP40_47 & 0xFFFFFF00) | 0x00000044,
		DB_88F78XX0_REV2_MPP48_55,
		DB_88F78XX0_REV2_MPP56_63,
		DB_88F78XX0_REV2_MPP64_67,
	} },
	{ { /* MV_BOARD_MII_GMII */
		(DB_88F78XX0_REV2_MPP0_7 & 0x00000000) | 0x11111111,
		(DB_88F78XX0_REV2_MPP8_15 & 0x00000000) | 0x11111111,
		(DB_88F78XX0_REV2_MPP16_23 & 0x000000FF) | 0x11111100,
		DB_88F78XX0_REV2_MPP24_31,
		DB_88F78XX0_REV2_MPP32_39,
		DB_88F78XX0_REV2_MPP40_47,
		DB_88F78XX0_REV2_MPP48_55,
		DB_88F78XX0_REV2_MPP56_63,
		DB_88F78XX0_REV2_MPP64_67,
	} },
	{ { /* MV_BOARD_OTHER */
		DB_88F78XX0_REV2_MPP0_7,
		DB_88F78XX0_REV2_MPP8_15,
		DB_88F78XX0_REV2_MPP16_23,
		DB_88F78XX0_REV2_MPP24_31,
		DB_88F78XX0_REV2_MPP32_39,
		DB_88F78XX0_REV2_MPP40_47,
		DB_88F78XX0_REV2_MPP48_55,
		DB_88F78XX0_REV2_MPP56_63,
		DB_88F78XX0_REV2_MPP64_67,
	} },
};

MV_SERDES_CFG db88f78XX0rev2InfoBoardSerdesConfigValue[] = {
	/* A0 */
	{MV_TRUE, 0x33221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* Default: No Pex module, PEX0 x1, disabled*/
	{MV_TRUE, 0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* Pex module, PEX0 x1, PEX1 x1*/
	{MV_TRUE, 0x33221111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* no Pex module, PEX0 x4, PEX1 disabled*/
	{MV_TRUE, 0x31211111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X1,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* Pex module, PEX0 x4, PEX1 x1*/
	{MV_TRUE, 0x11111111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* Pex module, PEX0 x1, PEX1 x4*/
	{MV_TRUE, 0x11111111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x00000030},/* Pex module, PEX0 x4, PEX1 x4*/
};

MV_BOARD_TDM_INFO	db88f78XX0rev2Tdm880[]	= { {1}, {2} };
MV_BOARD_TDM_INFO	db88f78XX0rev2Tdm792[]	= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	db88f78XX0rev2Tdm3215[]	= { {1} };

MV_BOARD_INFO db88f78XX0rev2Info = {
	.boardName			= "DB-78460-BP rev 2.0",
	.numBoardMppTypeValue		= ARRSZ(db88f78XX0rev2InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f78XX0rev2InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f78XX0rev2InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f78XX0rev2InfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(db88f78XX0rev2InfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= db88f78XX0rev2InfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f78XX0rev2InfoBoardDeCsInfo),
	.pDevCsInfo			= db88f78XX0rev2InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db88f78XX0rev2InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f78XX0rev2InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f78XX0rev2InfoBoardMacInfo),
	.pBoardMacInfo			= db88f78XX0rev2InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db88f78XX0rev2InfoBoardGppInfo),
	.pBoardGppInfo			= db88f78XX0rev2InfoBoardGppInfo,
	.activeLedsNumber		= ARRSZ(db88f6781InfoBoardDebugLedIf),
	.pLedGppPin			= db88f6781InfoBoardDebugLedIf,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F78XX0_REV2_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F78XX0_REV2_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F78XX0_REV2_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F78XX0_REV2_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F78XX0_REV2_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F78XX0_REV2_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F78XX0_REV2_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F78XX0_REV2_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F78XX0_REV2_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= {2, 6, 1},
	.pBoardTdmInt2CsInfo		= {db88f78XX0rev2Tdm880,
					   db88f78XX0rev2Tdm792,
					   db88f78XX0rev2Tdm3215},
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F78XX0_REV2_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F78XX0_REV2_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F78XX0_REV2_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= DB_88F78XX0_REV2_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F78XX0_REV2_BOARD_NOR_WRITE_PARAMS
};

/***************************/
/* ARMADA-XP RD NAS  BOARD */
/***************************/
#define RD_78460_NAS_BOARD_NAND_READ_PARAMS	0x000C0282
#define RD_78460_NAS_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define RD_78460_NAS_BOARD_NAND_CONTROL		0x01c00543

#define RD_78460_NAS_BOARD_NOR_READ_PARAMS	0x403E07CF
#define RD_78460_NAS_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_MAC_INFO rd78460nasInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
        /* speed will toggle to force link 1000 when SW module detected */
	{BOARD_MAC_SPEED_AUTO, 0x10,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x11,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x12,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x13,0x0}
};

MV_BOARD_MODULE_TYPE_INFO rd78460nasInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO rd78460nasInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_RESET, 21}
};

MV_DEV_CS_INFO rd78460nasInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd78460nasInfoBoardMppConfigValue[] = {
	{ {
	RD_78460_NAS_MPP0_7,
	RD_78460_NAS_MPP8_15,
	RD_78460_NAS_MPP16_23,
	RD_78460_NAS_MPP24_31,
	RD_78460_NAS_MPP32_39,
	RD_78460_NAS_MPP40_47,
	RD_78460_NAS_MPP48_55,
	RD_78460_NAS_MPP56_63,
	RD_78460_NAS_MPP64_67,
	} }
};

MV_SERDES_CFG rd78460nasInfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x00223001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{MV_TRUE, 0x33320201, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x000000f4},	/* Switch module */
};

MV_BOARD_INFO rd78460nasInfo = {
	.boardName			= "RD-AXP-NAS rev 1.0",
	.numBoardMppTypeValue		= ARRSZ(rd78460nasInfoBoardModTypeInfo),
	.pBoardModTypeValue		= rd78460nasInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(rd78460nasInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= rd78460nasInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(rd78460nasInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= rd78460nasInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd78460nasInfoBoardDeCsInfo),
	.pDevCsInfo			= rd78460nasInfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(rd78460nasInfoBoardMacInfo),
	.pBoardMacInfo			= rd78460nasInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(rd78460nasInfoBoardGppInfo),
	.pBoardGppInfo			= rd78460nasInfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_78460_NAS_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_78460_NAS_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RD_78460_NAS_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RD_78460_NAS_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_78460_NAS_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RD_78460_NAS_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RD_78460_NAS_GPP_POL_LOW,
	.gppPolarityValMid		= RD_78460_NAS_GPP_POL_MID,
	.gppPolarityValHigh		= RD_78460_NAS_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams		= RD_78460_NAS_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= RD_78460_NAS_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= RD_78460_NAS_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= RD_78460_NAS_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= RD_78460_NAS_BOARD_NOR_WRITE_PARAMS
};

/*****************************/
/* ARMADA-XP RD SERVER BOARD */
/*****************************/
#define RD_78460_BOARD_NAND_READ_PARAMS		0x000C0282
#define RD_78460_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define RD_78460_BOARD_NAND_CONTROL			0x01c00543

MV_BOARD_MAC_INFO rd78460InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x1,0x0},
	{BOARD_MAC_SPEED_1000M, 0x2,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0},
	{BOARD_MAC_SPEED_1000M, 0x1B,0x0}
};

MV_BOARD_MODULE_TYPE_INFO rd78460InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_DEV_CS_INFO rd78460InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd78460InfoBoardMppConfigValue[] = {
	{ {
		RD_78460_MPP0_7,
		RD_78460_MPP8_15,
		RD_78460_MPP16_23,
		RD_78460_MPP24_31,
		RD_78460_MPP32_39,
		RD_78460_MPP40_47,
		RD_78460_MPP48_55,
		RD_78460_MPP56_63,
		RD_78460_MPP64_67,
	} }
};

MV_SERDES_CFG rd78460InfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010},	/* CPU0 */
	{MV_TRUE, 0x00321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010}	/* CPU1-3 */
};

MV_BOARD_INFO rd78460Info = {
 .boardName				= "RD-78460-SERVER",
 .numBoardMppTypeValue			= ARRSZ(rd78460InfoBoardModTypeInfo),
 .pBoardModTypeValue			= rd78460InfoBoardModTypeInfo,
 .numBoardMppConfigValue		= ARRSZ(rd78460InfoBoardMppConfigValue),
 .pBoardMppConfigValue			= rd78460InfoBoardMppConfigValue,
 .numBoardSerdesConfigValue		= ARRSZ(rd78460InfoBoardSerdesConfigValue),
 .pBoardSerdesConfigValue		= rd78460InfoBoardSerdesConfigValue,
 .intsGppMaskLow			= 0,
 .intsGppMaskMid			= 0,
 .intsGppMaskHigh			= 0,
 .numBoardDeviceIf			= ARRSZ(rd78460InfoBoardDeCsInfo),
 .pDevCsInfo				= rd78460InfoBoardDeCsInfo,
 .numBoardTwsiDev			= 0,
 .pBoardTwsiDev				= NULL,
 .numBoardMacInfo			= ARRSZ(rd78460InfoBoardMacInfo),
 .pBoardMacInfo				= rd78460InfoBoardMacInfo,
 .numBoardGppInfo			= 0,
 .pBoardGppInfo				= NULL,
 .activeLedsNumber			= 0,
 .pLedGppPin				= NULL,
 .ledsPolarity				= 0,

 /* GPP values */
 .gppOutEnValLow			= RD_78460_GPP_OUT_ENA_LOW,
 .gppOutEnValMid			= RD_78460_GPP_OUT_ENA_MID,
 .gppOutEnValHigh			= RD_78460_GPP_OUT_ENA_HIGH,
 .gppOutValLow				= RD_78460_GPP_OUT_VAL_LOW,
 .gppOutValMid				= RD_78460_GPP_OUT_VAL_MID,
 .gppOutValHigh				= RD_78460_GPP_OUT_VAL_HIGH,
 .gppPolarityValLow			= RD_78460_GPP_POL_LOW,
 .gppPolarityValMid			= RD_78460_GPP_POL_MID,
 .gppPolarityValHigh			= RD_78460_GPP_POL_HIGH,

 /* TDM configuration */
	/* We hold a different configuration array for each possible slic that
 ** can be connected to board.
 ** When modules are scanned, then we select the index of the relevant
 ** slic's information array.
 ** For RD and Customers boards we only need to initialize a single
 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
 .numBoardTdmInfo			= {},
 .pBoardTdmInt2CsInfo			= {},
 .boardTdmInfoIndex			= -1,

 /* NAND init params */
 .nandFlashReadParams			= RD_78460_BOARD_NAND_READ_PARAMS,
 .nandFlashWriteParams			= RD_78460_BOARD_NAND_WRITE_PARAMS,
 .nandFlashControl			= RD_78460_BOARD_NAND_CONTROL
};


/*****************************/
/* ARMADA-XP RD SERVER REV2 BOARD */
/*****************************/
#define RD_78460_SERVER_REV2_BOARD_NAND_READ_PARAMS		0x000C0282
#define RD_78460_SERVER_REV2_BOARD_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define RD_78460_SERVER_REV2_BOARD_NAND_CONTROL			0x01c00543

MV_BOARD_MAC_INFO rd78460ServerRev2InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x1,0x0},
	{BOARD_MAC_SPEED_1000M, 0x2,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0},
	{BOARD_MAC_SPEED_1000M, 0x1B,0x0}
};

MV_BOARD_MODULE_TYPE_INFO rd78460ServerRev2InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_DEV_CS_INFO rd78460ServerRev2InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd78460ServerRev2InfoBoardMppConfigValue[] = {
	{ {
		RD_78460_SERVER_REV2_MPP0_7,
		RD_78460_SERVER_REV2_MPP8_15,
		RD_78460_SERVER_REV2_MPP16_23,
		RD_78460_SERVER_REV2_MPP24_31,
		RD_78460_SERVER_REV2_MPP32_39,
		RD_78460_SERVER_REV2_MPP40_47,
		RD_78460_SERVER_REV2_MPP48_55,
		RD_78460_SERVER_REV2_MPP56_63,
		RD_78460_SERVER_REV2_MPP64_67,
	} }
};

MV_SERDES_CFG rd78460ServerRev2InfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x00321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010},	/* CPU0 */
	{MV_TRUE, 0x00321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010}	/* CPU1-3 */
};

MV_BOARD_INFO rd78460ServerRev2Info = {
 .boardName				= "RD-78460-SERVER-REV2",
 .numBoardMppTypeValue			= ARRSZ(rd78460ServerRev2InfoBoardModTypeInfo),
 .pBoardModTypeValue			= rd78460ServerRev2InfoBoardModTypeInfo,
 .numBoardMppConfigValue		= ARRSZ(rd78460ServerRev2InfoBoardMppConfigValue),
 .pBoardMppConfigValue			= rd78460ServerRev2InfoBoardMppConfigValue,
 .numBoardSerdesConfigValue		= ARRSZ(rd78460ServerRev2InfoBoardSerdesConfigValue),
 .pBoardSerdesConfigValue		= rd78460ServerRev2InfoBoardSerdesConfigValue,
 .intsGppMaskLow			= 0,
 .intsGppMaskMid			= 0,
 .intsGppMaskHigh			= 0,
 .numBoardDeviceIf			= ARRSZ(rd78460ServerRev2InfoBoardDeCsInfo),
 .pDevCsInfo				= rd78460ServerRev2InfoBoardDeCsInfo,
 .numBoardTwsiDev			= 0,
 .pBoardTwsiDev				= NULL,
 .numBoardMacInfo			= ARRSZ(rd78460ServerRev2InfoBoardMacInfo),
 .pBoardMacInfo				= rd78460ServerRev2InfoBoardMacInfo,
 .numBoardGppInfo			= 0,
 .pBoardGppInfo				= NULL,
 .activeLedsNumber			= 0,
 .pLedGppPin				= NULL,
 .ledsPolarity				= 0,

 /* GPP values */
 .gppOutEnValLow			= RD_78460_SERVER_REV2_GPP_OUT_ENA_LOW,
 .gppOutEnValMid			= RD_78460_SERVER_REV2_GPP_OUT_ENA_MID,
 .gppOutEnValHigh			= RD_78460_SERVER_REV2_GPP_OUT_ENA_HIGH,
 .gppOutValLow				= RD_78460_SERVER_REV2_GPP_OUT_VAL_LOW,
 .gppOutValMid				= RD_78460_SERVER_REV2_GPP_OUT_VAL_MID,
 .gppOutValHigh				= RD_78460_SERVER_REV2_GPP_OUT_VAL_HIGH,
 .gppPolarityValLow			= RD_78460_SERVER_REV2_GPP_POL_LOW,
 .gppPolarityValMid			= RD_78460_SERVER_REV2_GPP_POL_MID,
 .gppPolarityValHigh			= RD_78460_SERVER_REV2_GPP_POL_HIGH,

 /* TDM configuration */
	/* We hold a different configuration array for each possible slic that
 ** can be connected to board.
 ** When modules are scanned, then we select the index of the relevant
 ** slic's information array.
 ** For RD and Customers boards we only need to initialize a single
 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
 .numBoardTdmInfo			= {},
 .pBoardTdmInt2CsInfo			= {},
 .boardTdmInfoIndex			= -1,

 /* NAND init params */
 .nandFlashReadParams			= RD_78460_SERVER_REV2_BOARD_NAND_READ_PARAMS,
 .nandFlashWriteParams			= RD_78460_SERVER_REV2_BOARD_NAND_WRITE_PARAMS,
 .nandFlashControl			= RD_78460_SERVER_REV2_BOARD_NAND_CONTROL
};
/***************************/
/* ARMADA-XP DB PCAC BOARD */
/***************************/
#define DB_78X60_PCAC_BOARD_NAND_READ_PARAMS		0x000C0282
#define DB_78X60_PCAC_BOARD_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define DB_78X60_PCAC_BOARD_NAND_CONTROL			0x01c00543

MV_U8	db78X60pcacInfoBoardDebugLedIf[] = {53, 54, 55, 56};

MV_BOARD_TWSI_INFO	db78X60pcacInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT}
};

MV_BOARD_MAC_INFO db78X60pcacInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x3,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x2,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0}		/* Dummy */
};


MV_BOARD_MODULE_TYPE_INFO db78X60pcacInfoBoardModTypeInfo[] = {
	{
		.boardMppMod	= MV_BOARD_AUTO,
		.boardOtherMod	= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO db78X60pcacInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    23} /* from MPP map */
};

MV_DEV_CS_INFO db78X60pcacInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db78X60pcacInfoBoardMppConfigValue[] = {
	{ {
		DB_78X60_PCAC_MPP0_7,
		DB_78X60_PCAC_MPP8_15,
		DB_78X60_PCAC_MPP16_23,
		DB_78X60_PCAC_MPP24_31,
		DB_78X60_PCAC_MPP32_39,
		DB_78X60_PCAC_MPP40_47,
		DB_78X60_PCAC_MPP48_55,
		DB_78X60_PCAC_MPP56_63,
		DB_78X60_PCAC_MPP64_67,
	} }
};

MV_SERDES_CFG db78X60pcacInfoBoardSerdesConfigValue[] = {
	 {MV_TRUE, 0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010} /* Default */
};


MV_BOARD_TDM_INFO	db78X60pcacTdm880[]		= { {1}, {2} };
MV_BOARD_TDM_INFO	db78X60pcacTdm792[]		= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	db78X60pcacTdm3215[]	= { {1} };

MV_BOARD_INFO db78X60pcacInfo = {
	.boardName			= "DB-78460-PCAC",
	.numBoardMppTypeValue		= ARRSZ(db78X60pcacInfoBoardModTypeInfo),
	.pBoardModTypeValue		= db78X60pcacInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db78X60pcacInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db78X60pcacInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(db78X60pcacInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= db78X60pcacInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db78X60pcacInfoBoardDeCsInfo),
	.pDevCsInfo			= db78X60pcacInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db78X60pcacInfoBoardTwsiDev),
	.pBoardTwsiDev			= db78X60pcacInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db78X60pcacInfoBoardMacInfo),
	.pBoardMacInfo			= db78X60pcacInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db78X60pcacInfoBoardGppInfo),
	.pBoardGppInfo			= db78X60pcacInfoBoardGppInfo,
	.activeLedsNumber		= ARRSZ(db78X60pcacInfoBoardDebugLedIf),
	.pLedGppPin			= db78X60pcacInfoBoardDebugLedIf,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= DB_78X60_PCAC_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_78X60_PCAC_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_78X60_PCAC_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_78X60_PCAC_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_78X60_PCAC_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_78X60_PCAC_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_78X60_PCAC_GPP_POL_LOW,
	.gppPolarityValMid		= DB_78X60_PCAC_GPP_POL_MID,
	.gppPolarityValHigh		= DB_78X60_PCAC_GPP_POL_HIGH,


	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= {2, 6, 1},
	.pBoardTdmInt2CsInfo		= {db78X60pcacTdm880,
					   db78X60pcacTdm792,
					   db78X60pcacTdm3215},
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= DB_78X60_PCAC_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_78X60_PCAC_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_78X60_PCAC_BOARD_NAND_CONTROL
};

/********************************/
/* ARMADA-XP DB PCAC REV2 BOARD */
/********************************/
#define DB_78X60_PCAC_REV2_BOARD_NAND_READ_PARAMS		0x000C0282
#define DB_78X60_PCAC_REV2_BOARD_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define DB_78X60_PCAC_REV2_BOARD_NAND_CONTROL			0x01c00543

MV_U8	db78X60pcacrev2InfoBoardDebugLedIf[] = {53, 54, 55, 56};

MV_BOARD_MAC_INFO db78X60pcacrev2InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x3,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x2,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0}		/* Dummy */
};


MV_BOARD_MODULE_TYPE_INFO db78X60pcacrev2InfoBoardModTypeInfo[] = {
	{
		.boardMppMod	= MV_BOARD_AUTO,
		.boardOtherMod	= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO db78X60pcacrev2InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    23} /* from MPP map */
};

MV_DEV_CS_INFO db78X60pcacrev2InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db78X60pcacrev2InfoBoardMppConfigValue[] = {
	{ {
		DB_78X60_PCAC_REV2_MPP0_7,
		DB_78X60_PCAC_REV2_MPP8_15,
		DB_78X60_PCAC_REV2_MPP16_23,
		DB_78X60_PCAC_REV2_MPP24_31,
		DB_78X60_PCAC_REV2_MPP32_39,
		DB_78X60_PCAC_REV2_MPP40_47,
		DB_78X60_PCAC_REV2_MPP48_55,
		DB_78X60_PCAC_REV2_MPP56_63,
		DB_78X60_PCAC_REV2_MPP64_67,
	} }
};

MV_SERDES_CFG db78X60pcacrev2InfoBoardSerdesConfigValue[] = {
	 {MV_TRUE, 0x23321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010} /* Default */
};

MV_BOARD_INFO db78X60pcacrev2Info = {
	.boardName			= "DB-78460-PCAC-REV2",
	.numBoardMppTypeValue		= ARRSZ(db78X60pcacrev2InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db78X60pcacrev2InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db78X60pcacrev2InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db78X60pcacrev2InfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(db78X60pcacrev2InfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= db78X60pcacrev2InfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db78X60pcacrev2InfoBoardDeCsInfo),
	.pDevCsInfo			= db78X60pcacrev2InfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(db78X60pcacrev2InfoBoardMacInfo),
	.pBoardMacInfo			= db78X60pcacrev2InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db78X60pcacrev2InfoBoardGppInfo),
	.pBoardGppInfo			= db78X60pcacrev2InfoBoardGppInfo,
	.activeLedsNumber		= ARRSZ(db78X60pcacrev2InfoBoardDebugLedIf),
	.pLedGppPin			= db78X60pcacrev2InfoBoardDebugLedIf,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= DB_78X60_PCAC_REV2_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_78X60_PCAC_REV2_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_78X60_PCAC_REV2_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_78X60_PCAC_REV2_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_78X60_PCAC_REV2_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_78X60_PCAC_REV2_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_78X60_PCAC_REV2_GPP_POL_LOW,
	.gppPolarityValMid		= DB_78X60_PCAC_REV2_GPP_POL_MID,
	.gppPolarityValHigh		= DB_78X60_PCAC_REV2_GPP_POL_HIGH,


	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams		= DB_78X60_PCAC_REV2_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_78X60_PCAC_REV2_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_78X60_PCAC_REV2_BOARD_NAND_CONTROL
};

/************************/
/* ARMADA-XP FPGA BOARD */
/************************/
#define FPGA_88F78XX0_BOARD_NAND_READ_PARAMS	0x000C0282
#define FPGA_88F78XX0_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define FPGA_88F78XX0_BOARD_NAND_CONTROL	0x01c00543

MV_BOARD_TWSI_INFO	fpga88f78XX0InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT}
};

MV_BOARD_MAC_INFO fpga88f78XX0InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x2,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x3,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x4,0x0}
};

MV_BOARD_MODULE_TYPE_INFO fpga88f78XX0InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO fpga88f78XX0InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
};

MV_DEV_CS_INFO fpga88f78XX0InfoBoardDeCsInfo[] = {
		/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO fpga88f78XX0InfoBoardMppConfigValue[] = {
	{ {
	FPGA_88F78XX0_MPP0_7,
	FPGA_88F78XX0_MPP8_15,
	FPGA_88F78XX0_MPP16_23,
	FPGA_88F78XX0_MPP24_31,
	FPGA_88F78XX0_MPP32_39,
	FPGA_88F78XX0_MPP40_47,
	FPGA_88F78XX0_MPP48_55,
	FPGA_88F78XX0_MPP56_63,
	FPGA_88F78XX0_MPP64_67,
	} }
};

MV_SERDES_CFG fpga88f78XX0InfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x00000000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000000} /* No PEX in FPGA */
};

MV_BOARD_TDM_INFO	fpga88f78XX0Tdm880[]	= { {1}, {2} };
MV_BOARD_TDM_INFO	fpga88f78XX0Tdm792[]	= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	fpga88f78XX0Tdm3215[]	= { {1} };

MV_BOARD_INFO fpga88f78XX0Info = {
	.boardName			= "FPGA-88F78XX0",
	.numBoardMppTypeValue		= ARRSZ(fpga88f78XX0InfoBoardModTypeInfo),
	.pBoardModTypeValue		= fpga88f78XX0InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(fpga88f78XX0InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= fpga88f78XX0InfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(fpga88f78XX0InfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= fpga88f78XX0InfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(fpga88f78XX0InfoBoardDeCsInfo),
	.pDevCsInfo			= fpga88f78XX0InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(fpga88f78XX0InfoBoardTwsiDev),
	.pBoardTwsiDev			= fpga88f78XX0InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(fpga88f78XX0InfoBoardMacInfo),
	.pBoardMacInfo			= fpga88f78XX0InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(fpga88f78XX0InfoBoardGppInfo),
	.pBoardGppInfo			= fpga88f78XX0InfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= FPGA_88F78XX0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= FPGA_88F78XX0_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= FPGA_88F78XX0_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= FPGA_88F78XX0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= FPGA_88F78XX0_GPP_OUT_VAL_MID,
	.gppOutValHigh			= FPGA_88F78XX0_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= FPGA_88F78XX0_GPP_POL_LOW,
	.gppPolarityValMid		= FPGA_88F78XX0_GPP_POL_MID,
	.gppPolarityValHigh		= FPGA_88F78XX0_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= { 2, 6, 1 },
	.pBoardTdmInt2CsInfo		= { fpga88f78XX0Tdm880,
					    fpga88f78XX0Tdm792,
					    fpga88f78XX0Tdm3215
					  },
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= FPGA_88F78XX0_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= FPGA_88F78XX0_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= FPGA_88F78XX0_BOARD_NAND_CONTROL
};

/***************************/
/* ARMADA-XP AMC BOARD     */
/***************************/
#define DB_78X60_AMC_BOARD_NAND_READ_PARAMS		0x000C0282
#define DB_78X60_AMC_BOARD_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define DB_78X60_AMC_BOARD_NAND_CONTROL			0x01c00543

MV_U8	db78X60amcInfoBoardDebugLedIf[] = {53, 54, 55, 56}; /* 7 segment MPPs*/

MV_BOARD_TWSI_INFO	db78X60amcInfoBoardTwsiDev[] = {
	/* No TWSI devices on board*/
};

MV_BOARD_MAC_INFO db78X60amcInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0xF,0x0},
	{BOARD_MAC_SPEED_AUTO, 0xE,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0}
};


MV_BOARD_MODULE_TYPE_INFO db78X60amcInfoBoardModTypeInfo[] = {
	/* No Modules */
};

MV_BOARD_GPP_INFO db78X60amcInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    46} /* from MPP map */
};

MV_DEV_CS_INFO db78X60amcInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db78X60amcInfoBoardMppConfigValue[] = {
	{ {
		DB_78X60_AMC_MPP0_7,
		DB_78X60_AMC_MPP8_15,
		DB_78X60_AMC_MPP16_23,
		DB_78X60_AMC_MPP24_31,
		DB_78X60_AMC_MPP32_39,
		DB_78X60_AMC_MPP40_47,
		DB_78X60_AMC_MPP48_55,
		DB_78X60_AMC_MPP56_63,
		DB_78X60_AMC_MPP64_67,
	} }
};

MV_SERDES_CFG db78X60amcInfoBoardSerdesConfigValue[] = {
	 {MV_TRUE, 0x33111111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030} /* Default */
};


MV_BOARD_TDM_INFO	db78X60amcTdm880[]	= {};
MV_BOARD_TDM_INFO	db78X60amcTdm792[]	= {};
MV_BOARD_TDM_INFO	db78X60amcTdm3215[]	= {};

MV_BOARD_INFO db78X60amcInfo = {
	.boardName			= "DB-78460-AMC",
	.numBoardMppTypeValue		= ARRSZ(db78X60amcInfoBoardModTypeInfo),
	.pBoardModTypeValue		= db78X60amcInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db78X60amcInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db78X60amcInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(db78X60amcInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= db78X60amcInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db78X60amcInfoBoardDeCsInfo),
	.pDevCsInfo			= db78X60amcInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db78X60amcInfoBoardTwsiDev),
	.pBoardTwsiDev			= db78X60amcInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db78X60amcInfoBoardMacInfo),
	.pBoardMacInfo			= db78X60amcInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db78X60amcInfoBoardGppInfo),
	.pBoardGppInfo			= db78X60amcInfoBoardGppInfo,
	.activeLedsNumber		= ARRSZ(db78X60amcInfoBoardDebugLedIf),
	.pLedGppPin			= db78X60amcInfoBoardDebugLedIf,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= DB_78X60_AMC_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_78X60_AMC_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_78X60_AMC_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_78X60_AMC_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_78X60_AMC_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_78X60_AMC_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_78X60_AMC_GPP_POL_LOW,
	.gppPolarityValMid		= DB_78X60_AMC_GPP_POL_MID,
	.gppPolarityValHigh		= DB_78X60_AMC_GPP_POL_HIGH,


	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	/* NAND init params */
	.nandFlashReadParams		= DB_78X60_AMC_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_78X60_AMC_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_78X60_AMC_BOARD_NAND_CONTROL
};

//////////////////////////////////////////////////////////////////////////////////

/***************************/
/* ARMADA-XP RD GP  BOARD */
/***************************/
#define RD_78460_GP_BOARD_NAND_READ_PARAMS		0x000C0282
#define RD_78460_GP_BOARD_NAND_WRITE_PARAMS		0x00010305

#define RD_78460_GP_BOARD_NAND_CONTROL			0x01c00543

#define RD_78460_GP_BOARD_NOR_READ_PARAMS		0x403E07CF
#define RD_78460_GP_BOARD_NOR_WRITE_PARAMS		0x000F0F0F


MV_BOARD_TWSI_INFO   rd78460gpInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT}
};

MV_BOARD_MAC_INFO rd78460gpInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
        /* speed will toggle to force link 1000 when SW module detected */
	{BOARD_MAC_SPEED_AUTO, 0x10,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x11,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x12,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x13,0x0}
};

MV_BOARD_MODULE_TYPE_INFO rd78460gpInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO rd78460gpInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_RESET, 21}
};

MV_DEV_CS_INFO rd78460gpInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd78460gpInfoBoardMppConfigValue[] = {
	{ {
	RD_78460_GP_MPP0_7,
	RD_78460_GP_MPP8_15,
	RD_78460_GP_MPP16_23,
	RD_78460_GP_MPP24_31,
	RD_78460_GP_MPP32_39,
	RD_78460_GP_MPP40_47,
	RD_78460_GP_MPP48_55,
	RD_78460_GP_MPP56_63,
	RD_78460_GP_MPP64_67,
	} }
};

MV_SERDES_CFG rd78460gpInfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x00223001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
};

MV_BOARD_INFO rd78460gpInfo = {
	.boardName			= "RD-AXP-GP rev 1.0",
	.numBoardMppTypeValue		= ARRSZ(rd78460gpInfoBoardModTypeInfo),
	.pBoardModTypeValue		= rd78460gpInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(rd78460gpInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= rd78460gpInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(rd78460gpInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= rd78460gpInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd78460gpInfoBoardDeCsInfo),
	.pDevCsInfo				= rd78460gpInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(rd78460gpInfoBoardTwsiDev),
	.pBoardTwsiDev			= rd78460gpInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(rd78460gpInfoBoardMacInfo),
	.pBoardMacInfo			= rd78460gpInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(rd78460gpInfoBoardGppInfo),
	.pBoardGppInfo			= rd78460gpInfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_78460_GP_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_78460_GP_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RD_78460_GP_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RD_78460_GP_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_78460_GP_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RD_78460_GP_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RD_78460_GP_GPP_POL_LOW,
	.gppPolarityValMid		= RD_78460_GP_GPP_POL_MID,
	.gppPolarityValHigh		= RD_78460_GP_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams		= RD_78460_GP_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= RD_78460_GP_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= RD_78460_GP_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= RD_78460_GP_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= RD_78460_GP_BOARD_NOR_WRITE_PARAMS
};

/***************************/
/* ARMADA-XP CUSTOMER BOARD */
/***************************/
#define RD_78460_CUSTOMER_BOARD_NAND_READ_PARAMS	0x000C0282
#define RD_78460_CUSTOMER_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define RD_78460_CUSTOMER_BOARD_NAND_CONTROL		0x01c00543

#define RD_78460_CUSTOMER_BOARD_NOR_READ_PARAMS	0x403E07CF
#define RD_78460_CUSTOMER_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_MAC_INFO rd78460customerInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x10,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x11,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x12,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x13,0x0}
};

MV_BOARD_MODULE_TYPE_INFO rd78460customerInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};
//////////////////////////////////////////////////////////////////////////////////

MV_BOARD_GPP_INFO rd78460customerInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_RESET, 21}
};

MV_DEV_CS_INFO rd78460customerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd78460customerInfoBoardMppConfigValue[] = {
	{ {
	RD_78460_CUSTOMER_MPP0_7,
	RD_78460_CUSTOMER_MPP8_15,
	RD_78460_CUSTOMER_MPP16_23,
	RD_78460_CUSTOMER_MPP24_31,
	RD_78460_CUSTOMER_MPP32_39,
	RD_78460_CUSTOMER_MPP40_47,
	RD_78460_CUSTOMER_MPP48_55,
	RD_78460_CUSTOMER_MPP56_63,
	RD_78460_CUSTOMER_MPP64_67,
	} }
};

MV_SERDES_CFG rd78460customerInfoBoardSerdesConfigValue[] = {
	{MV_TRUE, 0x00223001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{MV_TRUE, 0x33320201, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Switch module */
};

MV_BOARD_INFO rd78460customerInfo = {
	.boardName			= "RD-AXP-CUSTOMER",
	.numBoardMppTypeValue		= ARRSZ(rd78460customerInfoBoardModTypeInfo),
	.pBoardModTypeValue		= rd78460customerInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(rd78460customerInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= rd78460customerInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(rd78460customerInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= rd78460customerInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd78460customerInfoBoardDeCsInfo),
	.pDevCsInfo			= rd78460customerInfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(rd78460customerInfoBoardMacInfo),
	.pBoardMacInfo			= rd78460customerInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(rd78460customerInfoBoardGppInfo),
	.pBoardGppInfo			= rd78460customerInfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_78460_CUSTOMER_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_78460_CUSTOMER_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RD_78460_CUSTOMER_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RD_78460_CUSTOMER_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_78460_CUSTOMER_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RD_78460_CUSTOMER_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RD_78460_CUSTOMER_GPP_POL_LOW,
	.gppPolarityValMid		= RD_78460_CUSTOMER_GPP_POL_MID,
	.gppPolarityValHigh		= RD_78460_CUSTOMER_GPP_POL_HIGH,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams		= RD_78460_CUSTOMER_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= RD_78460_CUSTOMER_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= RD_78460_CUSTOMER_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= RD_78460_CUSTOMER_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= RD_78460_CUSTOMER_BOARD_NOR_WRITE_PARAMS
};
//////////////////////////////////////////////////////////////////////////////////////

MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f78XX0Info,
	&rd78460Info,
	&db78X60pcacInfo,
	&fpga88f78XX0Info,
	&db88f78XX0rev2Info,
	&rd78460nasInfo,
	&db78X60amcInfo,
	&db78X60pcacrev2Info,
	&rd78460ServerRev2Info,
	&rd78460gpInfo,
	&rd78460customerInfo
};
