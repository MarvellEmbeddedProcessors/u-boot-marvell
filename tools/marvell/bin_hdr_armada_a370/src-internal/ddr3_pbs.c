/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement uiRegarding license alternatives, (ii) delete the two
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

//#define S 2
#define STATES 3
#define RETEST_COUNTER 4
#define FINAL_STATE 3

/*
 * Debug
*/
#define DEBUG_PBS_C(s,d,l)        	DEBUG_PBS_S(s); DEBUG_PBS_D(d,l); DEBUG_PBS_S("\n")

#ifdef 	MV_DEBUG_PBS
#define DEBUG_PBS_S(s)				putstring(s)
#define DEBUG_PBS_D(d,l)			putdata(d,l)

#else
#define DEBUG_PBS_S(s)		
#define DEBUG_PBS_D(d,l)	
#endif

#ifdef MV88F78X60

static MV_U32 s_auiSkewArray[(MAX_PUP_NUM)*DQ_NUM] = {0};				/* Temp array for skew data storage */

/* PBS locked dq (per pup) */
extern MV_U32 s_auiPbsLockedDq[MAX_PUP_NUM][DQ_NUM];
extern MV_U32 s_auiPbsLockedValue[MAX_PUP_NUM][DQ_NUM];

extern MV_U32 g_auiPbsPattern[2][LEN_PBS_PATTERN];
extern MV_U32 s_auiPbsDqMapping[PUP_NUM_64BIT][DQ_NUM];

MV_STATUS 	ddr3TxShiftDqAdllStepBeforeFail(MV_DRAM_INFO *pDramInfo,MV_U32 uiCurrPup, MV_U32 uiPbsPatternIdx, MV_U32 uiEcc);
MV_STATUS 	ddr3RxShiftDqsToFirstFail(MV_DRAM_INFO *pDramInfo,MV_U32 uiCurrPup, MV_U32 uiPbsPatternIdx, MV_U32 uiEcc);
MV_STATUS 	ddr3PbsPerBit(MV_DRAM_INFO *pDramInfo, MV_BOOL *pbStartOver, MV_BOOL bIsTx, MV_U32 *puiCurrPup, MV_U32 	
				uiPbsPatternIdx, MV_U32 uiEcc);
MV_STATUS	ddr3LoadPbsPatterns(MV_DRAM_INFO *pDramInfo);

MV_STATUS	ddr3SetPbsResults (MV_DRAM_INFO *pDramInfo,  MV_BOOL bIsTx );
MV_VOID 	ddr3PbsWritePupDQSReg(MV_U32 uiCs, MV_U32 uiPup, MV_U32 uiDQSDelay);


/******************************************************************************
* Name:		ddr3PbsTx.
* Desc:		Execute the PBS TX phase.
* Args:		pDramInfo	ddr3 training information struct
* Notes:
* Returns: 	MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3PbsTx (MV_DRAM_INFO *pDramInfo)
{
	/* array of Deskew results */
	
	/* array to hold the total sum of skew from all iterations (for average purpose) */
	MV_U32 auiSkewSumArray[MAX_PUP_NUM][DQ_NUM] = {{0}};

	/* array to hold the total average skew from both patterns (for average purpose) */
	MV_U32 auiPatternsSkewArray[MAX_PUP_NUM][DQ_NUM] = {{0}};

	MV_U32 uiPbsRepTime = 0;     /* counts number of loop in case of fail */
	MV_BOOL bStartOver;           /* indicates whether we need to start the loop again */
	MV_U32 uiCurrPup;            /* bit array for unlock pups - used to repeat on the RX operation */
	MV_U32 uiMaxPup;
	MV_U32 uiPbsRetry;
	MV_U32 uiPup,uiPups, uiDq, uiCurMaxPup, validPups,uiReg;
	MV_U32 uiPatternIdx;
	MV_U32 uiEcc;


	DEBUG_PBS_S("DDR3 - PBS TX - Starting PBS TX procedure \n");

	uiPups = pDramInfo->uiNumOfTotalPups;
	uiMaxPup = pDramInfo->uiNumOfTotalPups;

/* Running twice for 2 different patterns. each patterns - 3 times */
	for (uiPatternIdx = 0; uiPatternIdx < COUNT_PBS_PATTERN; uiPatternIdx++ ) {

		DEBUG_PBS_C("DDR3 - PBS TX - Working with pattern - ",uiPatternIdx,1);

	/* Reset sum array */
		for ( uiPup = 0; uiPup < uiPups; uiPup++ ) {
			for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				auiSkewSumArray[uiPup][uiDq] = 0;
			}
		}

	/* perform PBS several of times (3 for each pattern). at the end, we'll use the average */
	/* If there is ECC, do each PBS again with mux change */
		for (uiPbsRetry = 0; uiPbsRetry < COUNT_PBS_REPEAT; uiPbsRetry++) {
			for(uiEcc=0;uiEcc<(pDramInfo->uiEccEna+1);uiEcc++) {
				
				/* This parameter stores the current PUP num - uiEcc mode dependent - 4-8 / 1 pups */
				uiCurMaxPup = (1-uiEcc)*pDramInfo->uiNumOfStdPups + uiEcc;
			
				if(uiEcc) 
					validPups = 0x1;	/* Only 1 pup in this case */
				else if(uiCurMaxPup > 4)
					validPups = 0xFF;	/* 64 bit - 8 pups */
				else if(uiCurMaxPup == 4)
					validPups = 0xF;	/* 32 bit - 4 pups */
				else
					validPups = 0x3;	/* 16 bit - 2 pups */
					
			/* ECC Support - Switch ECC Mux on uiEcc=1 */
				uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
				uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
				MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);
	
				if(uiEcc)
					DEBUG_PBS_S("DDR3 - PBS Tx - ECC Mux Enabled \n");
				else
					DEBUG_PBS_S("DDR3 - PBS Tx - ECC Mux Disabled \n");
			
			/* init iteration values */
				/* Clear the locked DQs */
				for ( uiPup = 0; uiPup < uiCurMaxPup; uiPup++ ) {
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						s_auiPbsLockedDq[uiPup+(uiEcc*uiCurMaxPup)][uiDq] = FALSE; 
					}
				}
				uiPbsRepTime = 0;
				uiCurrPup = validPups;
				bStartOver = FALSE;
				
			/* Run loop On current Pattern and current pattern iteration (just to cover the false fail problem) */
				do {
					DEBUG_PBS_S("DDR3 - PBS Tx - Pbs Rep Loop is ");	
					DEBUG_PBS_D(uiPbsRepTime,1);
					DEBUG_PBS_S(", for Retry No.");
					DEBUG_PBS_D(uiPbsRetry,1);
					DEBUG_PBS_S("\n");
	
				/* Set all PBS values to MIN (0) */
					DEBUG_PBS_S("DDR3 - PBS Tx - Set all PBS values to MIN \n");
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,PUP_BC,0,0);
					}
							
				/* Shift DQ ADLL right, One step before fail */
					DEBUG_PBS_S("DDR3 - PBS Tx - ADLL shift right one phase before fail\n");
					if(MV_OK != ddr3TxShiftDqAdllStepBeforeFail(pDramInfo, uiCurrPup, uiPatternIdx,uiEcc))
						return MV_FAIL;
	
				/* PBS For each bit*/
					DEBUG_PBS_S("DDR3 - PBS Tx - perform PBS for each bit\n");
	
					/* in this stage - bStartOver = FALSE; */
					if(MV_OK != ddr3PbsPerBit(pDramInfo, &bStartOver, TRUE, &uiCurrPup, uiPatternIdx,uiEcc))
						return MV_FAIL;
				
				} while ((bStartOver == TRUE) && (++uiPbsRepTime < COUNT_PBS_STARTOVER));
				
				if ( uiPbsRepTime == COUNT_PBS_STARTOVER && bStartOver == TRUE ) {
					DEBUG_PBS_S("DDR3 - PBS Tx - FAIL - Adll reach max value \n");
					return MV_FAIL;
				}
				
				DEBUG_PBS_C("DDR3 - PBS TX - values for iteration - ",uiPbsRetry,1);
				for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
					/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif
					
					for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						/* set skew value for all uiDq */
						/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
						DEBUG_PBS_S("DQ");	
						DEBUG_PBS_D(uiDq,1);
						DEBUG_PBS_S("-");
						DEBUG_PBS_D(s_auiSkewArray[((uiPup+(uiEcc*uiMaxPup)) * DQ_NUM) + uiDq], 2);
						DEBUG_PBS_S(", ");
					}
					DEBUG_PBS_S("\n");
				}

			/* collect the results we got on this trial of PBS */
				for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++) 
						auiSkewSumArray[uiPup+(uiEcc*uiMaxPup)][uiDq] += 
							s_auiSkewArray[(uiPup+(uiEcc*uiMaxPup))*DQ_NUM + uiDq];
				}
			}
		}

		/* calculate the average skew for current pattern for each pup and each bit*/
		DEBUG_PBS_C("DDR3 - PBS TX - Average for pattern - ",uiPatternIdx,1);
		for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				auiPatternsSkewArray[uiPup][uiDq] += (auiSkewSumArray[uiPup][uiDq] / COUNT_PBS_REPEAT);
			}
		}
		
		for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif				
			for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				/* set skew value for all uiDq */
				/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
				DEBUG_PBS_S("DQ");	
				DEBUG_PBS_D(uiDq,1);
				DEBUG_PBS_S("-");
				DEBUG_PBS_D(auiSkewSumArray[uiPup][uiDq] / COUNT_PBS_REPEAT, 2);
				DEBUG_PBS_S(", ");
			}
			DEBUG_PBS_S("\n");
		}
	}
	
	/* calculate the average skew */
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
		for(uiDq = 0; uiDq < DQ_NUM; uiDq++) 
			s_auiSkewArray[((uiPup) * DQ_NUM) + uiDq] = auiPatternsSkewArray[uiPup][uiDq] / COUNT_PBS_PATTERN;
	}

		
	DEBUG_PBS_S("DDR3 - PBS TX - Average for all patterns: \n");
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
		/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif
					
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
			/* set skew value for all uiDq */
			/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
			DEBUG_PBS_S("DQ");	
			DEBUG_PBS_D(uiDq,1);
			DEBUG_PBS_S("-");
			DEBUG_PBS_D(s_auiSkewArray[(uiPup * DQ_NUM) + uiDq], 2);
			DEBUG_PBS_S(", ");
		}
		DEBUG_PBS_S("\n");
	}
		
/* Return ADLL to default value */
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ )
		ddr3PbsWritePupDQSReg(CS0,uiPup, INIT_WL_DELAY);

/* Set averaged PBS results */
	ddr3SetPbsResults(pDramInfo, TRUE);

	DEBUG_PBS_S("DDR3 - PBS Tx - PBS TX ended successfuly \n");
	
	return MV_OK;
}


/******************************************************************************
* Name:		ddr3TxShiftDqAdllStepBeforeFail.
* Desc:		Execute the Tx shift DQ phase.
* Args:		pDramInfo			ddr3 training information struct
* 			uiCurrPup       	bit array of the function active pups.
*			uiPbsPatternIdx 	Index of PBS pattern
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3TxShiftDqAdllStepBeforeFail(MV_DRAM_INFO *pDramInfo,MV_U32 uiCurrPup, MV_U32 uiPbsPatternIdx, MV_U32 uiEcc) 
{
	MV_U32 uiUnlockPup;        			/* bit array of unlock pups  */
	MV_U32 uiNewLockPup;              		/* bit array of compare failed pups */
	MV_U32 uiAdllVal = INIT_WL_DELAY;
	MV_U32 uiMaxPup,uiPup;
	MV_U32 uiStdPups;
	MV_U32 uiDqsDlySet[MAX_PUP_NUM] = {0};
		
	uiStdPups = pDramInfo->uiNumOfStdPups;
	
	/* Set current pup number */ 
	if(uiCurrPup == 0x1)		/* Ecc mode */
		   uiMaxPup = 1;
	else 
		uiMaxPup = pDramInfo->uiNumOfStdPups;
	
	uiUnlockPup = uiCurrPup;	/* '1' for each unlocked pup */
	
	/* Loop on all ADLL Vaules */
	do {
		/* loop until found first fail */
		
		uiAdllVal++;

		/* Increment (Move to right - ADLL) DQ TX delay (broadcast to all Data PUPs) */
		for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ )
			ddr3PbsWritePupDQSReg(CS0,uiPup, uiAdllVal);

		/* Write and Read, compare results (read was already verified) */
		/* 0 - all locked */
		uiNewLockPup = 0; 
		
		if ( MV_OK != ddr3SdramCompare(pDramInfo,uiUnlockPup,&uiNewLockPup, (MV_U32 *)&g_auiPbsPattern[uiPbsPatternIdx],LEN_PBS_PATTERN,
			SDRAM_PBS_TX_OFFS,MV_TRUE, MV_FALSE, NULL, MV_FALSE))
			return MV_FAIL;

		uiUnlockPup &= ~uiNewLockPup;

		DEBUG_PBS_S("Shift DQS by 2 steps for PUPs: ");
		DEBUG_PBS_D(uiUnlockPup, 2);
		DEBUG_PBS_C(", Set ADLL value = ", (uiAdllVal - 2), 2);

		
		/* if any PUP failed there is '1' to mark the PUP */
		if ( uiNewLockPup != 0 ) { 	
			/* Decrement (Move Back to Left two steps - ADLL) DQ TX delay for current failed pups and save*/
			for(uiPup=0;uiPup<uiMaxPup;uiPup++) {
				if(((uiNewLockPup >> uiPup	) & BIT0) && uiDqsDlySet[uiPup]==0)
					uiDqsDlySet[uiPup] = uiAdllVal - 3;
//					uiDqsDlySet[uiPup] = uiAdllVal -1 ;
			}
		}
	} while ( ( uiUnlockPup != 0 ) && ( uiAdllVal != ADLL_MAX ) );

	if ( uiUnlockPup != 0 ) {
		
		DEBUG_PBS_S("DDR3 - PBS Tx - Shift DQ - Adll value reached maximum\n");
		
		for(uiPup=0;uiPup<uiMaxPup;uiPup++) {
			if(((uiUnlockPup >> uiPup) & BIT0) && uiDqsDlySet[uiPup]==0)
				uiDqsDlySet[uiPup] = uiAdllVal - 3;
//				uiDqsDlySet[uiPup] = uiAdllVal -1;
		}
	}

	DEBUG_PBS_C("PBS TX one step before fail last pups locked Adll ", uiAdllVal - 2,2);
			 
	/* Set the PUP DQS DLY Values */
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ )
		ddr3PbsWritePupDQSReg(CS0,uiPup, uiDqsDlySet[uiPup]);
//				ddr3WritePupReg(PUP_DQS_WR,CS0,uiPup+uiEcc*uiStdPups,0,uiDqsDlySet[uiPup]);


	/* found one phase before fail */
	return MV_OK;
}


/******************************************************************************
* Name:		ddr3PbsRx.
* Desc:		Execute the PBS RX phase.
* Args:		pDramInfo	ddr3 training information struct
* Notes:
* Returns: 	MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3PbsRx (MV_DRAM_INFO *pDramInfo)
{
	
	/* array to hold the total sum of skew from all iterations (for average purpose) */
	MV_U32 auiSkewSumArray[MAX_PUP_NUM][DQ_NUM] = {{0}};

	/* array to hold the total average skew from both patterns (for average purpose) */
	MV_U32 auiPatternsSkewArray[MAX_PUP_NUM][DQ_NUM] = {{0}};

	MV_U32 uiPbsRepTime = 0;     /* counts number of loop in case of fail */
	MV_U32 uiCurrPup;            /* bit array for unlock pups - used to repeat on the RX operation */
	MV_U32 uiMaxPup;
	MV_U32 uiPbsRetry;
	MV_U32 uiPup, uiDq, uiPups, uiCurMaxPup	, validPups, uiReg	;
	MV_U32 uiPatternIdx;
	MV_U32 uiEcc;
	MV_BOOL bStartOver;           /* indicates whether we need to start the loop again */
	
	DEBUG_PBS_S("DDR3 - PBS RX - Starting PBS RX procedure \n");

	uiPups = pDramInfo->uiNumOfTotalPups;
	uiMaxPup = pDramInfo->uiNumOfTotalPups;

	ddr3LoadPbsPatterns(pDramInfo);

	/* Running twice for 2 different patterns. each patterns - 3 times */
	for (uiPatternIdx = 0; uiPatternIdx < COUNT_PBS_PATTERN; uiPatternIdx++ ) {

		DEBUG_PBS_C("DDR3 - PBS RX - Working with pattern - ",uiPatternIdx,1);

		/* Reset sum array */
		for ( uiPup = 0; uiPup < uiPups; uiPup++ ) {
			for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				auiSkewSumArray[uiPup][uiDq] = 0;
			}
		}

		/* perform PBS several of times (3 for each pattern). at the end, we'll use the average */
		/* If there is ECC, do each PBS again with mux change */
		for (uiPbsRetry = 0; uiPbsRetry < COUNT_PBS_REPEAT; uiPbsRetry++) {
			for(uiEcc=0;uiEcc<(pDramInfo->uiEccEna+1);uiEcc++) {

				/* This parameter stores the current PUP num - uiEcc mode dependent - 4-8 / 1 pups */
				uiCurMaxPup = (1-uiEcc)*pDramInfo->uiNumOfStdPups + uiEcc;

				if(uiEcc) 
					validPups = 0x1;	/* Only 1 pup in this case */
				else if(uiCurMaxPup > 4)
					validPups = 0xFF;	/* 64 bit - 8 pups */
				else if(uiCurMaxPup == 4)
					validPups = 0xF;	/* 32 bit - 4 pups */
				else
					validPups = 0x3;	/* 16 bit - 2 pups */
				
				/* ECC Support - Switch ECC Mux on uiEcc=1 */
				uiReg = (MV_REG_READ(REG_DRAM_TRAINING_2_ADDR) & ~(1 << REG_DRAM_TRAINING_2_ECC_MUX_OFFS));
				uiReg |= (pDramInfo->uiEccEna*uiEcc << REG_DRAM_TRAINING_2_ECC_MUX_OFFS);
				MV_REG_WRITE(REG_DRAM_TRAINING_2_ADDR,uiReg);
	
				if(uiEcc)
					DEBUG_PBS_S("DDR3 - PBS Rx - ECC Mux Enabled \n");
				else
					DEBUG_PBS_S("DDR3 - PBS Rx - ECC Mux Disabled \n");
			
				/* init iteration values */
				/* Clear the locked DQs */
				for ( uiPup = 0; uiPup < uiCurMaxPup; uiPup++ ) {
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						s_auiPbsLockedDq[uiPup + uiEcc*uiCurMaxPup][uiDq] = FALSE; 
					}
				}
				uiPbsRepTime = 0;
				uiCurrPup = validPups;
				bStartOver = FALSE;
				
				/* Run loop On current Pattern and current pattern iteration (just to cover the false fail problem (max */
				do {
					DEBUG_PBS_S("DDR3 - PBS Rx - Pbs Rep Loop is ");	
					DEBUG_PBS_D(uiPbsRepTime,1);
					DEBUG_PBS_S(", for Retry No.");
					DEBUG_PBS_D(uiPbsRetry,1);
					DEBUG_PBS_S("\n");
	
					/* Set all PBS values to MAX (31) */
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++)
						ddr3WritePupReg(PUP_PBS_RX+uiDq,CS0,PUP_BC,0,MAX_PBS);

					/* Set all DQS PBS values to MIN (0) */
					ddr3WritePupReg(PUP_PBS_RX+DQ_NUM,CS0,PUP_BC,0,0);

					/* Shift DQS, To first Fail */
					DEBUG_PBS_S("DDR3 - PBS Rx - Shift RX DQS to first fail \n");

					if(MV_OK != ddr3RxShiftDqsToFirstFail(pDramInfo, uiCurrPup, uiPatternIdx, uiEcc))
						return MV_FAIL;

					/* PBS For each bit*/
					DEBUG_PBS_S("DDR3 - PBS Rx - perform PBS for each bit\n");
					/* in this stage - bStartOver = FALSE; */
					if(MV_OK != ddr3PbsPerBit(pDramInfo, &bStartOver, FALSE, &uiCurrPup, uiPatternIdx, uiEcc))
						return MV_FAIL;
				
				} while ((bStartOver == TRUE) && (++uiPbsRepTime < COUNT_PBS_STARTOVER));
				
				if (uiPbsRepTime == COUNT_PBS_STARTOVER && bStartOver == TRUE) {
					DEBUG_PBS_S("DDR3 - PBS Rx - FAIL - Algorithm failed doing RX PBS \n");
					return MV_FAIL;
				}

				/* Return DQS ADLL to default value - 15 */
				ddr3WritePupReg(PUP_DQS_RD,CS0,PUP_BC,0,INIT_RL_DELAY);
								
				DEBUG_PBS_C("DDR3 - PBS RX - values for iteration - ",uiPbsRetry,1);
				for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
					/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif
					
					for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						/* set skew value for all uiDq */
						/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
						DEBUG_PBS_S("DQ");	
						DEBUG_PBS_D(uiDq,1);
						DEBUG_PBS_S("-");
						DEBUG_PBS_D(s_auiSkewArray[((uiPup+(uiEcc*uiMaxPup)) * DQ_NUM) + uiDq], 2);
						DEBUG_PBS_S(", ");
					}
					DEBUG_PBS_S("\n");
				}
				
				/* collect the results we got on this trial of PBS */
				for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
					for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						auiSkewSumArray[uiPup+(uiEcc*uiMaxPup)][uiDq] += s_auiSkewArray[((uiPup+(uiEcc*uiMaxPup)) * DQ_NUM) + uiDq];
					}
				}
			}
		}

		/* calculate the average skew for current pattern for each pup and each bit*/
		DEBUG_PBS_C("DDR3 - PBS RX - Average for pattern - ",uiPatternIdx,1);
		for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			for(uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				auiPatternsSkewArray[uiPup][uiDq] += (auiSkewSumArray[uiPup][uiDq] / COUNT_PBS_REPEAT);
			}
		}
		
		for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif
					
			for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				/* set skew value for all uiDq */
				/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
				DEBUG_PBS_S("DQ");	
				DEBUG_PBS_D(uiDq,1);
				DEBUG_PBS_S("-");
				DEBUG_PBS_D(auiSkewSumArray[uiPup][uiDq] / COUNT_PBS_REPEAT, 2);
				DEBUG_PBS_S(", ");
			}
			DEBUG_PBS_S("\n");
		}
	}
	
	/* calculate the average skew */
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
		for(uiDq = 0; uiDq < DQ_NUM; uiDq++) 
			s_auiSkewArray[((uiPup) * DQ_NUM) + uiDq] = auiPatternsSkewArray[uiPup][uiDq] / COUNT_PBS_PATTERN;
	}

		
	DEBUG_PBS_S("DDR3 - PBS RX - Average for all patterns: \n");
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
		/* to minimize delay elements, inc from pbs value tht min pbs val */
#if 0
			DEBUG_PBS_S("DDR3 - PBS - PUP");
			DEBUG_PBS_D(uiPup,1);
			DEBUG_PBS_S(": ");
#endif
					
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
			/* set skew value for all uiDq */
			/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */
			DEBUG_PBS_S("DQ");	
			DEBUG_PBS_D(uiDq,1);
			DEBUG_PBS_S("-");
			DEBUG_PBS_D(s_auiSkewArray[(uiPup * DQ_NUM) + uiDq], 2);
			DEBUG_PBS_S(", ");
		}
		DEBUG_PBS_S("\n");
	}
	
	/* Return ADLL to default value */
	ddr3WritePupReg(PUP_DQS_RD, CS0, 0, 0, INIT_RL_DELAY);

	/* Set averaged PBS results */
	ddr3SetPbsResults(pDramInfo,FALSE);

	DEBUG_PBS_S("DDR3 - PBS RX - ended successfuly \n");
	
	return MV_OK;
}


/******************************************************************************
* Name:		ddr3RxShiftDqsToFirstFail.
* Desc:		Execute the Rx shift DQ phase.
* Args:		pDramInfo			ddr3 training information struct
* 			uiCurrPup       	bit array of the function active pups.
*			uiPbsPatternIdx 	Index of PBS pattern
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3RxShiftDqsToFirstFail(MV_DRAM_INFO *pDramInfo,MV_U32 uiCurrPup, MV_U32 uiPbsPatternIdx, MV_U32 uiEcc) 
{
	MV_U32 uiUnlockPup;        				/* bit array of unlock pups  */
	MV_U32 uiNewLockPup;              		/* bit array of compare failed pups */
	MV_U32 uiAdllVal = MAX_DELAY;
	MV_U32 uiDqsDeskewVal = 0; 				/* current value of DQS PBS deskew */
	MV_U32 uiMaxPup, uiPup, uiPassPup	;
	MV_U32 uiStdPups;

	uiStdPups = pDramInfo->uiNumOfStdPups;
	
	/* Set current pup number */ 
	if(uiCurrPup == 0x1)		/* Ecc mode */
		uiMaxPup = 1;
	else 
		uiMaxPup = pDramInfo->uiNumOfStdPups;
	
	uiUnlockPup = uiCurrPup;	/* '1' for each unlocked pup */
	
	DEBUG_PBS_S("DDR3 - PBS RX - Shift DQS - Starting... \n");

	/* Set DQS ADLL to MAX */
	DEBUG_PBS_S("DDR3 - PBS RX - Shift DQS - Set DQS ADLL to Max for all PUPs \n");
	ddr3WritePupReg(PUP_DQS_RD,CS0,PUP_BC,0,uiAdllVal);

	/* Loop on all ADLL Vaules */
	do {
		/* loop until found fail for all pups */
		uiNewLockPup = 0;

		if (MV_OK != ddr3SdramCompare(pDramInfo,uiUnlockPup,&uiNewLockPup, (MV_U32 *)&g_auiPbsPattern[uiPbsPatternIdx],
			LEN_PBS_PATTERN, SDRAM_PBS_I_OFFS + uiPbsPatternIdx*SDRAM_PBS_NEXT_OFFS ,MV_FALSE, MV_FALSE, NULL, MV_FALSE))
			return MV_FAIL;

		/* update all new locked pups */
		uiUnlockPup &= ~uiNewLockPup;

		if (( uiUnlockPup == 0) || ( uiDqsDeskewVal == MAX_PBS)) {
			if (uiDqsDeskewVal == MAX_PBS) {
				/* reach max value of dqs deskew or get fail for all pups */
				DEBUG_PBS_S("DDR3 - PBS RX - Shift DQS - DQS deskew reached maximum value\n");
			}
         	break;
		}

		DEBUG_PBS_S("DDR3 - PBS RX - Shift DQS - Inc DQS deskew for PUPs: ");
		DEBUG_PBS_D(uiUnlockPup, 2);
		DEBUG_PBS_C(", deskew = ", uiDqsDeskewVal, 2);

		/* Increment DQS deskew elements - Only for unlocked pups*/
		uiDqsDeskewVal++;
		for ( uiPup = 0; uiPup < (uiEcc+(1-uiEcc)*uiStdPups) ; uiPup++ ) {
			if ( IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE ) {
				ddr3WritePupReg(PUP_PBS_RX+DQ_NUM,CS0,uiPup+uiEcc*uiStdPups,0,uiDqsDeskewVal);
			}
		}
	} while (1);
	
	DEBUG_PBS_S("DDR3 - PBS RX - Shift DQS - ADLL shift one step before fail \n");
	/* Continue to ADLL shift one step before fail */
	uiUnlockPup = uiCurrPup;
	do {
		/* loop until pass compare for all pups */
		uiNewLockPup = 0;
		/* Read and compare results  */

		if (MV_OK != ddr3SdramCompare(pDramInfo, uiUnlockPup, &uiNewLockPup, (MV_U32 *)&g_auiPbsPattern[uiPbsPatternIdx],
			LEN_PBS_PATTERN, SDRAM_PBS_I_OFFS + uiPbsPatternIdx*SDRAM_PBS_NEXT_OFFS , MV_FALSE, MV_FALSE, NULL, MV_FALSE))
			return MV_FAIL;

		/* get mask for pup which passed so their adll will be changed to 2 steps before fails */
		uiPassPup = (uiUnlockPup & ~uiNewLockPup);

		DEBUG_PBS_S("Shift DQS by 2 steps for PUPs: ");
		DEBUG_PBS_D(uiPassPup, 2);
		DEBUG_PBS_C(", Set ADLL value = ", (uiAdllVal - 1), 2);

		/* only for pass pups	*/	
		for ( uiPup = 0; uiPup < (uiEcc+(1-uiEcc)*uiStdPups) ; uiPup++ ) {
			if ( IS_PUP_ACTIVE(uiPassPup, uiPup) == TRUE ) {
				ddr3WritePupReg(PUP_DQS_RD,CS0,uiPup+uiEcc*uiStdPups,0,(uiAdllVal - 1));
			}
		}

		/* locked pups that compare success  */
		uiUnlockPup &= uiNewLockPup;

		if (uiUnlockPup == 0) {
			/* all pups locked */
			break;
		}

		/* found error */
		if (uiAdllVal == 0)	{
			DEBUG_PBS_S("DDR3 - PBS Rx - Shift DQS - Adll reach min value \n");
			return MV_FAIL;
		}

		/* Decrement (Move Back to Left one phase - ADLL) dqs RX delay */
		uiAdllVal--;
		for ( uiPup = 0; uiPup < (uiEcc+(1-uiEcc)*uiStdPups) ; uiPup++ ) {
			if ( IS_PUP_ACTIVE(uiUnlockPup, uiPup) == TRUE ) {
				ddr3WritePupReg(PUP_DQS_RD,CS0,uiPup+uiEcc*uiStdPups,0,uiAdllVal);
			}
		}

	} while (1);

	return MV_OK;
}


/******************************************************************************
* Name:		ddr3PbsPerBit.
* Desc:		Execute the Per Bit Skew phase.
* Args:		pbStartOver	Return whether need to start over the algorithm 
*			bIsTx		Indicate whether Rx or Tx
*			puiCurrPup      bit array of the function active pups. return the
*					pups that need to repeat on the PBS
*			uiPbsPatternIdx	Index of PBS pattern
*
* Notes:	Current implementation supports double activation of this function.
*			i.e. in order to activate this function (using pbStartOver) more than
*			twice, the implementation should change.
*			imlementation limitation are marked using ' CHIP-ONLY! - Implementation Limitation '
* Returns:	MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3PbsPerBit(MV_DRAM_INFO *pDramInfo, MV_BOOL *pbStartOver, MV_BOOL bIsTx, MV_U32 *puiCurrPup, MV_U32 uiPbsPatternIdx, MV_U32 uiEcc) 
{
	/* bit array to indicate if we already get fail on bit per pup & dq bit */
	MV_U8 auiUnlockPupDqArray[DQ_NUM] = { *puiCurrPup, *puiCurrPup, *puiCurrPup, *puiCurrPup, *puiCurrPup,
		*puiCurrPup, *puiCurrPup, *puiCurrPup };

	MV_U8 auiCmpUnlockPupDqArray[COUNT_PBS_COMP_RETRY_NUM][DQ_NUM];
	MV_U32 uiPup, uiDq;	
	MV_U32 uiStartPbs, uiLastPbs;	/* value of pbs is according to RX or TX */
	MV_U32 uiPbsCurrVal;
	MV_U32 uiPupLocked;			/* bit array that indicates all dq of the pup locked */
	MV_U32 auiFirstFail[MAX_PUP_NUM] = {0}; 	/* count first fail per pup */   
	MV_BOOL bFirstFail[MAX_PUP_NUM] = {0};	/* indicates whether we get first fail per pup */
	MV_U32 uiSumPupFail;    		/* bit array that indicates pup already get fail */    
	MV_U32 uiMaxPup;
	MV_U32 uiCalcPbsDiff;   		/* use to calculate diff between curr pbs to first fail pbs */
	MV_U32 uiPbsCompRetry;

	/* Set init values for retry array - 8 retry */
	for ( uiPbsCompRetry=0; uiPbsCompRetry < COUNT_PBS_COMP_RETRY_NUM; uiPbsCompRetry++) {
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
			auiCmpUnlockPupDqArray[uiPbsCompRetry][uiDq] = *puiCurrPup;
		}
	}

	memset(&s_auiSkewArray, 0, MAX_PUP_NUM*DQ_NUM*sizeof(MV_U32));
	
	DEBUG_PBS_S("DDR3 - PBS Per bit - Started\n");

	/* the pbs value depends if rx or tx */
	if (bIsTx == TRUE) {
		uiStartPbs = MIN_PBS;
		uiLastPbs = MAX_PBS;
	} else {
		uiStartPbs = 20; //MAX_PBS;
		uiLastPbs = MIN_PBS;
	}

	uiPbsCurrVal = uiStartPbs;
	uiPupLocked = *puiCurrPup;

	/* Set current pup number */
	if(uiPupLocked == 0x1)		/* Ecc mode */
		uiMaxPup = 1;
	else 
		uiMaxPup = pDramInfo->uiNumOfStdPups;

	do {
	/* increment/ decrement PBS for un-lock bits only */
		if (bIsTx == TRUE) {
			uiPbsCurrVal++;
		} else {
			uiPbsCurrVal--;
		}
		
	/* Set Current PBS delay  */
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
		/* Check DQ bits to see if locked in all pups */
			if (auiUnlockPupDqArray[uiDq] == 0) {
				DEBUG_PBS_S("DDR3 - PBS Per bit - All pups are locked for DQ ");
				DEBUG_PBS_D(uiDq,1);
				DEBUG_PBS_S("\n");
				continue;
			}

			for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
				if ( IS_PUP_ACTIVE(auiUnlockPupDqArray[uiDq], uiPup) == FALSE ) {
					continue;
				}
				if (bIsTx == TRUE) 
//					ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,uiPup+uiEcc*uiMaxPup,0,uiPbsCurrVal);
					ddr3WritePupReg(PUP_PBS_TX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup+uiEcc*uiMaxPup,0,uiPbsCurrVal);
				else
//					ddr3WritePupReg(PUP_PBS_RX+uiDq,CS0,uiPup+uiEcc*uiMaxPup,0,uiPbsCurrVal);
					ddr3WritePupReg(PUP_PBS_RX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup+uiEcc*uiMaxPup,0,uiPbsCurrVal);
			}
		}

		/* Write Read and compare results - run the test DDR_PBS_COMP_RETRY_NUM times */
		/* Run number of read and write to verify */
		for ( uiPbsCompRetry=0; uiPbsCompRetry < COUNT_PBS_COMP_RETRY_NUM; uiPbsCompRetry++) {

			if(MV_OK != ddr3SdramPbsCompare(pDramInfo, uiPupLocked, bIsTx, uiPbsPatternIdx, 
			   uiPbsCurrVal, uiStartPbs, s_auiSkewArray, auiCmpUnlockPupDqArray[uiPbsCompRetry]))
				return MV_FAIL;

			for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
				for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
					if ((IS_PUP_ACTIVE(auiUnlockPupDqArray[uiDq], uiPup) == TRUE) && 
										(IS_PUP_ACTIVE(auiCmpUnlockPupDqArray[uiPbsCompRetry][uiDq], uiPup) == FALSE)) {
						DEBUG_PBS_S("DDR3 - PBS Per bit - PbsCurrVal: ");
						DEBUG_PBS_D(uiPbsCurrVal, 2);
						DEBUG_PBS_S(" PUP: "); 
						DEBUG_PBS_D(uiPup, 1);
						DEBUG_PBS_S(" DQ: "); 
						DEBUG_PBS_D(uiDq, 1);
						DEBUG_PBS_S(" - failed \n");
					}
				}
			}
			
			for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
				auiUnlockPupDqArray[uiDq] &= auiCmpUnlockPupDqArray[uiPbsCompRetry][uiDq];
			}
		}

		uiPupLocked = 0;
		uiSumPupFail = *puiCurrPup;
		
		/* Check which DQ is failed */
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++ ) {
			/* summarize the locked pup */
			uiPupLocked |= auiUnlockPupDqArray[uiDq];
			
			/* check if get fail */
			uiSumPupFail &= auiUnlockPupDqArray[uiDq];
		}

		/* If all PUPS are locked in all DQ - Break */
		if (uiPupLocked == 0) {
			/* all pups are locked */
			DEBUG_PBS_S("DDR3 - PBS Per bit -  All bit in all pups are successfully locked \n");
			break;
		}
	
		/* PBS deskew elements reach max ? */
		if ( uiPbsCurrVal == uiLastPbs ) {
			DEBUG_PBS_S("DDR3 - PBS Per bit - PBS deskew elements reach max \n");
			/* *pbStartOver = ( uiSumPupFail != 0 ) */
			/* CHIP-ONLY! - Implementation Limitation */
			*pbStartOver = ( uiSumPupFail != 0 ) &&  ( ! ( *pbStartOver ) ); 
			*puiCurrPup = uiPupLocked;

			DEBUG_PBS_S("DDR3 - PBS Per bit - StartOver: "); 
			DEBUG_PBS_D(*pbStartOver,1);
			DEBUG_PBS_S("  uiPupLocked: "); 
			DEBUG_PBS_D(uiPupLocked, 2);
			DEBUG_PBS_S("  uiSumPupFail: "); 
			DEBUG_PBS_D(uiSumPupFail	,2);		
			DEBUG_PBS_S(" \n");

		/* Lock PBS value for all remaining  bits */
			for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
				/* check if current pup already received error */ 
				if (IS_PUP_ACTIVE(uiPupLocked, uiPup) == TRUE) {
					/* valid pup for current function */

					if (IS_PUP_ACTIVE(uiSumPupFail, uiPup) == TRUE && ( *pbStartOver == TRUE ) ) {
						DEBUG_PBS_C("DDR3 - PBS Per bit - skipping lock of pup (first loop of pbs)", uiPup,1);
						continue;
					} else if ( IS_PUP_ACTIVE(uiSumPupFail, uiPup) == TRUE ) {
						DEBUG_PBS_C("DDR3 - PBS Per bit - Locking pup %d (even though it wasn't supposed to be locked)", uiPup,1);
					}
		
					/* already got fail on the PUP */
					/* Lock PBS value for all remaining bits */
					DEBUG_PBS_S("DDR3 - PBS Per bit - Locking remaning DQs for pup - ");
					DEBUG_PBS_D(uiPup, 1);
					DEBUG_PBS_S(": ");
					
					for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						if (IS_PUP_ACTIVE( auiUnlockPupDqArray[uiDq], uiPup ) == TRUE) {
									
							DEBUG_PBS_D(uiDq, 1);
							DEBUG_PBS_S(",");
							/* set current PBS */
							s_auiSkewArray[((uiPup + uiMaxPup*uiEcc) * DQ_NUM) + uiDq] = uiPbsCurrVal;
						}
					}
					if (*pbStartOver == TRUE) {
                  			/* reset this pup bit - when restart the PBS, ignore this pup */
						*puiCurrPup &= 	~(1 << uiPup);
					}
					DEBUG_PBS_S("\n");
				} else {
					DEBUG_PBS_S("DDR3 - PBS Per bit - Pup ");
					DEBUG_PBS_D(uiPup,1);
					DEBUG_PBS_C(" is not set in puplocked - ", uiPupLocked,1);
				}
				
			}
		/* need to start the PBS again */
			if (*pbStartOver == TRUE) { 
				DEBUG_PBS_S("DDR3 - PBS Per bit - false fail - returning to start \n");
				return MV_OK;
			}
			break;
		}
	
//	DEBUG_PBS_C("DDR3 - PBS Per bit - uiSumPupFail = ", uiSumPupFail,2);
	
	/* Diff Check */
		for (uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			if (IS_PUP_ACTIVE( uiPupLocked, uiPup) == TRUE) {
				/* pup is not locked */
				if ( bFirstFail[uiPup] == FALSE) {
					/* no first fail until now */
					if (IS_PUP_ACTIVE(uiSumPupFail, uiPup) == FALSE) {
						/* get first fail */
						DEBUG_PBS_C("DDR3 - PBS Per bit - First fail in pup ", uiPup,1);
						bFirstFail[uiPup] = TRUE;
						auiFirstFail[uiPup] = uiPbsCurrVal;
					}
				} else {
					/* already got first fail */
					if ( bIsTx == TRUE ) {
						/* TX - inc pbs */
						uiCalcPbsDiff = uiPbsCurrVal - auiFirstFail[uiPup];
					} else {
						/* RX - dec pbs */
						uiCalcPbsDiff = auiFirstFail[uiPup] - uiPbsCurrVal;
					}
					
					if ( uiCalcPbsDiff >= PBS_DIFF_LIMIT) {
						/* Lock PBS value for all remaining PUPs bits */
						DEBUG_PBS_S("DDR3 - PBS Per bit - Lock PBS value for all remaining PUPs bits, pup ");
						DEBUG_PBS_D(uiPup,1);
						DEBUG_PBS_C(" pbs value ", uiPbsCurrVal,2);
						
						uiPupLocked &= ~(1<< uiPup);
						
						for ( uiDq = 0; uiDq < DQ_NUM; uiDq++ ) {
							if ( IS_PUP_ACTIVE( auiUnlockPupDqArray[uiDq], uiPup ) == TRUE ) {
								/* lock current dq */
								auiUnlockPupDqArray[uiDq] &= ~(1<< uiPup);
								s_auiSkewArray[((uiPup + uiMaxPup*uiEcc) * DQ_NUM) + uiDq] =  uiPbsCurrVal;
		
								if (bIsTx == TRUE) 
//									ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,uiPup+uiEcc*uiMaxPup,0,uiStartPbs);
									ddr3WritePupReg(PUP_PBS_TX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup+uiEcc*uiMaxPup,0,uiStartPbs);
								else
//									ddr3WritePupReg(PUP_PBS_RX+uiDq,CS0,uiPup+uiEcc*uiMaxPup,0,uiStartPbs);
									ddr3WritePupReg(PUP_PBS_RX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup+uiEcc*uiMaxPup,0,uiStartPbs);
							}
						}
					}
				}
			}
		}
	} while (1);

	return MV_OK;
}


/******************************************************************************
* Name:			ddr3SetPbsResults.
* Desc:			Set to HW the PBS phase results.
* Args:			bIsTx		Indicates whether to set Tx or RX results
* Notes:
* Returns:		MV_OK if success, other error code if fail.
*/

MV_STATUS    ddr3SetPbsResults (MV_DRAM_INFO *pDramInfo,  MV_BOOL bIsTx )
{
	MV_U32 uiPup, uiDq;
	MV_U32 uiMaxPup;                 /* number of valid pups */
	MV_U32 uiPbsMinVal;              /* minimal pbs val per pup */
	MV_U32 uiPbsMaxVal;              /* maximum pbs val per pup */
	MV_U32 uiTemp;

	uiMaxPup = pDramInfo->uiNumOfTotalPups;
	DEBUG_PBS_S("DDR3 - PBS - ddr3SetPbsResults: \n");
	/* loop for all dqs & pups */
	for ( uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
		/* to minimize delay elements, inc from pbs value tht min pbs val */
		uiPbsMinVal = MAX_PBS;
		uiPbsMaxVal = 0;
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
			if ( uiPbsMinVal > s_auiSkewArray[(uiPup * DQ_NUM) + uiDq] ) {
				uiPbsMinVal = s_auiSkewArray[(uiPup * DQ_NUM) + uiDq];
			}
			if ( uiPbsMaxVal < s_auiSkewArray[(uiPup * DQ_NUM) + uiDq] ) {
				uiPbsMaxVal = s_auiSkewArray[(uiPup * DQ_NUM) + uiDq];
			}
		}

		uiPbsMaxVal -= uiPbsMinVal;
		
		
		DEBUG_PBS_S("DDR3 - PBS - PUP");
		DEBUG_PBS_D(uiPup,1);
		DEBUG_PBS_S(": Min Val = ");
		DEBUG_PBS_D(uiPbsMinVal,2);
		DEBUG_PBS_C(", Max Val = ",uiPbsMaxVal,2);
//		DEBUG_PBS_S("DDR3 - PBS - ");
		
		for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
			/* set skew value for all uiDq */
         	/* Bit# Deskew <- Bit# Deskew - last / first  failing bit Deskew For all bits (per PUP) (minimize delay elements) */

			DEBUG_PBS_S("DQ");	
			DEBUG_PBS_D(uiDq,1);
			DEBUG_PBS_S("-");
			DEBUG_PBS_D((s_auiSkewArray[(uiPup*DQ_NUM)+uiDq]-uiPbsMinVal), 2);
			DEBUG_PBS_S(", ");
			
			if (bIsTx == TRUE) {
#if 0
				if (s_auiSkewArray[(uiPup*DQ_NUM)+uiDq] == MAX_PBS)
					ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,uiPup,0,uiPbsMaxVal/6);
				else
#endif
//					ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,uiPup,0,s_auiSkewArray[(uiPup*DQ_NUM)+uiDq]-uiPbsMinVal);
					ddr3WritePupReg(PUP_PBS_TX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup,0,s_auiSkewArray[(uiPup*DQ_NUM)+uiDq]-uiPbsMinVal);
//				uiTemp = ddr3ReadPupReg(PUP_PBS_TX+uiDq,CS0,uiPup);
//				DEBUG_PBS_C("\nRead Value = ",uiTemp,8);
			}
			else {
//#if 0
				if (s_auiSkewArray[(uiPup*DQ_NUM)+uiDq] == MIN_PBS)
//					ddr3WritePupReg(PUP_PBS_TX+uiDq,CS0,uiPup,0,uiPbsMaxVal/4);
					ddr3WritePupReg(PUP_PBS_TX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup,0,uiPbsMaxVal/4);
				else
//#endif
//					ddr3WritePupReg(PUP_PBS_RX+uiDq,CS0,uiPup,0,s_auiSkewArray[(uiPup*DQ_NUM)+uiDq]-uiPbsMinVal);
					ddr3WritePupReg(PUP_PBS_RX+s_auiPbsDqMapping[uiPup][uiDq],CS0,uiPup,0,s_auiSkewArray[(uiPup*DQ_NUM)+uiDq]-uiPbsMinVal);
			}
		}

		DEBUG_PBS_S("\n");
		
		/* Set the DQS the half of the Max PBS of the DQs  */
//#if 0		
		if (bIsTx == TRUE) 
			ddr3WritePupReg(PUP_PBS_TX+8,CS0,uiPup,0,uiPbsMaxVal/2);
		else {
			uiTemp = ddr3ReadPupReg(PUP_PBS_RX+8,CS0,uiPup);
//			DEBUG_PBS_C("uiTemp = ",uiTemp,2);
			ddr3WritePupReg(PUP_PBS_RX+8,CS0,uiPup,0,uiPbsMaxVal/2);
		}
//#endif
	}

	return MV_OK;
}

MV_VOID ddr3PbsWritePupDQSReg(MV_U32 uiCs, MV_U32 uiPup, MV_U32 uiDQSDelay)
{
	MV_U32 uiReg, uiDelay;

	uiReg = (ddr3ReadPupReg(PUP_WL_MODE, uiCs, uiPup) & 0x3FF);
	uiDelay = uiReg & PUP_DELAY_MASK;
	uiReg |= ((uiDQSDelay+uiDelay) << REG_PHY_DQS_REF_DLY_OFFS);
	
	uiReg |= REG_PHY_REGISTRY_FILE_ACCESS_OP_WR;
	

	if (uiPup == PUP_BC)
		uiReg |= (1 << REG_PHY_BC_OFFS);
	else
		uiReg |= (uiPup << REG_PHY_PUP_OFFS);

	uiReg |= ((0x4*uiCs+PUP_WL_MODE) << REG_PHY_CS_OFFS);

//	DEBUG_PBS_C("write val = ",uiReg,8);
	
	MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg);  /* 0x16A0 */
	do {
		uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) &
				REG_PHY_REGISTRY_FILE_ACCESS_OP_DONE);
	} while (uiReg);				/* Wait for '0' to mark the end of the transaction */
	
	uDelay(10);

}

/*
 * Set training patterns
 */
MV_STATUS ddr3LoadPbsPatterns(MV_DRAM_INFO *pDramInfo)
{
	MV_U32 uiReg, uiCs, uiCsCount, uiCsTemp, ui;
	MV_U32 auiSdramAddr;

	/* Loop for each CS */
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (pDramInfo->uiCsEna & (1<<uiCs)) {

			uiCsCount = 0;
			for (uiCsTemp = 0; uiCsTemp < uiCs; uiCsTemp++) {
				if (pDramInfo->uiCsEna & (1<<uiCsTemp))
					uiCsCount++;
			}

			/* Init PBS II pattern */
			auiSdramAddr = (uiCsCount*(SDRAM_CS_SIZE+1) + SDRAM_PBS_II_OFFS);
			if (MV_OK != ddr3SdramCompare(pDramInfo, NULL, NULL, g_auiPbsPattern[1],
				LEN_STD_PATTERN, auiSdramAddr, TRUE, FALSE, NULL, FALSE))
				return MV_FAIL;
		}
	}
	

	return MV_OK;
}

#endif