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
* mvDdr3TrainingIP.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/

#ifndef __mvDdr3TrainingIP_PBS_H
#define __mvDdr3TrainingIP_PBS_H

typedef enum
{
    EBA_CONFIG,
    EEBA_CONFIG,
    SBA_CONFIG
} MV_HWS_PBS_STATE;

typedef enum 
{
    PBS_RD_ADDR         = 0x2000,
    PBS_WR_ADDR         = 0x2100,
    CENTRALIZATION_ADDR = 0x3000
}MV_HWS_PATTERN_ADDRESS;

typedef enum
{
    TrainingLoadOperation_UNLOAD,
    TrainingLoadOperation_LOAD
}MV_HWS_TrainingLoadOperation;

typedef enum
{
   TRAINING_EDGE_1,
   TRAINING_EDGE_2
}MV_HWS_Edge;

typedef enum
{
   TRAINING_EDGE_MAX,
   TRAINING_EDGE_MIN
}MV_HWS_EdgeSearch;


/******************************************************************************
* Name:     ddr3TipPbsRx.
* Desc:     PBS RX
* Args:     devNum - device number
* Notes:
* Returns:  OK if success, other error code if fail.
*/
/* TBD need arguments to funtion */
GT_STATUS    ddr3TipPbsRx
(
    GT_U32      devNum
);


/*****************************************************************************
Print PBS Result
******************************************************************************/
GT_STATUS    ddr3TipPrintAllPbsResult
(
    GT_U32      devNum
);

/******************************************************************************
* Name:     ddr3TipPbsTx.
* Desc:     PBS TX
* Args:     devNum - device number
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPbsTx
(
    GT_U32      devNum
);

#endif /* __mvDdr3TrainingIP_PBS_H */


