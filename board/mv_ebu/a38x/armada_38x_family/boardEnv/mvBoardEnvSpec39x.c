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

/***************************************** Customer Boards *****************************************/
/*******************************************************************************
 * A39x Customer Board 0 - Based on RD
 *******************************************************************************/
#define A39X_CUSTOMER_BOARD_0_NAND_READ_PARAMS		0x000C0282
#define A39X_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define A39X_CUSTOMER_BOARD_0_NAND_CONTROL		0x01c00543

#define A39X_CUSTOMER_BOARD_0_NOR_READ_PARAMS		0x403E07CF
#define A39X_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS		0x000F0F0F

MV_BOARD_TWSI_INFO armada_39x_customer_0_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	0,	0x50, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,	1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MAC_INFO armada_39x_customer_0_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0},
	{ BOARD_MAC_SPEED_AUTO, 0x5, 0x5},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4},
	{ BOARD_MAC_UNCONNECTED, -1, -1}
};

MV_BOARD_NET_COMPLEX_INFO armada_39x_customer_0_InfoBoardNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC1_2_SGMII_L1 |
			MV_NETCOMP_GE_MAC2_2_SGMII_L3 |
			MV_NETCOMP_GE_MAC0_2_RXAUI,
	}
};

MV_DEV_CS_INFO armada_39x_customer_0_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#ifdef MV_NAND
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8},  /* NAND DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16}, /* NOR DEV */
	{ SPI0_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8 } /* SPI DEV */
#else
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8 } /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO armada_39x_customer_0_BoardMppConfigValue[] = {
	{ {
		A39X_CUSTOMER_BOARD_0_MPP0_7,
		A39X_CUSTOMER_BOARD_0_MPP8_15,
		A39X_CUSTOMER_BOARD_0_MPP16_23,
		A39X_CUSTOMER_BOARD_0_MPP24_31,
		A39X_CUSTOMER_BOARD_0_MPP32_39,
		A39X_CUSTOMER_BOARD_0_MPP40_47,
		A39X_CUSTOMER_BOARD_0_MPP48_55,
		A39X_CUSTOMER_BOARD_0_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER armada_39x_customer_0_IoExpanderInfo[] = {
	{0, 6, 0xF4},
	{0, 7, 0xC3},
	{0, 2, 0x0B},
	{0, 3, 0x18},
	{1, 6, 0xE7},
	{1, 7, 0xF9},
	{1, 2, 0x08},
	{1, 3, 0x00}
};

MV_BOARD_INFO armada_39x_customer_board_0_info = {
	.boardName			= "A39x-Customer-Board-0",
	.numBoardNetComplexValue	= ARRSZ(armada_39x_customer_0_InfoBoardNetComplexInfo),
	.pBoardNetComplexInfo		= armada_39x_customer_0_InfoBoardNetComplexInfo,
	.pBoardMppConfigValue		= armada_39x_customer_0_BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(armada_39x_customer_0_BoardDeCsInfo),
	.pDevCsInfo			= armada_39x_customer_0_BoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(armada_39x_customer_0_BoardTwsiDev),
	.pBoardTwsiDev			= armada_39x_customer_0_BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(armada_39x_customer_0_BoardMacInfo),
	.pBoardMacInfo			= armada_39x_customer_0_BoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A39X_CUSTOMER_BOARD_0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A39X_CUSTOMER_BOARD_0_GPP_OUT_ENA_MID,
	.gppOutValLow			= A39X_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A39X_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= A39X_CUSTOMER_BOARD_0_GPP_POL_LOW,
	.gppPolarityValMid		= A39X_CUSTOMER_BOARD_0_GPP_POL_MID,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= A39X_CUSTOMER_BOARD_0_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS,
	.nandFlashControl		= A39X_CUSTOMER_BOARD_0_NAND_CONTROL,
	.nandIfMode				= NAND_IF_NFC,
	/* NOR init params */
	.norFlashReadParams		= A39X_CUSTOMER_BOARD_0_NOR_READ_PARAMS,
	.norFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(armada_39x_customer_0_IoExpanderInfo),
	.pIoExp				= armada_39x_customer_0_IoExpanderInfo,
	.boardOptionsModule		= MV_MODULE_NO_MODULE
};

/*
 * All supported 390 boards
 */
MV_BOARD_INFO *customerBoardInfoTbl[] = {
	&armada_39x_customer_board_0_info,
	&armada_39x_customer_board_0_info
};


/***************************************** Marvell Boards *****************************************/

/*******************************************************************************
 * 39x DB-88F6660 board */
/*******************************************************************************/
#define DB_88F69XX_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_88F69XX_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define DB_88F69XX_BOARD_NAND_CONTROL		0x01c00543

#define DB_88F69XX_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F69XX_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_MPP_INFO db88f69xxInfoBoardMppConfigValue[] = {
	{ {
		  DB_88F69XX_MPP0_7,
		  DB_88F69XX_MPP8_15,
		  DB_88F69XX_MPP16_23,
		  DB_88F69XX_MPP24_31,
		  DB_88F69XX_MPP32_39,
		  DB_88F69XX_MPP40_47,
		  DB_88F69XX_MPP48_55,
		  DB_88F69XX_MPP56_63
	 } }
};

MV_BOARD_TWSI_INFO db88f69xxInfoBoardTwsiDev[] = {
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

MV_BOARD_MAC_INFO db88f69xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0},
	{ BOARD_MAC_SPEED_AUTO, 0x5, 0x5},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4},
	{ BOARD_MAC_UNCONNECTED, -1, -1}
};

MV_BOARD_NET_COMPLEX_INFO db88f69xxInfoBoarNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC1_2_SGMII_L1 |
			MV_NETCOMP_GE_MAC2_2_SGMII_L3 |
			MV_NETCOMP_GE_MAC0_2_RXAUI,
	}
};

MV_DEV_CS_INFO db88f69xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#ifdef MV_NAND
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8},  /* NAND DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16}, /* NOR DEV */
	{ SPI0_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8 } /* SPI DEV */
#else
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8 } /* SPI DEV */
#endif
};

MV_BOARD_TDM_INFO db88f69xxTdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO db88f69xxTdmSpiInfo[] = { {1} };

MV_BOARD_INFO db88f69xx_board_info = {
	.boardName		= "DB-88F6920-BP",
	.numBoardNetComplexValue = ARRSZ(db88f69xxInfoBoarNetComplexInfo),
	.pBoardNetComplexInfo	= db88f69xxInfoBoarNetComplexInfo,
	.pBoardMppConfigValue	= db88f69xxInfoBoardMppConfigValue,
	.intsGppMaskLow		= 0,
	.intsGppMaskMid		= 0,
	.intsGppMaskHigh	= 0,
	.numBoardDeviceIf	= ARRSZ(db88f69xxInfoBoardDeCsInfo),
	.pDevCsInfo		= db88f69xxInfoBoardDeCsInfo,
	.numBoardTwsiDev	= ARRSZ(db88f69xxInfoBoardTwsiDev),
	.pBoardTwsiDev		= db88f69xxInfoBoardTwsiDev,
	.numBoardMacInfo	= ARRSZ(db88f69xxInfoBoardMacInfo),
	.pBoardMacInfo		= db88f69xxInfoBoardMacInfo,
	.numBoardGppInfo	= 0,
	.pBoardGppInfo		= 0,
	.activeLedsNumber	= 0,
	.pLedGppPin		= NULL,
	.ledsPolarity		= 0,

	/* PMU Power */
	.pmuPwrUpPolarity	= 0,
	.pmuPwrUpDelay		= 80000,

	/* GPP values */
	.gppOutEnValLow		= DB_88F69XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid		= DB_88F69XX_GPP_OUT_ENA_MID,
	.gppOutValLow		= DB_88F69XX_GPP_OUT_VAL_LOW,
	.gppOutValMid		= DB_88F69XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow	= DB_88F69XX_GPP_POL_LOW,
	.gppPolarityValMid	= DB_88F69XX_GPP_POL_MID,

	/* TDM */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f69xxTdm880},
	.boardTdmInfoIndex		= 0,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F69XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F69XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F69XX_BOARD_NAND_CONTROL,
	.nandIfMode				= NAND_IF_NFC,

	.pBoardTdmSpiInfo		= db88f69xxTdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= DB_88F69XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F69XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= 0,
	.pIoExp				= NULL,
	.boardOptionsModule		= MV_MODULE_NO_MODULE
};

MV_BOARD_INFO *marvellBoardInfoTbl[] = {
	&db88f69xx_board_info,
	&db88f69xx_board_info,
};
