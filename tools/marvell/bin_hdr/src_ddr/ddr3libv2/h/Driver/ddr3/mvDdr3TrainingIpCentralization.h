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
* mvDdr3TrainingIpCentralization.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __mvDdr3TrainingIP_CENTRALIZATION_H
#define __mvDdr3TrainingIP_CENTRALIZATION_H


typedef struct
{
    GT_U32 regAddr;
    GT_U32 startBit; /* lsb 4 bits */
    GT_U32 endBit;   /* lsb 5th bit */
}centralizationRes;

/******************************************************************************
* Name:     ddr3TipDDR3CentralizationTx.
* Desc:     Centralization TX
* Args:     devNum
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipCentralizationTx
(
    GT_U32    devNum
);

/******************************************************************************
* Name:     ddr3TipDDR3CentralizationRx.
* Desc:     Centralization RX
* Args:     devNum
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipCentralizationRx
(
    GT_U32    devNum
);

/******************************************************************************
* Name:     ddr3TipPrintCentralizationResult.
* Desc:     Print Centralization Results
* Args:     devNum
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPrintCentralizationResult
(
    GT_U32      devNum
);

/******************************************************************************
* Name:     ddr3TipSpecialRx.
* Desc:     Special RX pattern
* Args:     devNum
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipSpecialRx
(
    GT_U32   devNum
);

#endif /* __mvDdr3TrainingIP_CENTRALIZATION_H */


