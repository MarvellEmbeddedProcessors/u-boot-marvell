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
#include "ddr3_hw_training.h"


/************
 * Debug
*/
#define DEBUG_RL_C(s, d, l) 		DEBUG_RL_S(s); DEBUG_RL_D(d, l); DEBUG_RL_S("\n")
#define DEBUG_RL_FULL_C(s, d, l)	DEBUG_RL_FULL_S(s); DEBUG_RL_FULL_D(d, l); DEBUG_RL_FULL_S("\n")

#ifdef	MV_DEBUG_RL
#define DEBUG_RL_S(s)				putstring(s)
#define DEBUG_RL_D(d, l)			putdata(d, l)
#else
#define DEBUG_RL_S(s)
#define DEBUG_RL_D(d, l)
#endif

#ifdef	MV_DEBUG_RL_FULL
#define DEBUG_RL_FULL_S(s)			putstring(s)
#define DEBUG_RL_FULL_D(d, l)		putdata(d, l)
#else
#define DEBUG_RL_FULL_S(s)
#define DEBUG_RL_FULL_D(d, l)
#endif

extern MV_U32 g_auiRLPattern[LEN_STD_PATTERN];

#ifdef RL_MODE
static MV_STATUS ddr3ReadLevelingSingleCsRlMode(MV_U32 uiCs, MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_U32 uiEcc, MV_DRAM_INFO *pDramInfo);
#else
static MV_STATUS ddr3ReadLevelingSingleCsWindowMode(MV_U32 uiCs, MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_U32 uiEcc, MV_DRAM_INFO *pDramInfo);
#endif
/******************************************************************************
* Name:     ddr3ReadLevelingHw.
* Desc:     Execute the Read leveling uiPhase by HW
* Args:     pDramInfo	- main struct
*			uiFreq 		- current sequence uiFrequency
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

MV_STATUS ddr3ReadLevelingHw(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg;

	/* Debug message - Start Read leveling procedure */
	DEBUG_RL_S("DDR3 - Read Leveling - Starting HW RL procedure \n");

	/* Start Auto Read Leveling procedure */
	uiReg = (1 << REG_DRAM_TRAINING_RL_OFFS);
	/* Config the retest number */
	uiReg |= (COUNT_HW_RL << REG_DRAM_TRAINING_RETEST_OFFS);

	/* Enable CS in the automatic process */
	uiReg |= (pDramInfo->uiCsEna << REG_DRAM_TRAINING_CS_OFFS);

	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */

	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_SHADOW_ADDR) | (1 << REG_DRAM_TRAINING_AUTO_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_SHADOW_ADDR, uiReg);

	/* Wait */
	do {
		uiReg = (MV_REG_READ(REG_DRAM_TRAINING_SHADOW_ADDR) & (1<<REG_DRAM_TRAINING_AUTO_OFFS));
	} while (uiReg);				/* Wait for '0' */

	/* Check if Successful */
	if (MV_REG_READ(REG_DRAM_TRAINING_SHADOW_ADDR) & (1 << REG_DRAM_TRAINING_ERROR_OFFS)) {

		MV_U32 uiDelay, uiPhase, uiPup, uiCs;;
		/* Read results to arrays */
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {

				for (uiPup = 0; uiPup < pDramInfo->uiNumOfTotalPups; uiPup++) {
					uiReg = ddr3ReadPupReg(PUP_RL_MODE, uiCs, uiPup);
					uiPhase = (uiReg >> REG_PHY_PHASE_OFFS) & PUP_PHASE_MASK;
					uiDelay = uiReg & PUP_DELAY_MASK;
					pDramInfo->auiRlValues[uiCs][uiPup][P] = uiPhase;
					pDramInfo->auiRlValues[uiCs][uiPup][D] = uiDelay;
					pDramInfo->auiRlValues[uiCs][uiPup][S] = RL_FINAL_STATE;
					uiReg = ddr3ReadPupReg(PUP_RL_MODE+0x1, uiCs, uiPup);
					pDramInfo->auiRlValues[uiCs][uiPup][DQS] = (uiReg & 0x3F);
				}
#ifdef	MV_DEBUG_RL
				/* Print results */
				DEBUG_RL_C("DDR3 - Read Leveling - Results for CS - ", (MV_U32)uiCs, 1);

				for (uiPup = 0; uiPup < (pDramInfo->uiNumOfStdPups+pDramInfo->uiEccEna); uiPup++) {
					DEBUG_RL_S("DDR3 - Read Leveling - PUP: ");
					DEBUG_RL_D((MV_U32)uiPup, 1);
					DEBUG_RL_S(", Phase: ");
					DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][P], 1);
					DEBUG_RL_S(", Delay: ");
					DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][D], 2);
					DEBUG_RL_S("\n");
				}
#endif
			}
		}
		pDramInfo->uiRdRdyDly = (MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR) & REG_READ_DATA_SAMPLE_DELAYS_MASK);
		pDramInfo->uiRdSmplDly = (MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR) & REG_READ_DATA_READY_DELAYS_MASK);
#ifdef	MV_DEBUG_RL
		DEBUG_RL_C("DDR3 - Read Leveling - Read Sample Delay: ", pDramInfo->uiRdSmplDly, 2);
		DEBUG_RL_C("DDR3 - Read Leveling - Read Ready Delay: ", pDramInfo->uiRdRdyDly, 2);
		DEBUG_RL_S("DDR3 - Read Leveling - HW RL Ended Successfuly \n");
#endif
		return MV_OK;

	} else {
		DEBUG_RL_S("DDR3 - Read Leveling - HW RL Error \n");
		return MV_FAIL;
	}
	
}


/******************************************************************************
* Name:     ddr3ReadLevelingSw.
* Desc:     Execute the Read leveling uiPhase by SW
* Args:     pDramInfo	- main struct
*			uiFreq 		- current sequence uiFrequency
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

MV_STATUS ddr3ReadLevelingSw(MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_DRAM_INFO *pDramInfo) 
{
	MV_U32 uiReg,uiCs,uiEcc,uiPupNum,uiPhase,uiDelay,uiMaxPupNum,uiPup;
	
	/* Debug message - Start Read leveling procedure */
	DEBUG_RL_S("DDR3 - Read Leveling - Starting SW RL procedure \n");

	/* Enable SW Read Leveling */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS));
	uiReg &= ~(1 << REG_DRAM_TRAINING_2_RL_MODE_OFFS);
	/* [0]=1 - Enable SW override  */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */

#ifdef RL_MODE
	uiReg = ((pDramInfo->uiCsEna << REG_DRAM_TRAINING_CS_OFFS) | (1 << REG_DRAM_TRAINING_AUTO_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */
#endif
	
	/* Loop for each CS */
	for(uiCs=0;uiCs<pDramInfo->uiNumOfCS;uiCs++) {
		
		DEBUG_RL_C("DDR3 - Read Leveling - CS - ",(MV_U32)uiCs,1);
				
		for(uiEcc=0;uiEcc<=(pDramInfo->uiEccEna);uiEcc++) {
			
			/* ECC Support - Switch ECC Mux on uiEcc=1 */
			uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
			uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
			MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);
			
			uiMaxPupNum = pDramInfo->uiNumOfStdPups*(1-uiEcc) + uiEcc;
					
			if (uiEcc)
				DEBUG_RL_S("DDR3 - Read Leveling - ECC Mux Enabled \n");
			else
				DEBUG_RL_S("DDR3 - Read Leveling - ECC Mux Disabled \n");
		
			/* Set current sample uiDelays */
			uiReg = MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
			uiReg &= ~(REG_READ_DATA_SAMPLE_DELAYS_MASK << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
			uiReg |= (pDramInfo->uiCL << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
			MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR,uiReg);
		
			/* Set current Ready uiDelay */
			uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
			uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
			if (!b2to1ratio) 			/* 1:1 mode */
				uiReg |= ((pDramInfo->uiCL+1) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
			else 					/* 2:1 mode */
				uiReg |= ((pDramInfo->uiCL+2) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
			MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR,uiReg);
			
			/* Read leveling Single CS[uiCs] */
#ifdef RL_MODE
			if (MV_OK != ddr3ReadLevelingSingleCsRlMode(uiCs, uiFreq, b2to1ratio, uiEcc, pDramInfo)) {
				return MV_FAIL;
			}
#else
			if (MV_OK != ddr3ReadLevelingSingleCsWindowMode(uiCs, uiFreq, b2to1ratio, uiEcc, pDramInfo)) {
				return MV_FAIL;
			}
#endif
		}

		/* Print results */
		DEBUG_RL_C("DDR3 - Read Leveling - Results for CS - ", (MV_U32)uiCs, 1);

		for (uiPup = 0; uiPup < (pDramInfo->uiNumOfStdPups+pDramInfo->uiEccEna); uiPup++) {
			DEBUG_RL_S("DDR3 - Read Leveling - PUP: ");
			DEBUG_RL_D((MV_U32)uiPup, 1);
			DEBUG_RL_S(", Phase: ");
			DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][P], 1);
			DEBUG_RL_S(", Delay: ");
			DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][D], 2);
			DEBUG_RL_S("\n");
		}

		DEBUG_RL_C("DDR3 - Read Leveling - Read Sample Delay: ",pDramInfo->uiRdSmplDly,1);
		DEBUG_RL_C("DDR3 - Read Leveling - Read Ready Delay: ",pDramInfo->uiRdRdyDly,1);
	
		/* Configure PHY with average of 3 locked leveling settings */
		for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups+pDramInfo->uiEccEna);uiPup++) {
			uiPupNum = (uiPup==pDramInfo->uiNumOfStdPups) ? ECC_BIT : uiPup; 		/* ECC support - bit 8 */
			/* For now, set last uiCount result */
			uiPhase = pDramInfo->auiRlValues[uiCs][uiPup][P];
			uiDelay = pDramInfo->auiRlValues[uiCs][uiPup][D];
			
			/* BTS #490 */
			if (uiPhase == 1 && uiDelay <=6) {
				DEBUG_RL_C("DDR3 - Read Leveling - found BTS #490 - setting delay=6 on pup: ",(MV_U32)uiPup,1);
				uiDelay = 6;
				pDramInfo->auiRlValues[uiCs][uiPup][D] = 6;
			}

			ddr3WritePupReg(PUP_RL_MODE, uiCs, uiPupNum, uiPhase, uiDelay);
		}
	}

	/* Reset PHY read FIFO */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_FIFO_RST_OFFS)); 
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);		/* 0x15B8 - Training SW 2 Register */	
	
	do {
		uiReg = ((MV_REG_READ(REG_DRAM_TRAINING_2_ADDR)) & (1<<REG_DRAM_TRAINING_2_FIFO_RST_OFFS));
	} while (uiReg);				/* Wait for '0' */

	/* ECC Support - Switch ECC Mux off uiEcc=0 */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);

#ifdef RL_MODE
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, 0);  	/* 0x15B0 - Training Register */
#endif

	/* Disable SW Read Leveling */	
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS)); 
	/* [0] = 0 - Disable SW override  */
	uiReg = (uiReg | (0x1 << REG_DRAM_TRAINING_2_RL_MODE_OFFS)); 
	/* [3] = 1 - Disable RL MODE */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */	
	

	DEBUG_RL_S("DDR3 - Read Leveling - Finished RL procedure for all CS \n");
	return MV_OK;
}

#ifdef RL_MODE
/******************************************************************************
* Name:     ddr3ReadLevelingSingleCsRlMode.
* Desc:     Execute Read leveling for single Chip select
* Args:     uiCs 			- current chip select
*			uiFreq 		- current sequence uiFrequency
*			uiEcc 		- uiEcc iteration indication
*			pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

static MV_STATUS ddr3ReadLevelingSingleCsRlMode(MV_U32 uiCs, MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_U32 uiEcc, MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg,uiDelay,uiPhase,uiSum,uiPup,uiRdSampleDelay,uiAdd,uiLockedPups,uiRepeatMaxCount,uiSdramOffset,
	uiFinalSum,uiLockedSum;
	MV_U32 uiPhaseMin, uiMaxDelay;
	MV_BOOL	bAllLocked,bFirstOctetLocked,bCounterInProcess;
	MV_BOOL bFinalDelay = FALSE;

	DEBUG_RL_FULL_C("DDR3 - Read Leveling - Single CS - ",(MV_U32)uiCs,1);
	
	/* Init values */
	uiPhase = 0;
	uiDelay = 0;
	uiRdSampleDelay = pDramInfo->uiCL;
	bAllLocked = MV_FALSE;
	bFirstOctetLocked = MV_FALSE;
	uiRepeatMaxCount = 0;
	uiSum = 0;
	uiFinalSum = 0;
	uiLockedSum = 0;
	
	for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {  
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]=0;
	}
		
	/* Main loop */
	while (!bAllLocked) {
		bCounterInProcess = MV_FALSE;
					
		DEBUG_RL_FULL_S("DDR3 - Read Leveling - RdSmplDly = ");
		DEBUG_RL_FULL_D(uiRdSampleDelay,1);
		DEBUG_RL_FULL_S(", Phase = ");
		DEBUG_RL_FULL_D(uiPhase,1);
		DEBUG_RL_FULL_S(", Delay = ");
		DEBUG_RL_FULL_D(uiDelay,2);
		DEBUG_RL_FULL_S("\n");
	
		/*  Broadcast to all PUPs current RL uiDelays: DQS uiPhase,leveling uiDelay */
		ddr3WritePupReg(PUP_RL_MODE, uiCs, PUP_BC, uiPhase, uiDelay);
					
		/* Reset PHY read FIFO */
		uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_FIFO_RST_OFFS)); 
		MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);		/* 0x15B8 - Training SW 2 Register */	
			
		do {
			uiReg = ((MV_REG_READ(REG_DRAM_TRAINING_2_ADDR)) & (1<<REG_DRAM_TRAINING_2_FIFO_RST_OFFS));
		} while (uiReg);				/* Wait for '0' */
			
		/* Read pattern from SDRAM */
		uiSdramOffset = uiCs * (SDRAM_CS_SIZE+1) + SDRAM_RL_OFFS;
		uiLockedPups = 0;
		if (MV_OK != ddr3SdramCompare(pDramInfo, 0xFF, &uiLockedPups, g_auiRLPattern, LEN_STD_PATTERN,
				uiSdramOffset, FALSE, FALSE, NULL, FALSE))
			return MV_FAIL;

		/* Octet evaluation */
		for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {  /* uiPupNum = Q or 1 for ECC */

			/* Check Overrun */
			if (!((MV_REG_READ(REG_DRAM_TRAINING_2_ADDR)>>(REG_DRAM_TRAINING_2_OVERRUN_OFFS+uiPup)) & BIT0)) { 
				/* If no OverRun */
				if (((~uiLockedPups >> uiPup) & BIT0) && (bFinalDelay == FALSE)) {
					/* PUP passed, start examining */
					if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]==RL_UNLOCK_STATE) {
						/* Must be RL_UNLOCK_STATE */
						/* Match expected value ? - Update State Machine */
						if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]<RL_RETRY_COUNT) {
							DEBUG_RL_FULL_C("DDR3 - Read Leveling - We have no overrun and a match on pup: ",(MV_U32)uiPup,1);
							pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]++;
							/* if pup got to last state - lock the delays */
							if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C] == RL_RETRY_COUNT) {
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C] = 0;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DS] = uiDelay;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS] = uiPhase;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S] = RL_FINAL_STATE; /* Go to Final State */
								uiLockedSum++;
								DEBUG_RL_FULL_C("DDR3 - Read Leveling - We have locked pup: ",(MV_U32)uiPup,1);
								
								/* if First lock - need to lock uiDelays */
								if (bFirstOctetLocked == MV_FALSE) {	
									DEBUG_RL_FULL_C("DDR3 - Read Leveling - We got first lock on pup: ",(MV_U32)uiPup,1);
									bFirstOctetLocked = MV_TRUE;
								}
							/* if pup is in not in final state but there was match - dont increment counter */
							} else 
								bCounterInProcess = MV_TRUE;
						}
					}
				}	
			} else {
				DEBUG_RL_FULL_C("DDR3 - Read Leveling - We got overrun on pup: ",(MV_U32)uiPup,1);
			}
		}
		
		if (uiLockedSum == (pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc)){
			bAllLocked = MV_TRUE;
			DEBUG_RL_FULL_S("DDR3 - Read Leveling - Single Cs - All pups locked \n");
		}

		/* This is a fix for unstable condition where pups are toggling between match and no match */
		/* If some of the pups is >1 <3, check if we did it too many times */
		if (bCounterInProcess == MV_TRUE) {
			if (uiRepeatMaxCount < RL_RETRY_COUNT) {	/* Notify at least one Counter is >=1 and < 3 */
				uiRepeatMaxCount++;
				bCounterInProcess = MV_TRUE;
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - Counter is >=1 and <3 \n");
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - So we will not increment the delay to see if locked again \n");
			} else {
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - uiRepeatMaxCount reached max so now we will increment the delay \n");
				bCounterInProcess = MV_FALSE;
			}
		}
				
		/* Check some of the pups are in the middle of state machine and don't increment the delays */
		if (!bCounterInProcess && !bAllLocked) {
			uiRepeatMaxCount = 0;
			if ((!b2to1ratio) && ((uiPhase == 0) || (uiPhase == 4))) /* if 1:1 mode */
				uiMaxDelay = MAX_DELAY_INV;
			else
				uiMaxDelay = MAX_DELAY;

			/* Increment Delay */
//			DEBUG_RL_FULL_S("DDR3 - Read Leveling - Single Cs - Increment Delay \n");
			if (uiDelay < uiMaxDelay) {			/* Delay Incrementation */
				uiDelay++;
				if (uiDelay==uiMaxDelay) {	/* Mark the last delay/pahse place for window final place */
					if ((!b2to1ratio && uiPhase==MAX_PHASE_RL_L_1TO1) || 
										 (b2to1ratio && uiPhase==MAX_PHASE_RL_L_2TO1))  	
						bFinalDelay = TRUE;
				}
			} else { 							/* Phase+CL Incrementation */
				uiDelay = 0;
				if (!b2to1ratio) { 	/* 1:1 mode */
					if (bFirstOctetLocked) {	/* some Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_L_1TO1) {
							if (uiPhase==1)
								uiPhase = 4;
							else {
								uiPhase++;
								uiDelay = MIN_DELAY_PHASE_1_LIMIT;
							}
						} else {
							DEBUG_RL_FULL_S("DDR3 - Read Leveling - ERROR - NOT all PUPs Locked \n");
							return MV_FAIL;
						}
					} else { 				/* NO Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_UL_1TO1) {
								uiPhase++;
								uiDelay = MIN_DELAY_PHASE_1_LIMIT;
						} else
							uiPhase = 0;
					}
				} else {					/* 2:1 mode */
					if (bFirstOctetLocked) {	/* some Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_L_2TO1)
							uiPhase++;
						else {
							DEBUG_RL_FULL_S("DDR3 - Read Leveling - ERROR - NOT all PUPs Locked \n");
							return MV_FAIL;
						}
					} else { 				/* NO Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_UL_2TO1)
							uiPhase++;
						else 
							uiPhase = 0;
					}
				}

				/* If we finished a full Phases cycle (so now phase = 0, need to increment rdSampleDly */
				if (uiPhase==0 && bFirstOctetLocked == MV_FALSE) {
					uiRdSampleDelay++;
					if (uiRdSampleDelay == 0x10) {
						DEBUG_RL_FULL_S("DDR3 - Read Leveling - ERROR - NOT all PUPs Locked \n");
						return MV_FAIL;
					}
					/* Set current uiRdSampleDelay  */
					uiReg = MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
					uiReg &= ~(REG_READ_DATA_SAMPLE_DELAYS_MASK << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
					uiReg |= (uiRdSampleDelay << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
					MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR,uiReg);
				}
	
				/* Set current rdReadyDelay according to the hash table (Need to do this in every phase change) */
				if (!b2to1ratio) {			/* 1:1 mode */
					uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_2_ADDR);
					switch(uiPhase) {
						case 0:
							uiAdd = (uiAdd >> REG_TRAINING_DEBUG_2_OFFS);
							break;
						case 1:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+3));
							break;
						case 4:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+6));
							break;
						case 5:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+9));
							break;
					}
				} else { 				/* 2:1 mode */
					uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
					uiAdd = (uiAdd >> uiPhase*REG_TRAINING_DEBUG_3_OFFS);
				}
				uiAdd &= REG_TRAINING_DEBUG_2_MASK;
				uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				uiReg |= ((uiRdSampleDelay+uiAdd) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR,uiReg);
				pDramInfo->uiRdSmplDly = uiRdSampleDelay;
				pDramInfo->uiRdRdyDly = uiRdSampleDelay+uiAdd;
			}
	
			/* Reset counters for pups with states<RD_STATE_COUNT */
			for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {
				if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]<RL_RETRY_COUNT)
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]=0;
			}
		}
	}
	
	uiPhaseMin = 10;

	for (uiPup = 0; uiPup < (pDramInfo->uiNumOfStdPups); uiPup++) {
		if (pDramInfo->auiRlValues[uiCs][uiPup][PS] < uiPhaseMin)
			uiPhaseMin = pDramInfo->auiRlValues[uiCs][uiPup][PS];
	}

	
	/* Set current rdReadyDelay according to the hash table (Need to do this in every phase change) */
	if (!b2to1ratio) {			/* 1:1 mode */
		uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_2_ADDR);
		switch(uiPhaseMin) {
			case 0:
				uiAdd = (uiAdd >> REG_TRAINING_DEBUG_2_OFFS);
				break;
			case 1:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+3));
				break;
			case 4:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+6));
				break;
			case 5:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+9));
				break;
		}
	} else { 				/* 2:1 mode */
		uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
		uiAdd = (uiAdd >> uiPhaseMin*REG_TRAINING_DEBUG_3_OFFS);
	}
		
	uiAdd &= REG_TRAINING_DEBUG_2_MASK;
	uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
	uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
	uiReg |= ((uiRdSampleDelay+uiAdd) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
	MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR,uiReg);
	pDramInfo->uiRdRdyDly = uiRdSampleDelay+uiAdd;	
		
	for(uiCs=0;uiCs<pDramInfo->uiNumOfCS;uiCs++) {
		for(uiPup=0;uiPup<pDramInfo->uiNumOfTotalPups;uiPup++) {
			uiReg = ddr3ReadPupReg(PUP_RL_MODE+0x1, uiCs, uiPup);
			pDramInfo->auiRlValues[uiCs][uiPup][DQS] = (uiReg & 0x3F);
		}
	}

	return MV_OK;
}

#else

/******************************************************************************
* Name:     ddr3ReadLevelingSingleCsWindowMode.
* Desc:     Execute Read leveling for single Chip select
* Args:     uiCs 			- current chip select
*			uiFreq 		- current sequence uiFrequency
*			uiEcc 		- uiEcc iteration indication
*			pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

static MV_STATUS ddr3ReadLevelingSingleCsWindowMode(MV_U32 uiCs, MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_U32 uiEcc, MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg,uiDelay,uiPhase,uiSum,uiPup,uiRdSampleDelay,uiAdd,uiLockedPups,uiRepeatMaxCount,uiSdramOffset,
	uiFinalSum,uiLockedSum;
	MV_U32 uiDelayS, uiDelayE,uiTemp,uiPhaseMin, uiMaxDelay;
	MV_BOOL	bAllLocked,bFirstOctetLocked,bCounterInProcess;
	MV_BOOL bFinalDelay = FALSE;
	
	DEBUG_RL_FULL_C("DDR3 - Read Leveling - Single CS - ",(MV_U32)uiCs,1);
		
	/* Init values */
	uiPhase = 0;
	uiDelay = 0;
	uiRdSampleDelay = pDramInfo->uiCL;
	bAllLocked = MV_FALSE;
	bFirstOctetLocked = MV_FALSE;
	uiRepeatMaxCount = 0;
	uiSum = 0;
	uiFinalSum = 0;
	uiLockedSum = 0;
		
	for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {  
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]=0;
	}
			
	/* Main loop */
	while (!bAllLocked) {
		bCounterInProcess = MV_FALSE;
						
		DEBUG_RL_FULL_S("DDR3 - Read Leveling - RdSmplDly = ");
		DEBUG_RL_FULL_D(uiRdSampleDelay,1);
		DEBUG_RL_FULL_S(", Phase = ");
		DEBUG_RL_FULL_D(uiPhase,1);
		DEBUG_RL_FULL_S(", Delay = ");
		DEBUG_RL_FULL_D(uiDelay,2);
		DEBUG_RL_FULL_S("\n");
		
		/*  Broadcast to all PUPs current RL uiDelays: DQS uiPhase,leveling uiDelay */
		ddr3WritePupReg(PUP_RL_MODE, uiCs, PUP_BC, uiPhase, uiDelay);
						
		/* Reset PHY read FIFO */
		uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_FIFO_RST_OFFS)); 
		MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);		/* 0x15B8 - Training SW 2 Register */	

		do {
			uiReg = ((MV_REG_READ(REG_DRAM_TRAINING_2_ADDR)) & (1<<REG_DRAM_TRAINING_2_FIFO_RST_OFFS));
		} while (uiReg);				/* Wait for '0' */
		
		/* Read pattern from SDRAM */
		uiSdramOffset = uiCs * (SDRAM_CS_SIZE+1) + SDRAM_RL_OFFS;
		uiLockedPups = 0;
		if (MV_OK != ddr3SdramCompare(pDramInfo, 0xFF, &uiLockedPups, g_auiRLPattern, LEN_STD_PATTERN,
			uiSdramOffset, FALSE, FALSE, NULL, FALSE))
			return MV_FAIL;

		/* Octet evaluation */
		for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {  /* uiPupNum = Q or 1 for ECC */

			/* Check Overrun */
			if (!((MV_REG_READ(REG_DRAM_TRAINING_2_ADDR)>>(REG_DRAM_TRAINING_2_OVERRUN_OFFS+uiPup)) & BIT0)) { 
				/* If no OverRun */
						
				/* Inside the window */
				if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S] == RL_WINDOW_STATE) {
					/* Match expected value ? - Update State Machine */
												
					if (((~uiLockedPups >> uiPup) & BIT0) && (bFinalDelay == FALSE)) {  
						/* Match - Still inside the Window */
						DEBUG_RL_FULL_C("DDR3 - Read Leveling - We got another match inside the window	for pup: ",(MV_U32)uiPup,1);
							
					} else { 		/* We got fail -> this is the end of the window */
						pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DE] = uiDelay;
						pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE] = uiPhase;
						pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]++; 	/* Go to Final State */
						uiFinalSum++;
						DEBUG_RL_FULL_C("DDR3 - Read Leveling - We finished the window for pup: ",(MV_U32)uiPup,1);
					}
						
					/* Before the start of the window */
				} else if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]==RL_UNLOCK_STATE) { 	
					/* Must be RL_UNLOCK_STATE */
					/* Match expected value ? - Update State Machine */
					if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]<RL_RETRY_COUNT) {
						if (((~uiLockedPups >> uiPup) & BIT0)) { 		/* Match */
							DEBUG_RL_FULL_C("DDR3 - Read Leveling - We have no overrun and a match on pup: ",(MV_U32)uiPup,1);
							pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]++;
				
							/* if pup got to last state - lock the delays */
							if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C] == RL_RETRY_COUNT) {
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C] = 0;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DS] = uiDelay;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS] = uiPhase;
								pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][S]++; 	/* Go to Window State */
								uiLockedSum++;												
										/* Will count the pups that got locked */
										
								/* IF First lock - need to lock uiDelays */
								if (bFirstOctetLocked == MV_FALSE) {	
									DEBUG_RL_FULL_C("DDR3 - Read Leveling - We got first lock on pup: ",(MV_U32)uiPup,1);
									bFirstOctetLocked = MV_TRUE;
								}
							}
							/* if pup is in not in final state but there was match - dont increment counter */
							else
								bCounterInProcess = MV_TRUE;
						}
					}
				}	
			} else {
				DEBUG_RL_FULL_C("DDR3 - Read Leveling - We got overrun on pup: ",(MV_U32)uiPup,1);
				bCounterInProcess = MV_TRUE;
			}
		}
						
					
		if (uiFinalSum == (pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc)){
			bAllLocked = MV_TRUE;
			DEBUG_RL_FULL_S("DDR3 - Read Leveling - Single Cs - All pups locked \n");
		}
		
					
		/* This is a fix for unstable condition where pups are toggling between match and no match */
		/* If some of the pups is >1 <3, check if we did it too many times */
		if (bCounterInProcess == MV_TRUE) {
			if (uiRepeatMaxCount < RL_RETRY_COUNT) {	/* Notify at least one Counter is >=1 and < 3 */
				uiRepeatMaxCount++;
				bCounterInProcess = MV_TRUE;
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - Counter is >=1 and <3 \n");
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - So we will not increment the delay to see if locked again \n");
			} else {
				DEBUG_RL_FULL_S("DDR3 - Read Leveling - uiRepeatMaxCount reached max so now we will increment the delay \n");
				bCounterInProcess = MV_FALSE;
			}
		}
					
		/* Check some of the pups are in the middle of state machine and don't increment the delays */
		if (!bCounterInProcess && !bAllLocked) {
			uiRepeatMaxCount = 0;
			if (!b2to1ratio)
				uiMaxDelay = MAX_DELAY_INV;
			else
				uiMaxDelay = MAX_DELAY;
	
			/* Increment Delay */
//			DEBUG_RL_FULL_S("DDR3 - Read Leveling - Single Cs - Increment Delay \n");
			if (uiDelay < uiMaxDelay) {			/* Delay Incrementation */
				uiDelay++;
				if (uiDelay==uiMaxDelay) {	/* Mark the last delay/pahse place for window final place */
					if ((!b2to1ratio && uiPhase==MAX_PHASE_RL_L_1TO1) ||
										 (b2to1ratio && uiPhase==MAX_PHASE_RL_L_2TO1))
						bFinalDelay = TRUE;
				}
			} else { 							/* Phase+CL Incrementation */
				uiDelay = 0;
				if (!b2to1ratio) { 	/* 1:1 mode */
					if (bFirstOctetLocked) {	/* some uiPupet was Locked */
						if (uiPhase < MAX_PHASE_RL_L_1TO1) {
#ifdef RL_WINDOW_WA		
							if (uiPhase==0)
#else
							if (uiPhase==1)
#endif
								uiPhase = 4;
							else
								uiPhase++;
						} else {
							DEBUG_RL_FULL_S("DDR3 - Read Leveling - ERROR - NOT all PUPs Locked \n");
							return MV_FAIL;
						}
					} else { 				/* NO Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_UL_1TO1) {
#ifdef RL_WINDOW_WA		
							if (uiPhase == 0)
								uiPhase = 4;
#else
							uiPhase++;
#endif
						} else
							uiPhase = 0;
					}
				} else {					/* 2:1 mode */
					if (bFirstOctetLocked) {	/* some Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_L_2TO1)
							uiPhase++;
						else {
							DEBUG_RL_FULL_S("DDR3 - Read Leveling - ERROR - NOT all PUPs Locked \n");
							return MV_FAIL;
						}
					} else { 				/* NO Pup was Locked */
						if (uiPhase < MAX_PHASE_RL_UL_2TO1)
							uiPhase++;
						else 
							uiPhase = 0;
					}
				}
		
				/* If we finished a full Phases cycle (so now phase = 0, need to increment rdSampleDly */
				if (uiPhase==0 && bFirstOctetLocked == MV_FALSE) {
					uiRdSampleDelay++;
							
					/* Set current uiRdSampleDelay  */
					uiReg = MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
					uiReg &= ~(REG_READ_DATA_SAMPLE_DELAYS_MASK << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
					uiReg |= (uiRdSampleDelay << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
					MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR,uiReg);
				}
		
				/* Set current rdReadyDelay according to the hash table (Need to do this in every phase change) */
				if (!b2to1ratio) {			/* 1:1 mode */
					uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_2_ADDR);
					switch(uiPhase) {
						case 0:
							uiAdd = (uiAdd >> REG_TRAINING_DEBUG_2_OFFS);
							break;
						case 1:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+3));
							break;
						case 4:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+6));
							break;
						case 5:
							uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+9));
							break;
					}
				} else { 				/* 2:1 mode */
					uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
					uiAdd = (uiAdd >> uiPhase*REG_TRAINING_DEBUG_3_OFFS);
				}
				uiAdd &= REG_TRAINING_DEBUG_2_MASK;
				uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				uiReg |= ((uiRdSampleDelay+uiAdd) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR,uiReg);
				pDramInfo->uiRdSmplDly = uiRdSampleDelay;
				pDramInfo->uiRdRdyDly = uiRdSampleDelay+uiAdd;
			}
		
			/* Reset counters for pups with states<RD_STATE_COUNT */
			for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) {
				if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]<RL_RETRY_COUNT)
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][C]=0;
			}
		}
	}
		
	uiPhaseMin = 10;
			
	for (uiPup = 0; uiPup < (pDramInfo->uiNumOfStdPups); uiPup++) {
		DEBUG_RL_S("DDR3 - Read Leveling - Window info - PUP: ");
		DEBUG_RL_D((MV_U32)uiPup, 1);
		DEBUG_RL_S(", PS: ");
		DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][PS], 1);
		DEBUG_RL_S(", DS: ");
		DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][DS], 2);
		DEBUG_RL_S(", PE: ");
		DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][PE], 1);
		DEBUG_RL_S(", DE: ");
		DEBUG_RL_D((MV_U32)pDramInfo->auiRlValues[uiCs][uiPup][DE], 2);
		DEBUG_RL_S("\n");
	}
		
	/* Find center of the window procedure */
	for(uiPup=0;uiPup<(pDramInfo->uiNumOfStdPups*(1-uiEcc)+uiEcc);uiPup++) { 
#ifdef RL_WINDOW_WA	
		if (!b2to1ratio) { 	/* 1:1 mode */
			if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS] == 4)
				pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS] = 1;
			if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE] == 4)
				pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE] = 1;
	
			uiDelayS = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS]*MAX_DELAY_INV + 
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DS];
			uiDelayE = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE]*MAX_DELAY_INV + 
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DE];
	
			uiTemp = (uiDelayE-uiDelayS)/2 + uiDelayS;
			uiPhase = uiTemp / MAX_DELAY_INV;
			if (uiPhase == 1)  	/* 1:1 mode */
				uiPhase = 4;
	
			if (uiPhase<uiPhaseMin)		/* for the read ready delay */
				uiPhaseMin = uiPhase;
				
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][P] = uiPhase;
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][D] = uiTemp % MAX_DELAY_INV;
	
		} else {
			uiDelayS = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS]*MAX_DELAY + 
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DS];
			uiDelayE = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE]*MAX_DELAY + 
					pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DE];
	
			uiTemp = (uiDelayE-uiDelayS)/2 + uiDelayS;
			uiPhase = uiTemp / MAX_DELAY;
				
			if (uiPhase<uiPhaseMin)		/* for the read ready delay */
				uiPhaseMin = uiPhase;
				
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][P] = uiPhase;
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][D] = uiTemp % MAX_DELAY;
		}
#else
		if (!b2to1ratio) { 	/* 1:1 mode */
			if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS]>1)
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS]-=2;
			if (pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE]>1)
			pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE]-=2;
		}
						
		uiDelayS = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PS]*MAX_DELAY + 
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DS];
		uiDelayE = pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][PE]*MAX_DELAY + 
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][DE];
	
		uiTemp = (uiDelayE-uiDelayS)/2 + uiDelayS;
		uiPhase = uiTemp / MAX_DELAY;
		if (!b2to1ratio && uiPhase>1)  	/* 1:1 mode */
		uiPhase += 2;
			
		if (uiPhase<uiPhaseMin)		/* for the read ready delay */
		uiPhaseMin = uiPhase;
				
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][P] = uiPhase;
		pDramInfo->auiRlValues[uiCs][uiPup+uiEcc*ECC_BIT][D] = uiTemp % MAX_DELAY;
#endif
	}
			
	/* Set current rdReadyDelay according to the hash table (Need to do this in every phase change) */
	if (!b2to1ratio) {			/* 1:1 mode */
		uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_2_ADDR);
		switch(uiPhaseMin) {
			case 0:
				uiAdd = (uiAdd >> REG_TRAINING_DEBUG_2_OFFS);
				break;
			case 1:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+3));
				break;
			case 4:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+6));
				break;
			case 5:
				uiAdd = (uiAdd >> (REG_TRAINING_DEBUG_2_OFFS+9));
				break;
		}
	} else { 				/* 2:1 mode */
		uiAdd = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
		uiAdd = (uiAdd >> uiPhaseMin*REG_TRAINING_DEBUG_3_OFFS);
	}

	uiAdd &= REG_TRAINING_DEBUG_2_MASK;
	uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
	uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
	uiReg |= ((uiRdSampleDelay+uiAdd) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
	MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR,uiReg);
	pDramInfo->uiRdRdyDly = uiRdSampleDelay+uiAdd;	
			
	for(uiCs=0;uiCs<pDramInfo->uiNumOfCS;uiCs++) {
		for(uiPup=0;uiPup<pDramInfo->uiNumOfTotalPups;uiPup++) {
			uiReg = ddr3ReadPupReg(PUP_RL_MODE+0x1, uiCs, uiPup);
			pDramInfo->auiRlValues[uiCs][uiPup][DQS] = (uiReg & 0x3F);
		}
	}

	return MV_OK;
}

#endif