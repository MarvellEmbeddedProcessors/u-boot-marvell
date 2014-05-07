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

/***************************************** Customer Boards ****************************************/
/*******************************************************************************
	Bobcat2 board - Based on BOBCAT2-DB-DX
*******************************************************************************/
#define BOBCAT2_CUSTOMER_0_BOARD_NAND_READ_PARAMS	0x000C0282
#define BOBCAT2_CUSTOMER_0_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define BOBCAT2_CUSTOMER_0_BOARD_NAND_CONTROL		0x01c00543

#define BOBCAT2_CUSTOMER_0_BOARD_NOR_READ_PARAMS	0x403E07CF
#define BOBCAT2_CUSTOMER_0_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_TWSI_INFO	bobcat2_customer_board_0_InfoBoardTwsiDev[] = {
/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_PLD, 0x30, ADDR7_BIT},		/* Access to control PLD reg file */
	{BOARD_DEV_TWSI_ZARLINK, 0x18, ADDR7_BIT},		/* Access to Zarlink */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},         /* SatR bios 0		*/
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},         /* SatR bios 1		*/
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},          /* SatR bios 2		*/
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT},          /* SatR bios 3		*/
	{BOARD_DEV_TWSI_INIT_EPROM, 0x50, ADDR7_BIT},          /* Serial Ini EPROM	*/
	{BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0x20, ADDR7_BIT},          /* Qsgmii/sfp mux control PCA9555 IO expander */
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x70, ADDR7_BIT},          /* PCA9548 I2C mux 0	*/
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x71, ADDR7_BIT},          /* PCA9548 I2C mux 1	*/
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x75, ADDR7_BIT}          /* PCA9548 I2C mux 2	*/
};

MV_BOARD_MAC_INFO bobcat2_customer_board_0_InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0, 0x0, 0x0 },
	{BOARD_MAC_SPEED_AUTO, 0x1, 0x0, 0x1 },
};

MV_BOARD_MODULE_TYPE_INFO bobcat2_customer_board_0_InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
	}
};

MV_BOARD_GPP_INFO bobcat2_customer_board_0_InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
	/*{BOARD_GPP_RESET,       47},*/
};

MV_DEV_CS_INFO bobcat2_customer_board_0_InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO bobcat2_customer_board_0_InfoBoardMppConfigValue[] = {
	{ {
	BOBCAT2_CUSTOMER_0_MPP0_7,
	BOBCAT2_CUSTOMER_0_MPP8_15,
	BOBCAT2_CUSTOMER_0_MPP16_23,
	BOBCAT2_CUSTOMER_0_MPP24_31,
	BOBCAT2_CUSTOMER_0_MPP32_39,
	} },
};

MV_BOARD_INFO bobcat2_customer_board_0_Info = {
	.boardName			= "BOBCAT2-Customer-Board-0",
	.numBoardMppTypeValue		= ARRSZ(bobcat2_customer_board_0_InfoBoardModTypeInfo),
	.pBoardModTypeValue		= bobcat2_customer_board_0_InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(bobcat2_customer_board_0_InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= bobcat2_customer_board_0_InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(bobcat2_customer_board_0_InfoBoardDeCsInfo),
	.pDevCsInfo			= bobcat2_customer_board_0_InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(bobcat2_customer_board_0_InfoBoardTwsiDev),
	.pBoardTwsiDev			= bobcat2_customer_board_0_InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(bobcat2_customer_board_0_InfoBoardMacInfo),
	.pBoardMacInfo			= bobcat2_customer_board_0_InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(bobcat2_customer_board_0_InfoBoardGppInfo),
	.pBoardGppInfo			= bobcat2_customer_board_0_InfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= BOBCAT2_CUSTOMER_0_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= BOBCAT2_CUSTOMER_0_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= 0,
	.gppOutValLow			= BOBCAT2_CUSTOMER_0_GPP_OUT_VAL_LOW,
	.gppOutValMid			= BOBCAT2_CUSTOMER_0_GPP_OUT_VAL_MID,
	.gppOutValHigh			= 0,
	.gppPolarityValLow		= BOBCAT2_CUSTOMER_0_GPP_POL_LOW,
	.gppPolarityValMid		= BOBCAT2_CUSTOMER_0_GPP_POL_MID,
	.gppPolarityValHigh		= 0,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	/* NAND init params */
	.nandFlashReadParams		= BOBCAT2_CUSTOMER_0_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= BOBCAT2_CUSTOMER_0_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= BOBCAT2_CUSTOMER_0_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= BOBCAT2_CUSTOMER_0_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= BOBCAT2_CUSTOMER_0_BOARD_NOR_WRITE_PARAMS
};

MV_BOARD_INFO *customerBoardInfoTbl[] = {
	&bobcat2_customer_board_0_Info,
	&bobcat2_customer_board_0_Info,
};

/***************************************** Marvell Boards *****************************************/
/***********************/
/* BOBCAT2-DB-DX BOARD */
/***********************/
#define DB_DX_BC2_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_DX_BC2_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define DB_DX_BC2_BOARD_NAND_CONTROL		0x01c00543

#define DB_DX_BC2_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_DX_BC2_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_TWSI_INFO	db_dx_bc2InfoBoardTwsiDev[] = {
/* {{MV_BOARD_DEV_CLASS	devClass, MV_U8	twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_PLD, 0x30, ADDR7_BIT},		/* Access to control PLD reg file */
	{BOARD_DEV_TWSI_ZARLINK, 0x18, ADDR7_BIT},		/* Access to Zarlink 	*/
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},         /* SatR bios 0		*/
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},         /* SatR bios 1		*/
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},          /* SatR bios 2		*/
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT},          /* SatR bios 3		*/
	{BOARD_DEV_TWSI_INIT_EPROM, 0x50, ADDR7_BIT},          /* Serial Ini EPROM	*/
	{BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0x20, ADDR7_BIT},          /* Qsgmii/sfp mux control PCA9555 IO expander */
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x70, ADDR7_BIT},          /* PCA9548 I2C mux 0	*/
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x71, ADDR7_BIT},          /* PCA9548 I2C mux 1	*/
	{BOARD_DEV_TWSI_PCA9548_IO_MUX, 0x75, ADDR7_BIT}          /* PCA9548 I2C mux 2	*/
};

MV_BOARD_MAC_INFO db_dx_bc2InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0,0x0 	  , 0x0 },
	{BOARD_MAC_SPEED_AUTO, 0x1,0x0	  , 0x1 },
};

MV_BOARD_MODULE_TYPE_INFO db_dx_bc2InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
	}
};

MV_BOARD_GPP_INFO db_dx_bc2InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS,    24} /* from MPP map */
	/*{BOARD_GPP_RESET,       47},*/
};

MV_DEV_CS_INFO db_dx_bc2InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 8, 8} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO db_dx_bc2InfoBoardMppConfigValue[] = {
	{ {
#if defined(MV_INCLUDE_NOR)
	DB_DX_BC2_NOR_MPP0_7,
	DB_DX_BC2_NOR_MPP8_15,
#else
	DB_DX_BC2_MPP0_7,
	DB_DX_BC2_MPP8_15,
#endif
	DB_DX_BC2_MPP16_23,
	DB_DX_BC2_MPP24_31,
	DB_DX_BC2_MPP32_39,
	} },
};

MV_BOARD_INFO db_dx_bc2Info = {
	.boardName			= "DB-DXBC2-MM",
	.numBoardMppTypeValue		= ARRSZ(db_dx_bc2InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db_dx_bc2InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db_dx_bc2InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db_dx_bc2InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(db_dx_bc2InfoBoardDeCsInfo),
	.pDevCsInfo			= db_dx_bc2InfoBoardDeCsInfo,
	.numBoardTwsiDev		= ARRSZ(db_dx_bc2InfoBoardTwsiDev),
	.pBoardTwsiDev			= db_dx_bc2InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db_dx_bc2InfoBoardMacInfo),
	.pBoardMacInfo			= db_dx_bc2InfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(db_dx_bc2InfoBoardGppInfo),
	.pBoardGppInfo			= db_dx_bc2InfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= DB_DX_BC2_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_DX_BC2_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= 0,
	.gppOutValLow			= DB_DX_BC2_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_DX_BC2_GPP_OUT_VAL_MID,
	.gppOutValHigh			= 0,
	.gppPolarityValLow		= DB_DX_BC2_GPP_POL_LOW,
	.gppPolarityValMid		= DB_DX_BC2_GPP_POL_MID,
	.gppPolarityValHigh		= 0,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	/* NAND init params */
	.nandFlashReadParams		= DB_DX_BC2_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= DB_DX_BC2_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= DB_DX_BC2_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= DB_DX_BC2_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= DB_DX_BC2_BOARD_NOR_WRITE_PARAMS
};

/***********************/
/* BOBCAT2-RD-DX BOARD */
/***********************/
#define RD_DX_BC2_BOARD_NAND_READ_PARAMS		0x000C0282
#define RD_DX_BC2_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define RD_DX_BC2_BOARD_NAND_CONTROL			0x01c00543

MV_BOARD_MAC_INFO rd_dx_bc2InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x0, 0x0, 0x0},
	{BOARD_MAC_SPEED_1000M, 0x1, 0x0, 0x1},
};

MV_BOARD_MODULE_TYPE_INFO rd_dx_bc2InfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
	}
};

MV_DEV_CS_INFO rd_dx_bc2InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO rd_dx_bc2InfoBoardMppConfigValue[] = {
	{ {
		RD_DX_BC2_MPP0_7,
		RD_DX_BC2_MPP8_15,
		RD_DX_BC2_MPP16_23,
		RD_DX_BC2_MPP24_31,
		RD_DX_BC2_MPP32_39,
	} }
};

MV_BOARD_INFO rd_dx_bc2Info = {
	.boardName			= "RD-DXBC2-48G-12XG2XLG",
	.numBoardMppTypeValue		= ARRSZ(rd_dx_bc2InfoBoardModTypeInfo),
	.pBoardModTypeValue		= rd_dx_bc2InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(rd_dx_bc2InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= rd_dx_bc2InfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(rd_dx_bc2InfoBoardDeCsInfo),
	.pDevCsInfo			= rd_dx_bc2InfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(rd_dx_bc2InfoBoardMacInfo),
	.pBoardMacInfo			= rd_dx_bc2InfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= NULL,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= RD_DX_BC2_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_DX_BC2_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= 0,
	.gppOutValLow			= RD_DX_BC2_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_DX_BC2_GPP_OUT_VAL_MID,
	.gppOutValHigh			= 0,
	.gppPolarityValLow		= RD_DX_BC2_GPP_POL_LOW,
	.gppPolarityValMid		= RD_DX_BC2_GPP_POL_MID,
	.gppPolarityValHigh		= 0,

	/* External Switch Configuration */
	.pSwitchInfo			= NULL,
	.switchInfoNum			= 0,

	 /* NAND init params */
	.nandFlashReadParams		= RD_DX_BC2_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= RD_DX_BC2_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= RD_DX_BC2_BOARD_NAND_CONTROL
};

/*****************************/
/* BobCat2 RD BC2_MTL BOARD */
/*****************************/
#define RD_MTL_BC2_BOARD_NAND_READ_PARAMS		0x000C0282
#define RD_MTL_BC2_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define RD_MTL_BC2_BOARD_NAND_CONTROL			0x01c00543

MV_BOARD_MAC_INFO bc2_rd_mtlInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x0, 0x0, 0x0},
	{BOARD_MAC_SPEED_1000M, 0x1, 0x0, 0x1},
};

MV_BOARD_MODULE_TYPE_INFO bc2_rd_mtlInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
	}
};

MV_DEV_CS_INFO bc2_rd_mtlInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth}*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8} /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO bc2_rd_mtlInfoBoardMppConfigValue[] = {
	{ {
		RD_MTL_BC2_MPP0_7,
		RD_MTL_BC2_MPP8_15,
		RD_MTL_BC2_MPP16_23,
		RD_MTL_BC2_MPP24_31,
		RD_MTL_BC2_MPP32_39,
	} }
};

MV_BOARD_INFO bc2_rd_mtlInfo = {
	.boardName			= "RD-BC2-MTL-PoE-2QSFP-6SFP",
	.numBoardMppTypeValue		= ARRSZ(bc2_rd_mtlInfoBoardModTypeInfo),
	.pBoardModTypeValue		= bc2_rd_mtlInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(bc2_rd_mtlInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= bc2_rd_mtlInfoBoardMppConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(bc2_rd_mtlInfoBoardDeCsInfo),
	.pDevCsInfo			= bc2_rd_mtlInfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(bc2_rd_mtlInfoBoardMacInfo),
	.pBoardMacInfo			= bc2_rd_mtlInfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= NULL,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* GPP values */
	.gppOutEnValLow			= RD_MTL_BC2_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_MTL_BC2_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= 0,
	.gppOutValLow			= RD_MTL_BC2_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_MTL_BC2_GPP_OUT_VAL_MID,
	.gppOutValHigh			= 0,
	.gppPolarityValLow		= RD_MTL_BC2_GPP_POL_LOW,
	.gppPolarityValMid		= RD_MTL_BC2_GPP_POL_MID,
	.gppPolarityValHigh		= 0,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	/* NAND init params */
	.nandFlashReadParams		= RD_MTL_BC2_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= RD_MTL_BC2_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= RD_MTL_BC2_BOARD_NAND_CONTROL
};

/*********************************************************************************/

MV_BOARD_INFO *marvellBoardInfoTbl[] = {
	&db_dx_bc2Info,
	&rd_dx_bc2Info,
	&bc2_rd_mtlInfo
};
