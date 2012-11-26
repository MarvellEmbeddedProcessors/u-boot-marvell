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

#ifndef _INC_DDR3_TRAINING_H
#define _INC_DDR3_TRAINING_H

#include "config_marvell.h"  		/* Required to identify SOC and Board */
#include "mv_os.h"
#include "mvUart.h"

#ifdef MV88F78X60
#include "ddr3_axp.h"
#elif defined(MV88F67XX)
#include "ddr3_a370.h"
#endif

/***************************************************/
/* Debug (Enable/Disable modules) and Error report */
/***************************************************/
#define	MV_DEBUG_MAIN
/* #define BASIC_DEBUG */

#ifdef BASIC_DEBUG
#define	MV_DEBUG_WL
#define	MV_DEBUG_RL
#define	MV_DEBUG_DQS
#endif

#ifdef FULL_DEBUG
#define	MV_DEBUG_WL
#define	MV_DEBUG_RL
#define	MV_DEBUG_DQS
#define	MV_DEBUG_PBS
#define	MV_DEBUG_DFS
#define	MV_DEBUG_MAIN_FULL
#define	MV_DEBUG_DFS_FULL
#define	MV_DEBUG_DQS_FULL
#define	MV_DEBUG_RL_FULL
#define	MV_DEBUG_WL_FULL
#endif


/********************/
/* General Consts   */
/********************/

#define SDRAM_READ_WRITE_LEN_IN_WORDS 			16
#define SDRAM_READ_WRITE_LEN_IN_DOUBLE_WORDS	8
#define CACHE_LINE_SIZE							0x20

#define SDRAM_CS_SIZE							0xFFFFFFF
#define SDRAM_CS_BASE							0x0

#define SRAM_BASE								0x40000000
#define SRAM_SIZE								0xFFF

#define LEN_64BIT_STD_PATTERN					16
#define LEN_64BIT_KILLER_PATTERN				128
#define LEN_64BIT_SPECIAL_PATTERN				128
#define LEN_64BIT_PBS_PATTERN					16

#define LEN_16BIT_STD_PATTERN					4
#define LEN_16BIT_KILLER_PATTERN				128
#define LEN_16BIT_SPECIAL_PATTERN				128
#define LEN_16BIT_PBS_PATTERN					4

#define CMP_BYTE_SHIFT							8
#define CMP_BYTE_MASK							0xFF
#define PUP_SIZE								8

#define S 0
#define C 1
#define P 2
#define D 3
#define DQS 6
#define PS 2
#define DS 3
#define PE 4
#define DE 5

#define CS0										0
#define MAX_CS									4
#define MAX_DIMM_NUM							2
#define MAX_DELAY								0x1F

/* Invertion limit and phase1 limit are WA for the RL @ 1:1 design bug - Armada 370 & AXP Z1 */
#define MAX_DELAY_INV_LIMIT						0x5
#define MIN_DELAY_PHASE_1_LIMIT					0x10
/*#define MAX_DELAY_INV_LIMIT						0x0 */
/*#define MIN_DELAY_PHASE_1_LIMIT					0x0 */

#define MAX_DELAY_INV							(0x3F - MAX_DELAY_INV_LIMIT)
#define MIN_DELAY								0
#define MAX_PUP_NUM								9
#define DQ_NUM									8
#define INIT_WL_DELAY 							13
#define INIT_RL_DELAY 							15
#define TWLMRD_DELAY							20
#define TCLK_3_DELAY							3
#define ECC_BIT									8
#define DMA_SIZE								64
#define MV_DMA_0								0
#define MAX_TRAINING_RETRY						10

#define PUP_RL_MODE								0x2
#define PUP_WL_MODE								0
#define PUP_PBS_TX								0x10
#define PUP_PBS_RX								0x30
#define PUP_DQS_WR								0x1
#define PUP_DQS_RD								0x3
#define PUP_BC									10
#define PUP_DELAY_MASK							0x1F
#define PUP_PHASE_MASK							0x7
#define PUP_NUM_64BIT							8
#define PUP_NUM_32BIT							4
#define PUP_NUM_16BIT							2

/* WL */
#define COUNT_WL_HI_FREQ						3
#define COUNT_WL								2
#define COUNT_WL_RFRS							9
#define WL_HI_FREQ_SHIFT						2
#define WL_HI_FREQ_STATE 						1
#define COUNT_HW_WL								2

/* RL */
/* RL_MODE - this define uses the RL mode SW RL instead of the functional window SW RL */
#define RL_MODE
#define RL_WINDOW_WA
#define MAX_PHASE_1TO1							2
#define MAX_PHASE_2TO1							4

#define MAX_PHASE_RL_UL_1TO1					0
#define MAX_PHASE_RL_L_1TO1						4
/*#define MAX_PHASE_RL_UL_1TO1					1 */
/*#define MAX_PHASE_RL_L_1TO1					5 */
#define MAX_PHASE_RL_UL_2TO1					3
#define MAX_PHASE_RL_L_2TO1						7

#define RL_UNLOCK_STATE							0
#define RL_WINDOW_STATE							1
#define RL_FINAL_STATE							2
#define RL_RETRY_COUNT							2
#define COUNT_HW_RL								2


/* PBS */
#define MAX_PBS									31
#define MIN_PBS									0
#define COUNT_PBS_PATTERN						2
#define COUNT_PBS_STARTOVER						2
#define COUNT_PBS_REPEAT						3
#define COUNT_PBS_COMP_RETRY_NUM				2
#define PBS_DIFF_LIMIT							31
#define PATTERN_PBS_TX_A						0x55555555
#define PATTERN_PBS_TX_B						0xAAAAAAAA

/* DQS */
#define ADLL_ERROR								0x55
#define ADLL_MAX								31
#define ADLL_MIN								0
#define MIN_WIN_SIZE							4
#define VALID_WIN_THRS							MIN_WIN_SIZE

#define MODE_2TO1								1
#define MODE_1TO1								0

/***********/
/* Macros  */
/***********/
#define	IS_PUP_ACTIVE(_uiData_, _uiPup_)		(((_uiData_) >> (_uiPup_)) & 0x1)


/******************************/
/* DRAM information structure */
/******************************/
typedef struct _mvDramInfo {
	MV_U32  uiNumOfCS;
	MV_U32 	uiCsEna;
	MV_U32	uiNumOfStdPups;					/* Q value = ddrWidth/8 - Without ECC!! */
	MV_U32	uiNumOfTotalPups;				/* numOfStdPups + eccEna */
	MV_U32  uiTargetFrequency;				/* DDR Frequency */
	MV_U32	uiDdrWidth;						/* 32/64 Bit or 16/32 Bit */
	MV_U32 	uiEccEna;						/* 0/1 */
	MV_U32 	auiWlValues[MAX_CS][MAX_PUP_NUM][7];
	MV_U32 	auiRlValues[MAX_CS][MAX_PUP_NUM][7];
	MV_U32 	uiRdSmplDly;
	MV_U32 	uiRdRdyDly;
	MV_U32	uiCL;
	MV_U32	uiCWL;
	MV_U32	ui2TMode;
	MV_BOOL bRL400Bug;
	MV_BOOL bMultiCsMRSSupport;
	MV_BOOL bRegDimm;
	MV_BOOL bIsA0;
} MV_DRAM_INFO;

/**************************/
/* Function Declerations  */
/**************************/

MV_U32		cache_inv(MV_U32 uiAddr);
MV_VOID		flush_l1_v7(MV_U32 uiLine);
MV_VOID		flush_l1_v6(MV_U32 uiLine);

MV_VOID 	uDelay(MV_U32 uiDelay);

MV_U32 		ddr3CLtoValidCL(MV_U32 uiCL);
MV_U32 		ddr3ValidCLtoCL(MV_U32 uiValidCL);

MV_VOID 	ddr3WritePupReg(MV_U32 uiMode, MV_U32 uiCs, MV_U32 uiPup, MV_U32 uiPhase, MV_U32 uiDelay);
MV_U32	 	ddr3ReadPupReg(MV_U32 uiMode, MV_U32 uiCs, MV_U32 uiPup);

MV_STATUS 	ddr3SdramPbsCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiPupLocked, MV_BOOL bIsTx,
					MV_U32 uiPbsPatternIdx, MV_U32 uiPbsCurrVal, MV_U32 uiPbsLockVal,
	 				MV_U32 *auiSkewArray, MV_U8 *auiUnlockPupDqArray);

MV_STATUS 	ddr3SdramCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiUnlockPup, MV_U32 *puiNewLockedPup,
					MV_U32 *auiPattern, MV_U32 uiPatternLen, MV_U32 uiSdramOffset, MV_BOOL bWrite,
	 				MV_BOOL bMask, MV_U32 *auiMaskPattern, MV_BOOL bSpecialCompare);

MV_STATUS 	ddr3SdramDirectCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiUnlockPup, MV_U32 *puiNewLockedPup,
			MV_U32 *auiPattern, MV_U32 uiPatternLen, MV_U32 uiSdramOffset, MV_BOOL bWrite, MV_BOOL bMask,
			MV_U32 *auiMaskPattern);

MV_STATUS 	ddr3LoadPatterns(MV_DRAM_INFO *pDramInfo);

MV_STATUS 	ddr3ReadLevelingHw(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo);
MV_STATUS 	ddr3ReadLevelingSw(MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_DRAM_INFO *pDramInfo);

MV_STATUS 	ddr3WriteLevelingHw(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo);
MV_STATUS 	ddr3WriteLevelingSw(MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_DRAM_INFO *pDramInfo);
MV_STATUS	ddr3WriteHiFreqSup(MV_DRAM_INFO *pDramInfo);

MV_STATUS 	ddr3DfsHigh2Low(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo);
MV_STATUS 	ddr3DfsLow2High(MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_DRAM_INFO *pDramInfo);

MV_STATUS 	ddr3PbsTx(MV_DRAM_INFO *pDramInfo);
MV_STATUS 	ddr3PbsRx(MV_DRAM_INFO *pDramInfo);

MV_STATUS 	ddr3DqsCentralizationRx(MV_DRAM_INFO *pDramInfo);
MV_STATUS 	ddr3DqsCentralizationTx(MV_DRAM_INFO *pDramInfo);

MV_VOID 	ddr3StaticTrainingInit();

MV_U8 		ddr3GetEpromFabric(void);

#endif /* _INC_DDR3_TRAINING_H */
