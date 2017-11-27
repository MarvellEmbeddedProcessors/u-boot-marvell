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
 * A38x Customer Board 0 - Based on DB-BP
 *******************************************************************************/
#define A38x_CUSTOMER_BOARD_0_NAND_READ_PARAMS		0x000C0282
#define A38x_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS		0x00010305
/*NAND care support for small page chips*/
#define A38x_CUSTOMER_BOARD_0_NAND_CONTROL		0x01c00543

#define A38x_CUSTOMER_BOARD_0_NOR_READ_PARAMS		0x403E07CF
#define A38x_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS		0x000F0F0F

MV_BOARD_TWSI_INFO armada_38x_customer_0_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	0,	0x57, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,	1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO armada_38x_customer_0_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
};

MV_DEV_CS_INFO armada_38x_customer_0_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS1,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS2,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS3,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE },	/* SPI0 DEV */
	{ SPI0_CS1,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS2,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS3,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS1,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS2,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS3,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE }	/* SPI1 DEV */
};

MV_BOARD_MPP_INFO armada_38x_customer_0_BoardMppConfigValue[] = {
	{ {
		A38x_CUSTOMER_BOARD_0_MPP0_7,
		A38x_CUSTOMER_BOARD_0_MPP8_15,
		A38x_CUSTOMER_BOARD_0_MPP16_23,
		A38x_CUSTOMER_BOARD_0_MPP24_31,
		A38x_CUSTOMER_BOARD_0_MPP32_39,
		A38x_CUSTOMER_BOARD_0_MPP40_47,
		A38x_CUSTOMER_BOARD_0_MPP48_55,
		A38x_CUSTOMER_BOARD_0_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER armada_38x_customer_0_IoExpanderInfo[] = {
	{0, 6, 0xF4}, /* Configuration registers: Bit on --> Input bits  */
	{0, 7, 0xC3}, /* Configuration registers: Bit on --> Input bits  */
	{0, 2, 0x0B}, /* Output Data, register#0 */
	{0, 3, 0x18}, /* Output Data, register#1 */
	{1, 6, 0xE7}, /* Configuration registers: Bit on --> Input bits  */
	{1, 7, 0xF9}, /* Configuration registers: Bit on --> Input bits  */
	{1, 2, 0x08}, /* Output Data, register#0 */
	{1, 3, 0x00}  /* Output Data, register#1 */
};

MV_BOARD_GPP_INFO armada_38x_customer_0_GppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
/* USB_Host0 */ /* {BOARD_GPP_USB_VBUS,    44}, */ /* from MPP map */
};

MV_BOARD_USB_INFO armada_38x_customer_0_InfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOLEAN isActive} */
	{ USB3_UNIT_ID, 0, MV_TRUE},
	{ USB3_UNIT_ID, 1, MV_TRUE},
	{ USB_UNIT_ID, 0, MV_TRUE},
};

MV_BOARD_INFO armada_38x_customer_board_0_info = {
	.boardName			= "A38x-Customer-Board-0",
	.numBoardNetComplexValue		= 0,
	.pBoardNetComplexInfo		= NULL,
	.pBoardMppConfigValue		= armada_38x_customer_0_BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(armada_38x_customer_0_BoardDeCsInfo),
	.pDevCsInfo				= armada_38x_customer_0_BoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(armada_38x_customer_0_BoardTwsiDev),
	.pBoardTwsiDev			= armada_38x_customer_0_BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(armada_38x_customer_0_BoardMacInfo),
	.pBoardMacInfo			= armada_38x_customer_0_BoardMacInfo,
	.numBoardGppInfo		= ARRSZ(armada_38x_customer_0_GppInfo),
	.pBoardGppInfo			= armada_38x_customer_0_GppInfo,
	.numBoardIoExpPinInfo		= 0,
	.pBoardIoExpPinInfo		= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A38x_CUSTOMER_BOARD_0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A38x_CUSTOMER_BOARD_0_GPP_OUT_ENA_MID,
	.gppOutValLow			= A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= A38x_CUSTOMER_BOARD_0_GPP_POL_LOW,
	.gppPolarityValMid		= A38x_CUSTOMER_BOARD_0_GPP_POL_MID,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardUsbInfo                  = armada_38x_customer_0_InfoBoardUsbInfo,
	.numBoardUsbInfo                = ARRSZ(armada_38x_customer_0_InfoBoardUsbInfo),

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= A38x_CUSTOMER_BOARD_0_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A38x_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS,
	.nandFlashControl		= A38x_CUSTOMER_BOARD_0_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,

	/* NOR init params */
	.norFlashReadParams		= A38x_CUSTOMER_BOARD_0_NOR_READ_PARAMS,
	.norFlashWriteParams		= A38x_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(armada_38x_customer_0_IoExpanderInfo),
	.pIoExp				= armada_38x_customer_0_IoExpanderInfo,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0
};

/*******************************************************************************
 * A38x Customer Board 1 - Based on DB-GP
 *******************************************************************************/
MV_BOARD_TWSI_INFO armada_38x_customer_1_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO armada_38x_customer_1_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
	{ BOARD_MAC_SPEED_1000M, -1, -1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO,  -1,  -1, MV_TRUE}
};

MV_DEV_CS_INFO armada_38x_customer_1_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS1,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS2,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS3,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS1,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS2,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI0_CS3,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI0 DEV */
	{ SPI1_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_TRUE },	/* SPI1 DEV */
	{ SPI1_CS1,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS2,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE },	/* SPI1 DEV */
	{ SPI1_CS3,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE }	/* SPI1 DEV */
};

MV_BOARD_MPP_INFO armada_38x_customer_1_BoardMppConfigValue[] = {
	{ {
		A38x_CUSTOMER_BOARD_1_MPP0_7,
		A38x_CUSTOMER_BOARD_1_MPP8_15,
		A38x_CUSTOMER_BOARD_1_MPP16_23,
		A38x_CUSTOMER_BOARD_1_MPP24_31,
		A38x_CUSTOMER_BOARD_1_MPP32_39,
		A38x_CUSTOMER_BOARD_1_MPP40_47,
		A38x_CUSTOMER_BOARD_1_MPP48_55,
		A38x_CUSTOMER_BOARD_1_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER armada_38x_customer_1_IoExpanderInfo[] = {
	{0, 2, 0x40}, /* Deassert both mini pcie reset signals */
	{0, 6, 0xf9},
	{0, 2, 0x46}, /* Assert reset signals and enable USB3 current limiter */
	{0, 6, 0xb9},
	{0, 3, 0x0}, /* Set SFP_TX_DIS to zero */
	{0, 7, 0xbf}, /* Drive SFP_TX_DIS to zero */
};

MV_BOARD_USB_INFO armada_38x_customer_1_InfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_TRUE},
	{ USB3_UNIT_ID, 1, MV_TRUE},    /* xHCI port#1 connected only via USB2.0 UTMI (not via USB3.0 SerDes lane) */
	{ USB_UNIT_ID,  0, MV_TRUE},
};

struct MV_BOARD_SWITCH_INFO cfSwitchInfo[] = {
	{
		.isEnabled = MV_TRUE,
		.isCpuPortRgmii = MV_FALSE,
		.switchIrq = -1,	/* set to -1 for using PPU*/
		.switchPort = {0, 1, 2, 3, 4, 5, 6},
		.cpuPort = 5,
		.connectedPort = {-1, 5, -1},
		.smiScanMode = MV_SWITCH_SMI_MULTI_ADDR_MODE,
		.quadPhyAddr = 4,
		.forceLinkMask = 0x60
	}
};

MV_BOARD_IO_EXPANDER_TYPE_INFO armada_38x_customer_1_ioExpPinInfo[] = {
/*	{ IO Type enum,			bit offset, Io.exp num,		reg Num */
	{ MV_IO_EXPANDER_USB_VBUS,	6,		0,		2} /* VBUS_EN: IO.exp#1 (0x21), reg #3, bit 7 */
};

void a38x_customer_board_1_gpp_callback(MV_BOARD_INFO *board) {
	int i;
	/* Toggle GPIO41 to reset onboard switch and phy */
	MV_REG_BIT_RESET (0x18140, 0x200);
	MV_REG_BIT_RESET (0x18144, 0x200);
	/* GPIO 19 on ClearFog rev 2.1 controls the uSOM onboard phy reset */
	MV_REG_BIT_RESET (0x18100, 0x00080000);
	MV_REG_BIT_RESET (0x18104, 0x00080000);
	mvOsDelay(1);
	MV_REG_BIT_SET (0x18140, 0x200);
	MV_REG_BIT_SET (0x18100, 0x00080000);
	mvOsDelay(10);
	for (i = 0; i < board->numIoExp; i++) {
		if (MV_OK != mvBoardTwsiSet(BOARD_TWSI_IO_EXPANDER, board->pIoExp[i].addr,
					    board->pIoExp[i].offset, &board->pIoExp[i].val, 1)) {
			mvOsPrintf("%s: Write IO expander (addr=0x%x, offset=%d, value=0x%2x to  fail\n",
				   __func__,
				   mvBoardTwsiAddrGet(BOARD_TWSI_IO_EXPANDER, board->pIoExp[i].addr),
				   board->pIoExp[i].offset,
				   board->pIoExp[i].val);
		}
	}
}

MV_BOARD_INFO armada_38x_customer_board_1_info = {
	.boardName			= "A38x-Customer-Board-1",
	.numBoardNetComplexValue	= 0,
	.pBoardNetComplexInfo		= NULL,
	.pBoardMppConfigValue		= armada_38x_customer_1_BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(armada_38x_customer_1_BoardDeCsInfo),
	.pDevCsInfo				= armada_38x_customer_1_BoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(armada_38x_customer_1_BoardTwsiDev),
	.pBoardTwsiDev			= armada_38x_customer_1_BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(armada_38x_customer_1_BoardMacInfo),
	.pBoardMacInfo			= armada_38x_customer_1_BoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.numBoardIoExpPinInfo		= ARRSZ(armada_38x_customer_1_ioExpPinInfo),
	.pBoardIoExpPinInfo		= armada_38x_customer_1_ioExpPinInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A38x_CUSTOMER_BOARD_1_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A38x_CUSTOMER_BOARD_1_GPP_OUT_ENA_MID,
	.gppOutValLow			= A38x_CUSTOMER_BOARD_1_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A38x_CUSTOMER_BOARD_1_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= A38x_CUSTOMER_BOARD_1_GPP_POL_LOW,
	.gppPolarityValMid		= A38x_CUSTOMER_BOARD_1_GPP_POL_MID,
	.gppPostConfigCallBack		= a38x_customer_board_1_gpp_callback,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	.pBoardUsbInfo			= armada_38x_customer_1_InfoBoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(armada_38x_customer_1_InfoBoardUsbInfo),

	/* NAND init params */
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
	.nandFlashControl		= 0,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,

	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE,
	.numIoExp			= ARRSZ(armada_38x_customer_1_IoExpanderInfo),
	.pIoExp				= armada_38x_customer_1_IoExpanderInfo,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.pSwitchInfo			= cfSwitchInfo,
	.switchInfoNum			= ARRSZ(cfSwitchInfo)
};

/*
 * All supported A380 customer boards
 */
MV_BOARD_INFO *customerBoardInfoTbl[] = {
	&armada_38x_customer_board_0_info,
	&armada_38x_customer_board_1_info
};


/***************************************** Marvell Boards *****************************************/
/*******************************************************************************
* A38x DB-88F68XX-BP board */
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
	{ BOARD_DEV_TWSI_SATR,		0,	0x57,	   ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4c,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		2,	0x4d,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		3,	0x4e,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x21,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	0,	0x20,	   ADDR7_BIT, MV_FALSE},   /* modules */
	{ BOARD_TWSI_MODULE_DETECT,	1,	0x23,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	2,	0x24,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	3,	0x25,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	4,	0x26,	   ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	5,	0x27,	   ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO db88f68xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0, MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
};

MV_BOARD_USB_INFO db88f68xxInfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_TRUE},
	{ USB3_UNIT_ID, 1, MV_TRUE},
	{ USB_UNIT_ID,  0, MV_TRUE},
};

MV_DEV_CS_INFO db88f68xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE },	/* SPI0 DEV */
	{ SPI1_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_FALSE }	/* SPI1 DEV */
};

MV_BOARD_TDM_INFO db88f68xxTdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO db88f68xxTdmSpiInfo[] = { {1} };

struct MV_BOARD_SWITCH_INFO db88f68xxSwitchInfo[] = {
	{
		.isEnabled = MV_FALSE,
		.isCpuPortRgmii = MV_TRUE,
		.switchIrq = -1,	/* set to -1 for using PPU*/
		.switchPort = {0, 1, 2, 3, 4, 5, 6},
		.cpuPort = 6,
		.connectedPort = {5, 6, -1},
		.smiScanMode = MV_SWITCH_SMI_MULTI_ADDR_MODE,
		.quadPhyAddr = 2,
		.forceLinkMask = 0x60
	}
};

struct MV_BOARD_IO_EXPANDER db88f68xxInfoBoardioExpValue[] = {
	{0, 6, 0xFF}, /* Config reg#0: all bits as input (BIT on = Input) */
	{0, 7, 0xFC}, /* Config reg#1: BIT0(USB3.0 current limit), BIT1(USB3.1 current limit)*/
	{0, 2, 0xFF}, /* Output Data, reg#0  - no output bits*/
	{0, 3, 0xFF}, /* Output Data, reg#1:  BIT0,USB3.0_CURRENT=1, BIT1,USB3.1_CURRENT=1 */
};

MV_BOARD_INFO db88f68xx_board_info = {
	.boardName		= "DB-88F6820-BP",
	.compatibleDTName	= "a385-db",
	.numBoardNetComplexValue	= 0,
	.pBoardNetComplexInfo	= NULL,
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
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
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

	.pBoardUsbInfo		= db88f68xxInfoBoardUsbInfo,
	.numBoardUsbInfo	= ARRSZ(db88f68xxInfoBoardUsbInfo),

	/* TDM */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f68xxTdm880},
	.boardTdmInfoIndex		= 0,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F68XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F68XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_88F68XX_BOARD_NAND_CONTROL,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,
	.isSdMmc_1_8v_Connected		= MV_FALSE,

	.pBoardTdmSpiInfo		= db88f68xxTdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= DB_88F68XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F68XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(db88f68xxInfoBoardioExpValue),
	.pIoExp				= db88f68xxInfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= db88f68xxSwitchInfo,
	.switchInfoNum			= ARRSZ(db88f68xxSwitchInfo)
};

/*******************************************************************************
 * A380 RD-NAS-88F68XX board */
/*******************************************************************************/
MV_BOARD_TWSI_INFO rd88F68XXInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	0,	0x57, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,	1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO rd88F68XXInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0, MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE}
};

MV_DEV_CS_INFO rd88F68XXInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ SPI0_CS0,	N_A,	BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE }  /* SPI DEV */
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
	{0, 6, 0xF4}, /* Configuration registers: Bit on --> Input bits  */
	{0, 7, 0xC3}, /* Configuration registers: Bit on --> Input bits  */
	{0, 2, 0x0B}, /* Output Data, register#0 */
	{0, 3, 0x18}, /* Output Data, register#1 */
	{1, 6, 0xE7}, /* Configuration registers: Bit on --> Input bits  */
	{1, 7, 0xF9}, /* Configuration registers: Bit on --> Input bits  */
	{1, 2, 0x08}, /* Output Data, register#0 */
	{1, 3, 0x00}  /* Output Data, register#1 */
};

MV_BOARD_INFO rdNas88f68XX_board_info = {
	.boardName			= "RD-NAS-88F6820-DDR3",
	.numBoardNetComplexValue	= 0,
	.pBoardNetComplexInfo		= NULL,
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
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
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
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,
	.isSdMmc_1_8v_Connected		= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(ioExpNas),
	.pIoExp				= ioExpNas,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0
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
	{0, 6, 0xF4}, /* Configuration registers: Bit on --> Input bits  */
	{0, 7, 0xC3}, /* Configuration registers: Bit on --> Input bits  */
	{0, 2, 0x0B}, /* Output Data, register#0 */
	{0, 3, 0x18}, /* Output Data, register#1 */
	{1, 6, 0xE7}, /* Configuration registers: Bit on --> Input bits  */
	{1, 7, 0xF9}, /* Configuration registers: Bit on --> Input bits  */
	{1, 2, 0x18}, /* Output Data, register#0 */
	{1, 3, 0x06}  /* Output Data, register#1 */
};
MV_BOARD_INFO rdWAP88f68XX_board_info = {
	.boardName			= "RD-AP-88F6820-DDR3",
	.numBoardNetComplexValue	= 0,
	.pBoardNetComplexInfo		= NULL,
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
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
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
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,
	.isSdMmc_1_8v_Connected		= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(ioExpWap),
	.pIoExp				= ioExpWap,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0
};

/*******************************************************************************
* A380 DB-AP-88F6660 board */
/*******************************************************************************/
#define DB_AP_88F68XX_BOARD_NAND_READ_PARAMS       0x000C0282
#define DB_AP_88F68XX_BOARD_NAND_WRITE_PARAMS      0x00010305
/*NAND care support for small page chips*/
#define DB_AP_88F68XX_BOARD_NAND_CONTROL           0x01c00543

MV_BOARD_MPP_INFO dbAP88f68xxInfoBoardMppConfigValue[] = {
	{ {
		  DB_AP_88F68XX_MPP0_7,
		  DB_AP_88F68XX_MPP8_15,
		  DB_AP_88F68XX_MPP16_23,
		  DB_AP_88F68XX_MPP24_31,
		  DB_AP_88F68XX_MPP32_39,
		  DB_AP_88F68XX_MPP40_47,
		  DB_AP_88F68XX_MPP48_55,
		  DB_AP_88F68XX_MPP56_63
	  } }
};

MV_BOARD_TWSI_INFO dbAP88f68xxInfoBoardTwsiDev[] = {
	/* {{devClass,          devClassId, twsiDevAddr, twsiDevAddrType, moreThen256}} */
	{ BOARD_DEV_TWSI_SATR,          0,      0x57,      ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,          1,      0x4c,      ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,          2,      0x4d,      ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,          3,      0x4e,      ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,          4,      0x21,      ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,     0,      0x20,      ADDR7_BIT, MV_FALSE},   /* modules */
	{ BOARD_TWSI_MODULE_DETECT,     1,      0x23,      ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,     2,      0x24,      ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,     3,      0x25,      ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,     4,      0x26,      ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,     5,      0x27,      ADDR7_BIT, MV_FALSE},
};

MV_BOARD_MAC_INFO dbAP88f68xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x6, 0x4, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x4, 0x4, MV_TRUE},
};

MV_BOARD_USB_INFO dbAP88f68xxInfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_TRUE},
	{ USB_UNIT_ID,  0, MV_TRUE},
};

MV_DEV_CS_INFO dbAP88f68xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_TRUE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE }	/* NOR DEV */
};

MV_BOARD_GPP_INFO dbAP88f68xxInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
/* USB_Host0 */	{BOARD_GPP_USB_VBUS,    44}, /* from MPP map */
};

MV_BOARD_TDM_INFO dbAP88f68xxTdm880[] = { {0} };

MV_BOARD_TDM_SPI_INFO dbAP88f68xxTdmSpiInfo[] = { {1} };

MV_U8 dbAP88f68XXInfoBoardPicGpio[] = {21, 47};

MV_BOARD_INFO dbAP88f68xx_board_info = {
	.boardName              = "DB-88F6820-AP",
	.compatibleDTName	= "a385-db-ap",
	.numBoardNetComplexValue   = 0,
	.pBoardNetComplexInfo     = NULL,
	.pBoardMppConfigValue   = dbAP88f68xxInfoBoardMppConfigValue,
	.intsGppMaskLow         = 0,
	.intsGppMaskMid         = 0,
	.intsGppMaskHigh        = 0,
	.numBoardDeviceIf       = ARRSZ(dbAP88f68xxInfoBoardDeCsInfo),
	.pDevCsInfo             = dbAP88f68xxInfoBoardDeCsInfo,
	.numBoardTwsiDev        = ARRSZ(dbAP88f68xxInfoBoardTwsiDev),
	.pBoardTwsiDev          = dbAP88f68xxInfoBoardTwsiDev,
	.numBoardMacInfo        = ARRSZ(dbAP88f68xxInfoBoardMacInfo),
	.pBoardMacInfo          = dbAP88f68xxInfoBoardMacInfo,
	.numBoardGppInfo	= ARRSZ(dbAP88f68xxInfoBoardGppInfo),
	.pBoardGppInfo		= dbAP88f68xxInfoBoardGppInfo,
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
	.activeLedsNumber       = 0,
	.pLedGppPin             = NULL,
	.ledsPolarity           = 0,	/* PMU Power */
	.pmuPwrUpPolarity       = 0,
	.pmuPwrUpDelay          = 80000,	/* GPP values */
	.gppOutEnValLow         = DB_AP_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid         = DB_AP_88F68XX_GPP_OUT_ENA_MID,
	.gppOutEnValHigh        = DB_AP_88F68XX_GPP_OUT_ENA_HIGH,
	.gppOutValLow           = DB_AP_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid           = DB_AP_88F68XX_GPP_OUT_VAL_MID,
	.gppOutValHigh          = DB_AP_88F68XX_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow      = DB_AP_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid      = DB_AP_88F68XX_GPP_POL_MID,
	.gppPolarityValHigh     = DB_AP_88F68XX_GPP_POL_HIGH,

	.pBoardUsbInfo		= dbAP88f68xxInfoBoardUsbInfo,
	.numBoardUsbInfo	= ARRSZ(dbAP88f68xxInfoBoardUsbInfo),

	/* TDM */
	.numBoardTdmInfo                = {1},
	.pBoardTdmInt2CsInfo            = {dbAP88f68xxTdm880},
	.boardTdmInfoIndex              = 0,
	.pBoardSpecInit                 = NULL,
	.nandIfMode			= NAND_IF_NFC,

	.picGpioInfo			= dbAP88f68XXInfoBoardPicGpio,
	.numPicGpioInfo			= ARRSZ(dbAP88f68XXInfoBoardPicGpio),

	.isSdMmcConnected		= MV_FALSE,
	.isSdMmc_1_8v_Connected		= MV_FALSE,

	/* Enable modules auto-detection. */
	.configAutoDetect               = MV_FALSE,
	.numIoExp                       = 0,
	.pIoExp                         = NULL,
	.boardOptionsModule             = MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0
};

/*******************************************************************************
 * A380 DB-GP-88F68XX board */
/*******************************************************************************/
MV_BOARD_TWSI_INFO dbGP88F68XXInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	0,	0x57, ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,	1,	0x4C, ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,	2,	0x4D, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x20, ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	1,	0x21, ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO dbGP88F68XXInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO,  -1,  -1, MV_TRUE}
};

MV_BOARD_USB_INFO dbGP88f68xxInfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 0, MV_TRUE},
	{ USB3_UNIT_ID, 1, MV_TRUE},	/* xHCI port#1 connected only via USB2.0 UTMI (not via USB3.0 SerDes lane) */
	{ USB_UNIT_ID,  0, MV_TRUE},
};

MV_DEV_CS_INFO dbGP88F68XXInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ SPI0_CS0,	N_A,	BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE }  /* SPI DEV */
};

MV_BOARD_MPP_INFO dbGP88f68XXInfoBoardMppConfigValue[] = {
	{ {
		DB_GP_88F68XX_MPP0_7,
		DB_GP_88F68XX_MPP8_15,
		DB_GP_88F68XX_MPP16_23,
		DB_GP_88F68XX_MPP24_31,
		DB_GP_88F68XX_MPP32_39,
		DB_GP_88F68XX_MPP40_47,
		DB_GP_88F68XX_MPP48_55,
		DB_GP_88F68XX_MPP56_63,
	} }
};

struct MV_BOARD_IO_EXPANDER dbGP88f68XXInfoBoardioExpValue[] = {
	{0, 6, 0x20}, /* Configuration registers: Bit on --> Input bits */
	{0, 7, 0xC3}, /* Configuration registers: Bit on --> Input bits */
	{0, 2, 0x1D}, /* Output Data, register#0 */
	{0, 3, 0x3C}, /* Output Data, register#1 */
	{1, 6, 0xC3}, /* Configuration registers: Bit on --> Input bits  */
	{1, 7, 0x31}, /* Configuration registers: Bit on --> Input bits  */
	{1, 2, 0x08}, /* Output Data, register#0 */
	{1, 3, 0x40}  /* Output Data, register#1 */
};

MV_BOARD_IO_EXPANDER_TYPE_INFO dbGP88f68XXInfoBoardioExpPinInfo[] = {
/*	{ IO Type enum,			bit offset, Io.exp num,		reg Num */
	{ MV_IO_EXPANDER_USB_VBUS,	7,		1,		3} /* VBUS_EN: IO.exp#1 (0x21), reg #3, bit 7 */
};

MV_U8 dbGP88f68XXInfoBoardPicGpio[] = {33, 34, 35};

MV_BOARD_INFO dbGP88f68XX_board_info = {
	.boardName			= "DB-88F6820-GP",
	.compatibleDTName		= "a388-db-gp",
	.numBoardNetComplexValue	= 0,
	.pBoardNetComplexInfo		= NULL,
	.pBoardMppConfigValue		= dbGP88f68XXInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(dbGP88F68XXInfoBoardDeCsInfo),
	.pDevCsInfo				= dbGP88F68XXInfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(dbGP88F68XXInfoBoardTwsiDev),
	.pBoardTwsiDev			= dbGP88F68XXInfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(dbGP88F68XXInfoBoardMacInfo),
	.pBoardMacInfo			= dbGP88F68XXInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.numBoardIoExpPinInfo		= ARRSZ(dbGP88f68XXInfoBoardioExpPinInfo),
	.pBoardIoExpPinInfo		= dbGP88f68XXInfoBoardioExpPinInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_GP_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_GP_88F68XX_GPP_OUT_ENA_MID,
	.gppOutValLow			= DB_GP_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_GP_88F68XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow		= DB_GP_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid		= DB_GP_88F68XX_GPP_POL_MID,

	.pBoardUsbInfo			= dbGP88f68xxInfoBoardUsbInfo,
	.numBoardUsbInfo		= ARRSZ(dbGP88f68xxInfoBoardUsbInfo),

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.picGpioInfo			= dbGP88f68XXInfoBoardPicGpio,
	.numPicGpioInfo			= ARRSZ(dbGP88f68XXInfoBoardPicGpio),

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
	.nandFlashControl		= 0,
	.nandIfMode			= NAND_IF_NFC,

	.isSdMmcConnected		= MV_TRUE,
	.isSdMmc_1_8v_Connected		= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams		= 0,
	.norFlashWriteParams		= 0,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE,
	.numIoExp			= ARRSZ(dbGP88f68XXInfoBoardioExpValue),
	.pIoExp				= dbGP88f68XXInfoBoardioExpValue,
	.boardOptionsModule		= MV_MODULE_NO_MODULE,
	.isAmc				= MV_FALSE,
	.isAudioConnected		= MV_FALSE,
	.isTdmConnected			= MV_FALSE,
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0
};

/*******************************************************************************
 * A381 DB-88F6821-BP board */
/*******************************************************************************/
MV_BOARD_MPP_INFO db88f6821InfoBoardMppConfigValue[] = {
	{ {
		  DB_BP_88F6821_MPP0_7,
		  DB_BP_88F6821_MPP8_15,
		  DB_BP_88F6821_MPP16_23,
		  DB_BP_88F6821_MPP24_31,
		  DB_BP_88F6821_MPP32_39,
		  DB_BP_88F6821_MPP40_47,
	 } }
};

MV_BOARD_TWSI_INFO db88f6821InfoBoardTwsiDev[] = {
	/* {{devClass,		devClassId, twsiDevAddr, twsiDevAddrType, moreThen256}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x57,	ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4c,	ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		2,	0x4d,	ADDR7_BIT, MV_FALSE},
	{ BOARD_DEV_TWSI_SATR,		3,	0x4e,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_IO_EXPANDER,	0,	0x21,	ADDR7_BIT, MV_FALSE},
	{ BOARD_TWSI_MODULE_DETECT,	0,	0x56,	ADDR7_BIT, MV_TRUE},   /* SLM 1426/1427 module */
};
MV_BOARD_MAC_INFO db88f6821InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
};

MV_BOARD_USB_INFO db88f6821InfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB3_UNIT_ID, 1, MV_TRUE},
};

MV_DEV_CS_INFO db88f6821InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI0_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE }	/* SPI0 DEV */
};

struct MV_BOARD_IO_EXPANDER db88f6821InfoBoardioExpValue[] = {
	{0, 6, 0xFF}, /* Config reg#0: all bits as input -TBD (BIT on = Input) */
	{0, 7, 0x7E}, /* Config reg#1: BIT0(SFP_TX_DIS), BIT7(USB3 current limit) as output (BIT on = Input) */
	{0, 2, 0xFF}, /* Output Data, reg#0  - no output bits*/
	{0, 3, 0x80}, /* Output Data, reg#1:  BIT0,SFP_TX_DIS=0, BIT7,USB3_CURRENT=1 */
};

MV_BOARD_INFO db88f6821_board_info = {
	.boardName		= "DB-88F6821-BP",
	.compatibleDTName	= "a382-db",
	.numBoardNetComplexValue = 0,
	.pBoardNetComplexInfo	= NULL,
	.pBoardMppConfigValue	= db88f6821InfoBoardMppConfigValue,
	.intsGppMaskLow		= 0,
	.intsGppMaskMid		= 0,
	.intsGppMaskHigh	= 0,
	.numBoardDeviceIf	= ARRSZ(db88f6821InfoBoardDeCsInfo),
	.pDevCsInfo		= db88f6821InfoBoardDeCsInfo,
	.numBoardTwsiDev	= ARRSZ(db88f6821InfoBoardTwsiDev),
	.pBoardTwsiDev		= db88f6821InfoBoardTwsiDev,
	.numBoardMacInfo	= ARRSZ(db88f6821InfoBoardMacInfo),
	.pBoardMacInfo		= db88f6821InfoBoardMacInfo,
	.numBoardGppInfo	= 0,
	.pBoardGppInfo		= 0,
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
	.activeLedsNumber	= 0,
	.pLedGppPin		= NULL,
	.ledsPolarity		= 0,

	/* PMU Power */
	.pmuPwrUpPolarity	= 0,
	.pmuPwrUpDelay		= 80000,

	/* GPP values */
	.gppOutEnValLow		= DB_BP_88F6821_GPP_OUT_ENA_LOW,
	.gppOutEnValMid		= DB_BP_88F6821_GPP_OUT_ENA_MID,
	.gppOutValLow		= DB_BP_88F6821_GPP_OUT_VAL_LOW,
	.gppOutValMid		= DB_BP_88F6821_GPP_OUT_VAL_MID,
	.gppPolarityValLow	= DB_BP_88F6821_GPP_POL_LOW,
	.gppPolarityValMid	= DB_BP_88F6821_GPP_POL_MID,

	.pBoardUsbInfo		= db88f6821InfoBoardUsbInfo,
	.numBoardUsbInfo	= ARRSZ(db88f68xxInfoBoardUsbInfo),

	.pBoardSpecInit		= NULL,

	/* NAND init params */
	.nandFlashReadParams	= DB_88F68XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams	= DB_88F68XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl	= DB_88F68XX_BOARD_NAND_CONTROL,
	.nandIfMode		= NAND_IF_NFC,

	.isSdMmcConnected	= MV_TRUE,
	.isSdMmc_1_8v_Connected	= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams	= DB_88F68XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F68XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect	= MV_TRUE,
	.numIoExp		= ARRSZ(db88f6821InfoBoardioExpValue),
	.pIoExp			= db88f6821InfoBoardioExpValue,
	.boardOptionsModule	= MV_MODULE_NO_MODULE,
	.isAmc			= MV_FALSE,
	.isAudioConnected	= MV_FALSE,
	.isTdmConnected		= MV_FALSE,
	.pSwitchInfo		= NULL,
	.switchInfoNum		= 0
};

/*******************************************************************************
 * A38X-AMC (DB-88F6820-AMC) board */
/*******************************************************************************/
MV_BOARD_MPP_INFO dbAmc88f68xxInfoBoardMppConfigValue[] = {
	{ {
		  DB_AMC_88F68XX_MPP0_7,
		  DB_AMC_88F68XX_MPP8_15,
		  DB_AMC_88F68XX_MPP16_23,
		  DB_AMC_88F68XX_MPP24_31,
		  DB_AMC_88F68XX_MPP32_39,
		  DB_AMC_88F68XX_MPP40_47,
		  DB_AMC_88F68XX_MPP48_55,
		  DB_AMC_88F68XX_MPP56_63,
	 } }
};

MV_BOARD_TWSI_INFO dbAmc88f68xxInfoBoardTwsiDev[] = {
	/* {{devClass,		devClassId, twsiDevAddr, twsiDevAddrType, moreThen256}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x57,	ADDR7_BIT, MV_TRUE},  /* read only for HW configuration */
	{ BOARD_DEV_TWSI_SATR,		1,	0x4c,	ADDR7_BIT, MV_FALSE},
};
MV_BOARD_MAC_INFO dbAmc88f68xxInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr ,
	   MV_32 boardEthSmiAddr0 , MV_BOOL boardMacEnabled;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x1, 0x1, MV_TRUE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_FALSE},
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0, MV_TRUE},
};

MV_BOARD_USB_INFO dbAmc88f68xxInfoBoardUsbInfo[] = {
/* {MV_UNIT_ID usbType, MV_U8 usbPortNum, MV_BOOL isActive} */
	{ USB_UNIT_ID, 0, MV_TRUE},
};

MV_DEV_CS_INFO dbAmc88f68xxInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	8,	8,	0,	MV_TRUE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI1_CS0,	N_A, BOARD_DEV_SPI_FLASH,	8,	8,	1,	MV_TRUE }	/* SPI0 DEV */
};

MV_BOARD_INFO dbAmc88f68xx_board_info = {
	.boardName		= "DB-88F6820-AMC",
	.compatibleDTName	= "a385-db-amc",
	.numBoardNetComplexValue = 0,
	.pBoardNetComplexInfo	= NULL,
	.pBoardMppConfigValue	= dbAmc88f68xxInfoBoardMppConfigValue,
	.intsGppMaskLow		= 0,
	.intsGppMaskMid		= 0,
	.intsGppMaskHigh	= 0,
	.numBoardDeviceIf	= ARRSZ(dbAmc88f68xxInfoBoardDeCsInfo),
	.pDevCsInfo		= dbAmc88f68xxInfoBoardDeCsInfo,
	.numBoardTwsiDev	= ARRSZ(dbAmc88f68xxInfoBoardTwsiDev),
	.pBoardTwsiDev		= dbAmc88f68xxInfoBoardTwsiDev,
	.numBoardMacInfo	= ARRSZ(dbAmc88f68xxInfoBoardMacInfo),
	.pBoardMacInfo		= dbAmc88f68xxInfoBoardMacInfo,
	.numBoardGppInfo	= 0,
	.pBoardGppInfo		= 0,
	.numBoardIoExpPinInfo	= 0,
	.pBoardIoExpPinInfo	= 0,
	.activeLedsNumber	= 0,
	.pLedGppPin		= NULL,
	.ledsPolarity		= 0,

	/* PMU Power */
	.pmuPwrUpPolarity	= 0,
	.pmuPwrUpDelay		= 80000,

	/* GPP values */
	.gppOutEnValLow		= DB_AMC_88F68XX_GPP_OUT_ENA_LOW,
	.gppOutEnValMid		= DB_AMC_88F68XX_GPP_OUT_ENA_MID,
	.gppOutValLow		= DB_AMC_88F68XX_GPP_OUT_VAL_LOW,
	.gppOutValMid		= DB_AMC_88F68XX_GPP_OUT_VAL_MID,
	.gppPolarityValLow	= DB_AMC_88F68XX_GPP_POL_LOW,
	.gppPolarityValMid	= DB_AMC_88F68XX_GPP_POL_MID,

	.pBoardUsbInfo		= dbAmc88f68xxInfoBoardUsbInfo,
	.numBoardUsbInfo	= ARRSZ(db88f68xxInfoBoardUsbInfo),

	.pBoardSpecInit		= NULL,

	/* NAND init params */
	.nandFlashReadParams	= DB_88F68XX_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams	= DB_88F68XX_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl	= DB_88F68XX_BOARD_NAND_CONTROL,
	.nandIfMode		= NAND_IF_NFC,

	.isSdMmcConnected	= MV_FALSE,
	.isSdMmc_1_8v_Connected	= MV_FALSE,

	/* NOR init params */
	.norFlashReadParams	= DB_88F68XX_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F68XX_BOARD_NOR_WRITE_PARAMS,

	/* Enable modules auto-detection. */
	.configAutoDetect	= MV_TRUE,
	.numIoExp		= 0,
	.pIoExp			= NULL,
	.boardOptionsModule	= MV_MODULE_NO_MODULE,
	.isAmc			= MV_TRUE,	/* for DT update and switching services */
	.isAudioConnected	= MV_FALSE,
	.isTdmConnected		= MV_FALSE,
	.pSwitchInfo		= NULL,
	.switchInfoNum		= 0
};

MV_BOARD_INFO *marvellBoardInfoTbl[] = {
	&rdNas88f68XX_board_info,
	&db88f68xx_board_info,
	&rdWAP88f68XX_board_info,
	&dbAP88f68xx_board_info,
	&dbGP88f68XX_board_info,
	&db88f6821_board_info,
	&dbAmc88f68xx_board_info,
};
