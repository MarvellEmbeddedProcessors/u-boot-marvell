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
#include "mvBHboardEnvSpec.h"
#include "mvCtrlPex.h"
#include "mv_seq_exec.h"


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
#include "printf.h"

static MV_U32 gBoardId = -1;
MV_U32 mvBoardIdGet(MV_VOID)
{
	if (gBoardId != -1)
		return gBoardId;

/* Customer board ID's */
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	#ifdef CONFIG_BOBCAT2
	#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = BC2_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = BC2_CUSTOMER_BOARD_ID1;
		#endif
	#elif defined CONFIG_ALLEYCAT3
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = AC3_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = AC3_CUSTOMER_BOARD_ID1;
		#endif
	#endif
#else
/* BobCat2 Board ID's */
	#if defined(DB_BOBCAT2)
		gBoardId = DB_DX_BC2_ID;
	#elif defined(RD_BOBCAT2)
		gBoardId = RD_DX_BC2_ID;
	#elif defined(RD_MTL_BOBCAT2)
		gBoardId = RD_MTL_BC2;
/* AlleyCat3 Board ID's */
	#elif defined(DB_AC3)
		gBoardId = DB_AC3_ID;
	#else
		#error Invalid Board is configured
	#endif
#endif

	return gBoardId;
}


/*******************************************************************************
* mvBoardIdIndexGet
*
* DESCRIPTION:
*	returns an index for board arrays with direct memory access, according to board id
*
* INPUT:
*       boardId.
*
* OUTPUT:
*       direct access index for board arrays
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 as base for Board ID: mask MSB to receive index for board ID*/
	return boardId & (BOARD_ID_INDEX_MASK - 1);
}


#if defined MV_MSYS_AC3

MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 uiReg, serverBaseAddr;

	serverBaseAddr = MV_REG_READ(REG_XBAR_WIN_5_BASE_ADDR);

	uiReg = MV_REG_READ(serverBaseAddr + REG_DEVICE_SAR1_ADDR) & (PLL0_CNFIG_MASK << PLL0_CNFIG_OFFSET);

	switch (uiReg) {
	case 0:
		return MV_BOARD_TCLK_290MHZ;
		break;
	case 1:
		return MV_BOARD_TCLK_250MHZ;
		break;
	case 2:
		return MV_BOARD_TCLK_222MHZ;
		break;
	case 3:
		return MV_BOARD_TCLK_166MHZ;
		break;
	case 4:
		return MV_BOARD_TCLK_200MHZ;
		break;
	case 5:
		return MV_BOARD_TCLK_133MHZ;
		break;
	case 6:
		return MV_BOARD_TCLK_360MHZ;
		break;
	default:
		return MV_BOARD_TCLK_ERROR;
		break;
	}
}

/*****************/
/*    USB2       */
/*****************/

MV_OP_PARAMS usb2PowerUpParams[] =
{
	/* unitunitBaseReg unitOffset   mask       USB2 data  waitTime  numOfLoops */
	{ 0x804 ,          0x0 /*NA*/,	0x3,        {0x2},        0,        0}, /* Phy offset 0x1 - PLL_CONTROL1  */
	{ 0x80C ,          0x0 /*NA*/,	0x3000000,  {0x2000000},  0,        0}, /* Phy offset 0x3 - TX Channel control 0  */
	{ 0x800 ,          0x0 /*NA*/,	0x1FF007F,  {0x600005},   0,        0}, /* Phy offset 0x0 - PLL_CONTROL0  */
	{ 0x80C ,          0x0 /*NA*/,	0x3000000,  {0x3000000},  0,        0}, /* Phy offset 0x3 - TX Channel control 0  */
	{ 0x804 ,          0x0 /*NA*/,	0x3,        {0x3},        0,        0}, /* Phy offset 0x1 - PLL_CONTROL1  */
	{ 0x808 ,          0x0 /*NA*/,	0x80800000, {0x80800000}, 1,     1000}, /* check PLLCAL_DONE is set and IMPCAL_DONE is set*/
	{ 0x818 ,          0x0 /*NA*/,	0x80000000, {0x80000000}, 1,     1000}, /* check REG_SQCAL_DONE  is set*/
	{ 0x800 ,          0x0 /*NA*/,	0x80000000, {0x80000000}, 1,     1000} /* check PLL_READY  is set*/
};


MV_VOID serdesSeqInit(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### serdesSeqInit ###\n");

	/* SATA_ONLY_POWER_UP_SEQ sequence init */
	serdesSeqDb[USB2_POWER_UP_SEQ].opParamsPtr = usb2PowerUpParams;
	serdesSeqDb[USB2_POWER_UP_SEQ].cfgSeqSize  = sizeof(usb2PowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB2_POWER_UP_SEQ].dataArrIdx  = 0; /* Only USB2 uses these configurations */
}

MV_BOOL mvCtrlIsPexEndPointMode(MV_VOID)
{
	return  MV_FALSE; /*moti b - TBD will be impemented at nex patch*/
}
/* AC3: set PCIe mode as End Point */
MV_STATUS mvCtrlPexEndPointConfig(MV_VOID)
{
	return MV_OK; /*no EP config for AC3*/
}
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

MV_STATUS mvCtrlUsb2Config(MV_VOID)
{
	/* USB2 configuration */
	DEBUG_INIT_FULL_S("init USB2 Phys\n");
	CHECK_STATUS(mvSeqExec(0 /* not relevant */, USB2_POWER_UP_SEQ));
	return MV_OK;
}

#elif defined MV_MSYS_BC2

MV_U32 mvBoardTclkGet(MV_VOID)
{
	//TODO Add RD/DB detection. Currently set to DB.

	return MV_BOARD_TCLK_200MHZ;
}

MV_VOID serdesSeqInit(MV_VOID)
{

}

MV_BOOL mvCtrlIsPexEndPointMode(MV_VOID)
{
	MV_U32 uiReg = 0;
	/*Read SatR configuration(bit16)*/
	uiReg = MV_REG_READ(REG_DEVICE_SAR1_ADDR);
	return  ((uiReg & 0x10000) == 0);
}
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

MV_STATUS mvCtrlUsb2Config(MV_VOID)
{
	/* no USB2 in BC2 */
	return MV_OK;
}
#endif



MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
  /* Init serdes sequences DB */
  serdesSeqInit();

  if(mvCtrlIsPexEndPointMode() == MV_TRUE) {
	  /*PCI-E End Point configuration*/
	  mvCtrlPexEndPointConfig();
  }else {
	  /*PCI-E Root Complex configuration*/
	  mvCtrlPexRootComplexConfig();
  }

  /*USB2 configuration*/
  mvCtrlUsb2Config();


	return MV_OK;
}

