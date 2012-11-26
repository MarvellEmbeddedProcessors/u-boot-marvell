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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ddr3_hw_training.h"
#include "mvXor.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */

#ifdef MV88F78X60
	#include "ddr3_patterns_64bit.h"
#else
	#include "ddr3_patterns_16bit.h"
#endif

/*
 * Debug
*/

#define DEBUG_MAIN_C(s, d, l)        DEBUG_MAIN_S(s); DEBUG_MAIN_D(d, l); DEBUG_MAIN_S("\n")
#define DEBUG_MAIN_FULL_C(s, d, l)   DEBUG_MAIN_FULL_S(s); DEBUG_MAIN_FULL_D(d, l); DEBUG_MAIN_FULL_S("\n")

#ifdef	MV_DEBUG_MAIN
#define DEBUG_MAIN_S(s)				putstring(s)
#define DEBUG_MAIN_D(d, l)			putdata(d, l)

#else
#define DEBUG_MAIN_S(s)
#define DEBUG_MAIN_D(d, l)
#endif

#ifdef	MV_DEBUG_MAIN_FULL
#define DEBUG_MAIN_FULL_S(s)		putstring(s)
#define DEBUG_MAIN_FULL_D(d, l)		putdata(d, l)
#else
#define DEBUG_MAIN_FULL_S(s)
#define DEBUG_MAIN_FULL_D(d, l)
#endif

MV_STATUS ddr3StartTimer(void);
MV_STATUS ddr3StopTimer(void);

MV_STATUS ddr3HwTraining(MV_U32 uiTargetFreq, MV_U32 uiEccEna,
		MV_U32 uiDdrWidth, MV_BOOL bXorBypass, MV_U32 uiScrubOffs, MV_U32 uiScrubSize,
  		MV_BOOL bDQSCLKAligned, MV_BOOL bRegDimm, MV_BOOL bIsA0, MV_BOOL bDebugMode) {

	MV_U32 uiFreq, uiReg;
	MV_U32 uiFirstLoopFlag = 0;
	MV_DRAM_INFO dramInfo;
	MV_BOOL b2to1ratio = FALSE;
	MV_BOOL bTempRatio = TRUE;

	if (bDebugMode)
		DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 1 \n");

	memset(&dramInfo, 0, sizeof(dramInfo));
	dramInfo.uiNumOfCS = ddr3GetCSNumFromReg();
	dramInfo.uiCsEna = ddr3GetCSEnaFromReg();
	dramInfo.uiTargetFrequency = uiTargetFreq;
	dramInfo.uiEccEna = uiEccEna;
	dramInfo.uiDdrWidth = uiDdrWidth;
	dramInfo.uiNumOfStdPups = uiDdrWidth/PUP_SIZE;
	dramInfo.uiNumOfTotalPups = uiDdrWidth/PUP_SIZE + uiEccEna;
	dramInfo.bRL400Bug = FALSE;
	dramInfo.bMultiCsMRSSupport = FALSE;
	dramInfo.bRegDimm = bRegDimm;
	dramInfo.bIsA0 = bIsA0;

#ifdef MV88F67XX
	dramInfo.bRL400Bug = TRUE;
#endif

	if (dramInfo.uiTargetFrequency > DDR_666)
		uiFirstLoopFlag = 1;

	uiFreq = dramInfo.uiTargetFrequency;
	
	/* Ignore ECC errors - if ECC is enabled */
	uiReg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);
	if (uiReg & (1 << REG_SDRAM_CONFIG_ECC_OFFS)) {
		uiReg |= (1 << REG_SDRAM_CONFIG_IERR_OFFS);
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);
	}

	/* Get target 2T value */
	uiReg = MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR);
	dramInfo.ui2TMode = (uiReg >> REG_DUNIT_CTRL_LOW_2T_OFFS) & REG_DUNIT_CTRL_LOW_2T_MASK;

	/* Get target CL value */
	if (bIsA0)
		uiReg = (MV_REG_READ(REG_DDR3_MR0_CS_ADDR) >> 2);
	else
		uiReg = (MV_REG_READ(REG_DDR3_MR0_ADDR) >> 2);

	uiReg = (((uiReg >> 1) & 0xE) | (uiReg & 0x1)) & 0xF;
	dramInfo.uiCL = ddr3ValidCLtoCL(uiReg);

	/* Get target CWL value */
	if (bIsA0)
		uiReg = (MV_REG_READ(REG_DDR3_MR2_CS_ADDR) >> REG_DDR3_MR2_CWL_OFFS);
	else
		uiReg = (MV_REG_READ(REG_DDR3_MR2_ADDR) >> REG_DDR3_MR2_CWL_OFFS);
	uiReg &= REG_DDR3_MR2_CWL_MASK;
	dramInfo.uiCWL = uiReg;

	/* Init XOR */
	mvSysXorInit(&dramInfo);

	/* Get DRAM/HCLK ratio */
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1 << REG_DDR_IO_CLK_RATIO_OFFS))
		b2to1ratio = TRUE;

	/* Xor Bypass - ECC support in AXP is currently available for 1:1 modes frequency modes.
	Not all frequency modes support the ddr3 training sequence (Only 1200/300).
	Xor Bypass allows using the Xor initializations and scrubbing inside the ddr3 training sequence without 
	running the training itself. */
	if (bXorBypass == MV_FALSE) {
		
		if (dramInfo.uiTargetFrequency > DFS_MARGIN) {
			uiFreq = DDR_100;
			bTempRatio = FALSE;
			if (MV_OK != ddr3DfsHigh2Low(uiFreq, &dramInfo)) {
				/* Set low - 100Mhz DDR Frequency by HW */
				DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Dfs High2Low) \n");
				return MV_FAIL;
			}
		if (bDebugMode)
			DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 2 \n");
		} else {
			if(!bDQSCLKAligned) {
				/* If running training sequence without DFS, we must run Write leveling before writing the patterns */
				/* ODT - Multi CS system use SW WL, Single CS System use HW WL */
				if (dramInfo.uiCsEna > 1 && !bIsA0) {

					if (MV_OK != ddr3WriteLevelingSw(uiFreq, bTempRatio, &dramInfo)) {
						DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Write Leveling Sw) \n");
						return MV_FAIL;
					}

				} else {
					if (MV_OK != ddr3WriteLevelingHw(uiFreq, &dramInfo)) {
						DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Write Leveling Hw) \n");
						return MV_FAIL;
					}
				}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 3 \n");
			}
		}
	
		if (MV_OK != ddr3LoadHWPatterns(&dramInfo)) {
			DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Loading Patterns) \n");
			return MV_FAIL;
		}
		
		do {
			uiFreq = dramInfo.uiTargetFrequency;
			bTempRatio = b2to1ratio;
			DEBUG_MAIN_FULL_S("DDR3 Training Sequence - DEBUG - 4 \n");
			/* There is a difference on the DFS frequency at the first iteration of this loop */
			if (uiFirstLoopFlag) {
				uiFreq = DDR_400;
				bTempRatio = FALSE;
			}
			
			uiFirstLoopFlag = 0;

			if (uiFreq > DFS_MARGIN) {
				if (MV_OK != ddr3DfsLow2High(uiFreq, bTempRatio, &dramInfo)) {
					DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Dfs Low2High) \n");
					return MV_FAIL;
				}
			}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 5 \n");

			/* Write leveling */
			if(!bDQSCLKAligned) {
				/* ODT - Multi CS system that not support Multi CS MRS commands must use SW WL */
				if ((dramInfo.uiCsEna > 1) && !bIsA0) {

					if (MV_OK != ddr3WriteLevelingSw(uiFreq, bTempRatio, &dramInfo)) {
						DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Write Leveling Sw) \n");
						return MV_FAIL;
					}
				} else {
					if (MV_OK != ddr3WriteLevelingHw(uiFreq, &dramInfo)) {
						DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Write Leveling Hw) \n");
						return MV_FAIL;
					}
				}
				if (bDebugMode)
					DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 6 \n");
			}
			
			/* Read Leveling */
			/* Armada 370 - Support for HCLK @ 400MHZ - must use SW read leveling */
			if (uiFreq == DDR_400 && dramInfo.bRL400Bug) {

				if (MV_OK != ddr3ReadLevelingSw(uiFreq, bTempRatio, &dramInfo)) {
					DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Read Leveling Sw) \n");
					return MV_FAIL;
				}

			} else {
				if (MV_OK != ddr3ReadLevelingHw(uiFreq, &dramInfo)) {
					DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Read Leveling Hw) \n");
					return MV_FAIL;
				}
			}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 7 \n");

			if (uiFreq > MARGIN_FREQ) {
				if (MV_OK != ddr3WriteHiFreqSup(&dramInfo)) {
					DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (Write Leveling Hi-Freq Sup) \n");
					return MV_FAIL;
				}
			}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 8 \n");

#ifdef MV88F78X60
			/* PBS is not yet integrated */
			if (dramInfo.uiTargetFrequency > DDR_666 && uiFreq <= MARGIN_FREQ &&  dramInfo.uiNumOfCS == 1) {
				if (MV_OK != ddr3PbsRx(&dramInfo)) {
					DEBUG_MAIN_S("ddr3PbsRx() Failed \n");
					return MV_FAIL;
				}
				if (bDebugMode)
					DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 9 \n");
				if (MV_OK != ddr3PbsTx(&dramInfo)) {
					DEBUG_MAIN_S("ddr3PbsTx() Failed \n");
					return MV_FAIL;
				}
				if (bDebugMode)
					DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 10 \n");
			}
#endif
		} while (uiFreq != dramInfo.uiTargetFrequency);

		if (dramInfo.uiTargetFrequency > MARGIN_FREQ) {
			if (MV_OK != ddr3DqsCentralizationRx(&dramInfo)) {
				DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (DQS Centralization RX) \n");
				return MV_FAIL;
			}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 11 \n");
			if (MV_OK != ddr3DqsCentralizationTx(&dramInfo)) {
					DEBUG_MAIN_S("DDR3 Training Sequence - FAILED (DQS Centralization TX) \n");
					return MV_FAIL;
			}
			if (bDebugMode)
				DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 12 \n");
		}
	}

	if (uiEccEna) {
		/* Need to SCRUB the DRAM memory area to load U-boot */
		mvSysXorFinish();
		dramInfo.uiNumOfCS = 1;
		dramInfo.uiCsEna = 1;
		mvSysXorInit(&dramInfo);
		mvXorMemInit(0, uiScrubOffs, uiScrubSize, 0xdeadbeef, 0xdeadbeef);
		/* wait for previous transfer completion */
		while (mvXorStateGet(0) != MV_IDLE);
		if (bDebugMode)
			DEBUG_MAIN_S("DDR3 Training Sequence - DEBUG - 13 \n");
	}

	/* Return XOR State */
	mvSysXorFinish();
		
	return MV_OK;
}

/*
 * Delay in 1 us
 */
MV_VOID uDelay(MV_U32 uiDelay)
{
	MV_U32 uiCycles;
	
	/* reset Timer configurations */
	MV_REG_WRITE(REG_TIMERS_CTRL_ADDR, 0);
	/* From TCLK calculate the Cycles needed for the requested us Delay */
	/* 600 - L2 frequency */
	uiCycles = 600 * uiDelay;
	/* Set the Timer value */
	MV_REG_WRITE(REG_TIMER0_VALUE_ADDR, uiCycles);
	/* Enable the Timer */
	MV_REG_BIT_SET(REG_TIMERS_CTRL_ADDR, REG_TIMER0_ENABLE_MASK);
	/* loop waiting for the timer to expire */
	while (MV_REG_READ(REG_TIMER0_VALUE_ADDR)) {}
}

/*
 * Perform DDR3 PUP Indirect Write
 */
 MV_VOID ddr3WritePupReg(MV_U32 uiMode, MV_U32 uiCs, MV_U32 uiPup, MV_U32 uiPhase, MV_U32 uiDelay)
{
	MV_U32 uiReg;
	uiReg = REG_PHY_REGISTRY_FILE_ACCESS_OP_WR;

	if (uiPup == PUP_BC)
		uiReg |= (1 << REG_PHY_BC_OFFS);
	else
		uiReg |= (uiPup << REG_PHY_PUP_OFFS);

	uiReg |= ((0x4*uiCs+uiMode) << REG_PHY_CS_OFFS);
	uiReg |= (uiPhase << REG_PHY_PHASE_OFFS) | uiDelay;

	if (uiMode == PUP_WL_MODE)
		uiReg |= ((INIT_WL_DELAY+uiDelay) << REG_PHY_DQS_REF_DLY_OFFS);

	MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg);  /* 0x16A0 */
	do {
		uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
	} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

	/* If read Leveling mode - need to write to register 3 separetly */
	if (uiMode == PUP_RL_MODE) {
		uiReg = REG_PHY_REGISTRY_FILE_ACCESS_OP_WR;

		if (uiPup == PUP_BC)
			uiReg |= (1 << REG_PHY_BC_OFFS);
		else
			uiReg |= (uiPup << REG_PHY_PUP_OFFS);

		uiReg |= ((0x4*uiCs+uiMode+1) << REG_PHY_CS_OFFS);
		uiReg |= (INIT_RL_DELAY);

		MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg);  /* 0x16A0 */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
		} while (uiReg);
	}
}

/*
 * Perform DDR3 PUP Indirect Read
 */
MV_U32 ddr3ReadPupReg(MV_U32 uiMode, MV_U32 uiCs, MV_U32 uiPup)
{
	MV_U32 uiReg;
	uiReg = REG_PHY_REGISTRY_FILE_ACCESS_OP_RD | (uiPup << REG_PHY_PUP_OFFS) | ((0x4*uiCs+uiMode) << REG_PHY_CS_OFFS);
	MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg);  /* 0x16A0 */

	do {
		uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
	} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

	return MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
}

/*
 * Set training patterns
 */
MV_STATUS ddr3LoadHWPatterns(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg;
	
	/* Set Base Addr */
	MV_REG_WRITE(REG_DRAM_TRAINING_PATTERN_BASE_ADDR, 0);

	/* Set Patterns */
	uiReg = (pDramInfo->uiCsEna << REG_DRAM_TRAINING_CS_OFFS) | (1<<REG_DRAM_TRAINING_PATTERNS_OFFS);
	uiReg |= (1 << REG_DRAM_TRAINING_AUTO_OFFS);

	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);

	uDelay(100);

	/* Check if Successful */
	if (MV_REG_READ(REG_DRAM_TRAINING_ADDR) & (1 << REG_DRAM_TRAINING_ERROR_OFFS))
		return MV_OK;
	else
		return MV_FAIL;
}


MV_STATUS ddr3StartTimer(void)
{
	MV_REG_WRITE(REG_TIMERS_EVENTS_ADDR, 0x0);				/* Timers Events Status register Register - Read expired */
	MV_REG_WRITE(REG_TIMER1_VALUE_ADDR, 0xFFFFFFFF);		/* Timer 1 Register - Set initial Value */
	MV_REG_WRITE(REG_TIMERS_CTRL_ADDR, 0x4 | (1 <<12));				/* Timers Control Register - Timer 1 enable */

	return MV_OK;
}

MV_STATUS ddr3StopTimer(void)
{
	MV_U32 uiReg;

	MV_REG_WRITE(REG_TIMERS_CTRL_ADDR, 0x0);				/* Timers Control Register - Timer 1 disable */
	uiReg = MV_REG_READ(REG_TIMER1_VALUE_ADDR);			/* Timer 1 Register - Read initial Value */
		
	DEBUG_MAIN_C("Stage took (uSec): ",(0xFFFFFFFF-uiReg)*0.04, 8);
	
	uiReg = MV_REG_READ(REG_TIMERS_EVENTS_ADDR);			/* Timers Events Status register Register - Read expired */
	if (uiReg & (1<<8))
		DEBUG_MAIN_S("Timer Expired \n");
	
	return MV_OK;
}

#ifdef MV88F78X60_A0
/************************************************************************************
* Name:		fixPLLValue - fix wrong defualt value of the PLL Wrapper
* Desc:	 	
* Args:	 	None.
* Notes:
* Returns:	None.
*/

#define wfi()	__asm__ __volatile__ ("wfi" : : : "memory")

MV_VOID fixPLLValue(MV_U8 targetFabric)
{
	MV_U32 uiReg;
	MV_U32 uiBackupReg[4];
	MV_U32 newFabric = 0;

	/* Fix the Wrong init value */
	uiReg = MV_REG_READ(REG_SFABRIC_CLK_CTRL_ADDR);
	MV_REG_WRITE(REG_SFABRIC_CLK_CTRL_ADDR, uiReg | (1 << REG_SFABRIC_CLK_CTRL_SMPL_OFFS));

	newFabric = (MV_U8)targetFabric;

	/* Set new Sample At Reset Value */
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR) & ~(1 << 19);
	uiReg |= (((newFabric & 0x10) >> 4) << 19);
	MV_REG_WRITE(REG_SAMPLE_RESET_HIGH_ADDR, uiReg);
	uiReg = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR) & ~(REG_SAMPLE_RESET_FAB_MASK);
	MV_REG_WRITE(REG_SAMPLE_RESET_LOW_ADDR, uiReg | ((newFabric & 0xF) << REG_SAMPLE_RESET_FAB_OFFS));

	MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_2_ADDR, s_auiFabricRatio[newFabric]);
	MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_3_ADDR, 0x02020201);

	
	uiReg = MV_REG_READ(REG_CPU_DIV_CLK_CTRL_4_ADDR) & ~(0xFF << 0);
	MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_4_ADDR, uiReg | 0x04);

	/* Set PLL Configuration for smooth clock change and disable the nb_reset so new value will not be lost */
	uiReg = MV_REG_READ(REG_CPU_DIV_CLK_CTRL_0_ADDR) & ~(0xFFFF << 8);
	/* Unset it just in case */
	uiReg &= ~(1 << 24);
	MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg | (0xFF << 16) | (0x14 << 8));

	/* PLL WA with PMU */
	/* Enable Coherency Fabric IRQ */
	uiReg = (MV_REG_READ(REG_FABRIC_LOCAL_IRQ_MASK_ADDR) | (1 << REG_FABRIC_LOCAL_IRQ_PMU_MASK_OFFS));
	MV_REG_WRITE(REG_FABRIC_LOCAL_IRQ_MASK_ADDR, uiReg);
	MV_REG_WRITE(0x20B0C, 0x91000001);
	
	uiBackupReg[0] = MV_REG_READ(0x1C2A4);
	MV_REG_WRITE(0x1C2A4, uiBackupReg[0] & 0xFF000000);

	uiBackupReg[1] = MV_REG_READ(0x1C2AC);
	MV_REG_WRITE(0x1C2AC, (uiBackupReg[1] & ~(0xFF)) | 0x64);

	uiBackupReg[2] = MV_REG_READ(0x1C2B0);
	MV_REG_WRITE(0x1C2B0, uiBackupReg[2] & ~(0xFF));

	uiReg = (MV_REG_READ(REG_PM_STAT_MASK_ADDR) | (1 << REG_PM_STAT_MASK_CPU0_IDLE_MASK_OFFS));
	MV_REG_WRITE(REG_PM_STAT_MASK_ADDR, uiReg);

	uiReg = (MV_REG_READ(REG_PM_EVENT_STAT_MASK_ADDR) | (1 << REG_PM_EVENT_STAT_MASK_DFS_DONE_OFFS));
	MV_REG_WRITE(REG_PM_EVENT_STAT_MASK_ADDR, uiReg);

	uiReg = (MV_REG_READ(REG_PM_CTRL_CONFIG_ADDR) | (1 << REG_PM_CTRL_CONFIG_DFS_REQ_OFFS));
	MV_REG_WRITE(REG_PM_CTRL_CONFIG_ADDR, uiReg);

	/* Need to make sure that CPU will not make any request from the fabric and will wait for reset */
	wfi();

	/* Disable Coherency Fabric IRQ */
	uiReg = (MV_REG_READ(REG_FABRIC_LOCAL_IRQ_MASK_ADDR) & ~(1 << REG_FABRIC_LOCAL_IRQ_PMU_MASK_OFFS));
	MV_REG_WRITE(REG_FABRIC_LOCAL_IRQ_MASK_ADDR, uiReg);
	MV_REG_WRITE(0x20B0C, 0x01000000);

	/* Restore original defaults */
	MV_REG_WRITE(0x1C2A4, uiBackupReg[0]);
	MV_REG_WRITE(0x1C2AC, uiBackupReg[1]);
	MV_REG_WRITE(0x1C2B0, uiBackupReg[2]);

	uiReg = (MV_REG_READ(REG_PM_STAT_MASK_ADDR) & ~(1 << REG_PM_STAT_MASK_CPU0_IDLE_MASK_OFFS));
	MV_REG_WRITE(REG_PM_STAT_MASK_ADDR, uiReg);

	uiReg = (MV_REG_READ(REG_PM_EVENT_STAT_MASK_ADDR) & ~(1 << REG_PM_EVENT_STAT_MASK_DFS_DONE_OFFS));
	MV_REG_WRITE(REG_PM_EVENT_STAT_MASK_ADDR, uiReg);

	uiReg = (MV_REG_READ(REG_PM_CTRL_CONFIG_ADDR) & ~(1 << REG_PM_CTRL_CONFIG_DFS_REQ_OFFS));
	MV_REG_WRITE(REG_PM_CTRL_CONFIG_ADDR, uiReg);

	uiReg = MV_REG_READ(REG_DDR_IO_ADDR);
	if ((s_auiFabricRatio[newFabric] & 0xFF) != ((s_auiFabricRatio[newFabric] >> 8) & 0xFF))
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg | (1 << REG_DDR_IO_CLK_RATIO_OFFS));
	else
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg & ~(1 << REG_DDR_IO_CLK_RATIO_OFFS));
	
	/* Unset for case PMU/SW will use it later on */
	uiReg = MV_REG_READ(REG_CPU_DIV_CLK_CTRL_0_ADDR) & ~(1 << 24);
	MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);
}
#endif