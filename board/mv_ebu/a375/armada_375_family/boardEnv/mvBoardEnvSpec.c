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

/***************************************** Customer Boards *****************************************/
/*******************************************************************************
 * Armada 375 Customer board - Based on DB_88F6720 */
/*******************************************************************************/

#define A375_CUSTOMER_BOARD_0_NOR_READ_PARAMS	0x403E07CF
#define A375_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS	0x000F0F0F

#define A375_CUSTOMER_BOARD_0_NAND_READ_PARAMS	0x003E07CF
#define A375_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS	0x000F0F0F

MV_BOARD_TWSI_INFO armada_375_customer_board_0_BoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		1,	0x4D,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_EEPROM,	0,	0x52,	   ADDR7_BIT	},
};

MV_BOARD_MAC_INFO armada_375_customer_board_0_BoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr , MV_32 boardEthSmiAddr0;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0},
	{ BOARD_MAC_SPEED_AUTO, 0x3, 0x3},
	{ N_A,			N_A, N_A}
};
MV_BOARD_MPP_TYPE_INFO armada_375_customer_board_0_BoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_SLIC_DISABLED,
		.ethSataComplexOpt = (MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC0_2_RGMII0),
		.ethPortsMode = 0x0
	}
};

MV_DEV_CS_INFO armada_375_customer_board_0_BoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEV_BOOCS,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS0,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS1,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS2,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEVICE_CS3,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE },	/* SPI DEV */
	{ SPI_CS1,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS2,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS3,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS4,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS5,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS6,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE },	/* SPI DEV */
	{ SPI_CS7,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_FALSE }	/* SPI DEV */
};

MV_BOARD_MPP_INFO armada_375_customer_board_0_BoardMppConfigValue[] = {
	{ {
		  A375_CUSTOMER_BOARD_0_MPP0_7,
		  A375_CUSTOMER_BOARD_0_MPP8_15,
		  A375_CUSTOMER_BOARD_0_MPP16_23,
		  A375_CUSTOMER_BOARD_0_MPP24_31,
		  A375_CUSTOMER_BOARD_0_MPP32_39,
		  A375_CUSTOMER_BOARD_0_MPP40_47,
		  A375_CUSTOMER_BOARD_0_MPP48_55,
		  A375_CUSTOMER_BOARD_0_MPP56_63,
		  A375_CUSTOMER_BOARD_0_MPP64_67,
	 } }
};

MV_BOARD_INFO armada_375_customer_board_0_info = {
	.boardName			= "Armada-375-Customer-Board-0",
	.numBoardMppTypeValue		= ARRSZ(armada_375_customer_board_0_BoardModTypeInfo),
	.pBoardModTypeValue		= armada_375_customer_board_0_BoardModTypeInfo,
	.pBoardMppConfigValue		= armada_375_customer_board_0_BoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(armada_375_customer_board_0_BoardDeCsInfo),
	.pDevCsInfo			= armada_375_customer_board_0_BoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(armada_375_customer_board_0_BoardTwsiDev),
	.pBoardTwsiDev			= armada_375_customer_board_0_BoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(armada_375_customer_board_0_BoardMacInfo),
	.pBoardMacInfo			= armada_375_customer_board_0_BoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= A375_CUSTOMER_BOARD_0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= A375_CUSTOMER_BOARD_0_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= A375_CUSTOMER_BOARD_0_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= A375_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= A375_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID,
	.gppOutValHigh			= A375_CUSTOMER_BOARD_0_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= A375_CUSTOMER_BOARD_0_GPP_POL_LOW,
	.gppPolarityValMid		= A375_CUSTOMER_BOARD_0_GPP_POL_MID,
	.gppPolarityValHigh		= A375_CUSTOMER_BOARD_0_GPP_POL_HIGH,

	/* External Switch Configuration */
	.switchforceLinkMask		= 0x0,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= A375_CUSTOMER_BOARD_0_NAND_READ_PARAMS,
	.nandFlashWriteParams		= A375_CUSTOMER_BOARD_0_NAND_WRITE_PARAMS,
	.nandFlashControl		= 0,
	.nandIfMode				= NAND_IF_NFC,
	/* NOR init params */
	.norFlashReadParams		= A375_CUSTOMER_BOARD_0_NOR_READ_PARAMS,
	.norFlashWriteParams		= A375_CUSTOMER_BOARD_0_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_FALSE
};

MV_BOARD_INFO *customerBoardInfoTbl[] = {
	&armada_375_customer_board_0_info,
	&armada_375_customer_board_0_info
};

/***************************************** Marvell Boards *****************************************/
/*******************************************************************************
 * Armada 375 DB-88F6720 board */
/*******************************************************************************/

#define DB_88F6720_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F6720_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

#define DB_88F6720_BOARD_NAND_READ_PARAMS	0x003E07CF
#define DB_88F6720_BOARD_NAND_WRITE_PARAMS	0x000F0F0F

MV_BOARD_TWSI_INFO db88f6720InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		1,	0x4D,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_EEPROM,	0,	0x52,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_IO_EXPANDER,	0,	0x24,	   ADDR7_BIT	},
};

MV_BOARD_MAC_INFO db88f6720InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_32 boardEthSmiAddr , MV_32 boardEthSmiAddr0;}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0, 0x0 },
	{ BOARD_MAC_SPEED_AUTO, 0x3, 0x3 },
	{ N_A,			N_A, N_A },
};
MV_BOARD_MPP_TYPE_INFO db88f6720InfoBoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_SLIC_DISABLED,
		.ethSataComplexOpt = (MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC0_2_RGMII0),
		.ethPortsMode = 0x0
	}
};

MV_DEV_CS_INFO db88f6720InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth, busNum, active }*/
	{ DEV_BOOCS,	N_A, BOARD_DEV_NAND_FLASH,	16,	16,	0,	MV_FALSE },	/* NAND DEV */
	{ DEV_BOOCS,	N_A, BOARD_DEV_NOR_FLASH,	16,	16,	0,	MV_FALSE },	/* NOR DEV */
	{ SPI_CS0,		N_A, BOARD_DEV_SPI_FLASH,	8,	8,	0,	MV_TRUE }	/* SPI DEV */
};

MV_BOARD_MPP_INFO db88f6720InfoBoardMppConfigValue[] = {
	{ {
		  DB_88F6720_MPP0_7,
		  DB_88F6720_MPP8_15,
		  DB_88F6720_MPP16_23,
		  DB_88F6720_MPP24_31,
		  DB_88F6720_MPP32_39,
		  DB_88F6720_MPP40_47,
		  DB_88F6720_MPP48_55,
		  DB_88F6720_MPP56_63,
		  DB_88F6720_MPP64_67,
	 } }
};

MV_BOARD_INFO db88f6720_board_info = {
	.boardName			= "DB-88F6720-V2",
	.compatibleDTName		= "db",
	.numBoardMppTypeValue		= ARRSZ(db88f6720InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f6720InfoBoardModTypeInfo,
	.pBoardMppConfigValue		= db88f6720InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db88f6720InfoBoardDeCsInfo),
	.pDevCsInfo			= db88f6720InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db88f6720InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f6720InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f6720InfoBoardMacInfo),
	.pBoardMacInfo			= db88f6720InfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6720_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6720_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6720_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6720_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6720_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6720_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6720_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6720_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6720_GPP_POL_HIGH,

	/* External Switch Configuration */
	.switchforceLinkMask		= 0x0,

	/* TDM */
	.numBoardTdmInfo		= {},
	.pBoardTdmInt2CsInfo		= {},
	.boardTdmInfoIndex		= -1,

	.pBoardSpecInit			= NULL,

	/* NAND init params */
	.nandFlashReadParams		= DB_88F6720_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_88F6720_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= 0,
	/* NOR init params */
	.norFlashReadParams		= DB_88F6720_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_88F6720_BOARD_NOR_WRITE_PARAMS,
	/* Enable modules auto-detection. */
	.configAutoDetect		= MV_TRUE
};

/*
 * All supported Armada 375 boards
 */
MV_BOARD_INFO *marvellBoardInfoTbl[] = {
	&db88f6720_board_info,
};

