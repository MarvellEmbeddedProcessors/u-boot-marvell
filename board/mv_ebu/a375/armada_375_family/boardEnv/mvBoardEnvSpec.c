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
{ MV_SATR_CPU_DDR_L2_FREQ,	 0x003E0000,	17,	1, {1} },
{ MV_SATR_CORE_CLK_SELECT,	 0x00400000,	22,	1, {0} },
{ MV_SATR_CPU1_ENABLE,		 0x00008000,	15,	0, {0} },
{ MV_SATR_SSCG_DISABLE,		 0x00000002,	1,	0, {0} },
{ MV_SATR_I2C0_SERIAL_ROM,	 0X00000001,	0,	0, {0} },
{ MV_SATR_EXTERNAL_CPU_RESET,	 0X00000000,	0,	0, {0} },
{ MV_SATR_EXTERNAL_CORE_RESET,	 0X00000000,	0,	0, {0} },
{ MV_SATR_BOOT_DEVICE,		 0X000001F8,	3,	0, {1} },
{ MV_SATR_CPU_PLL_XTAL_BYPASS,	 0x00000200,	9,	0, {0} },
{ MV_SATR_PEX0_CLOCK,		 0x00000400,	10,	0, {0} },
{ MV_SATR_PEX1_CLOCK,		 0x00000800,	11,	0, {0} },
{ MV_SATR_REF_CLOCK_ENABLE,	 0x00000004,	2,	0, {0} },
{ MV_SATR_TESTER_OPTIONS,	 0x00080000,	19,	0, {0} },
{ MV_SATR_CPU0_ENDIANESS,	 0x00001000,	12,	0, {0} },
{ MV_SATR_CPU0_NMFI,		 0x00002000,	13,	0, {0} },
{ MV_SATR_CPU0_THUMB,		 0x00004000,	14,	0, {0} },
{ MV_SATR_EFUSE_BYPASS,		 0x00020000,	17,	0, {0} },
{ MV_SATR_POR_BYPASS,		 0x00100000,	20,	0, {0} },
{ MV_SATR_BOARD_ID,		 0x000000F0,	4,	1, {0} },
{ MV_SATR_WRITE_CPU_FREQ,	 0X0000001F,	0,	0, {0} },
{ MV_SATR_WRITE_CORE_CLK_SELECT, 0x00000001,	0,	1, {0} },
{ MV_SATR_WRITE_CPU1_ENABLE,	 0x00000002,	1,	1, {0} },
{ MV_SATR_WRITE_SSCG_DISABLE,	 0x00000004,	2,	1, {0} },
};

MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = {
/* {{MV_CONFIG_TYPE_ID ConfigID, MV_U32 Mask,  Offset, expanderNum,  regNum,    isActiveForBoard[]}} */
	{ MV_CONFIG_MAC0,	       0x3,	0,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [0:1] */
	{ MV_CONFIG_MAC1,	       0xC,	2,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [2:3] */
	{ MV_CONFIG_PON_SERDES,	       0x10,	4,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [4]   */
	{ MV_CONFIG_PON_BEN_POLARITY,  0x20,	5,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [5]   */
	{ MV_CONFIG_SGMII0_CAPACITY,   0x40,	6,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [6]   */
	{ MV_CONFIG_SGMII1_CAPACITY,   0x80,	7,	 0,		0, 	{0 } }, /* Exp#0, Reg#0, BITS [7]   */
	{ MV_CONFIG_SLIC_TDM_DEVICE,   0x3,	0,	 0,		1, 	{0 } }, /* Exp#0, Reg#1, BITS [0:1] */
	{ MV_CONFIG_LANE1,	       0xC,	2,	 0,		1, 	{0 } }, /* Exp#0, Reg#1, BITS [2:3] */
	{ MV_CONFIG_LANE2,	       0x10,	4,	 0,		1, 	{0 } }, /* Exp#0, Reg#1, BITS [4]   */
	{ MV_CONFIG_LANE3,	       0X60,	5,	 0,		1, 	{0 } }, /* Exp#0, Reg#1, BITS [5:6] */
	{ MV_CONFIG_DEVICE_BUS_MODULE, 0x3,	0,	 1,		0, 	{0 } }, /* Exp#1, Reg#0, BITS [0:1] */
};

/*******************************************************************************
 * Armada 375 DB-88F6720 board */
/*******************************************************************************/

#define DB_88F6720_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F6720_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_BOARD_TWSI_INFO db88f6720InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 devClassId,  MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,		0,	0x4C,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_SATR,		1,	0x4D,	   ADDR7_BIT	},
	{ BOARD_DEV_TWSI_EEPROM,	0,	0x52,	   ADDR7_BIT	},
};

MV_BOARD_MAC_INFO db88f6720InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x0									},
	{ BOARD_MAC_SPEED_AUTO, 0x3									},
	{ N_A,			N_A									}
};
MV_BOARD_MPP_TYPE_INFO db88f6720InfoBoardModTypeInfo[] = {
	{
		.boardMppSlic = MV_BOARD_AUTO,
		.ethSataComplexOpt = (MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3 | MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0),
		.ethPortsMode = 0x0
	}
};

MV_DEV_CS_INFO db88f6720InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 }, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16}, /* NOR DEV */
#endif
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
	.boardName			= "DB-88F6720",
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
	.nandFlashReadParams		= 0,
	.nandFlashWriteParams		= 0,
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
MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f6720_board_info,
};
