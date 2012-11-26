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

/*
 * Debug
*/
#define DEBUG_DQS_C(s, d, l)      DEBUG_DQS_S(s); DEBUG_DQS_D(d, l); DEBUG_DQS_S("\n")
#define DEBUG_DQS_FULL_C(s, d, l)      DEBUG_DQS_FULL_S(s); DEBUG_DQS_FULL_D(d, l); DEBUG_DQS_FULL_S("\n")

#ifdef	MV_DEBUG_DQS
#define DEBUG_DQS_S(s)				putstring(s)
#define DEBUG_DQS_D(d, l)			putdata(d, l)
#else
#define DEBUG_DQS_S(s)
#define DEBUG_DQS_D(d, l)
#endif

#ifdef	MV_DEBUG_DQS_FULL
#define DEBUG_DQS_FULL_S(s)			putstring(s)
#define DEBUG_DQS_FULL_D(d, l)		putdata(d, l)
#else
#define DEBUG_DQS_FULL_S(s)
#define DEBUG_DQS_FULL_D(d, l)
#endif


/* state machine for centralization - find low & high limit */
typedef enum {
	pupADllLimitsState_FAIL,
	pupADllLimitsState_PASS,
	pupADllLimitsState_FAIL_AFTER_PASS,
} pupADllLimitsState;

static MV_32 s_aiCentralizationLowLimit[MAX_PUP_NUM] 	= {0}; 	/* hold centralization low results */
static MV_32 s_aiCentralizationHighLimit[MAX_PUP_NUM] 	= {0}; 	/* hold centralization high results */


MV_STATUS	ddr3FindAdllLimits(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx);
MV_STATUS	ddr3CheckWindowLimits(MV_U32 uiPup, MV_32 iHighLimit, MV_32 iLowLimit, MV_BOOL bIsTx,
								   MV_BOOL *pbSizeValid);
MV_STATUS	ddr3CenterCalc(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx);
MV_STATUS	ddr3SpecialPatternISearch(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx,
									   MV_U32 uiSpecialPatternPup);
MV_STATUS	ddr3SpecialPatternIISearch(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx,
									    MV_U32 uiSpecialPatternPup);
MV_STATUS   ddr3SetDqsCentralizationResults(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx);
MV_STATUS	ddr3LoadDQSPatterns(MV_DRAM_INFO *pDramInfo);

extern MV_U32 g_auiKillerPattern[DQ_NUM][LEN_KILLER_PATTERN];
extern MV_U32 g_auiSpecialPattern[DQ_NUM][LEN_SPECIAL_PATTERN];

/******************************** centralization ******************************/
/******************************************************************************
* Name:     ddr3DqsCentralizationRx.
* Desc:     Execute the DQS centralization RX phase.
* Args:     pDramInfo
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3DqsCentralizationRx(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiCs, uiEcc, uiReg;

	DEBUG_DQS_S("DDR3 - DQS Centralization RX - Starting procedure \n");

	/* Start Auto Read Leveling procedure */
	uiReg = (1 << REG_DRAM_TRAINING_AUTO_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */

	/* Enable SW override */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS));
	/* [0] = 1 - Enable SW override  */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	DEBUG_DQS_FULL_S("DDR3 - DQS Centralization RX - SW Override Enabled \n");
	
	ddr3LoadDQSPatterns(pDramInfo);

	/* Loop for each CS */
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (pDramInfo->uiCsEna & (1<<uiCs)) {

			DEBUG_DQS_FULL_C("DDR3 - DQS Centralization RX - CS - ", (MV_U32)uiCs, 1);

			for (uiEcc = 0; uiEcc < (pDramInfo->uiEccEna+1); uiEcc++) {

				/* ECC Support - Switch ECC Mux on uiEcc=1 */
				uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
				uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
				MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);

				if (uiEcc)
					DEBUG_DQS_FULL_S("DDR3 - DQS Centralization RX - ECC Mux Enabled \n");
				else
					DEBUG_DQS_FULL_S("DDR3 - DQS Centralization RX - ECC Mux Disabled \n");

				DEBUG_DQS_FULL_S("DDR3 - DQS Centralization RX - Find all limits \n");

				if (MV_OK != ddr3FindAdllLimits(pDramInfo, uiCs, uiEcc, FALSE))
					return MV_FAIL;

				DEBUG_DQS_FULL_S("DDR3 - DQS Centralization RX - Start calculating center\n");

				if (MV_OK != ddr3CenterCalc(pDramInfo, uiCs, uiEcc, FALSE))
					return MV_FAIL;
			}
		}
	}

	/* ECC Support - Disable ECC MUX */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);

	/* Disable SW override - Must be in a different stage */
	/* [0]=0 - Enable SW override  */
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_2_ADDR);
	uiReg &= ~(1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	
	/* Clear training register */
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, 0);  	/* 0x15B0 - Training Register */

	return MV_OK;
}


/******************************************************************************
* Name:     ddr3DqsCentralizationTx.
* Desc:     Execute the DQS centralization TX phase.
* Args:     pDramInfo
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3DqsCentralizationTx(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiCs, uiEcc, uiReg;

	DEBUG_DQS_S("DDR3 - DQS Centralization TX - Starting procedure \n");

	uiReg = (1 << REG_DRAM_TRAINING_AUTO_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, uiReg);  	/* 0x15B0 - Training Register */

	/* Enable SW override */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) | (1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS));
	/* [0] = 1 - Enable SW override  */
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */
	DEBUG_DQS_FULL_S("DDR3 - DQS Centralization TX - SW Override Enabled \n");
	
	/* Loop for each CS */
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (pDramInfo->uiCsEna & (1<<uiCs)) {

			DEBUG_DQS_FULL_C("DDR3 - DQS Centralization TX - CS - ", (MV_U32)uiCs, 1);
			for (uiEcc = 0; uiEcc < (pDramInfo->uiEccEna+1); uiEcc++) {

				/* ECC Support - Switch ECC Mux on uiEcc=1 */
				uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
				uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
				MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);

				if (uiEcc)
					DEBUG_DQS_FULL_S("DDR3 - DQS Centralization TX - ECC Mux Enabled \n");
				else
					DEBUG_DQS_FULL_S("DDR3 - DQS Centralization TX - ECC Mux Disabled \n");

				DEBUG_DQS_FULL_S("DDR3 - DQS Centralization TX - Find all limits \n");

				if (MV_OK != ddr3FindAdllLimits(pDramInfo, uiCs, uiEcc, TRUE))
					return MV_FAIL;

				DEBUG_DQS_FULL_S("DDR3 - DQS Centralization TX - Start calculating center\n");

				if (MV_OK != ddr3CenterCalc(pDramInfo, uiCs, uiEcc, TRUE))
					return MV_FAIL;
			}
		}
	}

	/* ECC Support - Disable ECC MUX */
	uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR, uiReg);

	/* Disable SW override - Must be in a different stage */
	/* [0]=0 - Enable SW override  */
	uiReg = MV_REG_READ(REG_DRAM_TRAINING_2_ADDR);
	uiReg &= ~(1 << REG_DRAM_TRAINING_2_SW_OVRD_OFFS);
	MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);  		/* 0x15B8 - Training SW 2 Register */

	/* Clear training register */
	MV_REG_WRITE(REG_DRAM_TRAINING_ADDR, 0);  	/* 0x15B0 - Training Register */
	
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3FindAdllLimits.
* Desc:     Execute the Find ADLL limits phase.
* Args:     pDramInfo
			uiCs
			uiEccEna
*           bIsTx             Indicate whether Rx or Tx
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3FindAdllLimits(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx)
{
	MV_U32 uiVictimDq, uiPup, uiTemp;
	MV_U32 uiAdllAddr;
	MV_U32 uiMaxPup;       	    		/* maximal pup index */
	MV_U32 uiStdPups;
	MV_U32 uiPupMask = 0;
	MV_U32 uiValidPups;    	    		/* bit array of valid pups */
	MV_U32 uiUnlockPup;  				/* bit array of un locked pups */
	MV_U32 uiNewUnlockPup;   	    	/* bit array of compare failed pups */
	MV_U32 uiCurrAdll;
	MV_U32 uiAdllEndVal;  				/* adll end of loop value - for rx or tx limit */
	MV_U32 uiAdllStartVal;				/* adll start of loop value - for rx or tx limit */
	MV_U32 uiHighLimit;        			/* holds found High Limit */
	MV_U32 uiLowLimit;         			/* holds found Low Limit */
	MV_BOOL bWinValid;
	MV_BOOL bUpdateWin;
	MV_U32 uiSdramOffset;
	MV_U32 uj, uiCsCount, uiCsTemp;

	pupADllLimitsState aePupADllLimitState[MAX_PUP_NUM];  /* hold state of each pup */

	uiAdllAddr = ((bIsTx == TRUE) ? PUP_DQS_WR : PUP_DQS_RD);
	uiAdllEndVal = ((bIsTx == TRUE) ? ADLL_MIN : ADLL_MAX);
	uiAdllStartVal = ((bIsTx == TRUE) ? ADLL_MAX : ADLL_MIN);

	if (uiEcc)
		uiValidPups = 0x1;
	else if (pDramInfo->uiNumOfStdPups == PUP_NUM_64BIT)
		uiValidPups = 0xFF;
	else if (pDramInfo->uiNumOfStdPups == PUP_NUM_32BIT)
		uiValidPups = 0xF;
	else			/* Must be 16bit */
		uiValidPups = 0x3;

	uiMaxPup = (uiEcc + (1-uiEcc)*pDramInfo->uiNumOfStdPups);
	uiStdPups = pDramInfo->uiNumOfStdPups;

	DEBUG_DQS_FULL_S("DDR3 - DQS Find Limits - Starting Find ADLL Limits \n");

	/* init the array */
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
		s_aiCentralizationLowLimit[uiPup] = ADLL_MIN;
		s_aiCentralizationHighLimit[uiPup] = ADLL_MAX; /*((bIsTx == TRUE) ? 0x3F : ADLL_MAX);  ADLL_MAX; */
	}

	/* Killer Pattern */
	uiCsCount = 0;
	for (uiCsTemp = 0; uiCsTemp < uiCs; uiCsTemp++) {
		if (pDramInfo->uiCsEna & (1<<uiCsTemp))
			uiCsCount++;
	}
	uiSdramOffset = uiCsCount * (SDRAM_CS_SIZE + 1);
	uiSdramOffset += ((bIsTx == TRUE) ? SDRAM_DQS_TX_OFFS : SDRAM_DQS_RX_OFFS);

	/* prepare pup masks */
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++)
		uiPupMask |= (1<<uiPup);

	/* loop - use different pattern for each uiVictimDq */
	for (uiVictimDq = 0; uiVictimDq < DQ_NUM; uiVictimDq++) {
		DEBUG_DQS_FULL_C("DDR3 - DQS Find Limits - Victim DQ - ", (MV_U32)uiVictimDq, 1);
		/* 	the pups 3 bit arrays represent state machine. with 3 stages for each pup.
			1. fail and didn't get pass in earlier compares.
			2. pass compare
			3. fail after pass - end state.
			The window limits are the adll values where the adll was in the pass stage. */

		/* Set all states to Fail (1st state) */
		for (uiPup = 0; uiPup < uiMaxPup; uiPup++)
			aePupADllLimitState[uiPup] = pupADllLimitsState_FAIL;

		/* set current valid pups */
		/*uiUnlockPup = uiValidPups;*/
		uiUnlockPup = uiPupMask;

		/* set ADLL to start value */
		uiCurrAdll = uiAdllStartVal;

		for (uj = 0; uj < (ADLL_MAX); uj++) {

			DEBUG_DQS_FULL_C("DDR3 - DQS Find Limits - Setting ADLL to ", uiCurrAdll, 2);
			for (uiPup = 0 ; uiPup < uiMaxPup; uiPup++) {
				if (IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE) {
					uiTemp = ((bIsTx == TRUE) ? uiCurrAdll+pDramInfo->auiWlValues[uiCs][uiPup*(1-uiEcc) + uiEcc*ECC_BIT][D] : uiCurrAdll);
					ddr3WritePupReg(uiAdllAddr, uiCs, uiPup+(uiEcc*ECC_BIT), 0, uiTemp);
				}
			}

			uiNewUnlockPup = 0; /* '1' - means pup failed, '0' - means pup pass */

			/* Read and compare results for Victim_DQ# */

			if (MV_OK != ddr3SdramCompare(pDramInfo, uiUnlockPup, &uiNewUnlockPup, g_auiKillerPattern[uiVictimDq],
			   LEN_KILLER_PATTERN, uiSdramOffset+LEN_KILLER_PATTERN*4*uiVictimDq, bIsTx, FALSE, NULL, FALSE))
//				LEN_KILLER_PATTERN, uiSdramOffset+LEN_KILLER_PATTERN*4*uiVictimDq, TRUE, FALSE, NULL, FALSE))
				return MV_FAIL;

			uiPup = 0;
			DEBUG_DQS_FULL_C("DDR3 - DQS Find Limits - UnlockPup: ", uiUnlockPup, 2);
			DEBUG_DQS_FULL_C("DDR3 - DQS Find Limits - NewUnlockPup: ", uiNewUnlockPup, 2);

			/* update pup state */
			for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
				if (IS_PUP_ACTIVE(uiUnlockPup, uiPup) == FALSE) {
					DEBUG_DQS_FULL_C("DDR3 - DQS Find Limits - Skipping pup ", uiPup, 1);
					continue;
				}

				/* still didn't find the window limit of the pup */
				if (IS_PUP_ACTIVE(uiNewUnlockPup, uiPup) == TRUE) {
					/* current compare result == fail */
					if (aePupADllLimitState[uiPup] == pupADllLimitsState_PASS) {
						/* If now it failed but passed earlier */
						DEBUG_DQS_S("DDR3 - DQS Find Limits - PASS to FAIL: CS - ");
						DEBUG_DQS_D(uiCs, 1);
						DEBUG_DQS_S(", DQ - ");
						DEBUG_DQS_D(uiVictimDq, 1);
						DEBUG_DQS_S(", Pup - ");
						DEBUG_DQS_D(uiPup, 1);
						DEBUG_DQS_S(", ADLL - ");
						DEBUG_DQS_D(uiCurrAdll, 2);
						DEBUG_DQS_S("\n");
												
						bWinValid = TRUE;
						bUpdateWin = FALSE;

						/* keep min / max limit value */
						if (bIsTx == FALSE) {
							/* RX - found upper limit */
							if (s_aiCentralizationHighLimit[uiPup] > (uiCurrAdll - 1)) {
								uiHighLimit = uiCurrAdll - 1;
								uiLowLimit = s_aiCentralizationLowLimit[uiPup];
								bUpdateWin = TRUE;
								/*s_aiCentralizationHighLimit[uiPup] = (uiCurrAdll - 1);*/
							}
						} else {
							/* TX - found lower limit */
							if (s_aiCentralizationLowLimit[uiPup] < (uiCurrAdll + 1)) {
								uiHighLimit = s_aiCentralizationHighLimit[uiPup];
								uiLowLimit = uiCurrAdll + 1;
								bUpdateWin = TRUE;
								/*s_aiCentralizationLowLimit[uiPup] = (uiCurrAdll + 1);*/
							}
						}

						if (bUpdateWin == TRUE) {
							/* before updating window limits we need to check that the limits are valid */
							if (MV_OK != ddr3CheckWindowLimits(uiPup, uiHighLimit, uiLowLimit, bIsTx, &bWinValid))
								return MV_FAIL;

							if (bWinValid == TRUE) {
								/* window limits should be updated */
								s_aiCentralizationLowLimit[uiPup] = uiLowLimit;
								s_aiCentralizationHighLimit[uiPup] = uiHighLimit;
							}
						}

						if (bWinValid == TRUE) {
							/* found end of window - lock the pup */
							aePupADllLimitState[uiPup] = pupADllLimitsState_FAIL_AFTER_PASS;
							uiUnlockPup &= ~(1 << uiPup);
						} else {
							/* probably false pass - reset status */
							aePupADllLimitState[uiPup] = pupADllLimitsState_FAIL;
						}
					}
				} else {
					/* current compare result == pass */
					if (aePupADllLimitState[uiPup] == pupADllLimitsState_FAIL) {
						/* If now it passed but failed earlier */
						DEBUG_DQS_S("DDR3 - DQS Find Limits - FAIL to PASS: CS - ");
						DEBUG_DQS_D(uiCs, 1);
						DEBUG_DQS_S(", DQ - ");
						DEBUG_DQS_D(uiVictimDq, 1);
						DEBUG_DQS_S(", Pup - ");
						DEBUG_DQS_D(uiPup, 1);
						DEBUG_DQS_S(", ADLL - ");
						DEBUG_DQS_D(uiCurrAdll, 2);
						DEBUG_DQS_S("\n");
						
						/* found start of window */
						aePupADllLimitState[uiPup] = pupADllLimitsState_PASS;

						/* keep min / max limit value */
						if (bIsTx == FALSE) {
							/* RX - found low limit */
							if (s_aiCentralizationLowLimit[uiPup] <= uiCurrAdll)
								s_aiCentralizationLowLimit[uiPup] = uiCurrAdll;
						} else {
							/* TX - found high limit */
							if (s_aiCentralizationHighLimit[uiPup] >= uiCurrAdll)
								s_aiCentralizationHighLimit[uiPup] = uiCurrAdll;
						}
					}
				}
			}

			if (uiUnlockPup == 0)	{
				/* found limit to all pups */
				DEBUG_DQS_FULL_S("DDR3 - DQS Find Limits - found PUP limit \n");
				break;
			}

		/* Increment / decrement (Move to right / left one phase - ADLL) dqs RX / TX delay (for all un lock pups */
			if (bIsTx == FALSE)
				uiCurrAdll++;
			else
				uiCurrAdll--;
		}

		if (uiUnlockPup != 0) {
			/* found pups that didn't reach to the end of the state machine */
			DEBUG_DQS_C("DDR3 - DQS Find Limits - Pups that didn't reached end of the state machine: ",
						uiUnlockPup, 1);

			for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
				if (IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE) 	{
					if (aePupADllLimitState[uiPup] == pupADllLimitsState_FAIL) {
						/* ERROR - found fail for all window size */
						DEBUG_DQS_S("DDR3 - DQS Find Limits - Got FAIL for the complete range on pup - ");
						DEBUG_DQS_D(uiPup, 1);
						DEBUG_DQS_C(" victim DQ ", uiVictimDq, 1);

						/* for debug - set min limit to illegal limit */
						s_aiCentralizationLowLimit[uiPup] = ADLL_ERROR;
						/* in case the pup is in mode PASS - the limit is the min / max adll,
						no need to update because of the results array default value */
						return MV_FAIL;
					} 
#if 0
					else {
						/* This PUP is still pass - special pattern */
						if (bIsTx == FALSE)
							s_aiCentralizationHighLimit[uiPup] = ADLL_MAX;
						else
							s_aiCentralizationLowLimit[uiPup] = 0;
					}
#endif
				}
			}
		}
	}

	DEBUG_DQS_FULL_S("DDR3 - DQS Find Limits - Ended  \n");
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3CheckWindowLimits.
* Desc:     Check window High & Low limits.
* Args: 	uiPup                pup index
*           iHighLimit           window high limit
*           iLowLimit            window low limit
*           bIsTx                Indicate whether Rx or Tx
*           pbSizeValid          Indicate whether window size is valid
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3CheckWindowLimits(MV_U32 uiPup, MV_32 iHighLimit, MV_32 iLowLimit, MV_BOOL bIsTx, MV_BOOL *pbSizeValid)
{
	DEBUG_DQS_FULL_S("DDR3 - DQS Check Win Limits - Starting \n");

	/* Check that window size is valid, if not it was probably false pass before */
	if ((iHighLimit - iLowLimit) < MIN_WIN_SIZE) {
		/* since window size is too small probably there was false pass */
		*pbSizeValid = FALSE;

		DEBUG_DQS_S("DDR3 - DQS Check Win Limits - Pup ");
		DEBUG_DQS_D(uiPup, 1);
		DEBUG_DQS_S(" Window size is smaller than MIN_WIN_SIZE \n");

	} else if ((iHighLimit - iLowLimit) > ADLL_MAX) {
		*pbSizeValid = FALSE;

		DEBUG_DQS_S("DDR3 - DQS Check Win Limits - Pup ");
		DEBUG_DQS_D(uiPup, 1);
		DEBUG_DQS_S(" Window size is bigger than max ADLL taps (31)  Exiting. \n");

		return MV_FAIL;

	} else {
		*pbSizeValid = TRUE;

		DEBUG_DQS_FULL_S("DDR3 - DQS Check Win Limits - Pup ");
		DEBUG_DQS_FULL_D(uiPup, 1);
		DEBUG_DQS_FULL_C(" window size is ", (iHighLimit - iLowLimit), 2);
	}

	return MV_OK;
}


/******************************************************************************
* Name:     ddr3CenterCalc.
* Desc:     Execute the calculate the center of windows phase.
* Args:		pDram Info
*           bIsTx             Indicate whether Rx or Tx
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

/* static */
MV_STATUS ddr3CenterCalc(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx)
{

	MV_U32 uiSpecialPatternIPup = 0;      /* bit array of pups that need specail search */
	MV_U32 uiSpecialPatternIIPup = 0;
	MV_U32 uiPup;
	MV_U32 uiMaxPup;

	uiMaxPup = (uiEcc + (1-uiEcc)*pDramInfo->uiNumOfStdPups);
	
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
		if (bIsTx == FALSE) {
		/* Check special pattern I */
		/* Special pattern Low limit search - relevant only for Rx, win size < threshold and low limit = 0 */
			
			if (((s_aiCentralizationHighLimit[uiPup]-s_aiCentralizationLowLimit[uiPup]) < VALID_WIN_THRS) &&
					(s_aiCentralizationLowLimit[uiPup] == MIN_DELAY))
				uiSpecialPatternIPup |= (1 << uiPup);
		
		/* Check special pattern II */
		/* Special pattern High limit search - relevant only for Rx, win size < threshold and high limit = 31 */
			if (((s_aiCentralizationHighLimit[uiPup] - s_aiCentralizationLowLimit[uiPup]) < VALID_WIN_THRS) &&
						(s_aiCentralizationHighLimit[uiPup] == MAX_DELAY))

				uiSpecialPatternIIPup |= (1 << uiPup);
		}
	}

	/* run Special pattern Low limit search - for relevant pup */
	if (uiSpecialPatternIPup != 0)	{
		DEBUG_DQS_S("DDR3 - DQS Center Calc - Entering special pattern I for Low limit search \n");
		if (MV_OK != ddr3SpecialPatternISearch(pDramInfo, uiCs, uiEcc, bIsTx, uiSpecialPatternIPup))
			return MV_FAIL;
	}

	/* run Special pattern High limit search - for relevant pup */
	if (uiSpecialPatternIIPup != 0)	{
		DEBUG_DQS_S("DDR3 - DQS Center Calc - Entering special pattern II for High limit search \n");
		if (MV_OK != ddr3SpecialPatternIISearch(pDramInfo, uiCs, uiEcc, bIsTx, uiSpecialPatternIIPup))
			return MV_FAIL;
	}

	/* set adll to center = (General_High_limit + General_Low_limit)/2*/
	return ddr3SetDqsCentralizationResults(pDramInfo, uiCs,  uiEcc,  bIsTx);
}



/******************************************************************************
* Name:     ddr3SpecialPatternISearch.
* Desc:     Execute special pattern low limit search.
* Args:
*           uiSpecialPatternPup  The pups that need the special search
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SpecialPatternISearch(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx,
									 MV_U32 uiSpecialPatternPup)
{
	MV_U32 uiVictimDq;              /* loop index - victim DQ */
	MV_U32 uiAdllIndex;
	MV_U32 uiPup;
	MV_U32 uiUnlockPup;             /* bit array of the unlock pups  */
	MV_U32 uiFirstFail;             /* bit array - of pups that  get first fail */
	MV_U32 uiNewLockPup;            /* bit array of compare failed pups */
	MV_U32 uiPassPups;              /* bit array of compare pass pup */
	MV_U32 uiSdramOffset;
	MV_U32 uiMaxPup;
	MV_U32 uiCompareValue;
	MV_U32 auiSpecialResult[MAX_PUP_NUM]; /* hold temp results */

	DEBUG_DQS_S("DDR3 - DQS - Special Pattern I Search - Starting  \n");

	uiMaxPup = (uiEcc + (1-uiEcc)*pDramInfo->uiNumOfStdPups);

	/* init the temporary results to max ADLL value */
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++)
		auiSpecialResult[uiPup] = ADLL_MAX;

	

	/* run special pattern for all DQ - use the same pattern */
	for (uiVictimDq = 0; uiVictimDq < DQ_NUM; uiVictimDq++) {
		uiUnlockPup = uiSpecialPatternPup;
		uiFirstFail = 0;
		
		uiSdramOffset = uiCs*SDRAM_CS_SIZE + SDRAM_DQS_RX_OFFS + LEN_KILLER_PATTERN*4*uiVictimDq;
		
		for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
			/* Set adll value per PUP. adll = high limit per pup */
			if (IS_PUP_ACTIVE(uiUnlockPup, uiPup)) {
				/* only for pups that need special search */
				ddr3WritePupReg(PUP_DQS_RD, uiCs, uiPup, 0, s_aiCentralizationHighLimit[uiPup]);
			}
		}

		uiAdllIndex = 0;
		do {
		/* Perform read and compare simultaneously for all  un-locked MC use the special pattern mask */
			uiNewLockPup = 0;

			if (MV_OK != ddr3SdramCompare(pDramInfo, uiUnlockPup, &uiNewLockPup,
				g_auiSpecialPattern[uiVictimDq],LEN_SPECIAL_PATTERN, uiSdramOffset, FALSE,
//				g_auiSpecialPattern[uiVictimDq],LEN_SPECIAL_PATTERN, uiSdramOffset, TRUE,
				FALSE, NULL, TRUE)) /*TRUE, s_auiSpecialPatternMask); */
					return MV_FAIL;

			DEBUG_DQS_S("DDR3 - DQS - Special I - ADLL value is: ");
			DEBUG_DQS_D(uiAdllIndex, 2);
			DEBUG_DQS_S(", UnlockPup: ");
			DEBUG_DQS_D(uiUnlockPup, 2);
			DEBUG_DQS_S(", NewLockPup: ");
			DEBUG_DQS_D(uiNewLockPup, 2);
			DEBUG_DQS_S("\n");

			if (uiUnlockPup != uiNewLockPup)
				DEBUG_DQS_S("DDR3 - DQS - Special I - Some Pup passed!\n");

			/* search for pups with passed compare & already fail */
			uiPassPups = uiFirstFail & ~uiNewLockPup & uiUnlockPup;
			uiFirstFail |= uiNewLockPup;
			uiUnlockPup &= ~uiPassPups;


			/* get pass pups */
			if (uiPassPups != 0) 	{
				for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
					if (IS_PUP_ACTIVE(uiPassPups, uiPup) == TRUE) {
						/* If pup passed and has first fail = 1 */
						/* keep min value of ADLL max value - current adll */
						/* (s_aiCentralizationHighLimit[uiPup] + uiAdllIndex) = current adll !!! */
						uiCompareValue = (ADLL_MAX - (s_aiCentralizationHighLimit[uiPup] + uiAdllIndex));

						DEBUG_DQS_C("DDR3 - DQS - Special I - Pup - ", uiPup, 1);
						DEBUG_DQS_C(" uiCompareValue = ", uiCompareValue, 2);

						if (uiCompareValue < auiSpecialResult[uiPup]) {
							auiSpecialResult[uiPup] = uiCompareValue;
							s_aiCentralizationLowLimit[uiPup] = (-1) * uiCompareValue;

							DEBUG_DQS_C("DDR3 - DQS - Special I - Pup - ", uiPup, 1);
							DEBUG_DQS_C(" Changed Low limit to ", s_aiCentralizationLowLimit[uiPup], 2);
						}
					}
				}
			}

/* 	Did all PUP found missing window?
	check for each pup if adll (different for each pup) reach maximum if reach max value - lock the pup
	if not - increment (Move to right one phase - ADLL) dqs RX delay  */

			uiAdllIndex++;
			for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
				if (IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE) {
					/* check only unlocked pups */
					if ((s_aiCentralizationHighLimit[uiPup] + uiAdllIndex) >= ADLL_MAX) {
						/* reach maximum - lock the pup */
						DEBUG_DQS_C("DDR3 - DQS - Special I - reach maximum - lock pup ", uiPup, 1);
						uiUnlockPup &= 	~(1 << uiPup);

					} else 	{
						/* didn't reach maximum - increment ADLL */
						ddr3WritePupReg(PUP_DQS_RD, uiCs, uiPup, 0,
							(s_aiCentralizationHighLimit[uiPup] + uiAdllIndex));
					}
				}
			}
		} while (uiUnlockPup != 0);
	}
	return MV_OK;
}



/******************************************************************************
* Name:     ddr3SpecialPatternIISearch.
* Desc:     Execute special pattern high limit search.
* Args:
*           uiSpecialPatternPup  The pups that need the special search
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SpecialPatternIISearch(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx,
									 MV_U32 uiSpecialPatternPup)
{
	MV_U32 uiVictimDq;              /* loop index - victim DQ */
	MV_U32 uiAdllIndex;
	MV_U32 uiPup;
	MV_U32 uiUnlockPup;             /* bit array of the unlock pups  */
	MV_U32 uiFirstFail;             /* bit array - of pups that  get first fail */
	MV_U32 uiNewLockPup;            /* bit array of compare failed pups */
	MV_U32 uiPassPups;              /* bit array of compare pass pup */
	MV_U32 uiSdramOffset;
	MV_U32 uiMaxPup;
	MV_U32 uiCompareValue;
	MV_U32 auiSpecialResult[MAX_PUP_NUM]; /* hold temp results */

	DEBUG_DQS_S("DDR3 - DQS - Special Pattern II Search - Starting  \n");

	uiMaxPup = (uiEcc + (1-uiEcc)*pDramInfo->uiNumOfStdPups);

	/* init the temporary results to max ADLL value */
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++)
		auiSpecialResult[uiPup] = ADLL_MAX;

	uiSdramOffset = uiCs*SDRAM_CS_SIZE + SDRAM_DQS_RX_OFFS;

	/* run special pattern for all DQ - use the same pattern */
	for (uiVictimDq = 0; uiVictimDq < DQ_NUM; uiVictimDq++) {
		uiUnlockPup = uiSpecialPatternPup;
		uiFirstFail = 0;

		for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
			/* Set adll value per PUP. adll = 0 */
			if (IS_PUP_ACTIVE(uiUnlockPup, uiPup)) {
				/* only for pups that need special search */
				ddr3WritePupReg(PUP_DQS_RD, uiCs, uiPup, 0, ADLL_MIN);
			}
		}

		uiAdllIndex = 0;
		do {
			/* Perform read and compare simultaneously for all un-locked MC use the special pattern mask */
			uiNewLockPup = 0;

			if (MV_OK != ddr3SdramCompare(pDramInfo, uiUnlockPup, &uiNewLockPup, g_auiSpecialPattern[uiVictimDq],
				LEN_SPECIAL_PATTERN, uiSdramOffset, FALSE , FALSE, NULL, FALSE)) /*TRUE, s_auiSpecialPatternMask); */
//				LEN_SPECIAL_PATTERN, uiSdramOffset, TRUE , FALSE, NULL, FALSE)) /*TRUE, s_auiSpecialPatternMask); */
					return MV_FAIL;

			DEBUG_DQS_S("DDR3 - DQS - Special II - ADLL value is ");
			DEBUG_DQS_D(uiAdllIndex, 2);
			DEBUG_DQS_S("uiUnlockPup ");
			DEBUG_DQS_D(uiUnlockPup, 1);
			DEBUG_DQS_S("uiNewLockPup ");
			DEBUG_DQS_D(uiNewLockPup, 1);
			DEBUG_DQS_S("\n");

			if (uiUnlockPup != uiNewLockPup)
				DEBUG_DQS_S("DDR3 - DQS - Special II - Some Pup passed!\n");

			/* search for pups with passed compare & already fail */
			uiPassPups = uiFirstFail & ~uiNewLockPup & uiUnlockPup;
			uiFirstFail |= uiNewLockPup;
			uiUnlockPup &= ~uiPassPups;

			/* get pass pups */
			if (uiPassPups != 0) {
				for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
					if (IS_PUP_ACTIVE(uiPassPups, uiPup) == TRUE) {
						/* If pup passed and has first fail = 1 */
						/* keep min value of ADLL max value - current adll */
						/* (uiAdllIndex) = current adll !!! */
						uiCompareValue = uiAdllIndex;

						DEBUG_DQS_C("DDR3 - DQS - Special II - Pup - ", uiPup, 1);
						DEBUG_DQS_C(" uiCompareValue = ", uiCompareValue, 1);

						if (uiCompareValue < auiSpecialResult[uiPup]) {
							auiSpecialResult[uiPup] = uiCompareValue;
							s_aiCentralizationHighLimit[uiPup] = ADLL_MAX + uiCompareValue;

							DEBUG_DQS_C("DDR3 - DQS - Special II - Pup - ", uiPup, 1);
							DEBUG_DQS_C(" Changed High limit to ", s_aiCentralizationHighLimit[uiPup], 2);
						}
					}
				}
			}

/* 	Did all PUP found missing window?
	check for each pup if adll (different for each pup) reach maximum if reach max value - lock the pup
	if not - increment (Move to right one phase - ADLL) dqs RX delay  */

			uiAdllIndex++;
			for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
				if (IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE) {
					/* check only unlocked pups */
					if ((uiAdllIndex) >= ADLL_MAX) {
						/* reach maximum - lock the pup */
						DEBUG_DQS_C("DDR3 - DQS - Special II - reach maximum - lock pup ", uiPup, 1);
						uiUnlockPup &= 	~(1 << uiPup);

					} else 	{
						/* didn't reach maximum - increment ADLL */
						ddr3WritePupReg(PUP_DQS_RD, uiCs, uiPup, 0, (uiAdllIndex));
					}
				}
			}
		} while (uiUnlockPup != 0);
	}
	return MV_OK;
}


/******************************************************************************
* Name:     ddr3SetDqsCentralizationResults.
* Desc:     Set to HW the DQS centralization phase results.
* Args:
*           bIsTx             Indicates whether to set Tx or RX results
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SetDqsCentralizationResults(MV_DRAM_INFO *pDramInfo, MV_U32 uiCs, MV_U32 uiEcc, MV_BOOL bIsTx)
{
	MV_U32 uiPup, uiPupNum;
	MV_32  iAddlVal;
	MV_U32 uiMaxPup;

	uiMaxPup = (uiEcc + (1-uiEcc)*pDramInfo->uiNumOfStdPups);

	if (bIsTx) {
		DEBUG_DQS_C("DDR3 - DQS TX - Set Dqs Centralization Results - CS: ", uiCs, 1);
	} else {
		DEBUG_DQS_C("DDR3 - DQS RX - Set Dqs Centralization Results - CS: ", uiCs, 1);
	}
	
	/* set adll to center = (General_High_limit + General_Low_limit)/2 */
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
		iAddlVal = ((s_aiCentralizationHighLimit[uiPup] + s_aiCentralizationLowLimit[uiPup]) / 2);

		if (bIsTx)
			DEBUG_DQS_S("DDR3 - DQS TX - PUP - ");
		else
			DEBUG_DQS_S("DDR3 - DQS RX - PUP - ");

		DEBUG_DQS_D(uiPup, 1);
		DEBUG_DQS_S(", High - ");
		DEBUG_DQS_D(s_aiCentralizationHighLimit[uiPup], 2);
		DEBUG_DQS_S(", Low - ");
		DEBUG_DQS_D(s_aiCentralizationLowLimit[uiPup], 2);
		DEBUG_DQS_S(", WinSize - ");
		DEBUG_DQS_D(s_aiCentralizationHighLimit[uiPup]-s_aiCentralizationLowLimit[uiPup], 2);
		DEBUG_DQS_S(", Set - ");
		DEBUG_DQS_D(iAddlVal, 2);
		DEBUG_DQS_S("\n");

		if (iAddlVal < ADLL_MIN) {
			iAddlVal = ADLL_MIN;
			DEBUG_DQS_S("DDR3 - DQS - Setting ADLL value for Pup to MIN (since it was lower than 0)\n");
		}
		if (iAddlVal > ADLL_MAX) {
			iAddlVal = ADLL_MAX;
			DEBUG_DQS_S("DDR3 - DQS - Setting ADLL value for Pup to MAX (since it was higher than 31)\n");
		}

		uiPupNum = uiPup*(1-uiEcc) + uiEcc*ECC_BIT;

		if (bIsTx)
			ddr3WritePupReg(PUP_DQS_WR, uiCs, uiPupNum, 0, iAddlVal+pDramInfo->auiWlValues[uiCs][uiPupNum][D]);
		else
			ddr3WritePupReg(PUP_DQS_RD, uiCs, uiPupNum, 0, iAddlVal);
	}


#if 0
	for (uiPup = 0; uiPup < uiMaxPup; uiPup++) {
		if (s_aiCentralizationHighLimit[uiPup] < s_aiCentralizationLowLimit[uiPup]) 	{

			"Pup %d: High limit (%d) is lower than Low limit (%d)\n", uiPup,
					s_aiCentralizationHighLimit[uiPup], s_aiCentralizationLowLimit[uiPup]);
			return MV_FAIL;
		} else if ((s_aiCentralizationHighLimit[uiPup] - aiCentralizationLowLimit[uiPup]) < MIN_WIN_SIZE) {
			"Pup %d: Window size is smaller than %d\n", uiPup, MIN_WIN_SIZE);
			return MV_FAIL;
		}
	}
#endif
	return MV_OK;
}

/*
 * Set training patterns
 */
MV_STATUS ddr3LoadDQSPatterns(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg, uiCs, uiCsCount, uiCsTemp, ui, uiVictimDq, uiEcc;
	MV_U32 auiSdramAddr;

	/* Loop for each CS */
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (pDramInfo->uiCsEna & (1<<uiCs)) {

			uiCsCount = 0;
			for (uiCsTemp = 0; uiCsTemp < uiCs; uiCsTemp++) {
				if (pDramInfo->uiCsEna & (1<<uiCsTemp))
					uiCsCount++;
			}

			/* Init killer pattern */
			auiSdramAddr = (uiCsCount*(SDRAM_CS_SIZE+1) + SDRAM_DQS_RX_OFFS);
			for (uiVictimDq = 0; uiVictimDq < DQ_NUM; uiVictimDq++) {
				if (MV_OK != ddr3SdramCompare(pDramInfo, NULL, NULL, g_auiKillerPattern[uiVictimDq],
					LEN_KILLER_PATTERN, auiSdramAddr+LEN_KILLER_PATTERN*4*uiVictimDq, TRUE, FALSE, NULL, FALSE))
					return MV_FAIL;
			}

			/* Init special-killer pattern */
			auiSdramAddr = (uiCsCount*(SDRAM_CS_SIZE+1) + SDRAM_DQS_RX_SPECIAL_OFFS);
			for (uiVictimDq = 0; uiVictimDq < DQ_NUM; uiVictimDq++) {
				if (MV_OK != ddr3SdramCompare(pDramInfo, NULL, NULL, g_auiSpecialPattern[uiVictimDq],
					LEN_KILLER_PATTERN, auiSdramAddr+LEN_KILLER_PATTERN*4*uiVictimDq, TRUE, FALSE, NULL, FALSE))
					return MV_FAIL;
			}
		}
	}

	return MV_OK;
}


