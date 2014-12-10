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
#include "mvHighSpeedEnvSpec.h"
#include "mvSysEnvLib.h"
#include "printf.h"

#define	SERDES_VERION	"2.0"

#define COMMON_PHY_BASE_ADDR_MASK       0xFFF00
#define COMMON_PHY6_OFF                 0x600

/******************************** Sequences DB ********************************/

/*******************************/
/* XAUI, RXAUI */
/*******************************/

/* XAUI, RXAUI - power up seq */
MV_OP_PARAMS ethPortPowerUpParams[] =
{
	/* unitBaseReg                    unitOffset   	mask          XAUI data     RXAUI data    waitTime   numOfLoops */
	{ COMMON_PHY_CONFIGURATION1_REG,   0x28,  		0xF0006,	{ 0x80002,      0x80002  },     0,          0 },    /* Power Up */
	{ COMMON_PHY_CONFIGURATION1_REG,   0x28,		0x7800,	    { 0x6000,       0x6000   },     0,          0 },    /* Unreset */
	{ POWER_AND_PLL_CTRL_REG,	 	   0x800,		0xFF,	    { 0xFC81,       0xFC81   },     0,          0 },    /* Phy Selector */
	{ MISC_REG,			               0x800,	    0x4C0,	    { 0x480,        0x480    },     0,          0 }     /* Ref clock source select */
};

/* XAUI, RXAUI - speed config seq */
MV_OP_PARAMS ethPortSpeedConfigParams[] =
{
	/* unitBaseReg  				  unitOffset   	mask          XAUI data		RXAUI data        waitTime   numOfLoops */
	{ COMMON_PHY_CONFIGURATION1_REG,   0x28,		0x3FC00000,	{ 0x22000000,   0x2EC00000 },		0,		   0 }, /* Baud Rate */
	{ ISOLATE_REG,	 				   0x800,	    0xFF,	    { 0x88,         0xBB       },		0,	       0 }, /* Phy Gen RX and TX */
	{ LOOPBACK_REG,	 				   0x800,	    0xE,	    { 0x2,          0x2        },		0,	       0 }  /* Bus Width */
};

/* XAUI, RXAUI - Select electrical param seq */
MV_OP_PARAMS ethPortElectricalConfigParams[] =
{
    /* unitunitBaseReg      unitOffset   mask       XAUI data   RXAUI data      waitTime    numOfLoops */
    { G1_SETTINGS_1_REG,    0x800,      0x7FF,      { NO_DATA,  0x7C9       },      0,          0       }, /* G1_RX SELMUFF, SELMUFI, SELMUPF and SELMUPI */
    { DFE_REG0,             0x800,      0xF,        { NO_DATA,  0xA         },      0,          0       }, /* DFE_STEP_FINE_FX */
    { DFE_REG3,             0x800,      0xC000,     { NO_DATA,  0x0         },      0,          0       }, /* DFE_EN control from PIN and DFE Control From Pin */
    { G1_SETTINGS_0_REG,    0x800,      0xFFFF,     { NO_DATA,  0x8DF2      },      0,          0       }, /* Slew rate and emphasis */
    { G1_SETTINGS_3_REG,    0x800,      0xFF,       { NO_DATA,  0xAF        },      0,          0       }, /* G1 FFE Setting Force, RES and CAP */
    { RX_REG2,              0x800,      0x4,        { NO_DATA,  0x0         },      0,          0       }, /* DTL_FLOOP_EN */
    { VTHIMPCAL_CTRL_REG,   0x800,      0xFF00,     { NO_DATA,  0x3000      },      0,          0       }, /* tximpcal_th and rximpcal_th */
};

/* XAUI, RXAUI - TX config seq */
MV_OP_PARAMS ethPortTxConfigParams1[] =
{
	/* unitunitBaseReg               unitOffset   mask          XAUI data     RXAUI data      waitTime    numOfLoops */
	{ GLUE_REG,			              0x800,	  0x1800,     { 0x800,        0x800       },     0,	       0		   },
	{ RESET_DFE_REG,		          0x800,	  0x401,	  { 0x401,        0x401       },     0,	       0		   }, /* Sft Reset pulse */
	{ RESET_DFE_REG,		          0x800,	  0x401,	  { 0x0,          0x0         },     0,	       0		   }, /* Sft Reset pulse */
	{ LANE_ALIGN_REG0,                0x800,	  0x1000,	  { 0x0,          0x0         },     0,	       0		   }, /* Lane align */
	{ COMMON_PHY_CONFIGURATION1_REG,  0x28,		  0x70000,    { 0x70000,      0x70000     },     0,	       0		   }, /* Power up PLL, RX and TX */
	{ COMMON_PHY_CONFIGURATION1_REG,  0x28,		  0x80000,    { 0x80000,      0x80000     },     0,	       0		   }  /* Tx driver output idle */
};

MV_OP_PARAMS ethPortTxConfigParams2[] =
{
    /* unitunitBaseReg              unitOffset      mask            XAUI data       RXAUI data    waitTime    numOfLoops */
    { COMMON_PHY_STATUS1_REG,           0x28,       0xC,            { 0xC,          0xC         },  10,         1000        },  /* Wait for PHY power up sequence to finish */
    { COMMON_PHY_CONFIGURATION1_REG,    0x28,       0x40080000,     { 0x40000000,   0x40000000  },  0,          0           },  /* Assert Rx Init and Tx driver output valid */
    { COMMON_PHY_STATUS1_REG,           0x28,       0x1,            { 0x1,          0x1         },  1,          1000        },  /* Wait for PHY power up sequence to finish */
    { COMMON_PHY_CONFIGURATION1_REG,    0x28,       0x40000000,     { 0x0,          0x0         },  0,          0           },  /* De-assert Rx Init */
    { RX_REG3,                          0x800,      0xFF,           { NO_DATA,      0xD9        },  0,          0           },  /* os_ph_offset_force (align 90) */
    { RX_REG3,                          0x800,      0x100,          { NO_DATA,      0x100       },  0,          0           },  /* Set os_ph_valid */
    { RX_REG3,                          0x800,      0x100,          { NO_DATA,      0x0         },  0,          0           },  /* Unset os_ph_valid */
};

/************************* Local functions declarations ***********************/

/* Added from A380 */
MV_STATUS mvHwsSerdesSeqInit(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### serdesSeqInit ###\n");

	if (mvHwsSerdesSeqDbInit() != MV_OK){
		mvPrintf("mvHwsSerdesSeqInit: Error: Serdes initialization fail\n");
		return MV_FAIL;
	}

    /* additional sequences for a390x*/

    /* XAUI_POWER_UP_SEQ sequence init */
	serdesSeqDb[XAUI_POWER_UP_SEQ].opParamsPtr = ethPortPowerUpParams;
	serdesSeqDb[XAUI_POWER_UP_SEQ].cfgSeqSize  = sizeof(ethPortPowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[XAUI_POWER_UP_SEQ].dataArrIdx  = XAUI_SEQ_IDX;

    /* XAUI__3_125_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[XAUI__3_125_SPEED_CONFIG_SEQ].opParamsPtr = ethPortSpeedConfigParams;
	serdesSeqDb[XAUI__3_125_SPEED_CONFIG_SEQ].cfgSeqSize  = sizeof(ethPortSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[XAUI__3_125_SPEED_CONFIG_SEQ].dataArrIdx  = XAUI_SEQ_IDX;

	/* XAUI_ELECTRICAL_CONFIG_SEQ seq sequence init */
	serdesSeqDb[XAUI_ELECTRICAL_CONFIG_SEQ].opParamsPtr = ethPortElectricalConfigParams;
	serdesSeqDb[XAUI_ELECTRICAL_CONFIG_SEQ].cfgSeqSize  = sizeof(ethPortElectricalConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[XAUI_ELECTRICAL_CONFIG_SEQ].dataArrIdx  = XAUI_SEQ_IDX;

	/* XAUI_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[XAUI_TX_CONFIG_SEQ1].opParamsPtr = ethPortTxConfigParams1;
	serdesSeqDb[XAUI_TX_CONFIG_SEQ1].cfgSeqSize  = sizeof(ethPortTxConfigParams1) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[XAUI_TX_CONFIG_SEQ1].dataArrIdx  = XAUI_SEQ_IDX;

	/* XAUI_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[XAUI_TX_CONFIG_SEQ2].opParamsPtr = ethPortTxConfigParams2;
	serdesSeqDb[XAUI_TX_CONFIG_SEQ2].cfgSeqSize =  sizeof(ethPortTxConfigParams2) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[XAUI_TX_CONFIG_SEQ2].dataArrIdx =  XAUI_SEQ_IDX;

    /* RXAUI_POWER_UP_SEQ sequence init */
	serdesSeqDb[RXAUI_POWER_UP_SEQ].opParamsPtr = ethPortPowerUpParams;
	serdesSeqDb[RXAUI_POWER_UP_SEQ].cfgSeqSize  = sizeof(ethPortPowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[RXAUI_POWER_UP_SEQ].dataArrIdx  = RXAUI_SEQ_IDX;

	/* RXAUI__6_25_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[RXAUI__6_25_SPEED_CONFIG_SEQ].opParamsPtr = ethPortSpeedConfigParams;
	serdesSeqDb[RXAUI__6_25_SPEED_CONFIG_SEQ].cfgSeqSize  = sizeof(ethPortSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[RXAUI__6_25_SPEED_CONFIG_SEQ].dataArrIdx  = RXAUI_SEQ_IDX;

	/* RXAUI_ELECTRICAL_CONFIG_SEQ seq sequence init */
	serdesSeqDb[RXAUI_ELECTRICAL_CONFIG_SEQ].opParamsPtr = ethPortElectricalConfigParams;
	serdesSeqDb[RXAUI_ELECTRICAL_CONFIG_SEQ].cfgSeqSize  = sizeof(ethPortElectricalConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[RXAUI_ELECTRICAL_CONFIG_SEQ].dataArrIdx  = RXAUI_SEQ_IDX;

    /* RXAUI_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ1].opParamsPtr = ethPortTxConfigParams1;
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ1].cfgSeqSize  = sizeof(ethPortTxConfigParams1) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ1].dataArrIdx  = RXAUI_SEQ_IDX;

	/* RXAUI_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ2].opParamsPtr = ethPortTxConfigParams2;
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ2].cfgSeqSize  = sizeof(ethPortTxConfigParams2) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[RXAUI_TX_CONFIG_SEQ2].dataArrIdx  = RXAUI_SEQ_IDX;

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrlExt
(
	MV_U32 serdesNum,
	MV_BOOL serdesPowerUp,
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate,
	SERDES_MODE  serdesMode,
	REF_CLOCK refClock
)
{
	SERDES_SEQ speedSeqId;

	DEBUG_INIT_FULL_S("\n### mvSerdesPowerUpCtrlExt ###\n");

	if (serdesPowerUp == MV_TRUE) { /* Serdes power up */
		DEBUG_INIT_FULL_S("mvSerdesPowerUpCtrlExt: executing power up.. ");
		DEBUG_INIT_FULL_C("serdes num = ", serdesNum, 2);
		DEBUG_INIT_FULL_C("serdes type = ", serdesType, 2);

		DEBUG_INIT_FULL_S("Going access 1");

		/* Getting the Speed Select sequence id */
		speedSeqId = serdesTypeAndSpeedToSpeedSeq(serdesType, baudRate);
		if (speedSeqId == SERDES_LAST_SEQ) {
			DEBUG_INIT_S("mvSerdesPowerUpCtrlExt: serdes type and speed aren't supported together\n");
			return MV_BAD_PARAM;
		}

		/* Executing power up, ref clock set, speed config and TX config */
		switch (serdesType) {
        case SGMII3:
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_TX_CONFIG_SEQ1));
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_TX_CONFIG_SEQ2));
            break;
        case XAUI:
			CHECK_STATUS(mvSeqExec(serdesNum, XAUI_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, XAUI_ELECTRICAL_CONFIG_SEQ));
			CHECK_STATUS(mvSeqExec(serdesNum, XAUI_TX_CONFIG_SEQ1));
			CHECK_STATUS(mvSeqExec(serdesNum, XAUI_TX_CONFIG_SEQ2));
            break;
        case RXAUI:
			CHECK_STATUS(mvSeqExec(serdesNum, RXAUI_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, RXAUI_ELECTRICAL_CONFIG_SEQ));
			CHECK_STATUS(mvSeqExec(serdesNum, RXAUI_TX_CONFIG_SEQ1));
			CHECK_STATUS(mvSeqExec(serdesNum, RXAUI_TX_CONFIG_SEQ2));
            break;
		default:
			DEBUG_INIT_S("mvSerdesPowerUpCtrlExt: bad serdesType parameter\n");
			return MV_BAD_PARAM;
		}
	}else { /* Serdes power down */
		DEBUG_INIT_FULL_S("mvSerdesPowerUp: executing power down.. ");
		DEBUG_INIT_FULL_C("serdes num = ", serdesNum, 1);

		CHECK_STATUS(mvSeqExec(serdesNum, SERDES_POWER_DOWN_SEQ));
	}

	DEBUG_INIT_FULL_C("mvSerdesPowerUpCtrlExt ended successfully for serdes ", serdesNum, 2);

	return MV_OK;
}

/***************************************************************************/
MV_U32 mvHwsSerdesSiliconRefClockGet(MV_VOID)
{
    REF_CLOCK regSatR2;

    DEBUG_INIT_FULL_S("\n### mvHwsSerdesSiliconGetRefClockVal ###\n");

    regSatR2 = MV_REG_READ(DEVICE_SAMPLE_AT_RESET2_REG);
    regSatR2 = REF_CLK_SELECTOR_VAL(regSatR2);
    return (regSatR2 == REF_CLOCK__25MHz) ? REF_CLOCK__25MHz : REF_CLOCK__40MHz;
}

/***************************************************************************/
MV_U32 mvHwsSerdesGetMaxLane
(
	MV_VOID
)
{
    return 7;
}

/***************************************************************************/
MV_BOOL mvHwsIsSerdesActive(MV_U8 laneNum)
{
	if (laneNum < mvHwsSerdesGetMaxLane())
		return MV_TRUE;

	mvPrintf("%s: invalid SerDes lane for this device (%d)\n", __func__, laneNum);
	return MV_FALSE;
}

/***************************************************************************/
MV_STATUS mvHwsGetExtBaseAddr
(
	MV_U32 serdesNum,
	MV_U32 baseAddr,
	MV_U32 unitBaseOffset,
	MV_U32 *unitBaseReg,
	MV_U32 *unitOffset
)
{
	*unitBaseReg = baseAddr;
	*unitOffset  = unitBaseOffset;

	/* in a39x, serdes base addrress for COMPHY 6 is 0x18900
		(instead of 0x18300) */
	if (serdesNum == 6 && ((baseAddr & COMMON_PHY_BASE_ADDR_MASK) == COMMON_PHY_BASE_ADDR)) {
		*unitBaseReg += COMMON_PHY6_OFF;
		*unitOffset  = 0;
	}

	return MV_OK;
}

/*******************************************************************************
* mvHwsSerdesGetPhySelectorVal
*
* DESCRIPTION: Get the mapping of Serdes Selector values according to the
*               Serdes revision number
* INPUT:    serdesNum - Serdes number
*           serdesType - Serdes type
* OUTPUT: None
* RETURN:
* 		Mapping of Serdes Selector values
*******************************************************************************/
MV_U32 mvHwsSerdesGetPhySelectorVal(MV_32 serdesNum, SERDES_TYPE serdesType)
{
    if (serdesType >= LAST_SERDES_TYPE) {
        return 0xFF;
    }
	return commonPhysSelectorsSerdesRev2Map[serdesType][serdesNum];
}

/***************************************************************************/
MV_U32 mvHwsGetPhysicalSerdesNum(MV_U32 serdesNum)
{
	return serdesNum;
}

