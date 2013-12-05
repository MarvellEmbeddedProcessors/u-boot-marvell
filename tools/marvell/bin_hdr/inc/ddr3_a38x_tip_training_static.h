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

#ifndef __ddr3_a38x_training_static_H
#define __ddr3_a38x_training_static_H

#include <gtOs/gtGenTypes.h>
#include <ddr3/trainingIp/mvDdr3TrainingIpStatic.h>
#include <ddr3/trainingIp/mvDdr3TrainingIp.h>

#ifdef __cplusplus
extern "C" {
#endif

static MV_HWS_TOPOLOGY_MAP A38xTopologyMap[] =
{
    /* 1st board */
    {
    0x1, /* active interfaces */
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                                                                     speed_bin        memory_width  mem_size     frequency  casL casWL      temperature */
	{{{{0x1,0,0,0}, {0x1,0,0,0}, {0x3,0,0,0}, {0x3,0,0,0}, {0x0,0,0,0}, {0x0,0,0,0}, {0x0,0,0,0}, {0x0,0,0,0}}, SPEED_BIN_DDR_1866L, BUS_WIDTH_8 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_HIGH}},
    4, /* Num Of Bus Per Interface*/
    0  /* board number */
    },
    /* 2nd board */
    {
    0x1, /* active interfaces */
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                                     speed_bin             memory_width  mem_size     frequency  casL casWL      temperature */
	{{{{0x1,0,0,0},{ 0x1,0,0,0},{ 0x2,1,0,0},{ 0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866L, BUS_WIDTH_8 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_HIGH}},
    4, /* Num Of Bus Per Interface*/
    1  /* board number */
    }
};

/* this array hold the board round trip delay (DQ and CK) per <interfcae,bus> */
TripDelayElement A38xBoardRoundTripDelayArray[] =
{
   /* 1st board */
   /*interface bus DQS-delay CK-delay */
   { 3952,5060 },
   { 3192,4493 },
   { 4785,6677 },
   { 3413,7267 },
   { 4282,6086 },	/*ECC PUP*/
   { 3952,5134 },
   { 3192,4567 },
   { 4785,6751 },
   { 3413,7341 },
   { 4282,6160 },	/*ECC PUP*/

   /* 2nd board */
   /*interface bus DQS-delay CK-delay */
   { 3952,5060 },
   { 3192,4493 },
   { 4785,6677 },
   { 3413,7267 },
   { 4282,6086 },	/*ECC PUP*/
   { 3952,5134 },
   { 3192,4567 },
   { 4785,6751 },
   { 3413,7341 },
   { 4282,6160 }	/*ECC PUP*/
};

/* package trace */
TripDelayElement A38xPackageRoundTripDelayArray[] =
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


static GT_32 A38xSiliconDelayOffset[] =
{
    /* board 0 */
    0,
    /* board 1 */
    0,
    /* board 2 */
    0
};


GT_STATUS ddr3TipInitA38x
(
    GT_U32  devNum,
    GT_U32  boardId
);

#ifdef __cplusplus
}
#endif

#endif /* __ddr3_a38x_training_static_H */
