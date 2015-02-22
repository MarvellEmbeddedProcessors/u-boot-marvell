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
* mvDdr3TrainingIpDb.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef _MV_DDR3_TRAININGIP_DB_H_
#define _MV_DDR3_TRAININGIP_DB_H_

#ifdef __cplusplus
   extern "C"
   {
#endif

typedef enum
{
   PATTERN_PBS1,
   PATTERN_PBS2,
   PATTERN_RL,
   PATTERN_STATIC_PBS,
   PATTERN_KILLER_DQ0,
   PATTERN_KILLER_DQ1,
   PATTERN_KILLER_DQ2,
   PATTERN_KILLER_DQ3,
   PATTERN_KILLER_DQ4,
   PATTERN_KILLER_DQ5,
   PATTERN_KILLER_DQ6,
   PATTERN_KILLER_DQ7,
   PATTERN_PBS3,
   PATTERN_RL2,
   PATTERN_TEST,
   PATTERN_FULL_SSO0,
   PATTERN_FULL_SSO1,
   PATTERN_FULL_SSO2,
   PATTERN_FULL_SSO3,
   PATTERN_VREF,
   PATTERN_LIMIT
}MV_HWS_PATTERN;

#ifdef __cplusplus
   }
#endif

#endif /* _MV_DDR3_TRAININGIP_DB_H_ */


