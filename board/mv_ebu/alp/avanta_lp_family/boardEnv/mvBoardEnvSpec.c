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

MV_BOARD_SAR_INFO boardSarInfo[] = {
/* {{MV_SATR_TYPE_ID SarID, MV_U32 Mask, MV_U32 Offset, MV_U32 regNum}} */
	{ MV_SATR_CPU_FREQ,	       0x003E0000,	      17,		     1	      },
	{ MV_SATR_CORE_CLK_SELECT,     0x00400000,	      22,		     1	      },
	{ MV_SATR_CPU1_ENABLE,	       0x00008000,	      15,		     0	      },
	{ MV_SATR_SSCG_DISABLE,	       0x00000002,	      1,		     0	      },
	{ MV_SATR_I2C0_Serial_ROM,     0x00000001,	      0,		     0	      },
	{ MV_SATR_External_CPU_Reset,  0x0,		      0,		     0	      },        /* omriii : check with max the relevance of these fields */
	{ MV_SATR_External_CORE_Reset, 0x0,		      0,		     0	      },        /* omriii : check with max the relevance of these fields */
	{ MV_SATR_BOOT_DEVICE,	       0X1F8,		      3,		     0	      },
	{ MV_SATR_CPU_PLL_XTAL_BYPASS, 0x00000200,	      9,		     0	      },
	{ MV_SATR_PEX0_CLOCK,	       0x00000400,	      10,		     0	      },
	{ MV_SATR_PEX1_CLOCK,	       0x00000800,	      11,		     0	      },
	{ MV_SATR_REF_CLOCK_ENABLE,    0x00000004,	      2,		     0	      },
	{ MV_SATR_TESTER_OPTIONS,      0x00080000,	      19,		     0	      },
	{ MV_SATR_CPU0_ENDIANESS,      0x00001000,	      12,		     0	      },
	{ MV_SATR_CPU0_NMFI,	       0x00002000,	      13,		     0	      },
	{ MV_SATR_CPU0_THUMB,	       0x00004000,	      14,		     0	      },
	{ MV_SATR_EFUSE_BYPASS,	       0x00020000,	      17,		     0	      },
	{ MV_SATR_POR_BYPASS,	       0x00100000,	      20,		     0	      },
	{ MV_SATR_BOARD_ID,	       0xF0,		      4,		     1	      },
};

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
 * AvantaLP DB-88F6600 board */
/*******************************************************************************/

MV_BOARD_TWSI_INFO db88f6600InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{ BOARD_DEV_TWSI_SATR,	      0x4C,	   ADDR7_BIT	       },
	{ BOARD_DEV_TWSI_SATR,	      0x4D,	   ADDR7_BIT	       },
	{ BOARD_DEV_TWSI_EEPROM,      0x54,	   ADDR7_BIT	       },
	{ BOARD_DEV_TWSI_IO_EXPANDER, 0x21,	   ADDR7_BIT	       },
	{ BOARD_DEV_TWSI_IO_EXPANDER, 0x22,	   ADDR7_BIT	       },       /*omriii : re-verify that 0x22 is also IO_EXPANDER for 6600 board */
	{ BOARD_DEV_TWSI_IO_EXPANDER, 0x24,	   ADDR7_BIT	       },       /*omriii : re-verify that 0x22 is also IO_EXPANDER for 6600 board */
};
MV_BOARD_MAC_INFO db88f6600InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x8		     },
	{ BOARD_MAC_SPEED_AUTO, 0x9		     },
	{ N_A,			N_A		     }
};
MV_BOARD_MPP_TYPE_INFO db88f6600InfoBoardModTypeInfo[] = {
	{
		.boardMppTdm = MV_BOARD_AUTO,
		.ethSataComplexOpt = 0x0,
		.ethPortsMode = 0x0
	}
};

MV_BOARD_MPP_GROUP_INFO db88f6600InfoBoardMppGroupConfig[] = {
	{ {
		  DB_88F6600_GROUP_0_TYPE,
		  DB_88F6600_GROUP_1_TYPE,
		  DB_88F6600_GROUP_2_TYPE,
		  DB_88F6600_GROUP_3_TYPE,
		  DB_88F6600_GROUP_4_TYPE,
		  DB_88F6600_GROUP_5_TYPE,
		  DB_88F6600_GROUP_6_TYPE,
		  DB_88F6600_GROUP_7_TYPE,
		  DB_88F6600_GROUP_8_TYPE,
	  } }
};

MV_BOARD_INFO db88f6600_board_info = {
	.boardName			= "DB-88F6600",
	.numBoardMppTypeValue		= ARRSZ(db88f6600InfoBoardModTypeInfo),
	.pBoardModTypeValue		= db88f6600InfoBoardModTypeInfo,
	.numBoardMppGroupValue		= ARRSZ(db88f6600InfoBoardMppGroupConfig),
	.pBoardMppGroupValue		= db88f6600InfoBoardMppGroupConfig,
	.numBoardSerdesConfigValue	= 0,
	.pBoardSerdesConfigValue	= 0,
	.intsGppMaskLow			= 0,
	.intsGppMaskMid			= 0,
	.intsGppMaskHigh		= 0,
	.numBoardDeviceIf		= 0,
	.pDevCsInfo			= 0,
	.numBoardSarInfo		= ARRSZ(boardSarInfo),
	.pBoardSarInfo			= boardSarInfo,
	.numBoardTwsiDev		= ARRSZ(db88f6600InfoBoardTwsiDev),
	.pBoardTwsiDev			= db88f6600InfoBoardTwsiDev,
	.numBoardMacInfo		= ARRSZ(db88f6600InfoBoardMacInfo),
	.pBoardMacInfo			= db88f6600InfoBoardMacInfo,
	.numBoardGppInfo		= 0,
	.pBoardGppInfo			= 0,
	.activeLedsNumber		= 0,
	.pLedGppPin			= NULL,
	.ledsPolarity			= 0,

	/* PMU Power */
	.pmuPwrUpPolarity		= 0,
	.pmuPwrUpDelay			= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6600_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6600_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6600_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6600_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6600_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6600_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6600_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6600_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6600_GPP_POL_HIGH,

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
	.moduleAutoDetect		= MV_TRUE
};

/*******************************************************************************
* AvanataLP customer board
*******************************************************************************/

#define AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS       0x000C0282
#define AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS      0x00010305
/*NAND care support for small page chips*/
#define AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL           0x01c00543

#define AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS        0x403E07CF
#define AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS       0x000F0F0F

MV_BOARD_MAC_INFO avanta_lp_customerInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{ BOARD_MAC_SPEED_AUTO, 0x10, 0x0 },
	{ BOARD_MAC_SPEED_AUTO, 0x11, 0x0 },
	{ BOARD_MAC_SPEED_AUTO, 0x12, 0x0 },
	{ BOARD_MAC_SPEED_AUTO, 0x13, 0x0 }
};

MV_BOARD_MPP_TYPE_INFO avanta_lp_customerInfoBoardModTypeInfo[] = {
	{
		.boardMppTdm = MV_BOARD_AUTO,
		.ethSataComplexOpt = 0x0,
		.ethPortsMode = 0x0
	}
};

MV_BOARD_GPP_INFO avanta_lp_customerInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{ BOARD_GPP_RESET, 21 }
};

MV_DEV_CS_INFO avanta_lp_customerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{ SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8 } /* SPI DEV */
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
	{       /* default */
		MV_TRUE,
		0x00223001,
		0x11111111,
		PEX_BUS_MODE_X1,
		PEX_BUS_DISABLED,
		PEX_BUS_MODE_X4,
		PEX_BUS_MODE_X4,
		0x00000030
	},
	{       /* Switch module */
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
	.boardName				= "AvantaLP-CUSTOMER",
	.numBoardMppTypeValue			= ARRSZ(avanta_lp_customerInfoBoardModTypeInfo),
	.pBoardModTypeValue			= avanta_lp_customerInfoBoardModTypeInfo,
	.numBoardMppConfigValue			= ARRSZ(avanta_lp_customerInfoBoardMppConfigValue),
	.pBoardMppConfigValue			= avanta_lp_customerInfoBoardMppConfigValue,
	.numBoardSerdesConfigValue		= ARRSZ(avanta_lp_customerInfoBoardSerdesConfigValue),
	.pBoardSerdesConfigValue		= avanta_lp_customerInfoBoardSerdesConfigValue,
	.intsGppMaskLow				= 0,
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(avanta_lp_customerInfoBoardDeCsInfo),
	.pDevCsInfo				= avanta_lp_customerInfoBoardDeCsInfo,
	.numBoardTwsiDev			= 0,
	.pBoardTwsiDev				= NULL,
	.numBoardMacInfo			= ARRSZ(avanta_lp_customerInfoBoardMacInfo),
	.pBoardMacInfo				= avanta_lp_customerInfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(avanta_lp_customerInfoBoardGppInfo),
	.pBoardGppInfo				= avanta_lp_customerInfoBoardGppInfo,
	.activeLedsNumber			= 0,
	.pLedGppPin				= NULL,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow				= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_LOW,
	.gppOutEnValMid				= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_MID,
	.gppOutEnValHigh			= AVANTA_LP_CUSTOMER_GPP_OUT_ENA_HIGH,
	.gppOutValLow				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_LOW,
	.gppOutValMid				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_MID,
	.gppOutValHigh				= AVANTA_LP_CUSTOMER_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow			= AVANTA_LP_CUSTOMER_GPP_POL_LOW,
	.gppPolarityValMid			= AVANTA_LP_CUSTOMER_GPP_POL_MID,
	.gppPolarityValHigh			= AVANTA_LP_CUSTOMER_GPP_POL_HIGH,

	/* TDM */
	.numBoardTdmInfo			= {},
	.pBoardTdmInt2CsInfo			= {},
	.boardTdmInfoIndex			= -1,

	/* NAND init params */
	.nandFlashReadParams			= AVANTA_LP_CUSTOMER_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams			= AVANTA_LP_CUSTOMER_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl			= AVANTA_LP_CUSTOMER_BOARD_NAND_CONTROL,
	/* NOR init params */
	.norFlashReadParams			= AVANTA_LP_CUSTOMER_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams			= AVANTA_LP_CUSTOMER_BOARD_NOR_WRITE_PARAMS
};

/*
 * All supported avanta boards
 */
MV_BOARD_INFO *boardInfoTbl[] = {
	&avanta_lp_fpga_board_info,
	&db88f6600_board_info,
};