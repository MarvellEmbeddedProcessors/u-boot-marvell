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
* mvDdr3TrainingIpEngine.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef _MV_DDR3_TRAININGIP_ENGINE_H_
#define _MV_DDR3_TRAININGIP_ENGINE_H_

#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpFlow.h"

#ifdef __cplusplus
   extern "C"
   {
#endif


/******************************** defined ***************************************/
#define EDGE_1 (0)
#define EDGE_2 (1)
#define ALL_PUP_TRAINING                  (0xE)
#define PUP_RESULT_EDGE_1_MASK            (0xff)
#define PUP_RESULT_EDGE_2_MASK            (0xff << 8)
#define PUP_LOCK_RESULT_BIT               (25)
#define GET_TAP_RESULT(reg, edge)         (((edge) == EDGE_1) ? ((reg) & PUP_RESULT_EDGE_1_MASK):(((reg) & PUP_RESULT_EDGE_2_MASK) >> 8));
#define GET_LOCK_RESULT(reg)              (((reg) & (1<<PUP_LOCK_RESULT_BIT)) >> PUP_LOCK_RESULT_BIT)
#define EDGE_FAILURE                      (128)
#define ALL_BITS_PER_PUP                  (128)

#define MIN_WINDOW_SIZE                   (6)
#define MAX_WINDOW_SIZE_RX                (32)
#define MAX_WINDOW_SIZE_TX                (64)

GT_STATUS ddr3TipGetResult(GT_U32 *arResult, MV_HWS_Edge eEdge, MV_HWS_EdgeSearch eEdgeSearch, GT_U32 *pEdgeResult);

/******************************************************************************
* Name:     ddr3TipIpTraining.
* Desc:     IP Training search
* Args:     devNum - channel ID
*           accessType - if access type
*           pupAccessType - pup access
*           pupNum -  pup number
*           resultType - per bit/per byte
*           controlElement - control element (adll/dq/dsq)
*           searchDir - low2high/high2low
*           direction - tx or rx
*           interfaceMask - bitmask for selected interfaces
*           initValue - initial value
*           numIter - number of iterations
*           pattern - pattern type
*           edgeComp - type of compare: PF, FP, FPF, PFP
*           csType - multi/single
*           csNum -  CS number
*           trainStatus  training result
*                                          
* Notes:    
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS ddr3TipTrainingIpTest
(
    GT_U32                      devNum,
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,  
    MV_HWS_EdgeCompare          edge,
    GT_U32                      initVal1,
    GT_U32                      initVal2,
    GT_U32                      numOfItertaions,
    GT_U32                      startPattern,
    GT_U32                      endPattern
);

/******************************************************************************
* Name:     ddr3TipLoadPatternToMem.
* Desc:     Load expected Pattern to external memory
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipLoadPatternToMem
(  
    GT_U32          devNum,
    MV_HWS_PATTERN  pattern
);

/******************************************************************************
* load pattern to memory using ODPG
*/
GT_STATUS    ddr3TipLoadAllPatternToMem
(  
    GT_U32          devNum
);

/******************************************************************************
* Name:     ddr3TipReadTrainingResult.
* Desc:     Load/Unload training ip result
* Args:     devNum 
*           interfaceId 
*           pupAccessType 
*           pupNum 
*           search 
*           direction 
*           resultType 
*           loadRes
*           isReadFromDB
*
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipReadTrainingResult
(  
    GT_U32                          devNum,
    GT_U32                          interfaceId, 
    MV_HWS_ACCESS_TYPE              pupAccessType,
    GT_U32                          pupNum,
    GT_U32                          bitNum,
    MV_HWS_SearchDirection          search,
    MV_HWS_DIRECTION                direction,
    MV_HWS_TRAINING_RESULT          resultType,
    MV_HWS_TrainingLoadOperation    operation,
    GT_U32                          csNumType,
    GT_U32                          **loadRes,
    GT_BOOL                         isReadFromDB,
    GT_U8                         	consTap,
    GT_BOOL                         isCheckResultValidity
);


/******************************************************************************
* Name:     ddr3TipIpTraining.
* Desc:     IP Training search
* Args:     TBD
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipIpTraining
(   
    GT_U32                      devNum,
    MV_HWS_ACCESS_TYPE          accessType,
    GT_U32                      interfaceNum, 
    MV_HWS_ACCESS_TYPE          pupAccessType,
    GT_U32                      pupNum, 
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_ControlElement       controlElement,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,
    GT_U32                      interfaceMask, 
    GT_U32                      initValue,
    GT_U32                      numIter,
    MV_HWS_PATTERN              pattern,
    MV_HWS_EdgeCompare          edgeComp,
    MV_HWS_DDR_CS               csType,
    GT_U32                      csNum,
    MV_HWS_TrainingIpStatus*    trainStatus
);

GT_STATUS    ddr3TipIpTrainingWrapper
(   
    GT_U32                      devNum,
    MV_HWS_ACCESS_TYPE          accessType,
    GT_U32                      interfaceId, 
    MV_HWS_ACCESS_TYPE          pupAccessType,
    GT_U32                      pupNum, 
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_ControlElement       controlElement,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,
    GT_U32                      interfaceMask, 
    GT_U32                      initValue1,
    GT_U32                      initValue2,
    GT_U32                      numIter,
    MV_HWS_PATTERN              pattern,
    MV_HWS_EdgeCompare          edgeComp,
    MV_HWS_DDR_CS               trainCsType,
    GT_U32                      csNum,
    MV_HWS_TrainingIpStatus*    trainStatus
);

GT_STATUS isOdpgAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
);

/******************************************************************************
* print Bist Result
*/
void    ddr3TipPrintBistRes
(
);

/******************************************************************************
* get tables functions
*/
PatternInfo*  ddr3TipGetPatternTable();
GT_U16 *ddr3TipGetMaskResultsDqReg();
GT_U16 *ddr3TipGetMaskResultsPupRegMap();


#ifdef __cplusplus
   }
#endif

/*****************************************************************************/
#endif /* _MV_DDR3_TRAININGIP_ENGINE_H_ */
