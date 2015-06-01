/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvDdr3TrainingBist.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/




#ifndef _MV_DDR3_TRAINING_BIST_H_
#define _MV_DDR3_TRAINING_BIST_H_

#include "mvDdr3TrainingIp.h"

#ifdef __cplusplus
   extern "C"
   {
#endif

/******************************** enum ***************************************/

typedef enum 
{
   BIST_STOP = 0,
   BIST_START = 1
} MV_HWS_BIST_OPERATION;

typedef enum 
{
   STRESS_NONE = 0,
   STRESS_ENABLE = 1
} MV_HWS_STRESS_JUMP;

typedef enum 
{
   DURATION_SINGLE = 0,
   DURATION_STOP_AT_FAIL = 1,
   DURATION_ADDRESS = 2,
   DURATION_CONT = 4
   
} MV_HWS_PATTERN_DURATION;

/************************* Structures ***********************************************/

typedef struct
{
   GT_U32                  bistErrorCnt;
   GT_U32                  bistFailLow;
   GT_U32                  bistFailHigh;
   GT_U32                  bistLastFailAddr;
}BistResult;



/******************************************************************************
* Name:     ddr3TipBistReadResult.
* Desc:     Read DDR BIST Result
* Args:     devNum          - device number
*           interfaceId     - interface id
*           pstBistResult   - for returning the values 
* Notes:    
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipBistReadResult
(  
    GT_U32          devNum,
    GT_U32          interfaceId,
    BistResult      *pstBistResult
);

/******************************************************************************
* Name:     ddr3TipBistActivate.
* Desc:     Activate DDR BIST
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipBistActivate
(
    GT_U32                  devNum,
    MV_HWS_PATTERN          pattern,
    MV_HWS_ACCESS_TYPE      accessType,
    GT_U32                  ifNum,
    MV_HWS_DIRECTION        direction,
    MV_HWS_STRESS_JUMP      addrStressJump,
    MV_HWS_PATTERN_DURATION duration,
    MV_HWS_BIST_OPERATION   operType,
    GT_U32                  offset,
	GT_U32					csNum,
    GT_U32                  patternAddrLength
);

/******************************************************************************
* Name:     mvHwsDdr3RunBist.
* Desc:     Bist Flow
* Args:
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    mvHwsDdr3RunBist
(
	GT_U32	        devNum,
	MV_HWS_PATTERN  pattern,
	GT_U32          *result,
	GT_U32          csNum
);


/******************************************************************************
* Name:     ddr3TipRunSweepTest.
* Desc:     Bist sweep control
* Args:
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_BOOL ddr3TipRunSweepTest
(
    GT_32 devNum,
    GT_U32 RepeatNum,
    GT_U32 direction,
    GT_U32 mode
);

GT_BOOL ddr3TipRunLevelingSweepTest
(
    GT_32 devNum,
    GT_U32 RepeatNum,
    GT_U32 direction,
    GT_U32 mode
);
/******************************************************************************
* Name:     ddr3TipPrintRegs.
* Desc:     Print Specified Dunit/Pup Registers
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipPrintRegs
(
    GT_U32 devNum
);

/******************************************************************************
* Name:     ddr3TipRegDump.
* Desc:     Print Dunit/Pup Registers Dump
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipRegDump
(
    GT_U32 devNum
);


/******************************************************************************
* Name:     RunXsbTest.
* Desc:     Run XSB test
* Args:     devNum
*           uiMemAddress
*           startPattern
*           endPattern
*           writeMode
*           readMode
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS
RunXsbTest(GT_U32 devNum, GT_U32 uiMemAddress, GT_U32 writeType, GT_U32 readType, GT_U32 burstLength);

#ifdef __cplusplus
   }
#endif


#endif /* _MV_DDR3_TRAINING_BIST_H_ */


