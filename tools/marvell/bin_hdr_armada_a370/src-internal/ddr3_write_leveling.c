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

/**********
 * Debug
*/
#define DEBUG_WL_C(s, d, l)			DEBUG_WL_S(s); DEBUG_WL_D(d, l); DEBUG_WL_S("\n")
#define DEBUG_WL_FULL_C(s, d, l)	DEBUG_WL_FULL_S(s); DEBUG_WL_FULL_D(d, l); DEBUG_WL_FULL_S("\n")

#ifdef	MV_DEBUG_WL
#define DEBUG_WL_S(s)				putstring(s)
#define DEBUG_WL_D(d, l)			putdata(d, l)

#else
#define DEBUG_WL_S(s)
#define DEBUG_WL_D(d, l)
#endif

#ifdef	MV_DEBUG_WL_FULL
#define DEBUG_WL_FULL_S(s)			putstring(s)
#define DEBUG_WL_FULL_D(d, l)		putdata(d, l)
#else
#define DEBUG_WL_FULL_S(s)
#define DEBUG_WL_FULL_D(d, l)
#endif

static MV_STATUS ddr3WriteLevelingSingleCs(MV_U32 uiCs,MV_U32 uiFreq,MV_BOOL b2to1ratio,MV_U32 *result,MV_DRAM_INFO *pDramInfo);

extern MV_U16 auiODTStatic[ODT_OPT][MAX_CS];
extern MV_U16 auiODTDynamic[ODT_OPT][MAX_CS];


extern MV_U32 g_auiWLInvPattern[LEN_STD_PATTERN];
extern MV_U32 g_auiWLPattern[LEN_STD_PATTERN];
extern MV_U32 g_auiSpecialPattern[DQ_NUM][LEN_SPECIAL_PATTERN];
extern MV_U32 g_auiPbsPattern[2][LEN_PBS_PATTERN];
/******************************************************************************
* Name:     ddr3WriteLevelingHw.
* Desc:     Execute Write leveling uiPhase by HW
* Args:     uiFreq 		- current sequence frequency
*			pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

MV_STATUS ddr3WriteLevelingHw(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg, uiPhase, uiDelay, uiCs, uiPup;
	MV_BOOL bDpdeFlag = MV_FALSE;
	
	/* Debug message - Start Read leveling procedure */
	DEBUG_WL_S("DDR3 - Write Leveling - Starting HW WL procedure \n");
	
	if (!pDramInfo->bIsA0) {
		/* Dynamic pad issue (BTS669) during WL */
		uiReg = MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR);
		if (uiReg & (1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS)) {
			bDpdeFlag = MV_TRUE;
			MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg & ~(1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS));
		}
	}
	
	uiReg = (1 << REG_DRAM_TRAINING_WL_OFFS);
	/* Config the retest number */
	uiReg |= (COUNT_HW_WL << REG_DRAM_TRAINING_RETEST_OFFS);

	uiReg |= (pDramInfo->uiCsEna << (REG_DRAM_TRAINING_CS_OFFS));

	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */

	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_SHADOW_ADDR) | (1 << REG_DRAM_TRAINING_AUTO_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_SHADOW_ADDR, uiReg);

	/* Wait */
	do {
		uiReg = (MV_REG_READ(REG_DRAM_TRAINING_SHADOW_ADDR) & (1<<REG_DRAM_TRAINING_AUTO_OFFS));
	} while (uiReg);				/* Wait for '0' */
	
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_ADDR);
	/* Check if Successful */
	if (uiReg & (1 << REG_DRAM_TRAINING_ERROR_OFFS)) {
		/* Read results to arrays - Results are required for WL High freq Supplement and DQS Centralization */
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {
				for (uiPup = 0; uiPup < pDramInfo->uiNumOfTotalPups; uiPup++) {
					if (uiPup == pDramInfo->uiNumOfStdPups && pDramInfo->uiEccEna)
						uiPup = ECC_BIT;
					uiReg = ddr3ReadPupReg(PUP_WL_MODE, uiCs, uiPup);
					uiPhase = (uiReg >> REG_PHY_PHASE_OFFS) & PUP_PHASE_MASK;
					uiDelay = uiReg & PUP_DELAY_MASK;
					pDramInfo->auiWlValues[uiCs][uiPup][P] = uiPhase;
					pDramInfo->auiWlValues[uiCs][uiPup][D] = uiDelay;
					pDramInfo->auiWlValues[uiCs][uiPup][S] = WL_HI_FREQ_STATE-1;
					uiReg = ddr3ReadPupReg(PUP_WL_MODE+0x1, uiCs, uiPup);
					pDramInfo->auiWlValues[uiCs][uiPup][DQS] = (uiReg & 0x3F);
				}
#ifdef	MV_DEBUG_WL
				/* Debug message - Print auiResults for uiCs[i]: uiCs,PUP,Phase,Delay */
				DEBUG_WL_S("DDR3 - Write Leveling - Write Leveling Cs - ");
				DEBUG_WL_D((MV_U32)uiCs, 1);
				DEBUG_WL_S(" Results: \n");
				for (uiPup = 0; uiPup < pDramInfo->uiNumOfTotalPups; uiPup++) {
					DEBUG_WL_S("DDR3 - Write Leveling - PUP: ");
					DEBUG_WL_D((MV_U32)uiPup, 1);
					DEBUG_WL_S(", Phase: ");
					DEBUG_WL_D((MV_U32)pDramInfo->auiWlValues[uiCs][uiPup][P], 1);
					DEBUG_WL_S(", Delay: ");
					DEBUG_WL_D((MV_U32)pDramInfo->auiWlValues[uiCs][uiPup][D], 2);
					DEBUG_WL_S("\n");
				}
#endif
			}
		}

		/* Dynamic pad issue (BTS669) during WL */
		if (!pDramInfo->bIsA0 && bDpdeFlag) {
			uiReg = (MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR) | (1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS));
			MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);
		}
		
		DEBUG_WL_S("DDR3 - Write Leveling - HW WL Ended Successfuly \n");
		
		return MV_OK;
	} else {
		DEBUG_WL_S("DDR3 - Write Leveling - HW WL Error \n");
		return MV_FAIL;
	}
}


/******************************************************************************
* Name:     ddr3WriteHiFreqSup.
* Desc:     Write Leveling Hi-Freq Supplement (only for 2:1 mode)
* Args:     pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

MV_STATUS ddr3WriteHiFreqSup(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiCs, uiCount, uiPupNum, uiLockedPups, uiSum, uiPhase, uiDelay, uiMaxPupNum, uiPup, uiSdramOffset;
	MV_U32 uiTempCount, uiEcc, uiReg;
	DEBUG_WL_S("DDR3 - Write Leveling Hi-Freq Supplement - Starting \n");
	
	/* Enable SW override */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS));
	/* [0] = 1 - Enable SW override  */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	DEBUG_WL_FULL_S("DDR3 - Write Leveling - SW Override Enabled \n");
	
	/* Enable Training HW mechanism */
	uiReg = ((pDramInfo->uiCsEna << REG_DRAM_TRAINING_CS_OFFS) | (1 << REG_DRAM_TRAINING_AUTO_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */

	uiTempCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (pDramInfo->uiCsEna & (1<<uiCs)) {
			uiSum = 0;
			for (uiCount = 0; uiCount < COUNT_WL_HI_FREQ; uiCount++) {
				
				for (uiEcc = 0; uiEcc < (pDramInfo->uiEccEna+1); uiEcc++) {
	
					uiMaxPupNum = pDramInfo->uiNumOfStdPups*(1-uiEcc) + uiEcc;
					
					/* ECC Support - Switch ECC Mux on uiEcc=1 */
					uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
					uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
					MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);
				
					/* Read Write value to memory - uiCs[i] */
					uiSdramOffset = uiTempCount * (SDRAM_CS_SIZE+1) + SDRAM_WL_SW_OFFS;
									
					uiLockedPups = 0;

					if (MV_OK != ddr3SdramCompare(pDramInfo, 0xFF, &uiLockedPups, g_auiWLInvPattern, LEN_STD_PATTERN, uiSdramOffset, TRUE, FALSE, NULL, FALSE))
						return MV_FAIL;
					
//					if (MV_OK != ddr3SdramCompare(pDramInfo, 0xFF, &uiLockedPups, g_auiWLPattern, LEN_STD_PATTERN, uiSdramOffset, TRUE, FALSE, NULL, FALSE))
					if (MV_OK != ddr3SdramCompare(pDramInfo, 0xFF, &uiLockedPups, g_auiSpecialPattern[0], LEN_STD_PATTERN, uiSdramOffset, TRUE, FALSE, NULL, FALSE))  
						return MV_FAIL;
	
					/* Checpup which DQS/DATA is error */
					for (uiPup = 0; uiPup < uiMaxPupNum; uiPup++) {
						uiPupNum = (uiEcc) ? (pDramInfo->uiNumOfStdPups) : uiPup; /* ECC support - bit 8 */
	
						if ((uiLockedPups >> uiPup) & BIT0) {
							if (uiCount == 0) {
								/* Check is there is error in the PUP value */
								uiPhase = ((pDramInfo->auiWlValues[uiCs][uiPupNum][P]+WL_HI_FREQ_SHIFT) % MAX_PHASE_2TO1);
								pDramInfo->auiWlValues[uiCs][uiPupNum][P] = uiPhase;
								uiDelay = pDramInfo->auiWlValues[uiCs][uiPupNum][D];
								DEBUG_WL_S("DDR3 - Write Leveling Hi-Freq Supplement - Cs: ");
								DEBUG_WL_D((MV_U32)uiCs, 1);
								DEBUG_WL_S(" PUP: ");
								DEBUG_WL_D((MV_U32)uiPupNum, 1);
								DEBUG_WL_S(" Needs Write Leveling Hi-Freq Supplement \n");
	
								ddr3WritePupReg(PUP_WL_MODE, uiCs, uiPupNum, uiPhase, uiDelay);
							} else if (uiCount == COUNT_WL_HI_FREQ) {
								DEBUG_WL_C("DDR3 - Write Leveling Hi-Freq Supplement - didn't work for Cs - ", (MV_U32)uiCs, 1);
								return MV_FAIL;
							}
						} else if (pDramInfo->auiWlValues[uiCs][uiPupNum][S] < WL_HI_FREQ_STATE)
							pDramInfo->auiWlValues[uiCs][uiPupNum][S]++;	/* PUP is correct - increment State */
					}
					
					/* ECC Support - Disable ECC MUX */
					uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
					MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);
				}
			}

			for (uiPup = 0; uiPup < pDramInfo->uiNumOfTotalPups; uiPup++)
				uiSum = uiSum + pDramInfo->auiWlValues[uiCs][uiPup][S];

			if (uiSum < (WL_HI_FREQ_STATE*(pDramInfo->uiNumOfTotalPups))) {	/* checks if any uiPup is not locked after the change */
				DEBUG_WL_C("DDR3 - Write Leveling Hi-Freq Supplement - didn't work for Cs - ", (MV_U32)uiCs, 1);
				return MV_FAIL;
			}
			uiTempCount++;
		}
	}
	
	/* Disable SW override - Must be in a different stage */
	/* [0]=0 - Enable SW override  */
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_2_ADDR);
	uiReg &= ~(1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	
	/* Disable Training mechanism */
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, 0);  	/* 0x15B0 - Training Register */
		
	DEBUG_WL_S("DDR3 - Write Leveling Hi-Freq Supplement - Ended Successfuly \n");
	
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3WriteLevelingSw.
* Desc:     Execute Write leveling uiPhase by SW
* Args:     uiFreq 		- current sequence frequency
*			pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

MV_STATUS ddr3WriteLevelingSw(MV_U32 uiFreq,MV_BOOL b2to1ratio,MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg,uiCs,uiCount,uiMaxPupNum;
	MV_U32 auiResults[MAX_CS];
	MV_BOOL bDpdeFlag = MV_FALSE;
	uiMaxPupNum = pDramInfo->uiNumOfTotalPups;

	/* Debug message - Start Write leveling procedure */
	DEBUG_WL_S("DDR3 - Write Leveling - Starting SW WL procedure \n");
	
	/* Dynamic pad issue (BTS669) during WL */
	uiReg = MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR);
	if (uiReg & (1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS)) {
		bDpdeFlag = MV_TRUE;
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg & ~(1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS));
	}
	
	/* Set Output buffer-off to all CS and correct ODT values */
	for(uiCs=0;uiCs<MAX_CS;uiCs++) {
		if(pDramInfo->uiCsEna & (1<<uiCs)) {
			uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR) & REG_DDR3_MR1_ODT_MASK);
			uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
			uiReg |= (1 << REG_DDR3_MR1_OUTBUF_DIS_OFFS);
			
			MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg);  		/* 0x15D0 - DDR3 MR0 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
			
			uDelay(MRS_DELAY);
		}
	}
	
	DEBUG_WL_FULL_S("DDR3 - Write Leveling - Qoff and RTT Values are set for all Cs \n");
	
	/* Enable SW override */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS));
	/* [0] = 1 - Enable SW override  */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	DEBUG_WL_FULL_S("DDR3 - Write Leveling - SW Override Enabled \n");
	
	/* Enable PHY write leveling mode */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_WL_MODE_OFFS));
	/* [2] = 0 - TrnWLMode - Enable */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */

	/* Loop for each uiCs */
	for(uiCs=0;uiCs<MAX_CS;uiCs++) {
		if(pDramInfo->uiCsEna & (1<<uiCs)) {
			DEBUG_WL_FULL_C("DDR3 - Write Leveling - Starting working with Cs - ",(MV_U32)uiCs,1);
	
			/* Refresh X9 current uiCs */
			DEBUG_WL_FULL_S("DDR3 - Write Leveling - Refresh X9 \n");
			for(uiCount=0;uiCount<COUNT_WL_RFRS;uiCount++) {
				uiReg = REG_SDRAM_OPERATION_CMD_RFRS & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs));
				/* [3-0] = 0x2 - refresh, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR,uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				do {
					uiReg = ((MV_REG_READ(REG_SDRAM_OPERATION_ADDR)) & REG_SDRAM_OPERATION_CMD_RFRS_DONE);
				} while (uiReg);				/* Wait for '0' */
			}

			/* Configure MR1 in Cs[CsNum] - write leveling on, output buffer on */
			DEBUG_WL_FULL_S("DDR3 - Write Leveling - Configure MR1 for current Cs: WL-on,OB-on \n");
			uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR) & REG_DDR3_MR1_OUTBUF_WL_MASK);
			/* Set ODT Values */
			uiReg &= REG_DDR3_MR1_ODT_MASK;
			uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
			/* Enable WL MODE */
			uiReg |= (1 << REG_DDR3_MR1_WL_ENA_OFFS);
			/* [7]=1, [12]=0 - Output Buffer and write leveling enabled */
			MV_REG_WRITE(REG_DDR3_MR1_ADDR,uiReg);  		/* 0x15D4 - DDR3 MR1 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR,uiReg);  	/* 0x1418 - SDRAM Operation Register */

			uDelay(MRS_DELAY);
			
			/* Write leveling  uiCs[uiCs] */
			if(MV_OK != ddr3WriteLevelingSingleCs(uiCs,uiFreq,b2to1ratio,(MV_U32*)(auiResults+uiCs),pDramInfo)) {
				DEBUG_WL_FULL_C("DDR3 - Write Leveling single Cs - FAILED -  Cs - ",(MV_U32)uiCs,1);
				return MV_FAIL;
			}

			/* Set TrnWLDeUpd - After each CS is done */
			uiReg = (MV_REG_READ(REG_TRAINING_WL_ADDR) | (1<<REG_TRAINING_WL_CS_DONE_OFFS));
			MV_REG_WRITE(REG_TRAINING_WL_ADDR,uiReg);  		/* 0x16AC - Training Write leveling register */
			
			/* Debug message - Finished Write leveling uiCs[uiCs] - each PUP Fail/Success */
			DEBUG_WL_FULL_C("DDR3 - Write Leveling - Finished Cs - ",(MV_U32)uiCs,1);
			DEBUG_WL_FULL_C("DDR3 - Write Leveling - The Results: 1-PUP locked, 0-PUP failed -",
					   (MV_U32)auiResults[uiCs],3);

			/* Configure MR1 in uiCs[uiCs] - write leveling off (0), output buffer off (1) */
			uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR) & REG_DDR3_MR1_OUTBUF_WL_MASK);
			uiReg |= (1 << REG_DDR3_MR1_OUTBUF_DIS_OFFS);
			/* No need to sort ODT since it is same CS */
			MV_REG_WRITE(REG_DDR3_MR1_ADDR,uiReg);  		/* 0x15D4 - DDR3 MR1 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR,uiReg);  	/* 0x1418 - SDRAM Operation Register */

			uDelay(MRS_DELAY);
		}
	}
		
	/* Disable WL Mode */
	/* [2]=1 - TrnWLMode - Disable */
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_2_ADDR);
	uiReg |= (1 << REG_DRAM_TRAINING_2_WL_MODE_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */

	/* Disable SW override - Must be in a different stage */
	/* [0]=0 - Enable SW override  */
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_2_ADDR);
	uiReg &= ~(1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */

	/* Set Output buffer-on to all CS and correct ODT values */
	for(uiCs=0;uiCs<MAX_CS;uiCs++) {
		if(pDramInfo->uiCsEna & (1<<uiCs)) {
			
			uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR) & REG_DDR3_MR1_ODT_MASK);
			uiReg &= REG_DDR3_MR1_OUTBUF_WL_MASK;
			uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
				
			MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg);  		/* 0x15D0 - DDR3 MR1 Register */
			/* Issue MRS Command to current uiCs */
			uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
			/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
			
			uDelay(MRS_DELAY);
		}
	}

	/* Dynamic pad issue (BTS669) during WL */
	if (bDpdeFlag) {
		uiReg = (MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR) | (1 << REG_DUNIT_CTRL_LOW_DPDE_OFFS));
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);
	}
	
	DEBUG_WL_FULL_S("DDR3 - Write Leveling - Finished WL procedure for all Cs \n");
	return MV_OK;
}


/******************************************************************************
* Name:     ddr3WriteLevelingSingleCs.
* Desc:     Execute Write leveling for single Chip select
* Args:     uiCs 			- current chip select
*			uiFreq 		- current sequence frequency
*			result 		- auiResults array
*			pDramInfo	- main struct
* Notes:
* Returns:  MV_OK if success, MV_FAIL if fail.
*/

static MV_STATUS ddr3WriteLevelingSingleCs(MV_U32 uiCs,MV_U32 uiFreq,MV_BOOL b2to1ratio,MV_U32 *result,MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg,i,uiPupNum,uiDelay,uiPhase,phaseMax,uiMaxPupNum,uiPup;
	uiMaxPupNum = pDramInfo->uiNumOfTotalPups;
	*result = 0;
	
	DEBUG_WL_FULL_C("DDR3 - Write Leveling Single Cs - WL for Cs - ", (MV_U32)uiCs, 1);
		
	/* CS ODT Override */
	uiReg = (MV_REG_READ(REG_SDRAM_ODT_CTRL_HIGH_ADDR) & REG_SDRAM_ODT_CTRL_HIGH_OVRD_MASK);
	uiReg |=  (REG_SDRAM_ODT_CTRL_HIGH_OVRD_ENA << (2*uiCs));
	/* Set 0x3 - Enable ODT on the curent uiCs and disable on other uiCs */
	MV_REG_WRITE(REG_SDRAM_ODT_CTRL_HIGH_ADDR,uiReg);  	/* 0x1498 - SDRAM ODT Control high */
	
	DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - ODT Asserted for current Cs \n");
	
	/* tWLMRD Delay */
	uDelay(1); 		/* Delay of minimum 40 Dram clock cycles - 20 Tclk cycles */ 

	uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) & REG_TRAINING_WL_CS_MASK) | uiCs); /* [1:0] - current uiCs number */
	uiReg |= (1 << REG_TRAINING_WL_UPD_OFFS);			/* [2] - trnWLCsUpd */
	MV_REG_WRITE(REG_TRAINING_WL_ADDR,uiReg);  		/* 0x16AC - Training Write leveling register */
	
	/*  Broadcast to all PUPs: Reset DQS uiPhase, reset leveling uiDelay */
	ddr3WritePupReg(PUP_WL_MODE,uiCs,PUP_BC,0,0);
	
	/* Seek Edge */
	DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge - Current Cs \n");
		
	/* Drive DQS high for one cycle - All data PUPs */
	DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge - Driving DQS high for one cycle \n");
	if (!b2to1ratio)
		uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) & REG_TRAINING_WL_RATIO_MASK) | REG_TRAINING_WL_1TO1);
	else					
		uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) & REG_TRAINING_WL_RATIO_MASK) | REG_TRAINING_WL_2TO1);
	MV_REG_WRITE(REG_TRAINING_WL_ADDR,uiReg);  		/* 0x16AC - Training Write leveling register */

	/* Wait tWLdelay */
	do {
		uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR)) & REG_TRAINING_WL_DELAYEXP_MASK); /* [29] - trnWLDelayExp */
	} while (uiReg == 0x0);					/* Wait for '1' */

	/* Read WL auiResults */
	uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) >> REG_TRAINING_WL_RESULTS_OFFS) & REG_TRAINING_WL_RESULTS_MASK);
	/* [28:20] - TrnWLResult */

	/* Add one uiPhase to each DQS that is one */
	DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge - Perliminary Phase check \n");
	for(uiPup=0;uiPup<(uiMaxPupNum);uiPup++) {
		uiPupNum = (uiPup==pDramInfo->uiNumOfStdPups) ? ECC_BIT : uiPup; 	/* ECC support - bit 8 */
		if((uiReg >> uiPupNum) & BIT0) {
			pDramInfo->auiWlValues[uiCs][uiPup][P] = 1;
			DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge - Perliminary Add 1 Phase to PUP - ");
			DEBUG_WL_FULL_D((MV_U32)uiPup,1);
			DEBUG_WL_FULL_S("\n");
		}
	}
		
	if(!b2to1ratio)				/* Different uiPhase options for 2:1 or 1:1 modes */
		phaseMax = MAX_PHASE_1TO1;
	else 
		phaseMax = MAX_PHASE_2TO1;
	
	DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge - Shift DQS + Octet Leveling \n");
	/* Shift DQS + Octet leveling */
	for(uiPhase=0;uiPhase<phaseMax;uiPhase++) {
		for(uiDelay=0;uiDelay<MAX_DELAY;uiDelay++) {
			/*  Broadcast to all PUPs: DQS uiPhase,leveling uiDelay */
			ddr3WritePupReg(PUP_WL_MODE,uiCs,PUP_BC,uiPhase,uiDelay);
			
			uDelay(1); 	/* Delay of  3 Tclk cycles */ 
							
			for(i=0;i<COUNT_WL;i++) {
				DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge: Phase = ");
				DEBUG_WL_FULL_D((MV_U32)uiPhase,1);
				DEBUG_WL_FULL_S(", Delay = ");
				DEBUG_WL_FULL_D((MV_U32)uiDelay,1);
				DEBUG_WL_FULL_S(", Counter = ");
				DEBUG_WL_FULL_D((MV_U32)i,1);
				DEBUG_WL_FULL_S("\n");

				/* Drive DQS high for one cycle - All data PUPs */
				if (!b2to1ratio)
					uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) & REG_TRAINING_WL_RATIO_MASK) | REG_TRAINING_WL_1TO1);
				else					
					uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR) & REG_TRAINING_WL_RATIO_MASK) | REG_TRAINING_WL_2TO1);
				MV_REG_WRITE(REG_TRAINING_WL_ADDR,uiReg); 	/* 0x16AC  */
				
				/* Wait tWLdelay */
				do {
					uiReg = ((MV_REG_READ(REG_TRAINING_WL_ADDR)) & REG_TRAINING_WL_DELAYEXP_MASK);
				} while (uiReg == 0x0);				/* [29] Wait for '1' */

				/* Read WL auiResults */
				uiReg = MV_REG_READ(REG_TRAINING_WL_ADDR);
				uiReg = (uiReg >> REG_TRAINING_WL_RESULTS_OFFS) & REG_TRAINING_WL_RESULTS_MASK;		/* [28:20] */
		
				DEBUG_WL_FULL_C("DDR3 - Write Leveling Single Cs - Seek Edge: Results =  ",(MV_U32)uiReg,3);

				/* Update State machine */
				for(uiPup=0;uiPup<(uiMaxPupNum);uiPup++) {
					uiPupNum = (uiPup==pDramInfo->uiNumOfStdPups) ? ECC_BIT : uiPup; 	/* ECC support - bit 8 */
					if((pDramInfo->auiWlValues[uiCs][uiPup][S]<COUNT_WL) &&
							pDramInfo->auiWlValues[uiCs][uiPup][P]<=uiPhase) {
						pDramInfo->auiWlValues[uiCs][uiPup][P] = uiPhase; /* Update uiPhase to PUP */
						pDramInfo->auiWlValues[uiCs][uiPup][D] = uiDelay; /* Update uiDelay to PUP */
						if(((uiReg >> uiPupNum) & BIT0) && (pDramInfo->auiWlValues[uiCs][uiPup][S]==i)) {
							/* If the PUP is locked now and in last counter states */
							pDramInfo->auiWlValues[uiCs][uiPup][S]++; 	/* Go to next state */
							if(pDramInfo->auiWlValues[uiCs][uiPup][S] == COUNT_WL)
								 *result = *result | (1 << uiPupNum); /* Set auiResults */
						} else
							pDramInfo->auiWlValues[uiCs][uiPup][S] = 0;
							/* If we got 0 we go back to state 0 */
					}
				}
				if(uiReg==0x0)
					i=COUNT_WL; 	/* break the counter since no one was locked */
			}
			
			/* Check if all are locked */
			uiReg = 0;
			for(uiPup=0;uiPup<(uiMaxPupNum);uiPup++) {
				uiReg = uiReg + pDramInfo->auiWlValues[uiCs][uiPup][S];
			}
			/* If all locked - Break the loops - Finished */
			if(uiReg==(COUNT_WL*(uiMaxPupNum))) {
				uiPhase = phaseMax;
				uiDelay = MAX_DELAY;
				i = COUNT_WL;
				DEBUG_WL_FULL_S("DDR3 - Write Leveling Single Cs - Seek Edge: All Locked \n");
			}
		}
	}

	/* Debug message - Print auiResults for uiCs[i]: uiCs,PUP,Phase,Delay */
	DEBUG_WL_C("DDR3 - Write Leveling - Results for CS - ",(MV_U32)uiCs,1);
	for(uiPup=0;uiPup<(uiMaxPupNum);uiPup++) {
		DEBUG_WL_S("DDR3 - Write Leveling - PUP: ");
		DEBUG_WL_D((MV_U32)uiPup,1);
		DEBUG_WL_S(", Phase: ");
		DEBUG_WL_D((MV_U32)pDramInfo->auiWlValues[uiCs][uiPup][P],1);
		DEBUG_WL_S(", Delay: ");
		DEBUG_WL_D((MV_U32)pDramInfo->auiWlValues[uiCs][uiPup][D],2);
		DEBUG_WL_S("\n");
	}
	
	/* Check if some not locked and return error */
	if(uiReg!=(COUNT_WL*(uiMaxPupNum))) {
		DEBUG_WL_S("DDR3 - Write Leveling - ERROR - not all PUPS were locked \n");
	/*	return MV_FAIL; */
	}
	
	/* Configure Each PUP with locked leveling settings */
	for(uiPup=0;uiPup<(uiMaxPupNum);uiPup++) {
		uiPupNum = (uiPup==pDramInfo->uiNumOfStdPups) ? ECC_BIT : uiPup; 		/* ECC support - bit 8 */
		uiPhase = pDramInfo->auiWlValues[uiCs][uiPup][P];
		uiDelay = pDramInfo->auiWlValues[uiCs][uiPup][D];
		ddr3WritePupReg(PUP_WL_MODE,uiCs,uiPupNum,uiPhase,uiDelay);
	}

	/* CS ODT Override */
	uiReg = (MV_REG_READ(REG_SDRAM_ODT_CTRL_HIGH_ADDR) & REG_SDRAM_ODT_CTRL_HIGH_OVRD_MASK);
	MV_REG_WRITE(REG_SDRAM_ODT_CTRL_HIGH_ADDR,uiReg);  	/* 0x1498 - SDRAM ODT Control high */
	
	return MV_OK;
}
