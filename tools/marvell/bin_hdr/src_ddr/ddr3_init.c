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
#include "config_marvell.h"     /* Required to identify SOC and Board */

#ifdef MV88F78X60
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "ddr3_init.h"
#include "ddr3_spd.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"

#if defined(MV88F672X)|| defined (MV88F66XX) || defined(MV_MSYS_BC2) || defined(MV_MSYS_AC3) || defined(MV88F68XX)
#include "mvSysEnvLib.h"
#endif

#if defined(MV88F78X60)
#include "ddr3_axp_vars.h"
#elif defined(MV88F67XX)
#include "ddr3_a370_vars.h"
#elif defined(MV88F66XX)
#include "ddr3_alp_vars.h"
#include "mvHighSpeedEnvSpec.h"
#elif defined(MV88F672X)
#include "ddr3_a375_vars.h"
#include "mvHighSpeedEnvSpec.h"
#elif defined(MV88F68XX)
#include "ddr3_a38x.h"
#include "ddr3_a38x_vars.h"
#include "ddr3_a38x_topology.h"
#elif defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#include "ddr3_msys_bc2_vars.h"
#include "ddr3_msys_bc2_topology.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#include "ddr3_msys_ac3_vars.h"
#include "ddr3_msys_ac3_topology.h"
#endif

#if defined(MV88F68XX) || defined(MV_MSYS_BC2) || defined(MV_MSYS_AC3)
#include "ddr3_hws_hw_training.h"
#define MV_NEW_TIP
#if defined(MV_MSYS_BC2)
#define MARVELL_BOARD	BC2_MARVELL_BOARD_ID_BASE
#elif defined(MV_MSYS_AC3)
#define MARVELL_BOARD	AC3_MARVELL_BOARD_ID_BASE
#endif

extern MV_STATUS ddr3TipInitSpecificRegConfig
(
    MV_U32              devNum,
	MV_DRAM_MC_INIT		*regConfigArr
);
extern MV_STATUS    mvHwsDdr3TipLoadTopologyMap
(
    MV_U32                  devNum,
    MV_HWS_TOPOLOGY_MAP     *topology
);
extern MV_U32 ddr3TipGetInitFreq();
MV_STATUS ddr3LoadTopologyMap(void);
extern MV_VOID ddr3HwsSetLogLevel(
	MV_DDR_LIB_DEBUG_BLOCK 	block,
	MV_U8					level
);
#else
#define SUB_VERSION	1
#endif

#include "bootstrap_os.h"
#if defined(MV88F67XX)
static MV_VOID ddr3MRSCommand(MV_U32 uiMR1Value, MV_U32 uiMR2Value, MV_U32 uiCsNum, MV_U32 uiCsEna);
#endif
#ifdef STATIC_TRAINING
static MV_VOID ddr3StaticTrainingInit(void);
#endif
#if defined(DUNIT_STATIC) && !defined(MV_NEW_TIP)
static MV_VOID ddr3StaticMCInit(void);
#endif
#if defined(DUNIT_STATIC) || defined(STATIC_TRAINING) || defined(MV_NEW_TIP)
#ifndef MV_MSYS_BC2
static MV_U32 ddr3GetStaticDdrMode(void);
#endif
/*Set 1 to use dynamic DUNIT configuration,
	set 0(supported for A380 only) to configure DUNIT in values set by ddr3TipInitSpecificRegConfig*/
MV_U8 genericInitController = 1;
#endif
#if defined(MV88F66XX) || defined(MV88F672X) || defined(MV_NEW_TIP)
MV_VOID		getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs);
MV_VOID		ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna);
MV_VOID		ddr3FastPathStaticCsSizeConfig(MV_U32 uiCsEna);
#endif
MV_U32 	mvBoardIdIndexGet(MV_U32 boardId);
MV_U32 mvBoardIdGet(MV_VOID);
#if !defined(MV_NEW_TIP)
extern MV_VOID ddr3SetSwWlRlDebug(MV_U32);
extern MV_VOID ddr3SetPbs(MV_U32);
#endif
extern MV_VOID ddr3SetLogLevel(MV_U32 nLogLevel);
static MV_U32 gLogLevel = 0;

MV_STATUS ddr3CalcMemCsSize(MV_U32 uiCs, MV_U32* puiCsSize);

#if defined(MV88F78X60)
MV_U32 gTrefi;
#endif
#if !defined(MV_NEW_TIP)
/************************************************************************************
 * Name:     ddr3LogLevelInit
 * Desc:     This routine initialize the gLogLevel as defined in dd3_axp_config
 * Args:     None
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3LogLevelInit(MV_VOID)
{
	ddr3SetLogLevel(DDR3_LOG_LEVEL);
}

/************************************************************************************
 * Name:     ddr3PbsInit
 * Desc:     This routine initialize the PBS as defined in dd3_axp_config
 * Args:     None
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3PbsInit(MV_VOID)
{
	ddr3SetPbs(DDR3_PBS);
}
#endif
/************************************************************************************
 * Name:     setDdr3Log_Level
 * Desc:     This routine initialize the gLogLevel acording to nLogLevel which getting from user
 * Args:     nLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3SetLogLevel(MV_U32 nLogLevel)
{
	gLogLevel = nLogLevel;
}
/************************************************************************************
 * Name:     ddr3GetLogLevel
 * Desc:     This routine returns the log level
 * Args:     none
 * Notes:
 * Returns:  log level.
 */
MV_U32 ddr3GetLogLevel()
{
	return gLogLevel;
}

/************************************************************************************
 * Name:     levelLogPrintS
 * Desc:     This routine printing string "str" if gLogLevel>=eLogLevel
 * Args:     char *str,MV_LOG_LEVEL eLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID levelLogPrintS(char *str, MV_LOG_LEVEL eLogLevel)
{
	if (gLogLevel >= eLogLevel)
		putstring(str);
}

/************************************************************************************
 * Name:     levelLogPrintD
 * Desc:     This routine printing data in hex-decimal if gLogLevel>=eLogLevel
 * Args:     char *str,MV_LOG_LEVEL eLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID levelLogPrintD(MV_U32 dec_num, MV_U32 length, MV_LOG_LEVEL eLogLevel)
{
	if (gLogLevel >= eLogLevel)
		putdata(dec_num, length);
}

/************************************************************************************
 * Name:     levelLogPrintDD
 * Desc:     This routine printing data in decimal if gLogLevel>=eLogLevel
 * Args:     char *str,MV_LOG_LEVEL eLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID levelLogPrintDD(MV_U32 dec_num, MV_U32 length, MV_LOG_LEVEL eLogLevel)
{
	if (gLogLevel >= eLogLevel)
		putdataDec(dec_num, length);
}

#if !defined(STATIC_TRAINING)
static MV_VOID ddr3RestoreAndSetFinalWindows(MV_U32 *auWinBackup)
{
	MV_U32 winCtrlReg, numOfWinRegs;
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();

#if defined(MV88F66XX) || defined(MV88F672X)
	if(DDR3_FAST_PATH_EN == MV_FALSE)
		return;
#endif

#if defined(MV88F66XX) || defined(MV88F672X)
	winCtrlReg  = REG_XBAR_WIN_16_CTRL_ADDR;
	numOfWinRegs = 8;
#else
	winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
	numOfWinRegs = 16;
#endif
	MV_U32 ui;
	/* Return XBAR windows 4-7 or 16-19 init configuration */
	for (ui = 0; ui < numOfWinRegs; ui++)
		MV_REG_WRITE((winCtrlReg + 0x4 * ui), auWinBackup[ui]);

	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Switching XBAR Window to FastPath Window \n");

#if defined(MV88F66XX) || defined(MV88F672X) || defined(MV88F68XX)
#ifdef CONFIG_ALP_A375_ZX_REV
	ddr3FastPathStaticCsSizeConfig(uiCsEna);
#else
	ddr3FastPathDynamicCsSizeConfig(uiCsEna);
#endif
#else
	MV_U32 uiReg, uiCs;
	uiReg = 0x1FFFFFE1;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			uiReg |= (uiCs << 2);
			break;
		}
	}
	MV_REG_WRITE(REG_FASTPATH_WIN_0_CTRL_ADDR, uiReg); /*Open fast path Window to - 0.5G */
#endif
}

static MV_VOID ddr3SaveAndSetTrainingWindows(MV_U32 *auWinBackup)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiReg, uiTempCount, uiCs, ui;
	MV_U32 winCtrlReg, winBaseReg, winRemapReg;
	MV_U32 numOfWinRegs, winJumpIndex;
#if defined(MV88F66XX) || defined(MV88F672X)
	/* Disable L2 filtering */
	MV_REG_WRITE(0x8c04, 0);

	winCtrlReg  = REG_XBAR_WIN_16_CTRL_ADDR;
	winBaseReg  = REG_XBAR_WIN_16_BASE_ADDR;
	winRemapReg = REG_XBAR_WIN_16_REMAP_ADDR;
	winJumpIndex = 0x8;
	numOfWinRegs = 8;
#else
	winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
	winBaseReg  = REG_XBAR_WIN_4_BASE_ADDR;
	winRemapReg = REG_XBAR_WIN_4_REMAP_ADDR;
	winJumpIndex = 0x10;
	numOfWinRegs = 16;
#endif
	/*  Close XBAR Window 19 - Not needed */
	/* {0x000200e8}  -   Open Mbus Window - 2G */
	MV_REG_WRITE(REG_XBAR_WIN_19_CTRL_ADDR, 0);

	/* Save XBAR Windows 4-19 init configurations */
	for (ui = 0; ui < numOfWinRegs; ui++)
		auWinBackup[ui] = MV_REG_READ(winCtrlReg + 0x4 * ui);

	/*  Open XBAR Windows 4-7 or 16-19 for other CS */
	uiReg = 0;
	uiTempCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			switch (uiCs) {
			case 0:
				uiReg = 0x0E00;
				break;
			case 1:
				uiReg = 0x0D00;
				break;
			case 2:
				uiReg = 0x0B00;
				break;
			case 3:
				uiReg = 0x0700;
				break;
			}
			uiReg |= (1 << 0);
			uiReg |= (SDRAM_CS_SIZE & 0xFFFF0000);

			MV_REG_WRITE(winCtrlReg + winJumpIndex * uiTempCount, uiReg);
			uiReg = (((SDRAM_CS_SIZE + 1) * (uiTempCount)) & 0xFFFF0000);
			MV_REG_WRITE(winBaseReg + winJumpIndex * uiTempCount, uiReg);

			if(winRemapReg <= REG_XBAR_WIN_7_REMAP_ADDR)
				MV_REG_WRITE(winRemapReg + winJumpIndex * uiTempCount, 0);

			uiTempCount++;
		}
	}
}
#endif /*  !defined(STATIC_TRAINING) */
/************************************************************************************
 * Name:     ddr3Init - Main DDR3 Init function
 * Desc:     This routine initialize the DDR3 MC and runs HW training.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3Init_(void);
MV_STATUS ddr3Init(void)
{
	unsigned int status;
#if !defined(MV_NEW_TIP)
	ddr3LogLevelInit();
	ddr3SetPbs(DDR3_PBS);
	ddr3SetSwWlRlDebug(DDR3_RUN_SW_WHEN_HW_FAIL);
#endif
	status = ddr3Init_();
	/* DEBUG_INIT_S("Status = ");*/
	if (status == MV_DDR3_TRAINING_ERR_BAD_SAR)
		DEBUG_INIT_S("DDR3 Training Error: Bad sample at reset");
	if (status == MV_DDR3_TRAINING_ERR_BAD_DIMM_SETUP)
		DEBUG_INIT_S("DDR3 Training Error: Bad DIMM setup");
	if (status == MV_DDR3_TRAINING_ERR_MAX_CS_LIMIT)
		DEBUG_INIT_S("DDR3 Training Error: Max CS limit");
	if (status == MV_DDR3_TRAINING_ERR_MAX_ENA_CS_LIMIT)
		DEBUG_INIT_S("DDR3 Training Error: Max enable CS limit");
	if (status == MV_DDR3_TRAINING_ERR_BAD_R_DIMM_SETUP)
		DEBUG_INIT_S("DDR3 Training Error: Bad R-DIMM setup");
	if (status == MV_DDR3_TRAINING_ERR_TWSI_FAIL)
		DEBUG_INIT_S("DDR3 Training Error: TWSI failure");
	if (status == MV_DDR3_TRAINING_ERR_DIMM_TYPE_NO_MATCH)
		DEBUG_INIT_S("DDR3 Training Error: DIMM type no match");
	if (status == MV_DDR3_TRAINING_ERR_TWSI_BAD_TYPE)
		DEBUG_INIT_S("DDR3 Training Error: TWSI bad type");
	if (status == MV_DDR3_TRAINING_ERR_BUS_WIDTH_NOT_MATCH)
		DEBUG_INIT_S("DDR3 Training Error: bus width no match");
	if (status > MV_DDR3_TRAINING_ERR_HW_FAIL_BASE)
        DEBUG_INIT_C("DDR3 Training Error: HW Failure 0x", status, 8);

	return status;
}

void printA370DrrTargetFreq(MV_U32 uiCpuFreq, MV_U32 uiFabOpt)
{
	putstring("\nDDR3 Training Sequence - Run DDR3 at ");
	switch (uiCpuFreq) {
		case 3:
			putstring("400 Mhz\n");
			break;
		case 4:
			putstring("500 Mhz\n");
			break;
		case 5:
			putstring("533 Mhz\n");
			break;
		case 6:
			if (uiFabOpt == 5)
			putstring("600 Mhz\n");
			if (uiFabOpt == 9)
			putstring("400 Mhz\n");
			break;
		case 7:
			putstring("667 Mhz\n");
			break;
		default:
			putstring("NOT DEFINED FREQ\n");
	}
}

void printDrrTargetFreq(MV_U32 uiCpuFreq, MV_U32 uiFabOpt)
{
	putstring("\nDDR3 Training Sequence - Run DDR3 at ");
	switch (uiCpuFreq) {
#if defined(MV88F66XX) || defined(MV88F672X)
		case 21:
			putstring("533 Mhz\n");
			break;
#else
		case 1:
			putstring("533 Mhz\n");
			break;
		case 2:
			if (uiFabOpt == 5)
			putstring("600 Mhz\n");
			if (uiFabOpt == 9)
			putstring("400 Mhz\n");
			break;
		case 3:
			putstring("667 Mhz\n");
			break;
		case 4:
			if (uiFabOpt == 5)
			putstring("750 Mhz\n");
			if (uiFabOpt == 9)
			putstring("500 Mhz\n");
			break;
		case 0xa:
			putstring("400 Mhz\n");
			break;
		case 0xb:
			if (uiFabOpt == 5)
			putstring("800 Mhz\n");
			if (uiFabOpt == 9)
			putstring("553 Mhz\n");
			if (uiFabOpt == 0xA)
			putstring("640 Mhz\n");
			break;
#endif
		default:
			putstring("NOT DEFINED FREQ\n");
	}
}


MV_U32 ddr3Init_(void)
{
#ifndef MV_NEW_TIP
	MV_U32 uiTargetFreq, uiHClkTimePs, uiCpuFreq;
#endif

#if (defined(ECC_SUPPORT) || defined(DUNIT_SPD) || defined(MV88F78X60)) && !defined(MV_NEW_TIP)
	MV_U32 uiEcc = DRAM_ECC;
#endif

	MV_U32 uiReg = 0;
	MV_U32 uiFabOpt, socNum;
	MV_BOOL bPLLWAPatch = FALSE;
#if !defined(MV_NEW_TIP)
	MV_BOOL bDQSCLKAligned = FALSE;
	MV_U32 uiDdrWidth;
	MV_U32 uiScrubOffs, uiScrubSize;
#endif
#if defined(MV88F78X60) || defined(MV88F67XX)
	uiDdrWidth = BUS_WIDTH;
#endif

#if !defined(STATIC_TRAINING) || defined (DUNIT_SPD)
	MV_STATUS status;
	MV_U32 auWinBackup[16];
#endif
	/* SoC/Board special Initializtions */

	uiFabOpt = ddr3GetFabOpt();

#ifdef SPD_SUPPORT
	/* Twsi Init */
	{
		MV_TWSI_ADDR slave;
		slave.type = ADDR7_BIT;
		slave.address = 0;
		mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
	}
#endif

#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	/* Fix PLL init value WA */
	if (((uiFabOpt == 0x1A) || (uiFabOpt == 0x12)) && (mvCtrlRevGet() == MV_78XX0_A0_REV)) {
		/* Set original fabric setting */
#if defined(DB_88F78X60_REV2)
		fixPLLValue(ddr3GetEpromFabric());
#else
		fixPLLValue((MV_U8)DDR_TARGET_FABRIC);
#endif
		bPLLWAPatch = TRUE;
	}
#endif

	/*Get version from internal library*/
	ddr3PrintVersion();
	/*Add sub_version string*/
	DEBUG_INIT_C("",SUB_VERSION,1);

	uiFabOpt = ddr3GetFabOpt();
	if (bPLLWAPatch){
		DEBUG_INIT_C("DDR3 Training Sequence - Fabric DFS to: ", uiFabOpt, 1);
	}
	/* Switching CPU to MRVL ID */
	socNum = (MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET;
	switch (socNum) {
	case 0x3:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(3), CPU_MRVL_ID_OFFSET);
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(2), CPU_MRVL_ID_OFFSET);
	case 0x1:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(1), CPU_MRVL_ID_OFFSET);
	case 0x0:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(0), CPU_MRVL_ID_OFFSET);
	default:
		break;
	}
	/* Power down deskew PLL */
#if !defined(MV88F66XX) && !defined(MV88F672X) && !defined(MV_NEW_TIP)
	uiReg = (MV_REG_READ(REG_DDRPHY_APLL_CTRL_ADDR) & ~(1 << 25)); /* 0x18780 [25]  */
	MV_REG_WRITE(REG_DDRPHY_APLL_CTRL_ADDR, uiReg);
#endif
	/************************************************************************************/
	/* Stage 0 - Set board configuration                                                */
	/************************************************************************************/
	if (uiFabOpt > FAB_OPT)
		uiFabOpt = FAB_OPT - 1;

#ifndef MV_NEW_TIP
	uiCpuFreq = ddr3GetCpuFreq();
    if (ddr3GetLogLevel() > 0) {
#if defined(MV88F67XX)
		printA370DrrTargetFreq(uiCpuFreq, uiFabOpt);
#else
        printDrrTargetFreq(uiCpuFreq, uiFabOpt);
#endif
    }
#endif

#if defined(MV88F66XX) || defined(MV88F672X)
    getTargetFreq(uiCpuFreq, &uiTargetFreq, &uiHClkTimePs);
#else
#ifndef MV_NEW_TIP
    uiTargetFreq = s_auiCpuDdrRatios[uiFabOpt][uiCpuFreq];
    uiHClkTimePs = s_auiCpuFabClkToHClk[uiFabOpt][uiCpuFreq];
#endif
#endif
#ifndef MV_NEW_TIP
	if ((uiTargetFreq == 0) || (uiHClkTimePs == 0)) {
		DEBUG_INIT_S("DDR3 Training Sequence - FAILED - Wrong Sample at Reset Configurations \n");
		if (uiTargetFreq == 0) {
			DEBUG_INIT_C("uiTargetFreq", uiTargetFreq, 2);
			DEBUG_INIT_C("uiFabOpt", uiFabOpt, 2);
			DEBUG_INIT_C("uiCpuFreq", uiCpuFreq, 2);
		} else if (uiHClkTimePs == 0) {
			DEBUG_INIT_C("uiHClkTimePs", uiHClkTimePs, 2);
			DEBUG_INIT_C("uiFabOpt", uiFabOpt, 2);
			DEBUG_INIT_C("uiCpuFreq", uiCpuFreq, 2);
		}
		return MV_DDR3_TRAINING_ERR_BAD_SAR;
	}
#endif
#if !defined(MV_NEW_TIP)
#if defined(ECC_SUPPORT)
	uiScrubOffs = U_BOOT_START_ADDR;
	uiScrubSize = U_BOOT_SCRUB_SIZE;
#else
	uiScrubOffs = 0;
	uiScrubSize = 0;
#endif
#endif
#if defined(ECC_SUPPORT) && defined(AUTO_DETECTION_SUPPORT)
	uiEcc = DRAM_ECC;
#endif

#if defined(ECC_SUPPORT) && defined(AUTO_DETECTION_SUPPORT)
	uiEcc = 0;
	if (ddr3CheckConfig(BUS_WIDTH_ECC_TWSI_ADDR, CONFIG_ECC))
		uiEcc = 1;
#endif

#ifdef DQS_CLK_ALIGNED
	bDQSCLKAligned = TRUE;
#endif

	/* Check if DRAM is already initialized  */
	if (MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR) & (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS)) {
		DEBUG_INIT_S("DDR3 Training Sequence - 2nd boot - Skip \n");
		return MV_OK;
	}

	/************************************************************************************/
	/* Stage 1 - Dunit Setup                                                            */
	/************************************************************************************/

#if defined(DUNIT_STATIC) && !defined(MV_NEW_TIP)
	/* For Static D-Unit Setup use must set the correct static values at the ddr3_*soc*_vars.h file */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Static MC Init \n");
	ddr3StaticMCInit();

#if defined(MV88F66XX)
	ddr3GetAlpBusWidth();
#endif
#if defined(MV88F672X)
	ddr3GetA375BusWidth();
#endif

#ifdef ECC_SUPPORT
	uiEcc = DRAM_ECC;
	if (uiEcc) {
		uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
		uiReg |= (1 << REG_SDRAM_CONFIG_ECC_OFFS);
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
	}
#endif
#endif

#if defined(MV88F78X60) || defined(MV88F66XX)  || defined(MV88F672X)
#if defined(AUTO_DETECTION_SUPPORT)
	/* Configurations for both static and dynamic MC setups */
	/* Dynamically Set 32Bit and ECC for AXP (Relevant only for Marvell DB boards) */
	if (ddr3CheckConfig(BUS_WIDTH_ECC_TWSI_ADDR, CONFIG_BUS_WIDTH)) {
		uiDdrWidth = 32;
		DEBUG_INIT_S("DDR3 Training Sequence - DRAM bus width 32Bit \n");
	}
#endif
#if defined(MV88F66XX) || defined(MV88F672X)
	uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
	if ((uiReg >> 15) & 1)
		uiDdrWidth = 32;
	else
		uiDdrWidth = 16;
#endif
#endif

#if defined(DUNIT_SPD)
	status = ddr3DunitSetup(uiEcc, uiHClkTimePs, &uiDdrWidth);
	if (MV_OK != status) {
		DEBUG_INIT_S("DDR3 Training Sequence - FAILED (ddr3 Dunit Setup) \n");
		return status;
	}
#else
#if defined (MV88F78X60)
	gTrefi = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & REG_SDRAM_CONFIG_RFRS_MASK) * uiHClkTimePs;/*initiate gTrefi for static D-Unit*/
#endif
#endif
	/* Fix read ready phases for all SOC in reg 0x15C8*/
	uiReg = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK);
	uiReg |= 0x4;                                           /* Phase 0 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << REG_TRAINING_DEBUG_3_OFFS);
	uiReg |= (0x4 << (1 * REG_TRAINING_DEBUG_3_OFFS));      /* Phase 1 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (3 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (3 * REG_TRAINING_DEBUG_3_OFFS));      /* Phase 3 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (4 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (4 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (5 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (5 * REG_TRAINING_DEBUG_3_OFFS));
	MV_REG_WRITE(REG_TRAINING_DEBUG_3_ADDR, uiReg);

#if defined(MV88F66XX) || defined(MV88F672X) || defined(MV_NEW_TIP)
	/* AxiBrespMode[8] = Compliant, AxiAddrDecodeCntrl[11] = Internal, AxiDataBusWidth[0] = 128bit */
	MV_REG_WRITE(REG_DRAM_AXI_CTRL_ADDR, 0);                /* 0x14A8 - AXI Control Register */
#else
	MV_REG_WRITE(REG_DRAM_AXI_CTRL_ADDR, 0x00000100);       /* 0x14A8 - AXI Control Register */
	MV_REG_WRITE(REG_CDI_CONFIG_ADDR, 0x00000006);

	if ((uiDdrWidth == 64) && (MV_REG_READ(REG_DDR_IO_ADDR) & (1 << REG_DDR_IO_CLK_RATIO_OFFS))) {
		MV_REG_WRITE(REG_DRAM_AXI_CTRL_ADDR, 0x00000101); /* 0x14A8 - AXI Control Register */
		MV_REG_WRITE(REG_CDI_CONFIG_ADDR, 0x00000007);
	}
#endif

#if !defined(MV88F67XX) && !defined(MV_NEW_TIP)
/* ARMADA-370 activate DLB later at the u-boot, Armada38x - No DLB activation at this time */
MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x18C01E);
#if defined(MV88F78X60)
/* WA according to eratta GL-8672902*/
	if (mvCtrlRevGet() == MV_78XX0_B0_REV){
        MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0xc19e);
    }
#endif
	MV_REG_WRITE(DLB_AGING_REGISTER, 0x0f7f007f);
	MV_REG_WRITE(DLB_EVICTION_CONTROL_REG, 0x0);
	MV_REG_WRITE(DLB_EVICTION_TIMERS_REGISTER_REG, 0x00FF3C1F);

	MV_REG_WRITE(MBUS_UNITS_PRIORITY_CONTROL_REG, 0x55555555);
#if defined(MV88F78X60) || defined(MV88F67XX)
	MV_REG_WRITE(FABRIC_UNITS_PRIORITY_CONTROL_REG, 0xAA);
#endif
	MV_REG_WRITE(MBUS_UNITS_PREFETCH_CONTROL_REG, 0xffff);
	MV_REG_WRITE(FABRIC_UNITS_PREFETCH_CONTROL_REG, 0xf0f);
#if defined(MV88F78X60)
/* WA according to eratta GL-8672902*/
	if (mvCtrlRevGet() == MV_78XX0_B0_REV) {
		uiReg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);
		uiReg |= DLB_ENABLE;
		MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, uiReg);
	}
#endif /* end defined(MV88F78X60) */
#endif /* end !defined(MV88F67XX) */
	if (ddr3GetLogLevel() >= MV_LOG_LEVEL_1)
		printDunitSetup();

	/************************************************************************************/
	/* Stage 2 - Training Values Setup                                                  */
	/************************************************************************************/
#ifdef STATIC_TRAINING
	/* DRAM Init - After all the D-unit values are set, its time to init the D-unit */
	/* Wait for '0' */
	MV_REG_WRITE(REG_SDRAM_INIT_CTRL_ADDR, 0x1);
	do
		uiReg = ((MV_REG_READ(REG_SDRAM_INIT_CTRL_ADDR)) & (1 << REG_SDRAM_INIT_CTRL_OFFS));
	while (uiReg);

	/* ddr3 init using static parameters - HW training is disabled */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Static Training Parameters \n");
	ddr3StaticTrainingInit();
#if defined(MV88F78X60)
	/* if ECC is enabled, need to scrub the U-Boot area memory region - Run training function with Xor bypass
	   just to scrub the memory */
	status = ddr3HwTraining(uiTargetFreq, uiDdrWidth,
				MV_TRUE, uiScrubOffs, uiScrubSize, bDQSCLKAligned, DDR3_TRAINING_DEBUG, REG_DIMM_SKIP_WL);
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED  \n");
		return status;
	}
#endif
#else
	/* Set X-BAR windows for the training sequence */
	ddr3SaveAndSetTrainingWindows(auWinBackup);

#if defined(MV_NEW_TIP)
#ifndef MV_MSYS_BC2
	if( genericInitController == 0){
		ddr3TipInitSpecificRegConfig(0, ddr_modes[ddr3GetStaticDdrMode()].regs);
	}
#endif
	/*Load topology for New Training IP*/
	status = ddr3LoadTopologyMap();
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence topology load - FAILED\n");
		return status;
	}

	/*Set log level for training lib*/
	ddr3HwsSetLogLevel(MV_DEBUG_BLOCK_ALL, DEBUG_LEVEL_ERROR);



	/*Start New Training IP*/
	status = ddr3HwsHwTraining();
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED\n");
		return status;
	}
#else
	/* Run DDR3 Training Sequence */
	/* DRAM Init */
	MV_REG_WRITE(REG_SDRAM_INIT_CTRL_ADDR, 0x1);
	do
		uiReg = ((MV_REG_READ(REG_SDRAM_INIT_CTRL_ADDR)) & (1 << REG_SDRAM_INIT_CTRL_OFFS));
	while (uiReg);              /* Wait for '0' */

#if defined (MV88F67XX)
	/* MRS Command - required for A370 - only one set of MR registers */
	ddr3MRSCommand(0, 0, ddr3GetCSNumFromReg(), ddr3GetCSEnaFromReg());
#endif

#ifndef MV_NEW_TIP
	/* ddr3 init using DDR3 HW training procedure */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - HW Training Procedure \n");
	status = ddr3HwTraining(uiTargetFreq, uiDdrWidth,
				MV_FALSE, uiScrubOffs, uiScrubSize, bDQSCLKAligned, DDR3_TRAINING_DEBUG, REG_DIMM_SKIP_WL);
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED  \n");
		return status;
	}
#endif
#endif
#endif

	/************************************************************************************/
	/* Stage 3 - Finish                                                                 */
	/************************************************************************************/
#if defined(MV88F78X60) || defined(MV88F66XX) || defined(MV88F672X)
	/* Disable ECC Ignore bit */
	uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & ~(1 << REG_SDRAM_CONFIG_IERR_OFFS));
	MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
#endif
#if !defined(STATIC_TRAINING)
	/* Restore and set windows */
	ddr3RestoreAndSetFinalWindows(auWinBackup);
#endif
	/* Update DRAM init indication in bootROM register */
	uiReg = MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR);
	MV_REG_WRITE(REG_BOOTROM_ROUTINE_ADDR, uiReg | (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS));

#if !defined(MV88F67XX) && !defined(MV_NEW_TIP)
#if defined(MV88F78X60)
	if (mvCtrlRevGet() == MV_78XX0_B0_REV) {
		uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
		if (uiEcc == 0)
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg | (1 << 19));
	}
#endif /* end defined(MV88F78X60) */
	MV_REG_WRITE(DLB_EVICTION_CONTROL_REG, 0x9);

	uiReg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);
	uiReg |= (DLB_ENABLE | DLB_WRITE_COALESING | DLB_AXI_PREFETCH_EN | DLB_MBUS_PREFETCH_EN | PreFetchNLnSzTr);
	MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, uiReg);
#endif /* end !defined(MV88F67XX) */
#if defined(MV_NEW_TIP)
	ddr3NewTipDlbConfig();
#endif
#ifdef STATIC_TRAINING
	DEBUG_INIT_S("DDR3 Training Sequence - Ended Successfully (S) \n");
#else
	DEBUG_INIT_S("DDR3 Training Sequence - Ended Successfully \n");
#endif

#if defined(MV88F68XX)
	if( MV_TRUE == ddr3IfEccEnabled()){
		ddr3NewTipEccScrub();
	}
#endif

	return MV_OK;
}

/******************************************************************************
 * Name:     ddr3GetCpuFreq
 * Desc:     read S@R and return CPU frequency
 * Args:
 * Notes:
 * Returns:  required value
 */

MV_U32 ddr3GetCpuFreq(void)
{
#if !defined(MV_NEW_TIP)
	MV_U32 uiReg;
#endif
	MV_U32 uiCpuFreq;

#if defined(MV_NEW_TIP)
	uiCpuFreq = ddr3TipGetInitFreq();
#else
#if defined(MV88F66XX) || defined(MV88F672X)
	/* Read sample at reset setting */
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR); /* 0xE8200 */
	uiCpuFreq = ((uiReg & REG_SAMPLE_RESET_CPU_FREQ_MASK) >> REG_SAMPLE_RESET_CPU_FREQ_OFFS);
#else
	/* Read sample at reset setting */
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR); /* 0x18230 [23:21] */
#if defined(MV88F78X60)
	uiCpuFreq = ((uiReg & REG_SAMPLE_RESET_CPU_FREQ_MASK) >> REG_SAMPLE_RESET_CPU_FREQ_OFFS);
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR); /* 0x18234 [20] */
	uiCpuFreq |= (((uiReg >> REG_SAMPLE_RESET_HIGH_CPU_FREQ_OFFS) & 0x1) << 3);
#elif defined(MV88F67XX)
	uiCpuFreq = ((uiReg & REG_SAMPLE_RESET_CPU_FREQ_MASK) >> REG_SAMPLE_RESET_CPU_FREQ_OFFS);
#endif
#endif
#endif

	return uiCpuFreq;
}

/******************************************************************************
 * Name:     ddr3GetFabOpt
 * Desc:     read S@R and return CPU frequency
 * Args:
 * Notes:
 * Returns:  required value
 */

MV_U32 ddr3GetFabOpt(void)
{
#ifndef MV88F66XX
#ifndef MV88F672X
#ifndef MV_NEW_TIP
	MV_U32 uiReg, uiFabOpt;
#endif
#endif
#endif

#if  defined(MV88F66XX) || defined(MV88F672X) || defined(MV_NEW_TIP)
	return 0; /* No fabric */
#else
	/* Read sample at reset setting */
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR);
	uiFabOpt = ((uiReg & REG_SAMPLE_RESET_FAB_MASK) >> REG_SAMPLE_RESET_FAB_OFFS);

#if defined(MV88F78X60)
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR);
	uiFabOpt |= (((uiReg >> 19) & 0x1) << 4);
#endif

#if defined(MV_NEW_TIP)
	return 0;
#endif

	return uiFabOpt;
#endif
}

/******************************************************************************
 * Name:     ddr3GetVCOFreq
 * Desc:     read S@R and return VCO frequency
 * Args:
 * Notes:
 * Returns:  required value
 */

MV_U32 ddr3GetVCOFreq(void)
{
	MV_U32 uiFab, uiCpuFreq, uiVCOFreq;

	uiFab = ddr3GetFabOpt();
	uiCpuFreq = ddr3GetCpuFreq();

	if (uiFab == 2 || uiFab == 3 || uiFab == 7 || uiFab == 8 || uiFab == 10 || uiFab == 15 ||
	    uiFab == 17 || uiFab == 20)
		uiVCOFreq = uiCpuFreq + CLK_CPU;
	else
		uiVCOFreq = uiCpuFreq;

	return uiVCOFreq;
}

#if defined(MV88F67XX)
/************************************************************************************
 * Name:     ddr3MRSCommand - Set MR register values to DRAM devices
 * Desc:
 * Args:     uiMR1Value - MR1 value - NULL for read-modify-write
 *           uiMR2Value - MR2 value - NULL for read-modify-write
 *           uiCsNum - number of CS
 *           uiCsEna - CS configuration
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3MRSCommand(MV_U32 uiMR1Value, MV_U32 uiMR2Value, MV_U32 uiCsNum, MV_U32 uiCsEna)
{
	MV_U32 uiReg, uiCs;

	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			if (!uiMR1Value)
				uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR) & REG_DDR3_MR1_ODT_MASK);
			else
				uiReg = (uiMR1Value & REG_DDR3_MR1_ODT_MASK);

			uiReg |= auiODTStatic[uiCsEna][uiCs];
			if (uiCsNum > 1)
				uiReg |= BIT1;/*DIC enable*/

			MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg); /* 0x15D0 - DDR3 MR0 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg); /* 0x1418 - SDRAM Operation Register */

			uDelay(MRS_DELAY);

			if (!uiMR2Value)
				uiReg = (MV_REG_READ(REG_DDR3_MR2_ADDR) & REG_DDR3_MR2_ODT_MASK);
			else
				uiReg = (uiMR2Value & REG_DDR3_MR2_ODT_MASK);

			uiReg |= auiODTDynamic[uiCsEna][uiCs];

			MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg); /* 0x15D0 - DDR3 MR0 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR2 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR2 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg); /* 0x1418 - SDRAM Operation Register */

			uDelay(MRS_DELAY);
		}
	}
}

#endif
#ifdef STATIC_TRAINING
/************************************************************************************
 * Name:     ddr3StaticTrainingInit - Init DDR3 Training with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3StaticTrainingInit()
{
	MV_U32 uiReg, ddrMode;
	int j;

	ddrMode = ddr3GetStaticDdrMode();

	j = 0;
	while (ddr_modes[ddrMode].vals[j].reg_addr != 0) {
		mvOsDelay(10); //uDelay(1000); /* haim want to delay each write */
		MV_REG_WRITE(ddr_modes[ddrMode].vals[j].reg_addr, ddr_modes[ddrMode].vals[j].reg_value);
		if (ddr_modes[ddrMode].vals[j].reg_addr == REG_PHY_REGISTRY_FILE_ACCESS_ADDR)
			do
				uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) &
					 REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
			while (uiReg);
		j++;
	}
}
#endif
/************************************************************************************
 * Name:     ddr3GetStaticMCValue - Init Memory controller with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3GetStaticMCValue(MV_U32 regAddr, MV_U32 offset1, MV_U32 mask1, MV_U32 offset2, MV_U32 mask2)
{
	MV_U32 uiReg, uiTemp;

	uiReg = MV_REG_READ(regAddr);

	uiTemp = (uiReg >> offset1) & mask1;
	if (mask2)
		uiTemp |= (uiReg >> offset2) & mask2;

	return uiTemp;
}

/************************************************************************************
 * Name:     ddr3GetStaticDdrMode - Init Memory controller with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3GetStaticDdrMode(void)
{

	MV_U32 chipBoardRev, i;
	MV_U32 size;

/* Do not modify this code. relevant only for marvell Boards */
#if defined (MV88F78X60_Z1)
	chipBoardRev = Z1;
#elif defined(DB_78X60_PCAC)
	chipBoardRev = Z1_PCAC;
#elif defined(DB_78X60_AMC)
	chipBoardRev = A0_AMC;
#elif defined (DB_88F6710_PCAC)
	chipBoardRev = A0_PCAC;
#elif defined (RD_88F6710)
	chipBoardRev = A0_RD;
#elif defined (MV88F66XX) || defined(MV88F672X)
	chipBoardRev = mvBoardIdGet();
#elif defined (MV_NEW_TIP)
/*Valid only for A380 only, MSYS using dynamic controller config*/
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	chipBoardRev = mvBoardIdGet();	/* Customer boards select DDR mode according to board ID & Sample@Reset*/
#else
	chipBoardRev = MARVELL_BOARD;	/* Marvell boards select DDR mode according to Sample@Reset only */
#endif /* MV_NEW_TIP */
#else
	chipBoardRev = A0;
#endif

	size = sizeof(ddr_modes) / sizeof(MV_DRAM_MODES);
	for (i = 0; i < size; i++) {
		if ((ddr3GetCpuFreq() == ddr_modes[i].cpuFreq) && (ddr3GetFabOpt() == ddr_modes[i].fabFreq) &&
		    (chipBoardRev == ddr_modes[i].chipBoardRev))
			return i;
	}

	DEBUG_INIT_S("\n*** Error: ddr3GetStaticDdrMode: No match for requested DDR mode. ***\n\n");
	return 0;
}

#ifdef DUNIT_STATIC
/************************************************************************************
 * Name:     ddr3StaticMCInit - Init Memory controller with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_VOID ddr3StaticMCInit(void)
{
	MV_U32 ddrMode, uiReg;
	int j;

	ddrMode = ddr3GetStaticDdrMode();
	j = 0;
	while (ddr_modes[ddrMode].regs[j].reg_addr != 0) {
		MV_REG_WRITE(ddr_modes[ddrMode].regs[j].reg_addr, ddr_modes[ddrMode].regs[j].reg_value);
		if (ddr_modes[ddrMode].regs[j].reg_addr == REG_PHY_REGISTRY_FILE_ACCESS_ADDR)
			do
				uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) &
						REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
			while (uiReg);
		j++;
	}
}

#endif

/************************************************************************************
 * Name:     ddr3CheckConfig - Check user configurations: ECC/MultiCS
 * Desc:
 * Args:     twsi Address
 * Notes:    Only Available for ArmadaXP/Armada 370 DB boards
 * Returns:  None.
 */
MV_BOOL ddr3CheckConfig(MV_U32 twsiAddr, MV_CONFIG_TYPE configType)
{
#ifdef AUTO_DETECTION_SUPPORT
	MV_U8 ucData = 0;
	MV_TWSI_SLAVE twsiSlave;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_FALSE;
	twsiSlave.slaveAddr.address = twsiAddr;
	if ((configType == CONFIG_ECC) || (configType == CONFIG_BUS_WIDTH))
		twsiSlave.offset = 1;
	else
		twsiSlave.offset = 0;

	if (MV_OK == mvTwsiRead(0, &twsiSlave, &ucData, 1)) {
		switch (configType) {
		case CONFIG_ECC:
			if (ucData & 0x2)
				return MV_TRUE;
			break;
		case CONFIG_BUS_WIDTH:
			if (ucData & 0x1)
				return MV_TRUE;
			break;
#ifdef DB_88F6710
		case CONFIG_MULTI_CS:
			if (CFG_MULTI_CS_MODE(ucData))
				return MV_TRUE;
			break;
#else
		case CONFIG_MULTI_CS:
			break;
#endif
		}
	}
#endif
	return MV_FALSE;
}

#if defined(DB_88F78X60_REV2)
/************************************************************************************
 * Name:     ddr3GetEpromFabric - Get Fabric configuration from EPROM
 * Desc:
 * Args:     twsi Address
 * Notes:    Only Available for ArmadaXP DB Rev2 boards
 * Returns:  None.
 */
MV_U8 ddr3GetEpromFabric(void)
{
#ifdef AUTO_DETECTION_SUPPORT
	MV_U8 ucData = 0;
	MV_TWSI_SLAVE twsiSlave;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_FALSE;
	twsiSlave.slaveAddr.address = NEW_FABRIC_TWSI_ADDR;
	twsiSlave.offset = 1;

	if (MV_OK == mvTwsiRead(0, &twsiSlave, &ucData, 1))
		return ucData & 0x1F;
#endif
	return MV_FALSE;
}

#endif

/******************************************************************************
 * Name:     ddr3CLtoValidCL - this return register matching CL value
 * Desc:
 * Args:     uiCLValue - the value

 * Notes:
 * Returns:  required CL value
 */
MV_U32 ddr3CLtoValidCL(MV_U32 uiCL)
{
	switch (uiCL) {
	case 5:
		return 2;
		break;
	case 6:
		return 4;
		break;
	case 7:
		return 6;
		break;
	case 8:
		return 8;
		break;
	case 9:
		return 10;
		break;
	case 10:
		return 12;
		break;
	case 11:
		return 14;
		break;
	case 12:
		return 1;
		break;
	case 13:
		return 3;
		break;
	case 14:
		return 5;
		break;
	default:
		return 2;
	}
}

/******************************************************************************
 * Name:     ddr3CLtoValidCL - this return register matching CL value
 * Desc:
 * Args:     uiCLValue - the value

 * Notes:
 * Returns:  required CL value
 */
MV_U32 ddr3ValidCLtoCL(MV_U32 uiValidCL)
{
	switch (uiValidCL) {
	case 1:
		return 12;
		break;
	case 2:
		return 5;
		break;
	case 3:
		return 13;
		break;
	case 4:
		return 6;
		break;
	case 5:
		return 14;
		break;
	case 6:
		return 7;
		break;
	case 8:
		return 8;
		break;
	case 10:
		return 9;
		break;
	case 12:
		return 10;
		break;
	case 14:
		return 11;
		break;
	default:
		return 0;
	}
}

/******************************************************************************
 * Name:     ddr3GetCSNumFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSNumFromReg(void)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiCsCount = 0;
	MV_U32 uiCs;

	for (uiCs = 0; uiCs < MAX_CS; uiCs++)
		if (uiCsEna & (1 << uiCs))
			uiCsCount++;

	return uiCsCount;
}

/******************************************************************************
 * Name:     ddr3GetCSEnaFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSEnaFromReg(void)
{
	return MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & REG_DDR3_RANK_CTRL_CS_ENA_MASK;
}

/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
#if !defined(MV88F66XX) && !defined(MV88F672X)
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U8 revNum;

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID, 0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	revNum = (MV_U8)MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PCI_CLASS_CODE_AND_REVISION_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return (revNum & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS;
}

#endif

MV_VOID     printDunitSetup(MV_VOID)
{
	MV_32 uiReg;

#define DEBUG_DUNIT_REG(reg)    uiReg = MV_REG_READ(reg); putstring("0x"); putdata(reg, 8); putstring(" = 0x"); putdata(uiReg, 8); putstring("\n")

	putstring("\n########### LOG LEVEL 1 (D-UNIT SETUP)###########\n");

#ifdef DUNIT_STATIC
	putstring("\nStatic D-UNIT Setup:\n");
#endif
#ifdef DUNIT_SPD
	putstring("\nDynamic(using SPD) D-UNIT Setup:\n");
#endif
	DEBUG_DUNIT_REG(REG_SDRAM_CONFIG_ADDR);
	DEBUG_DUNIT_REG(REG_DUNIT_CTRL_LOW_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_TIMING_LOW_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_TIMING_HIGH_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_ADDRESS_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_OPEN_PAGES_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_OPERATION_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_MODE_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_EXT_MODE_ADDR);
	DEBUG_DUNIT_REG(REG_DDR_CONT_HIGH_ADDR);
	DEBUG_DUNIT_REG(REG_ODT_TIME_LOW_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_ERROR_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_AUTO_PWR_SAVE_ADDR);
	DEBUG_DUNIT_REG(REG_OUDDR3_TIMING_ADDR);
	DEBUG_DUNIT_REG(REG_ODT_TIME_HIGH_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_ODT_CTRL_LOW_ADDR);
	DEBUG_DUNIT_REG(REG_SDRAM_ODT_CTRL_HIGH_ADDR);
	DEBUG_DUNIT_REG(REG_DUNIT_ODT_CTRL_ADDR);
#ifndef MV88F67XX
	DEBUG_DUNIT_REG(REG_DRAM_FIFO_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_AXI_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_ADDR_CTRL_DRIVE_STRENGTH_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_DATA_DQS_DRIVE_STRENGTH_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_VER_CAL_MACHINE_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_MAIN_PADS_CAL_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_HOR_CAL_MACHINE_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_CS_SIZE_SCRATCH_ADDR);
	DEBUG_DUNIT_REG(REG_DYNAMIC_POWER_SAVE_ADDR);
	DEBUG_DUNIT_REG(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
	DEBUG_DUNIT_REG(REG_READ_DATA_READY_DELAYS_ADDR);
	DEBUG_DUNIT_REG(REG_DDR3_MR0_ADDR);
	DEBUG_DUNIT_REG(REG_DDR3_MR1_ADDR);
	DEBUG_DUNIT_REG(REG_DDR3_MR2_ADDR);
	DEBUG_DUNIT_REG(REG_DDR3_MR3_ADDR);
	DEBUG_DUNIT_REG(REG_DDR3_RANK_CTRL_ADDR);
	DEBUG_DUNIT_REG(REG_DRAM_PHY_CONFIG_ADDR);
	DEBUG_DUNIT_REG(REG_STATIC_DRAM_DLB_CONTROL);
	DEBUG_DUNIT_REG(DLB_BUS_OPTIMIZATION_WEIGHTS_REG);
	DEBUG_DUNIT_REG(DLB_AGING_REGISTER);
	DEBUG_DUNIT_REG(DLB_EVICTION_CONTROL_REG);
	DEBUG_DUNIT_REG(DLB_EVICTION_TIMERS_REGISTER_REG);
#if defined(MV88F66XX)
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_CTRL_ADDR(0));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_BASE_ADDR(0));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_CTRL_ADDR(1));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_BASE_ADDR(1));
#elif defined(MV88F672X) || defined(MV_NEW_TIP)
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_CTRL_ADDR(0));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_BASE_ADDR(0));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_CTRL_ADDR(1));
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_BASE_ADDR(1));
#else
	DEBUG_DUNIT_REG(REG_FASTPATH_WIN_0_CTRL_ADDR);
#endif
	DEBUG_DUNIT_REG(REG_CDI_CONFIG_ADDR);
#endif
}

#if defined(MV88F66XX)  || defined(MV88F672X)
MV_VOID getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs)
{
	MV_U32 tmp = DDR_100, hclk = 84;
	switch(uiFreqMode)
	{
		case CPU_333MHz_DDR_167MHz_L2_167MHz:
			hclk = 84;
			tmp = DDR_100;
			break;
		case CPU_266MHz_DDR_266MHz_L2_133MHz:
		case CPU_333MHz_DDR_222MHz_L2_167MHz:
		case CPU_400MHz_DDR_200MHz_L2_200MHz:
		case CPU_400MHz_DDR_267MHz_L2_200MHz:
		case CPU_533MHz_DDR_267MHz_L2_267MHz:
		case CPU_500MHz_DDR_250MHz_L2_250MHz:
		case CPU_600MHz_DDR_300MHz_L2_300MHz:
		case CPU_800MHz_DDR_267MHz_L2_400MHz:
		case CPU_900MHz_DDR_300MHz_L2_450MHz:
			tmp = DDR_300;
			hclk = 150;
			break;
		case CPU_333MHz_DDR_333MHz_L2_167MHz:
		case CPU_500MHz_DDR_334MHz_L2_250MHz:
		case CPU_666MHz_DDR_333MHz_L2_333MHz:
			tmp = DDR_333;
			hclk = 165;
			break;
		case CPU_533MHz_DDR_356MHz_L2_267MHz:
			tmp = DDR_360;
			hclk = 180;
			break;
		case CPU_400MHz_DDR_400MHz_L2_200MHz:
		case CPU_600MHz_DDR_400MHz_L2_300MHz:
		case CPU_800MHz_DDR_400MHz_L2_400MHz:
		case CPU_400MHz_DDR_400MHz_L2_400MHz:
			tmp = DDR_400;
			hclk = 200;
			break;
		case CPU_666MHz_DDR_444MHz_L2_333MHz:
		case CPU_900MHz_DDR_450MHz_L2_450MHz:
			tmp = DDR_444;
			hclk = 222;
			break;
		case CPU_500MHz_DDR_500MHz_L2_250MHz:
		case CPU_1000MHz_DDR_500MHz_L2_500MHz:
		case CPU_1000MHz_DDR_500MHz_L2_333MHz:
			tmp = DDR_500;
			hclk = 250;
			break;
		case CPU_533MHz_DDR_533MHz_L2_267MHz:
		case CPU_800MHz_DDR_534MHz_L2_400MHz:
		case CPU_1100MHz_DDR_550MHz_L2_550MHz:
			tmp = DDR_533;
			hclk = 267;
			break;
		case CPU_600MHz_DDR_600MHz_L2_300MHz:
		case CPU_900MHz_DDR_600MHz_L2_450MHz:
		case CPU_1200MHz_DDR_600MHz_L2_600MHz:
			tmp = DDR_600;
			hclk = 300;
			break;
		case CPU_666MHz_DDR_666MHz_L2_333MHz:
		case CPU_1000MHz_DDR_667MHz_L2_500MHz:
			tmp = DDR_666;
			hclk = 333;
			break;
		default:
		*ddrFreq = 0;
		*hclkPs = 0;
		break;
	}
	*ddrFreq = tmp; /* DDR freq define */
	*hclkPs = 1000000/hclk; /* values are 1/HCLK in ps */
	return;
}
#endif

#if defined(MV_NEW_TIP)
/*******************************************************************************/
MV_STATUS ddr3LoadTopologyMap(void)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap;
	MV_U8 	devNum = 0;
	MV_U32 boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());

	/*Get topology data by board ID*/
	if (sizeof(TopologyMap)/sizeof(MV_HWS_TOPOLOGY_MAP*) > boardIdIndex)
		topologyMap = &TopologyMap[boardIdIndex];
	else {
		DEBUG_INIT_FULL_S("Failed loading DDR3 Topology map (invalid board ID)\n");
		return MV_FAIL;
	}

	/*Set topology data for internal DDR training usage*/
	if(MV_OK != mvHwsDdr3TipLoadTopologyMap(devNum, topologyMap))
		return MV_FAIL;

	return MV_OK;
}

MV_VOID getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs)
{
	MV_U32 tmp, hclk = 200;

	switch(uiFreqMode)
	{
		case 4:
			tmp = 1; //DDR_400;
			hclk = 200;
			break;
		case 0x8:
			tmp = 1; //DDR_666;
			hclk = 333;
			break;
		case 0xc:
			tmp = 1; //DDR_800;
			hclk = 400;
			break;
		default:
			*ddrFreq = 0;
			*hclkPs = 0;
			break;
	}
	*ddrFreq = tmp; /* DDR freq define */
	*hclkPs = 1000000/hclk; /* values are 1/HCLK in ps */
		return;
}

MV_VOID ddr3NewTipDlbConfig()
{
	MV_U32 uiReg;

	MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, 0x2000005C);
	MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x0008C19E);
	MV_REG_WRITE(DLB_AGING_REGISTER, 0x0F7F007F);
	MV_REG_WRITE(DLB_EVICTION_CONTROL_REG, 0x00000209);
	MV_REG_WRITE(DLB_EVICTION_TIMERS_REGISTER_REG, 0x00FF0000);
	MV_REG_WRITE(DLB_USER_COMMAND_REG, 0x0);

	MV_REG_WRITE(MBUS_UNITS_PRIORITY_CONTROL_REG, 0x55555555);
	MV_REG_WRITE(FABRIC_UNITS_PRIORITY_CONTROL_REG, 0xA);
	MV_REG_WRITE(MBUS_UNITS_PREFETCH_CONTROL_REG, 0xffff);
	MV_REG_WRITE(FABRIC_UNITS_PREFETCH_CONTROL_REG, 0xf0f);
	/*Enable DLB*/
	uiReg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);
	uiReg |= (DLB_ENABLE | DLB_WRITE_COALESING | DLB_AXI_PREFETCH_EN | DLB_MBUS_PREFETCH_EN | PreFetchNLnSzTr);
	MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, uiReg);
}
#endif

#if defined(MV_NEW_TIP) || defined(MV88F66XX) || defined(MV88F672X)
#ifdef CONFIG_ALP_A375_ZX_REV
MV_VOID ddr3FastPathStaticCsSizeConfig(MV_U32 uiCsEna) {

	MV_U32 uiReg, uiCs;
	/* Set L2 filtering to 1G */
	MV_REG_WRITE(0x8c04, 0x40000000);

    /* Open fast path windows */
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
		/* set fast path window control for the cs */
			uiReg = 0x1FFFFFE1;
			uiReg |= (uiCs << 2);
			uiReg |= (SDRAM_CS_SIZE & 0xFFFF0000);
			MV_REG_WRITE(REG_FASTPATH_WIN_CTRL_ADDR(uiCs), uiReg); /*Open fast path Window */
			/* set fast path window base address for the cs */
			uiReg = (((SDRAM_CS_SIZE + 1) * uiCs) & 0xFFFF0000);
			MV_REG_WRITE(REG_FASTPATH_WIN_BASE_ADDR(uiCs), uiReg); /*Set base address */
		}
	}
}
#else
MV_VOID ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna) {

	MV_U32 uiReg, uiCs;
    MV_U32 uiMemTotalSize = 0;
    MV_U32 uiCsMemSize = 0;
    /* Open fast path windows */
    for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
        if (uiCsEna & (1 << uiCs)) {
            /* get CS size */

            if (ddr3CalcMemCsSize(uiCs, &uiCsMemSize) != MV_OK)
                return;

            /* set fast path window control for the cs */
            uiReg = 0xFFFFE1;
            uiReg |= (uiCs << 2);
            uiReg |= (uiCsMemSize - 1) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_CTRL_ADDR(uiCs), uiReg); /*Open fast path Window */
            /* set fast path window base address for the cs */
            uiReg = ((uiCsMemSize) * uiCs) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_BASE_ADDR(uiCs), uiReg); /*Set base address */
            uiMemTotalSize += uiCsMemSize;
#if defined(MV88F68XX)
			break;/*KW28 works with single CS*/
#endif
        }
    }
    /* Set L2 filtering to Max Memory size */
    MV_REG_WRITE(0x8c04, uiMemTotalSize);
}

MV_U32 ddr3GetBusWidth(void) {

    MV_U32 uiBusWidth;

    uiBusWidth = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & 0x8000) >> REG_SDRAM_CONFIG_WIDTH_OFFS;

    return (uiBusWidth == 0) ? 16 : 32;
}

MV_U32 ddr3GetDeviceWidth(MV_U32 uiCs) {

    MV_U32 uiDeviceWidth;
    uiDeviceWidth = (MV_REG_READ(REG_SDRAM_ADDRESS_CTRL_ADDR) & (0x3 << (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs))) >> (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs);

    return (uiDeviceWidth == 0) ? 8 : 16;
}

MV_FLOAT ddr3GetDeviceSize(MV_U32 uiCs) {

    MV_U32 uiDeviceSizeLow, uiDeviceSizeHigh, uiDeviceSize;
    MV_U32 uiRegData, uiCsLowOffset, uiCsHighOffset;

    uiCsLowOffset = REG_SDRAM_ADDRESS_SIZE_OFFS + uiCs * 4;
    uiCsHighOffset = REG_SDRAM_ADDRESS_SIZE_OFFS + REG_SDRAM_ADDRESS_SIZE_HIGH_OFFS + uiCs;

    uiRegData = MV_REG_READ(REG_SDRAM_ADDRESS_CTRL_ADDR);

    uiDeviceSizeLow = (uiRegData >> uiCsLowOffset) & 0x3;
    uiDeviceSizeHigh = (uiRegData >> uiCsHighOffset) & 0x1;

    uiDeviceSize = uiDeviceSizeLow | (uiDeviceSizeHigh << 2);

    switch(uiDeviceSize)
    {
        case 0:
            return 2;
        case 2:
            return 0.5;
        case 3:
            return 1;
        case 4:
            return 4;
        case 5:
            return 8;
        case 1:
        default:
            DEBUG_INIT_C("Error: Wrong device size of Cs: ", uiCs, 1);
            return 0.01; /* small value will give wrong emem size in ddr3CalcMemCsSize */
    }
}

MV_STATUS ddr3CalcMemCsSize(MV_U32 uiCs, MV_U32* puiCsSize){

    MV_FLOAT uiCsMemSize;

    uiCsMemSize = ((ddr3GetBusWidth() / ddr3GetDeviceWidth(uiCs)) * ddr3GetDeviceSize(uiCs)) / 8;/*calculate in Gbyte*/;

	if (uiCsMemSize == 0.125) {
        *puiCsSize = _128M;
    } else if (uiCsMemSize == 0.25) {
        *puiCsSize = _256M;
    } else if (uiCsMemSize == 0.5) {
        *puiCsSize = _512M;
    } else if (uiCsMemSize == 1) {
        *puiCsSize = _1G;
    } else if (uiCsMemSize == 2) {
        *puiCsSize = _2G;
    } else {
        DEBUG_INIT_C("Error: Wrong Memory size of Cs: ", uiCs, 1);
        return MV_BAD_VALUE;
    }
    return MV_OK;
}
#endif
#endif
#if defined(MV88F66XX)
MV_VOID ddr3GetAlpBusWidth(void){

	MV_U8 configVal[MV_IO_EXP_MAX_REGS];
	MV_U32 uiReg;

	/* if the board is DB-88F6660*/
	if (mvBoardIdGet() == DB_6660_ID) {
		mvBoardDb6660LaneConfigGet(configVal);
		if((configVal[2] & 0x4) >> 2){
			/* change sdram bus width to X16 according to DIP switch(SW7 pin3) or EEPROM*/
			uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
			uiReg &= ~(0x1 << REG_SDRAM_CONFIG_WIDTH_OFFS);
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
		}
	}
}
#endif

#if defined(MV88F672X)
MV_VOID ddr3GetA375BusWidth(void){

	MV_U8 configVal;
	MV_STATUS status;
	MV_U32 uiReg;

	/* DDR bus width configuration fields is located at:
	   S@R I2c = 0x4c , regNum = 1 , Bit0 */
	status = mvBoardTwsiGet(0x4C, 0x0, 0x1, &configVal);
	/* verify that all TWSI reads were successfully */
	if (MV_OK != status) {
		DEBUG_INIT_S("Error reading from TWSI\n");
		return;
	}
	/* check 1st bit for DDR bus width: 0x0 = 32 Bit, 0x1 = 16 Bit*/
	if((configVal & 0x1)){
		uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
		uiReg &= ~(0x1 << REG_SDRAM_CONFIG_WIDTH_OFFS);
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
	}
}
#endif
