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

#if defined(MV88F66XX) || defined(MV88F672X) || defined(MV88F67XX) || defined(MV88F78X60)

#ifdef MV88F78X60
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "ddr3_init.h"
#include "ddr3_spd.h"

#include "ddr3_spd.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"

#if defined(MV88F78X60)
#include "ddr3_axp_vars.h"
#elif defined(MV88F67XX)
#include "ddr3_a370_vars.h"
#elif defined(MV88F66XX)
#include "ddr3_alp_vars.h"
#elif defined(MV88F672X)
#include "ddr3_a375_vars.h"
#endif

#include "bootstrap_os.h"
#if defined(MV88F78X60_Z1)
static MV_VOID ddr3MRSCommand(MV_U32 uiMR1Value, MV_U32 uiMR2Value, MV_U32 uiCsNum, MV_U32 uiCsEna);
#endif
#ifdef STATIC_TRAINING
static MV_VOID ddr3StaticTrainingInit(void);
#endif
#ifdef DUNIT_STATIC
static MV_VOID ddr3StaticMCInit(void);
#endif
#if defined(DUNIT_STATIC) || defined(STATIC_TRAINING)
static MV_U32 ddr3GetStaticDdrMode(void);
#endif
#if defined(MV88F66XX) || defined(MV88F672X)
MV_VOID getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs);
#endif
MV_U32 mvBoardIdGet(MV_VOID);

extern MV_VOID ddr3SetSwWlRlDebug(MV_U32);
extern MV_VOID ddr3SetPbs(MV_U32);
extern MV_VOID ddr3SetLogLevel(MV_U32 nLogLevel);
static MV_U32 gLogLevel = 0;

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

static MV_VOID ddr3RestoreAndSetFinalWindows(MV_U32 *auWinBackup)
{
	MV_U32 ui, uiReg, uiCs;
    MV_U32 winCtrlReg, numOfWinRegs;
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();

#if defined(MV88F66XX) || defined(MV88F672X)
    if(DDR3_FAST_PATH_EN == MV_FALSE)
    {
        return;
    }
#endif

#if defined(MV88F66XX) || defined(MV88F672X)
    winCtrlReg  = REG_XBAR_WIN_16_CTRL_ADDR;
    numOfWinRegs = 8;
#else
    winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
    numOfWinRegs = 16;
#endif

	/* Return XBAR windows 4-7 or 16-19 init configuration */
	for (ui = 0; ui < numOfWinRegs; ui++)
		MV_REG_WRITE((winCtrlReg + 0x4 * ui), auWinBackup[ui]);

	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Switching XBAR Window to FastPath Window \n");

#if defined(MV88F66XX) || defined(MV88F672X)
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
#else
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
	/*{0x000200e8}  -   Open Mbus Window - 2G */
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
            if(winRemapReg <= REG_XBAR_WIN_7_REMAP_ADDR) {
                MV_REG_WRITE(winRemapReg + winJumpIndex * uiTempCount, 0);
            }
			uiTempCount++;
		}
	}
}
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

	ddr3LogLevelInit();
	ddr3SetPbs(DDR3_PBS);
    ddr3SetSwWlRlDebug(DDR3_RUN_SW_WHEN_HW_FAIL);

	status = ddr3Init_();
/*	DEBUG_INIT_S("Status = ");*/
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

	if (status > MV_DDR3_TRAINING_ERR_HW_FAIL_BASE) {
        DEBUG_INIT_C("DDR3 Training Error: HW Failure 0x", status, 8);
    }

	return status;
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
	MV_U32 uiTargetFreq;

#ifndef RD_88F6710
#if defined(MV88F78X60) || defined(ECC_SUPPORT) || defined(DUNIT_SPD)
	MV_U32 uiEcc;
#endif
#endif
	MV_U32 uiReg = 0;
	MV_U32 uiCpuFreq, uiFabOpt, uiHClkTimePs, socNum, uiScrubOffs, uiScrubSize;
	MV_U32 auWinBackup[16];
	MV_BOOL bDQSCLKAligned = FALSE;
	MV_BOOL bPLLWAPatch = FALSE;
	MV_U32 uiDdrWidth = BUS_WIDTH;
	MV_STATUS status;

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

	ddr3PrintVersion();
	DEBUG_INIT_S("0\n");
	/* Lib version 5.4.0 */

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
#ifndef MV88F66XX
#ifndef MV88F672X
	uiReg = (MV_REG_READ(REG_DDRPHY_APLL_CTRL_ADDR) & ~(1 << 25)); /* 0x18780 [25]  */
	MV_REG_WRITE(REG_DDRPHY_APLL_CTRL_ADDR, uiReg);
#endif
#endif
	/************************************************************************************/
	/* Stage 0 - Set board configuration                                                */
	/************************************************************************************/
	uiCpuFreq = ddr3GetCpuFreq();
	if (uiFabOpt > FAB_OPT)
		uiFabOpt = FAB_OPT - 1;
    if (ddr3GetLogLevel() > 0) {
        printDrrTargetFreq(uiCpuFreq, uiFabOpt);
    }
#if defined(MV88F66XX)  || defined(MV88F672X)
    getTargetFreq(uiCpuFreq, &uiTargetFreq, &uiHClkTimePs);
#else
    uiTargetFreq = s_auiCpuDdrRatios[uiFabOpt][uiCpuFreq];
    uiHClkTimePs = s_auiCpuFabClkToHClk[uiFabOpt][uiCpuFreq];
#endif
	if ((uiTargetFreq == 0) || (uiHClkTimePs == 0)) {
		DEBUG_INIT_S("DDR3 Training Sequence - FAILED - Wrong Sample at Reset Configurations \n");
		return MV_DDR3_TRAINING_ERR_BAD_SAR;
	}

#if defined(ECC_SUPPORT)
	uiScrubOffs = U_BOOT_START_ADDR;
	uiScrubSize = U_BOOT_SCRUB_SIZE;
#else
	uiScrubOffs = 0;
	uiScrubSize = 0;
#endif
#ifndef RD_88F6710
#if defined(MV88F78X60) || defined(ECC_SUPPORT )  || defined(DUNIT_SPD)
	uiEcc = DRAM_ECC;
#endif
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

#ifdef DUNIT_STATIC
	/* For Static D-Unit Setup use must set the correct static values at the ddr3_*soc*_vars.h file */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Static MC Init \n");
	ddr3StaticMCInit();
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

#ifdef DUNIT_SPD
	status = ddr3DunitSetup(uiEcc, uiHClkTimePs, &uiDdrWidth);
	if (MV_OK != status) {
		DEBUG_INIT_S("DDR3 Training Sequence - FAILED (ddr3 Dunit Setup) \n");
		return status;
	}
#endif
#if defined(MV88F78X60)
	/* RL WA for B0 */
	if (mvCtrlRevGet() == MV_78XX0_B0_REV) {
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
	}
#endif
#if defined(MV88F66XX) || defined(MV88F672X)
	uiReg = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
	/* 0x15C8[2:0] - should be 0x4  // TrnDbgRdyIncPh0_2to1 = 4
	   0x15C8[5:3] - should be 0x4  // TrnDbgRdyIncPh1_2to1 = 4
	   0x15C8[8:6] - should be 0x4  // TrnDbgRdyIncPh2_2to1 = 4
	   0x15C8[11:9] - should be 0x4  // TrnDbgRdyIncPh3_2to1 = 4
	   0x15C8[14:12] - should be 0x6  // TrnDbgRdyIncPh4_2to1 = 6
	   0x15C8[17:15] - should be 0x6  // TrnDbgRdyIncPh5_2to1 = 6
	 */
	uiReg &= 0xFFFC0000;
	uiReg |= 0x36924;

	MV_REG_WRITE(REG_TRAINING_DEBUG_3_ADDR, uiReg);
#endif

	/* Set X-BAR windows for the training sequence */
    ddr3SaveAndSetTrainingWindows(auWinBackup);
	uiReg |= 0x36024;
	MV_REG_WRITE(REG_TRAINING_DEBUG_3_ADDR, uiReg);
	/* Memory interface initializations */
#if defined(MV88F66XX) || defined(MV88F672X)
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
#if !defined(MV88F67XX)
/* ARMADA-370 activate DLB later at the u-boot*/
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
	MV_REG_WRITE(FABRIC_UNITS_PRIORITY_CONTROL_REG, 0xAA);
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
	/* ddr3 init using static parameters - HW training is disabled */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Static Training Parameters \n");
	ddr3StaticTrainingInit();

	/* if ECC is enabled, need to scrub the U-Boot area memory region - Run training function with Xor bypass
	   just to scrub the memory */
	status = ddr3HwTraining(uiTargetFreq, uiDdrWidth,
				MV_TRUE, uiScrubOffs, uiScrubSize, bDQSCLKAligned, DDR3_TRAINING_DEBUG, REG_DIMM_SKIP_WL);
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED  \n");
		return status;
	}
#else
	/* Run DDR3 Training Sequence */
	/* DRAM Init */
	MV_REG_WRITE(REG_SDRAM_INIT_CTRL_ADDR, 0x1);
	do
		uiReg = ((MV_REG_READ(REG_SDRAM_INIT_CTRL_ADDR)) & (1 << REG_SDRAM_INIT_CTRL_OFFS));
	while (uiReg);              /* Wait for '0' */

#if defined(MV88F78X60_Z1)
	/* MRS Command - required for AXP Z1 devices and A370 - only one set of MR registers */
	ddr3MRSCommand(0, 0, ddr3GetCSNumFromReg(), ddr3GetCSEnaFromReg());
#endif
	/* ddr3 init using DDR3 HW training procedure */
	DEBUG_INIT_FULL_S("DDR3 Training Sequence - HW Training Procedure \n");
	status = ddr3HwTraining(uiTargetFreq, uiDdrWidth,
				MV_FALSE, uiScrubOffs, uiScrubSize, bDQSCLKAligned, DDR3_TRAINING_DEBUG, REG_DIMM_SKIP_WL);
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED  \n");
		return status;
	}
#endif

	/************************************************************************************/
	/* Stage 3 - Finish                                                                 */
	/************************************************************************************/
#if defined(MV88F78X60) || defined(MV88F66XX) || defined(MV88F672X)
	/* Disable ECC Ignore bit */
	uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & ~(1 << REG_SDRAM_CONFIG_IERR_OFFS));
	MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
#endif

	/* Restore and set windows */
	//yelena - enable this function after fat path is open
	ddr3RestoreAndSetFinalWindows(auWinBackup);

	/* Update DRAM init indication in bootROM register */
	uiReg = MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR);
	MV_REG_WRITE(REG_BOOTROM_ROUTINE_ADDR, uiReg | (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS));

#if !defined(MV88F67XX)
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

#ifdef STATIC_TRAINING
	DEBUG_INIT_S("DDR3 Training Sequence - Ended Successfully (S) \n");
#else
	DEBUG_INIT_S("DDR3 Training Sequence - Ended Successfully \n");
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
	MV_U32 uiReg, uiCpuFreq;

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
	MV_U32 uiReg, uiFabOpt;
#endif
#endif

#if  defined(MV88F66XX) || defined(MV88F672X)
	return 0; /* No fabric */
#else
	/* Read sample at reset setting */
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR);
	uiFabOpt = ((uiReg & REG_SAMPLE_RESET_FAB_MASK) >> REG_SAMPLE_RESET_FAB_OFFS);

#if defined(MV88F78X60)
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR);
	uiFabOpt |= (((uiReg >> 19) & 0x1) << 4);
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

#if defined(MV88F78X60_Z1)
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
	MV_U32 uiReg, chipId, ddrMode;
	int j;

#ifdef RD_88F78460_SERVER
	/* Set MPP as GPIO */
	reg = (MV_REG_READ(MPP_CONTROL_REG(2)) & 0xFFFF0000);
	MV_REG_WRITE(MPP_CONTROL_REG(2), reg);

	/* Set data out disable */
	reg = (MV_REG_READ(GPP_DATA_OUT_EN_REG(0)) | 0xF0000);
	MV_REG_WRITE(GPP_DATA_OUT_EN_REG(0), reg);
#if 0
	/* Set data in polarity */
	reg = (MV_REG_READ(GPP_DATA_IN_POL_REG(0)) | 0xF0000);
	MV_REG_WRITE(GPP_DATA_IN_POL_REG(0), reg);
#endif
	/* read chip ID */
	chipId = ((MV_REG_READ(GPP_DATA_IN_REG(0)) >> 16) & 0xF);
	chipBoardRev = Z1_RD_SLED;
#else
	chipId = 0x0;
#endif

	ddrMode = ddr3GetStaticDdrMode();

	j = 0;
	while (ddr_modes[ddrMode].vals[j].reg_addr != 0) {
        uDelay(1000); /* haim want to delay each write */
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
#else
	chipBoardRev = A0;
#endif

	size = sizeof(ddr_modes) / sizeof(MV_DRAM_MODES);
	for (i = 0; i < size; i++) {
		if ((ddr3GetCpuFreq() == ddr_modes[i].cpuFreq) && (ddr3GetFabOpt() == ddr_modes[i].fabFreq) &&
		    (chipBoardRev == ddr_modes[i].chipBoardRev))
			return i;
	}

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
	MV_U32 ddrMode;
	int j;

	ddrMode = ddr3GetStaticDdrMode();
	j = 0;
	while (ddr_modes[ddrMode].regs[j].reg_addr != 0) {
		MV_REG_WRITE(ddr_modes[ddrMode].regs[j].reg_addr, ddr_modes[ddrMode].regs[j].reg_value);
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
	#elif defined(MV88F672X)
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
    MV_U32 tmp, hclk;

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
#endif