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
#include "boardEnv/mvBoardEnvLib.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPexRegs.h"

/*******************************************************************************
 * A380 DB-88F6660 board */
/*******************************************************************************/
#define DB_88F68XX_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_88F68XX_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define DB_88F68XX_BOARD_NAND_CONTROL		0x01c00543

#define DB_88F68XX_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F68XX_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_MPP_INFO db88f68xxInfoBoardMppConfigValue[] = {
	{ {
		  DB_88F68XX_MPP0_7,
		  DB_88F68XX_MPP8_15,
		  DB_88F68XX_MPP16_23,
		  DB_88F68XX_MPP24_31,
		  DB_88F68XX_MPP32_39,
		  DB_88F68XX_MPP40_47,
		  DB_88F68XX_MPP48_55,
		  DB_88F68XX_MPP56_63
	 } }
};

MV_BOARD_TWSI_INFO db88f68xxInfoBoardTwsiDev[] = {
	/* {{devClass,		devClassId, twsiDevAddr, twsiDevAddrType, moreThen256}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x50,	   ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4c,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		2,	0x4d,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		3,	0x4e,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		4,	0x21,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	0,	0x20,	   ADDR7_BIT, MV_FALSE},   /* modules */
	{ BOARD_TWSI_MODULE_DETECT,	1,	0x23,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	2,	0x24,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	3,	0x25,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	4,	0x26,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	5,	0x27,	   ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO db88f68xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0},
	{ BOARD_MAC_SPEED_AUTO, 0x1},
};

MV_DEV_CS_INFO db88f68xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8 } /* SPI DEV */
#endif
#ifdef MV_NAND
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8},  /* NAND DEV */
#endif
};
/*
MV_BOARD_MPP_TYPE_INFO db88f68XXInfoBoardModTypeInfo[] = {
	{
		.externalModule = MV_BOARD_NONE,
		.ModuleMpp = 0,
	}
};
*/

MV_BOARD_TDM_INFO db88f68xxTdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO db88f68xxTdmSpiInfo[] = { {1} };

MV_BOARD_INFO db88f68xx_board_info = {
	.boardName		= "DB-88F6820-BP",
	.numBoardMppTypeValue	= 0,		/* ARRSZ(db88f68XXInfoBoardModTypeInfo), */
	.pBoardModTypeValue	= NULL,		/* db88f68XXInfoBoardModTypeInfo, */
	.pBoardMppConfigValue	= db88f68xxInfoBoardMppConfigValue,
	.intsGppMaskLow		= 0,
	.intsGppMaskMid		= 0,
	.intsGppMaskHigh	= 0,
	.numBoardDeviceIf	= ARRSZ(db88f68xxInfoBoardDeCsInfo),
	.pDevCsInfo		= db88f68xxInfoBoardDeCsInfo,
	.numBoardTwsiDev	= ARRSZ(db88f68xxInfoBoardTwsiDev),
	.pBoardTwsiDev		= db88f68xxInfoBoardTwsiDev,
	.numBoardMacInfo	= ARRSZ(db88f68xxInfoBoardMacInfo),
	.pBoardMacInfo		= db88f68xxInfoBoardMacInfo,
	.numBoardGppInfo	= 0,
	.pBoardGppInfo		= 0,
	.activeLedsNumber	= 0,
	.pLedGppPin		= NULL,
	.ledsPolarity		= 0,

	/* PMU Power */
	.pmuPwrUpPolarity	= 0,
	.pmuPwrUpDelay		= 80000,

	/* GPP values */
	.gppOutEnValLow		= DB_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid		= DB_88F68XX_GPP_OUT_ENA_MID,
	.gppOutEnValHigh	= DB_88F68XX_GPP_OUT_ENA_HIGH,
	.gppOutValLow		= DB_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid		= DB_88F68XX_GPP_OUT_VAL_MID,
	.gppOutValHigh		= DB_88F68XX_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow	= DB_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid	= DB_88F68XX_GPP_POL_MID,
	.gppPolarityValHigh	= DB_88F68XX_GPP_POL_HIGH,

	/* TDM */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f68xxTdm880},
	.boardTdmInfoIndex		= 0,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F68XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F68XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F68XX_BOARD_NAND_CONTROL,
	.pBoardTdmSpiInfo		= db88f68xxTdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= 0,
	.pIoExp				= NULL
};

/*******************************************************************************
 * A380 RD-NAS-88F68XX board */
/*******************************************************************************/
MV_BOARD_TWSI_INFO rd88F68XXInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	0,	0x50, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,	1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO rd88F68XXInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x1},
	{ BOARD_MAC_SPEED_AUTO, 0x0}
};

MV_DEV_CS_INFO rd88F68XXInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI0_CS0,	N_A,	BOARD_DEV_SPI_FLASH,	8,	8}  /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rdNas88f68XXInfoBoardMppConfigValue[] = {
	{ {
		RD_88F68XX_MPP0_7,
		RD_88F68XX_MPP8_15,
		RD_NAS_88F68XX_MPP16_23,
		RD_88F68XX_MPP24_31,
		RD_88F68XX_MPP32_39,
		RD_88F68XX_MPP40_47,
		RD_88F68XX_MPP48_55,
		RD_88F68XX_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER ioExpNas[] = {
	{0, 6, 0xF4},
	{0, 7, 0xC3},
	{0, 2, 0x0B},
	{0, 3, 0x18},
	{1, 6, 0xE7},
	{1, 7, 0xF9},
	{1, 2, 0x08},
	{1, 3, 0x00}
};

MV_BOARD_INFO rdNas88f68XX_board_info = {
	.boardName			= "RD-NAS-88F6820-DDR3",
	.numBoardMppTypeValue	= ARRSZ(rdNas88f68XXInfoBoardMppConfigValue),
	.pBoardModTypeValue		= NULL,
	.pBoardMppConfigValue	= rdNas88f68XXInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd88F68XXInfoBoardDeCsInfo),
	.pDevCsInfo				= rd88F68XXInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(rd88F68XXInfoBoardTwsiDev),
	.pBoardTwsiDev			= rd88F68XXInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(rd88F68XXInfoBoardMacInfo),
	.pBoardMacInfo			= rd88F68XXInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_88F68XX_GPP_OUT_ENA_MID,
	.gppOutValLow			= RD_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_88F68XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= RD_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid		= RD_88F68XX_GPP_POL_MID,

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
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(ioExpNas),
	.pIoExp				= ioExpNas
};
/*******************************************************************************
 * A380 RD-WAP-88F68XX board */
/*******************************************************************************/
MV_BOARD_MPP_INFO rdWap88f68XXInfoBoardMppConfigValue[] = {
	{ {
		RD_88F68XX_MPP0_7,
		RD_88F68XX_MPP8_15,
		RD_WAP_88F68XX_MPP16_23,
		RD_88F68XX_MPP24_31,
		RD_88F68XX_MPP32_39,
		RD_88F68XX_MPP40_47,
		RD_88F68XX_MPP48_55,
		RD_88F68XX_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER ioExpWap[] = {
	{0, 6, 0xF4},
	{0, 7, 0xC3},
	{0, 2, 0x0B},
	{0, 3, 0x18},
	{1, 6, 0xE7},
	{1, 7, 0xF9},
	{1, 2, 0x18},
	{1, 3, 0x06}
};
MV_BOARD_INFO rdWAP88f68XX_board_info = {
	.boardName			= "RD-AP-88F6820-DDR3",
	.numBoardMppTypeValue	= ARRSZ(rdWap88f68XXInfoBoardMppConfigValue),
	.pBoardModTypeValue		= NULL,
	.pBoardMppConfigValue	= rdWap88f68XXInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd88F68XXInfoBoardDeCsInfo),
	.pDevCsInfo				= rd88F68XXInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(rd88F68XXInfoBoardTwsiDev),
	.pBoardTwsiDev			= rd88F68XXInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(rd88F68XXInfoBoardMacInfo),
	.pBoardMacInfo			= rd88F68XXInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_WAP_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_88F68XX_GPP_OUT_ENA_MID,
	.gppOutValLow			= RD_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_88F68XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= RD_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid		= RD_88F68XX_GPP_POL_MID,

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
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(ioExpWap),
	.pIoExp				= ioExpWap
};


#define A380_CUSTOMER_BOARD_NAND_READ_PARAMS       0x000C0282
#define A380_CUSTOMER_BOARD_NAND_WRITE_PARAMS      0x00010305
/*NAND care support for small page chips*/
#define A380_CUSTOMER_BOARD_NAND_CONTROL           0x01c00543

#define A380_CUSTOMER_BOARD_NOR_READ_PARAMS        0x403E07CF
#define A380_CUSTOMER_BOARD_NOR_WRITE_PARAMS       0x000F0F0F

MV_BOARD_TWSI_INFO A380_customerInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		1,	0x4D,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		0,	0x54,	   ADDR7_BIT	},
};

MV_BOARD_MAC_INFO A380_customerInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x10},
	{ BOARD_MAC_SPEED_AUTO, 0x11},
	{ BOARD_MAC_SPEED_AUTO, 0x12},
	{ BOARD_MAC_SPEED_AUTO, 0x13},
};

MV_DEV_CS_INFO A380_customerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI0_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 } /* SPI DEV */
#endif
};

MV_BOARD_TDM_INFO A380_customerTdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO A380_customerTdmSpiInfo[] = { {1} };

MV_BOARD_MPP_INFO A380_customerInfoBoardMppConfigValue[] = {
	{ {
		  A380_CUSTOMER_MPP0_7,
		  A380_CUSTOMER_MPP8_15,
		  A380_CUSTOMER_MPP16_23,
		  A380_CUSTOMER_MPP24_31,
		  A380_CUSTOMER_MPP32_39,
		  A380_CUSTOMER_MPP40_47,
		  A380_CUSTOMER_MPP48_55,
		  A380_CUSTOMER_MPP56_63
	 } }
};

MV_BOARD_INFO A380_customer_board_info = {
	.boardName				= "A380-CUSTOMER",
	.numBoardMppTypeValue	= 0,
	.pBoardModTypeValue		= NULL,
	.pBoardMppConfigValue	= A380_customerInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(A380_customerInfoBoardDeCsInfo),
	.pDevCsInfo				= A380_customerInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(A380_customerInfoBoardTwsiDev),
	.pBoardTwsiDev			= A380_customerInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(A380_customerInfoBoardMacInfo),
	.pBoardMacInfo			= A380_customerInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= NULL,
	.activeLedsNumber		= 0,
	.pLedGppPin				= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A380_CUSTOMER_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A380_CUSTOMER_GPP_OUT_ENA_MID,
	.gppOutValLow			= A380_CUSTOMER_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A380_CUSTOMER_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= A380_CUSTOMER_GPP_POL_LOW,
	.gppPolarityValMid		= A380_CUSTOMER_GPP_POL_MID,

	/* TDM */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {A380_customerTdm880},
	.boardTdmInfoIndex		= 0,

	/* NAND init params */
	.nandFlashReadParams		= A380_CUSTOMER_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A380_CUSTOMER_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= A380_CUSTOMER_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= A380_CUSTOMER_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= A380_CUSTOMER_BOARD_NOR_WRITE_PARAMS,
	.numIoExp			= 0,
	.pIoExp				= NULL
};

/*
 * All supported A380 boards
 */
MV_BOARD_INFO *boardInfoTbl[] = {
	&rdNas88f68XX_board_info,
	&db88f68xx_board_info,
	&rdWAP88f68XX_board_info,
	&A380_customer_board_info
};

