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

#define ARRSZ(x)		(sizeof(x) / sizeof(x[0]))

/*******************************************************************************
 * AvantaLP FPGA board
 */
MV_BOARD_INFO avanta_lp_fpga_board_info = {
	.boardName			= "CortexA9-FPGA",
	.numBoardMppTypeValue		= 0,
	.pBoardModTypeValue		= NULL,
	.numBoardMppConfigValue		= 0,
	.pBoardMppConfigValue		= NULL,
	.numBoardSerdesConfigValue	= 0,
	.pBoardSerdesConfigValue	= NULL,
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
 */
#define AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS	0x000C0282
#define AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS	0x00010305
/*NAND care support for small page chips*/
#define AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL		0x01c00543

#define AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS	0x403E07CF
#define AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_MAC_INFO avanta_lp_customerInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x10,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x11,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x12,0x0},
	{BOARD_MAC_SPEED_AUTO, 0x13,0x0}
};

MV_BOARD_MODULE_TYPE_INFO avanta_lp_customerInfoBoardModTypeInfo[] = {
	{
		.boardMppMod		= MV_BOARD_AUTO,
		.boardOtherMod		= MV_BOARD_NONE
	}
};

MV_BOARD_GPP_INFO avanta_lp_customerInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_RESET, 21}
};

MV_DEV_CS_INFO avanta_lp_customerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8} /* SPI DEV */
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

MV_SERDES_CFG avanta_lp_customerInfoBoardSerdesConfigValue[] = {
	{ /* default */
		MV_TRUE,
		0x00223001,
		0x11111111,
		PEX_BUS_MODE_X1,
		PEX_BUS_DISABLED,
		PEX_BUS_MODE_X4,
		PEX_BUS_MODE_X4,
		0x00000030
	},
	{ /* Switch module */
		MV_TRUE,
		0x33320201,
		0x11111111,
		PEX_BUS_MODE_X1,
		PEX_BUS_DISABLED,
		PEX_BUS_MODE_X4,
		PEX_BUS_MODE_X4,
		0x00000030
	},
};

MV_BOARD_INFO avanta_lp_customer_board_info = {
	.boardName			= "AvantaLP-CUSTOMER",
	.numBoardMppTypeValue		= ARRSZ(avanta_lp_customerInfoBoardModTypeInfo),
	.pBoardModTypeValue		= avanta_lp_customerInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(avanta_lp_customerInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= avanta_lp_customerInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue	= ARRSZ(avanta_lp_customerInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue	= avanta_lp_customerInfoBoardSerdesConfigValue,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= ARRSZ(avanta_lp_customerInfoBoardDeCsInfo),
	.pDevCsInfo			= avanta_lp_customerInfoBoardDeCsInfo,
	.numBoardTwsiDev		= 0,
	.pBoardTwsiDev			= NULL,
	.numBoardMacInfo		= ARRSZ(avanta_lp_customerInfoBoardMacInfo),
	.pBoardMacInfo			= avanta_lp_customerInfoBoardMacInfo,
	.numBoardGppInfo		= ARRSZ(avanta_lp_customerInfoBoardGppInfo),
	.pBoardGppInfo			= avanta_lp_customerInfoBoardGppInfo,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_LOW,
	.gppOutValMid			= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_MID,
	.gppOutValHigh			= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= AVANTA_LP_CUSTOMER_GPP_POL_LOW,
	.gppPolarityValMid		= AVANTA_LP_CUSTOMER_GPP_POL_MID,
	.gppPolarityValHigh		= AVANTA_LP_CUSTOMER_GPP_POL_HIGH,

	/* TDM */
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams		= AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams		= AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl		= AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams		= AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams		= AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS
};

/*
 * All supported avanta boards
 */
MV_BOARD_INFO *boardInfoTbl[] = {
	&avanta_lp_fpga_board_info,
};
