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
 * A39x Customer Board 0 - Based on DB board
 *******************************************************************************/
#define A39X_CUSTOMER_BOARD_0_NAND_READ_PARAMS		0x000C0282
#define A39X_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define A39X_CUSTOMER_BOARD_0_NAND_CONTROL		0x01c00543

#define A39X_CUSTOMER_BOARD_0_NOR_READ_PARAMS		0x403E07CF
#define A39X_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS		0x000F0F0F
MV_BOARD_TWSI_INFO armada_39x_customer_0_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x50, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MAC_INFO armada_39x_customer_0_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr, MV_8
	 * boardEthSmiAddr0, MV_PHY_PORT_TYPE portType, MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x5, 0x5, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE}
};

MV_BOARD_NET_COMPLEX_INFO armada_39x_customer_0_InfoBoardNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC1_2_SGMII_L1 |
			MV_NETCOMP_GE_MAC2_2_SGMII_L3 |
			MV_NETCOMP_GE_MAC0_2_RXAUI,
	}
};

MV_DEV_CS_INFO armada_39x_customer_0_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS1,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS2,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS3,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS2,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS3,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_TRUE },	/* SPI1 DEV */
	{ SPI1_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS2,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS3,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE }	/* SPI1 DEV */
};

struct MV_BOARD_IO_EXPANDER armada_39x_customer_0_InfoBoardioExpValue[] = {
	{0, 6, 0xFF}, /* Config reg#0: all bits as input (BIT on = Input) */
	{0, 7, 0x7d}, /* Config reg#1: BIT7(VDDOE), BIT1(USB3 current limit) as output (BIT on = Input) */
	{0, 2, 0xFF}, /* Output Data, reg#0  - no output bits*/
	{0, 3, 0x7b}, /* Output Data, reg#1:  BIT7,VDDOE=0, BIT1,USB3_CURRENT=1 */
};

MV_BOARD_USB_INFO armada_39x_customer_0_BoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_FALSE},
	{ USB3_UNIT_ID, 1, MV_FALSE},
	{ USB_UNIT_ID,  0, MV_TRUE},
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

	.pBoardUsbInfo			= armada_39x_customer_0_BoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(armada_39x_customer_0_BoardUsbInfo),
	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= A39X_CUSTOMER_BOARD_0_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS,
	.nandFlashControl		= A39X_CUSTOMER_BOARD_0_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected               = MV_FALSE,
	/* NOR init params */
	.norFlashReadParams		= A39X_CUSTOMER_BOARD_0_NOR_READ_PARAMS,
	.norFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(armada_39x_customer_0_InfoBoardioExpValue),
	.pIoExp				= armada_39x_customer_0_InfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0,
	.isAmc				= MV_FALSE
};

/*******************************************************************************
 * A39x Customer Board 1 - Based on DB-88F6925-GP
 *******************************************************************************/
MV_BOARD_TWSI_INFO armada_39x_customer_1_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x57,	ADDR7_BIT, MV_TRUE}, /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4C,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21,	ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MAC_INFO armada_39x_customer_1_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr, MV_8
	 * boardEthSmiAddr0, MV_PHY_PORT_TYPE portType, MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE},
	{ BOARD_MAC_SPEED_1000M, -1, -1, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4, SMI, MV_FALSE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_FALSE}
};

MV_BOARD_NET_COMPLEX_INFO armada_39x_customer_1_InfoBoardNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC1_2_SGMII_L4,
	}
};

MV_DEV_CS_INFO armada_39x_customer_1_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_TRUE },	/* NAND DEV */
	{ DEVICE_CS1,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS2,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS3,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS2,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS3,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS1,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS2,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS3,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE }	/* SPI1 DEV */
};

struct MV_BOARD_IO_EXPANDER armada_39x_customer_1_InfoBoardioExpValue[] = {
	{0, 6, 0xFF}, /* Config reg#0: all bits as input (BIT on = Input) */
	{0, 7, 0x7d}, /* Config reg#1: BIT7(VDDOE), BIT1(USB3 current limit) as output (BIT on = Input) */
	{0, 2, 0xFF}, /* Output Data, reg#0  - no output bits*/
	{0, 3, 0x7b}, /* Output Data, reg#1:  BIT7,VDDOE=0, BIT1,USB3_CURRENT=1 */
};

MV_BOARD_USB_INFO armada_39x_customer_1_BoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_FALSE},
	{ USB3_UNIT_ID, 1, MV_FALSE},
	{ USB_UNIT_ID,  0, MV_TRUE},
};

MV_BOARD_MPP_INFO armada_39x_customer_1_BoardMppConfigValue[] = {
	{ {
		A39X_CUSTOMER_BOARD_1_MPP0_7,
		A39X_CUSTOMER_BOARD_1_MPP8_15,
		A39X_CUSTOMER_BOARD_1_MPP16_23,
		A39X_CUSTOMER_BOARD_1_MPP24_31,
		A39X_CUSTOMER_BOARD_1_MPP32_39,
		A39X_CUSTOMER_BOARD_1_MPP40_47,
		A39X_CUSTOMER_BOARD_1_MPP48_55,
		A39X_CUSTOMER_BOARD_1_MPP56_63,
	} }
};

MV_BOARD_TDM_INFO armada_39x_customer_1_Tdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO armada_39x_customer_1_TdmSpiInfo[] = { {1} };

MV_BOARD_INFO armada_39x_customer_board_1_info = {
	.boardName			= "A39x-Customer-Board-1",
	.numBoardNetComplexValue	= ARRSZ(armada_39x_customer_1_InfoBoardNetComplexInfo),
	.pBoardNetComplexInfo		= armada_39x_customer_1_InfoBoardNetComplexInfo,
	.pBoardMppConfigValue		= armada_39x_customer_1_BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(armada_39x_customer_1_BoardDeCsInfo),
	.pDevCsInfo			= armada_39x_customer_1_BoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(armada_39x_customer_1_BoardTwsiDev),
	.pBoardTwsiDev			= armada_39x_customer_1_BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(armada_39x_customer_1_BoardMacInfo),
	.pBoardMacInfo			= armada_39x_customer_1_BoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A39X_CUSTOMER_BOARD_1_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A39X_CUSTOMER_BOARD_1_GPP_OUT_ENA_MID,
	.gppOutValLow			= A39X_CUSTOMER_BOARD_1_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A39X_CUSTOMER_BOARD_1_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= A39X_CUSTOMER_BOARD_1_GPP_POL_LOW,
	.gppPolarityValMid		= A39X_CUSTOMER_BOARD_1_GPP_POL_MID,

	.pBoardUsbInfo			= armada_39x_customer_1_BoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(armada_39x_customer_1_BoardUsbInfo),
	/* TDM */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {armada_39x_customer_1_Tdm880},
	.boardTdmInfoIndex		= 0,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= A39X_CUSTOMER_BOARD_0_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS,
	.nandFlashControl		= A39X_CUSTOMER_BOARD_0_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected               = MV_FALSE,
	/* NOR init params */
	.norFlashReadParams		= A39X_CUSTOMER_BOARD_0_NOR_READ_PARAMS,
	.norFlashWriteParams		= A39X_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(armada_39x_customer_1_InfoBoardioExpValue),
	.pIoExp				= armada_39x_customer_1_InfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0,
	.isAmc				= MV_FALSE
};

/*
 * All supported 390 boards
 */
MV_BOARD_INFO *customerBoardInfoTbl[] = {
	&armada_39x_customer_board_0_info,
	&armada_39x_customer_board_1_info
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
	{ BOARD_DEV_TWSI_SATR,		0,	0x57,	ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4c,	ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		2,	0x4d,	ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		3,	0x4e,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x21,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	0,	0x20,	ADDR7_BIT, MV_FALSE},   /* modules */
	{ BOARD_TWSI_MODULE_DETECT,	1,	0x23,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	2,	0x24,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	3,	0x25,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	4,	0x26,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	5,	0x27,	ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MAC_INFO db88f69xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr, MV_8
	 * boardEthSmiAddr0, MV_PHY_PORT_TYPE portType, MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x5, 0x5, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE}
};

struct MV_BOARD_IO_EXPANDER db88f69xxInfoBoardioExpValue[] = {
	{0, 6, 0xFF}, /* Config reg#0: all bits as input (BIT on = Input) */
	{0, 7, 0x7d}, /* Config reg#1: BIT7(VDDOE), BIT1(USB3 current limit) as output (BIT on = Input) */
	{0, 2, 0xFF}, /* Output Data, reg#0  - no output bits*/
	{0, 3, 0x7b}, /* Output Data, reg#1:  BIT7,VDDOE=0, BIT1,USB3_CURRENT=1 */
};

MV_BOARD_NET_COMPLEX_INFO db88f69xxInfoBoarNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC1_2_SGMII_L1 |
			MV_NETCOMP_GE_MAC2_2_SGMII_L3 |
			MV_NETCOMP_GE_MAC0_2_RXAUI,
	}
};

MV_BOARD_USB_INFO db88f69xxInfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_FALSE},
	{ USB3_UNIT_ID, 1, MV_TRUE},
	{ USB_UNIT_ID,  0, MV_TRUE},
};

MV_DEV_CS_INFO db88f69xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_TRUE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_TRUE }	/* SPI1 DEV */
};

MV_BOARD_INFO db88f69xx_board_info = {
	.boardName			= "DB-88F6920-BP",
	.compatibleDTName		= "a390-db",
	.numBoardNetComplexValue	= ARRSZ(db88f69xxInfoBoarNetComplexInfo),
	.pBoardNetComplexInfo		= db88f69xxInfoBoarNetComplexInfo,
	.pBoardMppConfigValue		= db88f69xxInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f69xxInfoBoardDeCsInfo),
	.pDevCsInfo			= db88f69xxInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db88f69xxInfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f69xxInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f69xxInfoBoardMacInfo),
	.pBoardMacInfo			= db88f69xxInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F69XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F69XX_GPP_OUT_ENA_MID,
	.gppOutValLow			= DB_88F69XX_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F69XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= DB_88F69XX_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F69XX_GPP_POL_MID,

	.pBoardUsbInfo			= db88f69xxInfoBoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(db88f69xxInfoBoardUsbInfo),

	/* TDM */
	.numBoardTdmInfo		= { },
	.pBoardTdmInt2CsInfo		= { },
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F69XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F69XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F69XX_BOARD_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.pBoardTdmSpiInfo		= NULL,

	 .isSdMmcConnected		= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams		= DB_88F69XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F69XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(db88f69xxInfoBoardioExpValue),
	.pIoExp				= db88f69xxInfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	/* Audio */
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0,
	.isAmc				= MV_FALSE,

	.modelName			= "A390 Development Board"
};

/*******************************************************************************
 * 39x GP-EAP-10G-88F6925 board */
/*******************************************************************************/
MV_BOARD_TWSI_INFO gp88f6925BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass,MV_U8 devClassId,MV_U8 twsiDevAddr,MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0, 0x57, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1, 0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0, 0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1, 0x21, ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MPP_INFO gp88f6925BoardMppConfigValue[] = {
	{ {
		GP_EAP_10G_88f6925_MPP0_7,
		GP_EAP_10G_88f6925_MPP8_15,
		GP_EAP_10G_88f6925_MPP16_23,
		GP_EAP_10G_88f6925_MPP24_31,
		GP_EAP_10G_88f6925_MPP32_39,
		GP_EAP_10G_88f6925_MPP40_47,
		GP_EAP_10G_88f6925_MPP48_55,
		GP_EAP_10G_88f6925_MPP56_63,
	} }
};

MV_BOARD_NET_COMPLEX_INFO gp88f6925InfoBoarNetComplexInfo[] = {
	{
		.netComplexOpt = MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC1_2_SGMII_L4,
	}
};

struct MV_BOARD_SWITCH_INFO gp88f6925SwitchInfo[] = {
	{
		.isEnabled = MV_TRUE,
		.isCpuPortRgmii = MV_FALSE,
		.switchIrq = -1,	/* set to -1 for using PPU*/
		.switchPort = {0, 1, 2, 3, 4, 5, 6},
		.cpuPort = 5,
		.connectedPort = {5, -1},
		.smiScanMode = MV_SWITCH_SMI_MULTI_ADDR_MODE,
		.quadPhyAddr = 0x10,
		.forceLinkMask = 0x60
	}
};

MV_BOARD_MAC_INFO gp88f6925InfoBoardMacInfo[] = {
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, XSMI, MV_TRUE},
	{ BOARD_MAC_SPEED_1000M, -1, -1, SMI, MV_TRUE},
	{ BOARD_MAC_SPEED_1000M, -1, -1, SMI, MV_FALSE},
	{ BOARD_MAC_SPEED_1000M, -1, -1, SMI, MV_FALSE}
};

MV_BOARD_USB_INFO gp88f6925InfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID,	0, MV_TRUE},
	{ USB3_UNIT_ID,	1, MV_FALSE},
	{ USB_UNIT_ID,	0, MV_TRUE},
};

MV_DEV_CS_INFO gp88f6925InfoBoardDeCsInfo[] = {
/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0, MV_TRUE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0, MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0, MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1, MV_FALSE }	/* SPI1 DEV */
};

struct MV_BOARD_IO_EXPANDER gp88f6925InfoBoardioExpValue[] = {
	{0, 6, 0xAA}, /* Config reg#0: BIT0(SGMII select), BIT2(PCIe1_W disable),
			 BIT4(PCIe2_W disable), BIT6(PCIe3_W disable) */
	{0, 7, 0xF8}, /* Config reg#1: BIT0(PWR_EN_SATA0), BIT1(SDIO), BIT2(PWR_EN_Module) */
	{0, 2, 0xFE}, /* Output Data, reg#0: BIT0, SGMII=0 */
	{0, 3, 0xFD}, /* Output Data, reg#1: BIT1, SDIO=0 */
};

MV_BOARD_GPP_INFO gp88f6925InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
/* USB_Host0 */	{BOARD_GPP_USB_VBUS,    39}, /* from MPP map */
};

MV_BOARD_INFO gp88f6925_board_info = {
	.boardName			= "DB-88F6925-GP",
	.numBoardNetComplexValue	= ARRSZ(gp88f6925InfoBoarNetComplexInfo),
	.pBoardNetComplexInfo		= gp88f6925InfoBoarNetComplexInfo,
	.pBoardMppConfigValue		= gp88f6925BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(gp88f6925InfoBoardDeCsInfo),
	.pDevCsInfo			= gp88f6925InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(gp88f6925BoardTwsiDev),
	.pBoardTwsiDev			= gp88f6925BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(gp88f6925InfoBoardMacInfo),
	.pBoardMacInfo			= gp88f6925InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(gp88f6925InfoBoardGppInfo),
	.pBoardGppInfo			= gp88f6925InfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= GP_EAP_10G_88f6925_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= GP_EAP_10G_88f6925_GPP_OUT_ENA_MID,
	.gppOutValLow			= GP_EAP_10G_88f6925_GPP_OUT_VAL_LOW,
	.gppOutValMid			= GP_EAP_10G_88f6925_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= GP_EAP_10G_88f6925_GPP_POL_LOW,
	.gppPolarityValMid		= GP_EAP_10G_88f6925_GPP_POL_MID,

	.pBoardUsbInfo			= gp88f6925InfoBoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(gp88f6925InfoBoardUsbInfo),

	/* TDM */
	.numBoardTdmInfo		= { },
	.pBoardTdmInt2CsInfo		= { },
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F69XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F69XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F69XX_BOARD_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.pBoardTdmSpiInfo		= NULL,

	.isSdMmcConnected		= MV_TRUE,

	/* NOR init params */
	.norFlashReadParams		= DB_88F69XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F69XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(gp88f6925InfoBoardioExpValue),
	.pIoExp				= gp88f6925InfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	/* Audio */
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= gp88f6925SwitchInfo,
	.switchInfoNum			= ARRSZ(gp88f6925SwitchInfo),
	.isAmc				= MV_FALSE,

	.modelName			= "A395 General Purpose Development Board"
};


MV_BOARD_INFO *marvellBoardInfoTbl[] = {
	&db88f69xx_board_info,
	&gp88f6925_board_info,
};
