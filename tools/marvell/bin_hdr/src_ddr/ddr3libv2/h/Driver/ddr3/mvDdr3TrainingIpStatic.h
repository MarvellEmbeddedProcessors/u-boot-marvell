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
* mvDdr3TrainingIpStatic.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/




#ifndef _MV_DDR3_TRAININGIP_STATIC_H_
#define _MV_DDR3_TRAININGIP_STATIC_H_

#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIp.h"

#ifdef __cplusplus
   extern "C"
   {
#endif


/******************************** defined ***************************************/

/******************************************************************************/
/* Definitions of INTERFACE registers                                             */
/******************************************************************************/


/******************************************************************************/
/* Definitions of PHY registers                                               */
/******************************************************************************/


/************************* Structures ***********************************************/

typedef struct 
{
   GT_U32 dqsDelay; /* DQS delay (mSec) */
   GT_U32 ckDelay;  /* CK Delay  (mSec) */
      
} TripDelayElement;

typedef struct
{
	GT_U32				siliconDelay;
	TripDelayElement	*packageTraceArr;
	TripDelayElement	*boardTraceArr;

} MV_HWS_TIP_STATIC_CONFIG_INFO;


/************************* Declarations ***********************************************/

GT_STATUS    ddr3TipRunStaticAlg
( 
    GT_U32  devNum,
    MV_HWS_DDR_FREQ freq
);

GT_STATUS ddr3TipInitStaticConfigDb
(
    GT_U32                          devNum,
	MV_HWS_TIP_STATIC_CONFIG_INFO*	staticConfigInfo
);

GT_STATUS ddr3TipInitSpecificRegConfig
(
    GT_U32              devNum,
	RegData				*regConfigArr
);

GT_STATUS    ddr3TipStaticPhyInitController
(
    GT_U32    devNum
);

#ifdef __cplusplus
}
#endif

#endif /* _MV_DDR3_TRAININGIP_STATIC_H_ */


