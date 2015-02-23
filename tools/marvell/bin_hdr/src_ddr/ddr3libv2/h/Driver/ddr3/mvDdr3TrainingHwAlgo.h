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
* mvDdr3TrainingAdditional.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef _MV_DDR3_TRAINING_HW_ALGO_H_
#define _MV_DDR3_TRAINING_HW_ALGO_H_



#ifdef __cplusplus
   extern "C"
   {
#endif

/*****************************************************************************
Vref training
******************************************************************************/
GT_STATUS ddr3TipVref
(
    GT_U32 devNum
);


/*****************************************************************************
ODT additional timing
******************************************************************************/
GT_STATUS    ddr3TipWriteAdditionalOdtSetting
(
    GT_U32                  devNum,
    GT_U32                  interfaceId
) ;


/*****************************************************************************
CK/CA Delay
******************************************************************************/
GT_STATUS    ddr3TipCmdAddrInitDelay
(
    GT_U32      devNum,
	GT_U32		adllTap
);

#ifdef __cplusplus
   }
#endif

/*****************************************************************************/

#endif /* _MV_DDR3_TRAINING_HW_ALGO_H_ */


