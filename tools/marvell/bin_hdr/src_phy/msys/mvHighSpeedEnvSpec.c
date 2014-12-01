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
#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvHighSpeedEnvSpec.h"
#include "mvHighSpeedTopologySpec.h"
#include "mv_seq_exec_ext.h"
#include "mvSysEnvLib.h"
#include "mvCtrlPex.h"


#if defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#endif

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"

/************************************ definitions ***********************************/

#define MV_SERDES_BASE					0x13000000
#define MV_SERDES_PHY_BASE				0x13000800
#define MV_SERDES_OFFSET				0x1000
#define MV_INTERNAL_OFFSET				0x1000

/************************************ globals ***********************************/
/* serdesSeqDb - holds all SERDES sequences, their size and the relevant index in the data array
   initialized in mvSerdesSeqInit */
MV_CFG_EXT_SEQ serdesSeqDb[SERDES_LAST_SEQ];

/* SERDES type to ref clock map */
REF_CLOCK serdesTypeToRefClockMap[LAST_SERDES_TYPE] =
{
	REF_CLOCK__100MHz,      /* PEX0 */
	REF_CLOCK__25MHz,       /* SGMII0 */
	REF_CLOCK__25MHz,       /* SGMII1 */
	REF_CLOCK_UNSUPPORTED   /* DEFAULT_SERDES */
};

/*****************/
/*    USB2       */
/*****************/

MV_OP_EXT_PARAMS usb2PowerUpParams[] =
{
	/* unitunitBaseReg unitOffset   mask       USB2 data     waitTime  numOfLoops */
	{ USB_REG_UNIT,    0x50804,     0x3,        { 0x2        }, 0,        0 }, /* Phy offset 0x1 - PLL_CONTROL1  */
	{ USB_REG_UNIT,    0x5080C,     0x3000000,  { 0x2000000  }, 0,        0 }, /* Phy offset 0x3 - TX Channel control 0  */
	{ USB_REG_UNIT,    0x50800,     0x1FF007F,  { 0x600005   }, 0,        0 }, /* Phy offset 0x0 - PLL_CONTROL0  */
	{ USB_REG_UNIT,    0x5080C,     0x3000000,  { 0x3000000  }, 0,        0 }, /* Phy offset 0x3 - TX Channel control 0  */
	{ USB_REG_UNIT,    0x50804,     0x3,        { 0x3        }, 0,        0 }, /* Phy offset 0x1 - PLL_CONTROL1  */
	{ USB_REG_UNIT,    0x50808,     0x80800000, { 0x80800000 }, 1,     1000 }, /* check PLLCAL_DONE is set and IMPCAL_DONE is set*/
	{ USB_REG_UNIT,    0x50818,     0x80000000, { 0x80000000 }, 1,     1000 }, /* check REG_SQCAL_DONE  is set*/
	{ USB_REG_UNIT,    0x50800,     0x80000000, { 0x80000000 }, 1,     1000 }  /* check PLL_READY  is set*/
};

/******************/
/*    SGMII       */
/******************/

MV_OP_EXT_PARAMS sgmiiSpeedExtConfigParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x0,    0x7F8,  { 0x330 }, 0,    0 },    /* Setting PIN_GEN_TX, PIN_GEN_RX */
	{ SERDES_UNIT,  0x28,   0x1F,   { 0xC   }, 0,    0 }    /* PIN_FREF_SEL=C (156.25MHz) */
};

MV_OP_EXT_PARAMS sgmiiSpeedIntConfigParams[] =
{
	/* unit             offset  mask      data     wait  numOf  */
	/* ID                                 1.25G    Time  Loops */
	{ SERDES_PHY_UNIT,  0x4,    0xFFFF, { 0xFD8C }, 0,    0 },    /*  FFE_R=0x0, FFE_C=0xF, SQ_THRESH=0x2*/
	{ SERDES_PHY_UNIT,  0x18,   0xFFFF, { 0x4000 }, 0,    0 },    /*  DFE_RES = 3.3mv*/
	{ SERDES_PHY_UNIT,  0x1C,   0xFFFF, { 0xF047 }, 0,    0 },    /*  DFE UPDAE all coefficient*/
	{ SERDES_PHY_UNIT,  0x34,   0xFFFF, { 0x406C }, 0,    0 },    /*  TX_AMP=31, AMP_ADJ=1*/
	{ SERDES_PHY_UNIT,  0x38,   0xFFFF, { 0x1E52 }, 0,    0 },    /*  MUPI/F=2, rx_digclkdiv=2*/
	{ SERDES_PHY_UNIT,  0x94,   0xFFFF, { 0x0FFF }, 0,    0 },    /*       */
	{ SERDES_PHY_UNIT,  0x98,   0xFFFF, { 0x66   }, 0,    0 },    /*  set password       */
	{ SERDES_PHY_UNIT,  0x104,  0xFFFF, { 0x2208 }, 0,    0 },    /*       */
	{ SERDES_PHY_UNIT,  0x108,  0xFFFF, { 0x243F }, 0,    0 },    /*  Set Gen_RX/TX       */
	{ SERDES_PHY_UNIT,  0x114,  0xFFFF, { 0x47EF }, 0,    0 },    /*  EMPH0 enable, EMPH_mode=2 */
	{ SERDES_PHY_UNIT,  0x134,  0xFFFF, { 0x004A }, 0,    0 },    /*  RX_IMP_VTH=2, TX_IMP_VTH=2 */
	{ SERDES_PHY_UNIT,  0x13C,  0xFBFF, { 0xE028 }, 0,    0 },    /*  clock set    */
	{ SERDES_PHY_UNIT,  0x140,  0xFFFF, { 0x800  }, 0,    0 },    /*  clk 8T enable for 10G and up */
	{ SERDES_PHY_UNIT,  0x154,  0xFFFF, { 0x87   }, 0,    0 },    /*  rxdigclk_dv_force=1  */
	{ SERDES_PHY_UNIT,  0x168,  0xFFFF, { 0xE014 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x16C,  0xFFFF, { 0x14   }, 0,    0 },    /*  DTL_FLOOP_EN=0  */
	{ SERDES_PHY_UNIT,  0x184,  0xFFFF, { 0x1013 }, 0,    0 },    /*  Force ICP=7        */
	{ SERDES_PHY_UNIT,  0x1A8,  0xFFFF, { 0x4000 }, 0,    0 },    /*  rxdigclk_dv_force=1*/
	{ SERDES_PHY_UNIT,  0x1AC,  0xFFFF, { 0x8498 }, 0,    0 },    /*   */
	{ SERDES_PHY_UNIT,  0x1DC,  0xFFFF, { 0x780  }, 0,    0 },    /*           */
	{ SERDES_PHY_UNIT,  0x1E0,  0xFFFF, { 0x03FE }, 0,    0 },    /* PHY_MODE=0x4,FREF_SEL=0xC   */
	{ SERDES_PHY_UNIT,  0x214,  0xFFFF, { 0x4418 }, 0,    0 },    /* PHY_MODE=0x4,FREF_SEL=0xC   */
	{ SERDES_PHY_UNIT,  0x220,  0xFFFF, { 0x400  }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x228,  0xFFFF, { 0x2FC0 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x268,  0xFFFF, { 0x8C02 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x278,  0xFFFF, { 0x21F3 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x280,  0xFFFF, { 0xC9F8 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x29C,  0xFFFF, { 0x05BC }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x2DC,  0xFFFF, { 0x2233 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x31C,  0xFFFF, { 0x318  }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x330,  0xFFFF, { 0x010F }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x334,  0xFFFF, { 0x0C03 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x338,  0xFFFF, { 0x3C00 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x33C,  0xFFFF, { 0x3C00 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x368,  0xFFFF, { 0x1000 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x36C,  0xFFFF, { 0x0AFA }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x378,  0xFFFF, { 0x1800 }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x418,  0xFFFF, { 0xE737 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x420,  0xFFFF, { 0x9B60 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x440,  0xFFFF, { 0x003E }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x444,  0xFFFF, { 0x2681 }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x468,  0xFFFF, { 0x1    }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x46C,  0xFFFF, { 0xFC7C }, 0,    0 },    /*      */
	{ SERDES_PHY_UNIT,  0x4A0,  0xFFFF, { 0x007C }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x4A4,  0xFFFF, { 0xFC7C }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x4A8,  0xFFFF, { 0xFC7C }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x4AC,  0xFFFF, { 0xFC7C }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x10C,  0xFFFF, { 0x838  }, 0,    0 }
};

MV_OP_EXT_PARAMS sgmiiSdResetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x8,    { 0x0 },   0,    0 }	/* SERDES_SD_RESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiSdUnresetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x8,    { 0x8 },   0,    0 }	/* SERDES_SD_UNRESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiRfResetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x40,   { 0x0 },  0,    0 }	/* SERDES_RF_RESET Sequence init */
};

MV_OP_EXT_PARAMS sgmiiRfUnresetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x40,   { 0x40 },  0,    0 },	/* SERDES_RF_UNRESET Sequence init */
};

MV_OP_EXT_PARAMS sgmiiCoreResetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x20,   { 0x0 },  0,    0 }	/* SERDES_CORE_RESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiCoreUnresetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0x20,   { 0x20 },  0,    0 }	/* SERDES_CORE_UNRESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiSynceResetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x8,    0x8,    { 0x0 },   0,    0 }	/* SERDES_SYNCE_RESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiSynceUnresetParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x4,    0xFF00, { 0xDD00 }, 0,    0 },	/* SERDES_SYNCE_UNRESET_SEQ Sequence init */
	{ SERDES_UNIT,  0x8,    0xB,    { 0xB    }, 0,    0 }		/* SERDES_SYNCE_UNRESET_SEQ Sequence init */
};

MV_OP_EXT_PARAMS sgmiiPowerUpCtrlParams[] =
{
	/* unit             offset  mask      data     wait  numOf  */
	/* ID                                 1.25G    Time  Loops */
	{ SERDES_UNIT,      0x0,    0x1802, { 0x1802 }, 0,    0 },
	{ SERDES_UNIT,      0x8,    0x10,   { 0x10   }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x8,    0x8000, { 0x8000 }, 0,    0 },
	{ SERDES_PHY_UNIT,  0x8,    0x4000, { 0x4000 }, 10,   10},
	{ SERDES_PHY_UNIT,  0x8,    0x8000, { 0x0    }, 0,    0 },
	{ 0,                0,      0,      { 0      }, 6,    0 }
};

MV_OP_EXT_PARAMS sgmiiPowerDownCtrlParams[] =
{
	/* unit         offset  mask      data     wait  numOf  */
	/* ID                             1.25G    Time  Loops */
	{ SERDES_UNIT,  0x0,   0x1802,  { 0x0 },   0,      0 },
	{ SERDES_UNIT,  0x4,   0x4000,  { 0x0 },   0,      0 }
};

/**************************************************************************************/

/*********************************** Enums ************************************/

/************************* Local functions declarations ***********************/

MV_STATUS boardTopologyLoad(SERDES_MAP  *serdesMapArray);

/*************************** Functions implementation *************************/

/**************************************************************************
 * mvHwsSerdesLastLaneGet -
 *
 * DESCRIPTION:          return last lane number.
 * INPUT:   			 NONE.
 * OUTPUT:               None.
 * RETURNS:              number of lanes
 ***************************************************************************/
MV_U32 mvHwsSerdesLastLaneGet(MV_VOID)
{
	return LAST_LANE_NUM;
}

/**************************************************************************
 * boardTopologyLoad -
 *
 * DESCRIPTION:          Loads the board topology for the DB_A38X_BP board
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology map.
 * RETURNS:              MV_OK           -   for success
 *                       MV_FAIL         -   for failure (a wrong
 *                                           topology mode was read
 *                                           from the board)
 ***************************************************************************/
MV_STATUS boardTopologyLoad(SERDES_MAP  *serdesMapArray)
{
	MV_U32		laneNum;
	MV_U32		boardIdIndex;
	SERDES_MAP	*topologyConfigPtr;

	DEBUG_INIT_FULL_S("\n### loadTopologyDB ###\n");

	boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());

	topologyConfigPtr = mvHwsSerdesTopologyGet(boardIdIndex);
	if (topologyConfigPtr == NULL)
	{
		return MV_NOT_SUPPORTED;
	}

	/* Updating the topology map */
	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
		serdesMapArray[laneNum].serdesMode  =  topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesNum   =  topologyConfigPtr[laneNum].serdesNum;
		serdesMapArray[laneNum].serdesSpeed =  topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  =  topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      =  topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      =  topologyConfigPtr[laneNum].swapTx;
	}

	return MV_OK;
}

#if defined MV_MSYS_AC3

/*AC3: check S@R for PCIe mode (EP/RC) ****************************************/
MV_BOOL mvCtrlIsPexEndPointMode(MV_VOID)
{
	MV_U32 uiReg = 0;
	/*Read AC3 SatR configuration SAR0[14]*/
	CHECK_STATUS(mvGenUnitRegisterGet(SERVER_REG_UNIT, 0, REG_DEVICE_SAR0_ADDR, &uiReg, BIT14));
	return  (uiReg == 0);
}

/* AC3: Get the Serdes revision number **************************************/
MV_U8 mvHwsSerdesRevGet(MV_VOID)
{
	return MV_SERDES_28NM_REV_3;
}

/* AC3: init silicon related configurations *********************************/
MV_STATUS mvSiliconInit(MV_VOID)
{
	MV_TWSI_ADDR slave;
	MV_U32 tClock;

	/* Prepare data to be used by access functions for various SOC regions */
	mvUnitInfoSet(INTERNAL_REG_UNIT,	INTER_REGS_BASE,		MV_INTERNAL_OFFSET);
	mvUnitInfoSet(MG_UNIT,				0,						MV_INTERNAL_OFFSET);
	mvUnitInfoSet(SERDES_UNIT,			MV_SERDES_BASE,			MV_SERDES_OFFSET);
	mvUnitInfoSet(SERDES_PHY_UNIT,		MV_SERDES_PHY_BASE,		MV_SERDES_OFFSET);
	mvUnitInfoSet(USB_REG_UNIT,			MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(USB_WIN_ID)) & 0xFFFF0000,		0);
	mvUnitInfoSet(SERVER_REG_UNIT,		MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(SERVER_WIN_ID)) & 0xFFFF0000,	0);

	/* Set legacy mode address completion */
	mvGenUnitRegisterSet(MG_UNIT, 0, 0x140, (1 << 16), (1 << 16));
	/* TWSI init */
	DEBUG_INIT_FULL_S("mvSiliconInit: Init TWSI interface.\n");
	slave.type = ADDR7_BIT;
	slave.address = 0;
	tClock = mvBoardTclkGet();
	if (tClock == MV_BOARD_TCLK_ERROR) {
		DEBUG_INIT_FULL_S("mvSiliconInit: TClk read from the board is not supported\n");
		return MV_NOT_SUPPORTED;
	}

	mvTwsiInit(0, TWSI_SPEED, tClock, &slave, 0);

	return MV_OK;
}

/****************************************************************************/
/* AC3: set PCIe mode as End Point */
MV_STATUS mvCtrlPexEndPointConfig(MV_VOID)
{
	return MV_OK; /*no EP config for AC3*/
}

/****************************************************************************/
/* AC3: set PCIe mode as Root Complex */
MV_STATUS mvCtrlPexRootComplexConfig(MV_VOID)
{
	MV_U32 uiReg = 0;
	/* Reg 0x18204, Set PCIe0nEn[0] to 0x0*/
	uiReg = MV_REG_READ(SOC_CTRL_REG);
	uiReg &= ~(0x1 << 0);
	uiReg |= (0x0 << 0);
	MV_REG_WRITE(SOC_CTRL_REG, uiReg);

	/* Reg 0x40060, Set DevType[23:20] to 0x4(Root Complex)*/
	uiReg = MV_REG_READ(PEX_CAPABILITIES_REG(0));
	uiReg &= ~(0xF << 20);
	uiReg |= (0x4 << 20);
	MV_REG_WRITE(PEX_CAPABILITIES_REG(0), uiReg);

	/* Reg 0x41a60, Assert soft_reset[20] to 0x1,
					Set DisLinkRestartRegRst[19] to 0x1,
					Set ConfMskLnkRestart[16] to 0x1*/
	uiReg = MV_REG_READ(PEX_DBG_CTRL_REG(0));
	uiReg &= 0xFFE6FFFF;
	uiReg |= 190000;
	MV_REG_WRITE(PEX_DBG_CTRL_REG(0), uiReg);

	/* Reg 0x41a00, Set ConfRoot_Complex to 0x1*/
	uiReg = MV_REG_READ(PEX_CTRL_REG(0));
	uiReg &= ~(0x1 << 1);
	uiReg |= (0x1 << 1);
	MV_REG_WRITE(PEX_CTRL_REG(0), uiReg);

	/* Reg 0x41a60, Deassert soft_reset[20] to 0x0*/
	uiReg = MV_REG_READ(PEX_DBG_CTRL_REG(0));
	uiReg &= ~(0x1 << 20);
	MV_REG_WRITE(PEX_DBG_CTRL_REG(0), uiReg);

	/* Reg 0x18204, Set PCIe0nEn[0] to 0x1*/
	uiReg = MV_REG_READ(SOC_CTRL_REG);
	uiReg &= ~(0x1 << 0);
	uiReg |= (0x1 << 0);
	MV_REG_WRITE(SOC_CTRL_REG, uiReg);

	return mvHwsPexConfig();
}

/*AC3: initialize USB2.0 UTMI PHY**********************************************/
MV_STATUS mvCtrlUsb2Config(MV_VOID)
{
	/* USB2 configuration */
	DEBUG_INIT_FULL_S("init USB2 PHYs\n");
	CHECK_STATUS(mvSeqExecExt(0 /* not relevant */, USB2_POWER_UP_SEQ));

	return MV_OK;
}

MV_STATUS mvHwsComH28nmSerdesTxIfSelect(MV_U32 serdesNum)
{
	MV_U8	serdesTxIfNum = (serdesNum == 10) ? 3 : 1; /* OOB port MSYS0/MSYS1 */
	/* Configure TX SERDES interface number */
	CHECK_STATUS(mvGenUnitRegisterSet(SERDES_UNIT, serdesNum, 0xC, serdesTxIfNum, 0x7));

	return MV_OK;
}


/*AC3: Set Ref Clock**********************************************/
MV_STATUS mvHwsRefClockGet (MV_U32 serdesNum ,MV_U8 *refClockSource)
{
	*refClockSource = PRIMARY; /* in AC3 all serdes has to use the same reference clock = PRIMARY */

	return MV_OK;
}
	/* Reference clock source */

#elif defined MV_MSYS_BC2

/* BC2: init silicon related configurations *********************************/
MV_STATUS mvSiliconInit(MV_VOID)
{
	MV_TWSI_ADDR slave;
	MV_U32 tClock;

	/* Prepare data to be used by access functions for various SOC regions */
	mvUnitInfoSet(INTERNAL_REG_UNIT,	INTER_REGS_BASE,		MV_INTERNAL_OFFSET);
	mvUnitInfoSet(MG_UNIT,				0,						MV_INTERNAL_OFFSET);
	mvUnitInfoSet(SERDES_UNIT,			MV_SERDES_BASE,			MV_SERDES_OFFSET);
	mvUnitInfoSet(SERDES_PHY_UNIT,		MV_SERDES_PHY_BASE,		MV_SERDES_OFFSET);
	mvUnitInfoSet(SERVER_REG_UNIT,		MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(SERVER_WIN_ID)) & 0xFFFF0000,	0);

	/* Set legacy mode address completion */
	mvGenUnitRegisterSet(MG_UNIT, 0, 0x140, (1 << 16), (1 << 16));

	/*(errata : 4410555):
	 BC2 specific register Device Control 0 (0x000F8250) Bit 4 should be 0
	 before serdes power up */
	mvGenUnitRegisterSet(SERVER_REG_UNIT,0,REG_DEVICE_SERVER_CONTROL_0,(0 << 4),(1 << 4));

	/* initialize TWSI interface */
	DEBUG_INIT_FULL_S("mvSiliconInit: Init TWSI interface.\n");
	slave.type = ADDR7_BIT;
	slave.address = 0;
	tClock = mvBoardTclkGet();
	if (tClock == MV_BOARD_TCLK_ERROR) {
		DEBUG_INIT_FULL_S("mvSiliconInit: TClk read from the board is not supported\n");
		return MV_NOT_SUPPORTED;
	}

	mvTwsiInit(0, TWSI_SPEED, tClock, &slave, 0);
	return MV_OK;
}

/****************************************************************************/
/* BC2: set PCIe mode as End Point */
MV_STATUS mvCtrlPexEndPointConfig(MV_VOID)
{
	MV_U32 uiReg = 0;
		/*Do End Point pex config*/
		uiReg = MV_REG_READ(PEX_CAPABILITIES_REG(0));
		uiReg &= ~(0xF << 20);
		uiReg |= (0x1 << 20);
		MV_REG_WRITE(PEX_CAPABILITIES_REG(0), uiReg);
		MV_REG_WRITE(0x41a60, 0xF63F0C0);
		mvPrintf("EP detected.\n");
		return MV_OK;
}

/****************************************************************************/
/* BC2: set PCIe mode as Root Complex */
MV_STATUS mvCtrlPexRootComplexConfig(MV_VOID)
{
	MV_U32 uiReg = 0;
	/*Do Root Complex pex config*/
	uiReg = MV_REG_READ(PEX_CAPABILITIES_REG(0));
	uiReg &= ~(0xF << 20);
	uiReg |= (0x4 << 20);
	MV_REG_WRITE(PEX_CAPABILITIES_REG(0), uiReg);
	mvPrintf("RC detected.\n");
	return mvHwsPexConfig();
}

/*BC2: check S@R for PCIe mode (EP/RC) ****************************************/
MV_BOOL mvCtrlIsPexEndPointMode(MV_VOID)
{
	MV_U32 uiReg = 0;

	/*Read BC2 SatR configuration SAR0[16]*/
	CHECK_STATUS(mvGenUnitRegisterGet(SERVER_REG_UNIT, 0, REG_DEVICE_SAR1_ADDR, &uiReg, BIT16));

	/* check BIT16 for PCIe mode status: 0 = EP , 1 = RC */
	return  (uiReg == 0);
}

/* BC2: Get the Serdes revision number **************************************/
MV_U8 mvHwsSerdesRevGet(MV_VOID)
{
	return (mvSysEnvDeviceRevGet() == MV_MSYS_BC2_A0_ID) ? MV_SERDES_28NM_REV_1 : MV_SERDES_28NM_REV_3;
}

/*BC2: initialize USB2.0 UTMI PHY**********************************************/
MV_STATUS mvCtrlUsb2Config(MV_VOID)
{
	/* no USB2 in BC2 */
	return MV_OK;
}

MV_STATUS mvHwsComH28nmSerdesTxIfSelect(MV_U32 serdesNum)
{

	MV_U32	msys2sdShift = ((serdesNum == 20) || (serdesNum == 0))  ? 14 : 13; /* serdes 0 and 20 are connected by bit 14 
																			    and serdes 1 and 21 by bit 13 of REG_DEVICE_SERVER_CONTROL_14 */ 
	/* set RF_QSGMII_PORT_TO_CPU_EN bit in DFX */
	CHECK_STATUS(mvGenUnitRegisterSet(SERVER_REG_UNIT, 0, REG_DEVICE_SERVER_CONTROL_14, 1 << msys2sdShift , 1 << msys2sdShift));
	/* Configure TX SERDES interface number - for BC2 - always 1*/
	CHECK_STATUS(mvGenUnitRegisterSet(SERDES_UNIT, serdesNum, 0xC, 1, 0x7));

	return MV_OK;
}
MV_STATUS mvHwsRefClockGet (MV_U32 serdesNum ,MV_U8 *refClockSource)
{
	*refClockSource = (serdesNum >= 20) ? SECONDARY : PRIMARY;
	/* in BC2 2 reference clocks: SECONDARY and PRIMARY
		serdeses 20 and 21 has to use SECONDARY others -primary */

	return MV_OK;
}



#endif

/* Init serdes sequences DB ********************************************/
MV_VOID mvSerdesSeqInit(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### serdesSeqInit ###\n");

	/* SATA_ONLY_POWER_UP_SEQ sequence init */
	serdesSeqDb[USB2_POWER_UP_SEQ].opParamsPtr = usb2PowerUpParams;
	serdesSeqDb[USB2_POWER_UP_SEQ].cfgSeqSize  = sizeof(usb2PowerUpParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[USB2_POWER_UP_SEQ].dataArrIdx  = 0; /* Only USB2 uses these configurations */

	/* SGMII_INT_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII_INT_SPEED_CONFIG_SEQ].opParamsPtr = sgmiiSpeedIntConfigParams;
	serdesSeqDb[SGMII_INT_SPEED_CONFIG_SEQ].cfgSeqSize  = sizeof(sgmiiSpeedIntConfigParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_INT_SPEED_CONFIG_SEQ].dataArrIdx  = 0 /* speed */;

	/* SGMII_EXT_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII_EXT_SPEED_CONFIG_SEQ].opParamsPtr = sgmiiSpeedExtConfigParams;
	serdesSeqDb[SGMII_EXT_SPEED_CONFIG_SEQ].cfgSeqSize  = sizeof(sgmiiSpeedExtConfigParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_EXT_SPEED_CONFIG_SEQ].dataArrIdx  = 0 /* speed */;

	/* SGMII_SD_RESET_SEQ sequence init */
	serdesSeqDb[SGMII_SD_RESET_SEQ].opParamsPtr = sgmiiSdResetParams;
	serdesSeqDb[SGMII_SD_RESET_SEQ].cfgSeqSize  = sizeof(sgmiiSdResetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_SD_RESET_SEQ].dataArrIdx  = 0;

	/* SGMII_SD_UNRESET_SEQ sequence init */
	serdesSeqDb[SGMII_SD_UNRESET_SEQ].opParamsPtr = sgmiiSdUnresetParams;
	serdesSeqDb[SGMII_SD_UNRESET_SEQ].cfgSeqSize  = sizeof(sgmiiSdUnresetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_SD_UNRESET_SEQ].dataArrIdx  = 0;

	/* SGMII_RF_RESET_SEQ sequence init */
	serdesSeqDb[SGMII_RF_RESET_SEQ].opParamsPtr = sgmiiRfResetParams;
	serdesSeqDb[SGMII_RF_RESET_SEQ].cfgSeqSize  = sizeof(sgmiiRfResetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_RF_RESET_SEQ].dataArrIdx  = 0;

	/* SGMII_RF_UNRESET_SEQ sequence init */
	serdesSeqDb[SGMII_RF_UNRESET_SEQ].opParamsPtr = sgmiiRfUnresetParams;
	serdesSeqDb[SGMII_RF_UNRESET_SEQ].cfgSeqSize  = sizeof(sgmiiRfUnresetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_RF_UNRESET_SEQ].dataArrIdx  = 0;

	/* SGMII_CORE_RESET_SEQ sequence init */
	serdesSeqDb[SGMII_CORE_RESET_SEQ].opParamsPtr = sgmiiCoreResetParams;
	serdesSeqDb[SGMII_CORE_RESET_SEQ].cfgSeqSize  = sizeof(sgmiiCoreResetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_CORE_RESET_SEQ].dataArrIdx  = 0;

	/* SGMII_CORE_UNRESET_SEQ sequence init */
	serdesSeqDb[SGMII_CORE_UNRESET_SEQ].opParamsPtr = sgmiiCoreUnresetParams;
	serdesSeqDb[SGMII_CORE_UNRESET_SEQ].cfgSeqSize  = sizeof(sgmiiCoreUnresetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_CORE_UNRESET_SEQ].dataArrIdx  = 0;

	/* SGMII_SYNCE_RESET_SEQ sequence init */
	serdesSeqDb[SGMII_SYNCE_RESET_SEQ].opParamsPtr = sgmiiSynceResetParams;
	serdesSeqDb[SGMII_SYNCE_RESET_SEQ].cfgSeqSize  = sizeof(sgmiiSynceResetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_SYNCE_RESET_SEQ].dataArrIdx  = 0;

	/* SGMII_SYNCE_UNRESET_SEQ sequence init */
	serdesSeqDb[SGMII_SYNCE_UNRESET_SEQ].opParamsPtr = sgmiiSynceUnresetParams;
	serdesSeqDb[SGMII_SYNCE_UNRESET_SEQ].cfgSeqSize  = sizeof(sgmiiSynceUnresetParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_SYNCE_UNRESET_SEQ].dataArrIdx  = 0;

	/* SGMII_POWER_UP_SEQ sequence init */
	serdesSeqDb[SGMII_POWER_UP_SEQ].opParamsPtr = sgmiiPowerUpCtrlParams;
	serdesSeqDb[SGMII_POWER_UP_SEQ].cfgSeqSize  = sizeof(sgmiiPowerUpCtrlParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_POWER_UP_SEQ].dataArrIdx  = 0;

	/* SGMII_POWER_DOWN_SEQ sequence init */
	serdesSeqDb[SGMII_POWER_DOWN_SEQ].opParamsPtr = sgmiiPowerDownCtrlParams;
	serdesSeqDb[SGMII_POWER_DOWN_SEQ].cfgSeqSize  = sizeof(sgmiiPowerDownCtrlParams) / sizeof(MV_OP_EXT_PARAMS);
	serdesSeqDb[SGMII_POWER_DOWN_SEQ].dataArrIdx  = 0;
}

/****************************************************************************/
MV_STATUS mvSerdesReset(
	MV_U32 serdesNum,
	MV_BOOL analogReset,
	MV_BOOL digitalReset,
	MV_BOOL syncEReset)
{
	MV_U8 seqId;

	seqId = (analogReset == MV_TRUE) ? SGMII_SD_RESET_SEQ : SGMII_SD_UNRESET_SEQ;
	CHECK_STATUS(mvSeqExecExt(serdesNum, seqId));

	seqId = (digitalReset == MV_TRUE) ? SGMII_RF_RESET_SEQ : SGMII_RF_UNRESET_SEQ;
	CHECK_STATUS(mvSeqExecExt(serdesNum, seqId));

	seqId = (syncEReset == MV_TRUE) ? SGMII_SYNCE_RESET_SEQ : SGMII_SYNCE_UNRESET_SEQ;
	CHECK_STATUS(mvSeqExecExt(serdesNum, seqId));

	return MV_OK;
}

/****************************************************************************/
MV_STATUS mvSerdesCoreReset(MV_U32 serdesNum, MV_BOOL coreReset)
{
	MV_U8 seqId;
  /* Init serdes sequences DB */
	seqId = (coreReset == MV_TRUE) ? SGMII_CORE_RESET_SEQ : SGMII_CORE_UNRESET_SEQ;
	CHECK_STATUS(mvSeqExecExt(serdesNum, seqId));

	return MV_OK;
}
/****************************************************************************/
MV_STATUS mvHwsComH28nmSerdesPolaritySwap(MV_U32 serdesNum, MV_BOOL isRx, MV_BOOL doSwap)
{
	/* bit[10] swaps TX polarity, bit[11] - RX polarity */
	MV_U16	polarityBit = (isRx == MV_TRUE) ? (1 << 11) : (1 << 10);
	MV_U16	data        = (doSwap == MV_TRUE) ? polarityBit : 0;

	CHECK_STATUS(mvGenUnitRegisterSet(SERDES_PHY_UNIT, serdesNum, 0x90, data, polarityBit));

	return MV_OK;
}

/****************************************************************************/
MV_STATUS mvHwsComH28nmSerdesPowerCtrl
(
	MV_U32	serdesNum,
	MV_BOOL	powerUp
)
{
	MV_U32 regData;
	MV_U8 refClockSource;

	if (mvHwsSerdesRevGet() == MV_SERDES_28NM_REV_1) {
		/* in BC2-A0 there is no MSYS Serdes support */
		return MV_OK;
	}

	if (powerUp == MV_TRUE) {
		DEBUG_INIT_FULL_S("mvSerdesPowerUpCtrl: executing power up.. ");
		/* config media */
		regData = 0; /* (media == RXAUI_MEDIA) ? (1 << 15) : 0; */

		/* config 10BIT mode */
		regData += (1 << 14); /* (mode == _10BIT_ON) ? (1 << 14) : 0; */
		CHECK_STATUS(mvGenUnitRegisterSet(SERDES_UNIT, serdesNum, 0, regData, (3 << 14)));

		/* Serdes Analog Un Reset*/
		CHECK_STATUS(mvSerdesReset(serdesNum, MV_FALSE, MV_TRUE, MV_TRUE));
		CHECK_STATUS(mvSerdesCoreReset(serdesNum, MV_FALSE));

		/* Reference clock source */
		CHECK_STATUS(mvHwsRefClockGet (serdesNum,&refClockSource));
		CHECK_STATUS(mvGenUnitRegisterSet(SERDES_PHY_UNIT, serdesNum, 0x13C, (refClockSource << 10), (1 << 10)));
		/* Serdes speed */
		CHECK_STATUS(mvSeqExecExt(serdesNum, SGMII_EXT_SPEED_CONFIG_SEQ));
		CHECK_STATUS(mvSeqExecExt(serdesNum, SGMII_INT_SPEED_CONFIG_SEQ));

		/* Serdes Power up Ctrl */
		CHECK_STATUS(mvSeqExecExt(serdesNum, SGMII_POWER_UP_SEQ));

		/* VDD calibration start (pulse) */
		regData = (1 << 2);
		CHECK_STATUS(mvGenUnitRegisterSet(SERDES_PHY_UNIT, serdesNum, 0x15C, regData, (1 << 2)));

		mvOsDelay(1);

		/* VDD calibration end (pulse) */
		regData = 0;
		CHECK_STATUS(mvGenUnitRegisterSet(SERDES_PHY_UNIT, serdesNum, 0x15C, regData, (1 << 2)));

		/* wait 1 msec */
		mvOsDelay(1);

		CHECK_STATUS(mvGenUnitRegisterGet(SERDES_UNIT, serdesNum, 0x18, &regData, 0x1C));

		/* Serdes Digital Un Reset */
		CHECK_STATUS(mvSerdesReset(serdesNum, MV_FALSE, MV_FALSE, MV_FALSE));
		CHECK_STATUS(mvSerdesCoreReset(serdesNum, MV_FALSE));

		if (regData != 0x1C)
			return MV_ERROR;

	} else {

		/*PCI-E Root Complex configuration*/
		DEBUG_INIT_FULL_S("mvSerdesPowerUpCtrl: executing power down.. ");

		CHECK_STATUS(mvSerdesReset(serdesNum, MV_TRUE, MV_FALSE, MV_FALSE));
		CHECK_STATUS(mvSerdesCoreReset(serdesNum, MV_TRUE));

		CHECK_STATUS(mvSeqExecExt(serdesNum, SGMII_POWER_DOWN_SEQ));

		CHECK_STATUS(mvSerdesReset(serdesNum, MV_TRUE, MV_TRUE, MV_TRUE));
		CHECK_STATUS(mvSerdesCoreReset(serdesNum, MV_TRUE));
	}

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvCtrlPexPolaritySet(MV_PCIE_POLARITY polarity)
{
	MV_TWSI_SLAVE	twsiSlave;

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = 0x18;	/* Address of AC3 CPLD */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0x1A;			/* Address of PEX polarity register */
	twsiSlave.moreThen256 = MV_FALSE;

	DEBUG_INIT_FULL_S("mvCtrlPexPolaritySet: Setting PEX polarity in CPLD\n");
	if (mvTwsiWrite(0, &twsiSlave, &polarity, 1) != MV_OK)
		DEBUG_INIT_S("mvCtrlPexPolaritySet: TWSI Write failed, leaving PEX polarity in EP mode\n");

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrl(
	MV_U32			serdesNum,
	MV_BOOL			serdesPowerUp,
	SERDES_TYPE		serdesType,
	SERDES_SPEED	baudRate,
	SERDES_MODE		serdesMode,
	REF_CLOCK		refClock,
	MV_BOOL			swapRx,
	MV_BOOL			swapTx
)
{

	DEBUG_INIT_FULL_S("\n### mvSerdesPowerUpCtrl ###\n");

	DEBUG_INIT_FULL_C("serdes num = ", serdesNum, 2);
	DEBUG_INIT_FULL_C("serdes type = ", serdesType, 2);

	/* Executing power up, ref clock set, speed config and TX config */
	switch (serdesType) {
	case SGMII0:
	case SGMII1:
		if (mvHwsSerdesRevGet() == MV_SERDES_28NM_REV_1) {
			/* in BC2-A0 there is no MSYS Serdes support so it is skipped */
			return MV_OK;
		}

		DEBUG_INIT_FULL_C("== Init SGMII\n", (serdesType == SGMII0 ? 0 :1), 1);
		CHECK_STATUS(mvHwsComH28nmSerdesPowerCtrl(serdesNum, serdesPowerUp));
		CHECK_STATUS(mvHwsComH28nmSerdesPolaritySwap(serdesNum, MV_TRUE, swapRx));
		CHECK_STATUS(mvHwsComH28nmSerdesPolaritySwap(serdesNum, MV_FALSE, swapTx));
		return mvHwsComH28nmSerdesTxIfSelect(serdesNum);

	case PEX0:
		DEBUG_INIT_FULL_S("== Init PEX0\n");
		if(mvCtrlIsPexEndPointMode() == MV_TRUE) {
			CHECK_STATUS(mvCtrlPexPolaritySet(MV_PCIE_POLARITY_EP));
			return mvCtrlPexEndPointConfig(); /*PCI-E End Point configuration*/
		} else {
			CHECK_STATUS(mvCtrlPexPolaritySet(MV_PCIE_POLARITY_RC));
			return mvCtrlPexRootComplexConfig(); /*PCI-E Root Complex configuration*/
		}
	default:
		DEBUG_INIT_S("mvSerdesPowerUpCtrl: bad serdesType parameter\n");
		return MV_BAD_PARAM;
	}

	DEBUG_INIT_FULL_C("mvSerdesPowerUpCtrl ended successfully for serdes ", serdesNum, 2);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS powerUpSerdesLanes(SERDES_MAP  *serdesConfigMap)
{
	MV_U32			serdesId;
	MV_U32			serdesNum;
	REF_CLOCK		refClock;
	SERDES_TYPE		serdesType;
	SERDES_SPEED	serdesSpeed;
	SERDES_MODE		serdesMode;
	MV_BOOL			swapRx;
	MV_BOOL			swapTx;

	DEBUG_INIT_FULL_S("\n### powerUpSerdesLanes ###\n");

	/* per Serdes Power Up */
	for (serdesId = 0; serdesId < MAX_SERDES_LANES; serdesId++) {

		DEBUG_INIT_FULL_S("calling serdesPowerUpCtrl: serdes lane number ");
		DEBUG_INIT_FULL_D_10(serdesId, 1);
		DEBUG_INIT_FULL_S("\n");

		serdesType  = serdesConfigMap[serdesId].serdesType;
		serdesNum   = serdesConfigMap[serdesId].serdesNum;
		serdesSpeed = serdesConfigMap[serdesId].serdesSpeed;
		serdesMode  = serdesConfigMap[serdesId].serdesMode;
		swapRx      = serdesConfigMap[serdesId].swapRx;
		swapTx      = serdesConfigMap[serdesId].swapTx;

		/* serdes lane is not in use */
		if (serdesType == DEFAULT_SERDES)
			continue;

		/* TBD - do we need to configure ref clock*/
		refClock = serdesTypeToRefClockMap[serdesType];
		if (refClock == REF_CLOCK_UNSUPPORTED) {
			DEBUG_INIT_S("powerUpSerdesLanes: unsupported ref clock\n");
			return MV_NOT_SUPPORTED;
		}

		CHECK_STATUS(mvSerdesPowerUpCtrl(serdesNum,
										 MV_TRUE,
										 serdesType,
										 serdesSpeed,
										 serdesMode,
										 refClock,
										 swapRx,
										 swapTx));
	}

	return MV_OK;
}

/****************************************************************************/
MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	SERDES_MAP serdesConfigurationMap[MAX_SERDES_LANES];

	mvPrintf("Serdes initialization - Version: 1.0.2\n");

	/* init silicon related configurations */
	mvSiliconInit();

	/* Init serdes sequences DB */
	mvSerdesSeqInit();

	CHECK_STATUS(boardTopologyLoad(serdesConfigurationMap));

	CHECK_STATUS(powerUpSerdesLanes(serdesConfigurationMap));

	/* USB2 configuration */
	mvCtrlUsb2Config();
	DEBUG_INIT_FULL_S("### powerUpSerdesLanes ended successfully ###\n");
	return MV_OK;
}
