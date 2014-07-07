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
* ddr3_a38x_training_static.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#ifndef __ddr3_msys_ac3_training_static_H
#define __ddr3_msys_ac3_training_static_H

#include <gtGenTypes.h>
#include <mvDdr3TrainingIpStatic.h>
#include <mvDdr3TrainingIp.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************** Globals ******************************/

GT_U8 debugAc3 = 0;

/* this array hold the board round trip delay (DQ and CK) per <interfcae,bus> */
TripDelayElement Ac3BoardRoundTripDelayArray[] =
{
   /* 1st board DB + No ECC*/
   /*interface bus DQS-delay CK-delay */
   { 3486,7164 },
   { 2354,4962 },
   /* 2nd board DB + ECC*/
   /*interface bus DQS-delay CK-delay */
   { 3486,7164 },
   { 2354,4962 },
   { 3410,7754 },/*ECC PUP*/
   /* 3nd board RD + No ECC*/
   /*interface bus DQS-delay CK-delay */
   { 1230,1850 },
   { 740,1850 },
};

/* package trace */
TripDelayElement Ac3PackageRoundTripDelayArray[] =
{
     /*IF BUS DQ_DELYA CK_DELAY */
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 } ,
    {  0,    0 }
};


static GT_32 Ac3SiliconDelayOffset[] =
{
    /* board 0 */
    0,
    /* board 1 */
    0,
    /* board 2 */
    0
};


GT_STATUS ddr3TipInitAc3
(
    GT_U32  devNum,
    GT_U32  boardId
);

#ifdef __cplusplus
}
#endif

#endif /* __ddr3_msys_ac3_training_static_H */
