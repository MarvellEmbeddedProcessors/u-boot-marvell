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
	{0x32221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4, 0x00000030},	/* PEX module */
	/* Z1A */
	{0x32220000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000030},	/* Default - Z1A */
	{0x31210000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, 0x00000030}	/* PEX module - Z1A */
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

MV_U8	db88f6781InfoBoardDebugLedIf_rev2[] = {26, 27, 48}; //Faraj ??

MV_BOARD_TWSI_INFO	db88f78XX0rev2InfoBoardTwsiDev[] = { //Faraj: update from new board
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

MV_BOARD_MODULE_TYPE_INFO db88f78XX0rev2InfoBoardModTypeInfo[] = { //Faraj ??
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO db88f78XX0rev2InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
	//{BOARD_GPP_RESET,       47},
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

MV_SERDES_CFG db88f78XX0rev2InfoBoardSerdesConfigValue[] = { //Faraj: change accordign to DB ==> stays the same
	/* Z1B */
	{0x33221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4, 0x00000030},	/* PEX module */
	/* Z1A */
	{0x32220000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000030},	/* Default - Z1A */
	{0x31210000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, 0x00000030}	/* PEX module - Z1A */
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
	.activeLedsNumber		= ARRSZ(db88f6781InfoBoardDebugLedIf),//Faraj???
	.pLedGppPin			= db88f6781InfoBoardDebugLedIf, //Faraj ???
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
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x19,0x800},  /* Port 1 */
	{BOARD_MAC_SPEED_AUTO, 0x1B,0x1800}  /* Port 3 */
};

MV_BOARD_MODULE_TYPE_INFO rd78460nasInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO rd78460nasInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
	//{BOARD_GPP_RESET,       47},
};

MV_DEV_CS_INFO rd78460nasInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
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

MV_SERDES_CFG rd78460nasInfoBoardSerdesConfigValue[] = { //Faraj: change accordign to DB ==> stays the same
	{0x00226001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{0x43320301, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4, 0x00000030},	/* Switch module */

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
	{0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010},	/* CPU0 */
	{0x00321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010}	/* CPU1-3 */
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
	 {0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000010} /* Default */
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
	{0x00000000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x00000000} /* No PEX in FPGA */
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

MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f78XX0Info,
	&rd78460Info,
	&db78X60pcacInfo,
	&fpga88f78XX0Info,
	&db88f78XX0rev2Info,
	&rd78460nasInfo
};
