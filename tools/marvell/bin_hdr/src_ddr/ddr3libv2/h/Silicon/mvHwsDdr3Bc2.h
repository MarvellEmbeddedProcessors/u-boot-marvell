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
* mvHwsDdr3Bc2.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#ifndef __mvHwsDdr3_BC2_H
#define __mvHwsDdr3_BC2_H

#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingIp.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************* Definitions ***********************************************/

#define BC2_NUMBER_OF_INTERFACES               (5)
#define BC2_NUMBER_OF_PUP                      (4)
#define BC2_NUMBER_OF_BOARDS                   (2)

#define BC2_DEVICE_SAR1_REG_ADDR               0xF8200
#define BC2_DEVICE_SAR2_REG_ADDR               0xF8204

#define BC2_JTAG_DEV_ID_STATUS_REG_ADDR        0x000F8244
#define BC2_DEV_GENERAL_STATUS_REG1_ADDR_A0    0x000F82B4
#define BC2_DEV_GENERAL_STATUS_REG1_ADDR_B0    0x000F8C84

#define BC2_JTAG_DEV_ID_STATUS_VERSION_OFFSET  28

#define BC2_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_OFFSET   11
#define BC2_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_MASK     0x1

/************************* Enums ***********************************************/

typedef enum {
	TM_EN,
	MSYS_EN
}DDR_IF_ASSIGNMENT;

/************************* Globals ***********************************************/

#ifndef DEFINE_GLOBALS
extern TripDelayElement bc2BoardRoundTripDelayArray[BC2_NUMBER_OF_INTERFACES*BC2_NUMBER_OF_PUP*BC2_NUMBER_OF_BOARDS];
#else
/* this array hold the board round trip delay (DQ and CK) per <interfcae,bus> */
TripDelayElement bc2BoardRoundTripDelayArray[BC2_NUMBER_OF_INTERFACES*BC2_NUMBER_OF_PUP*BC2_NUMBER_OF_BOARDS] =
{
   /* 1st board */
   /*interface bus DQS-delay CK-delay */
   { 3012,   6715 },
   { 2625,   6715 },
   { 3023,   6458 },
   { 2663,   6458 },
   { 2596,   6691 },
   { 2555,   6691 },
   { 2830,   6429 },
   { 2592,   6429 },
   { 3039,   6905 },
   { 2731,   6905 },
   { 2830,   6429 },
   { 2592,   6429 },
   { 2828,   6505 },
   { 2608,   6505 },
   { 3129,   6789 },
   { 2877,   6789 },
   { 2755,   7208 },
   { 3489,   7208 },
   { 2677,   7187 },
   { 3472,   7187 },

   /* 2nd board */
   /*interface bus DQS-delay CK-delay */
   { 3012,   6715 },
   { 2625,   6715 },
   { 3023,   6458 },
   { 2663,   6458 },
   { 2596,   6691 },
   { 2555,   6691 },
   { 2830,   6429 },
   { 2592,   6429 },
   { 3039,   6905 },
   { 2731,   6905 },
   { 2830,   6429 },
   { 2592,   6429 },
   { 2828,   6505 },
   { 2608,   6505 },
   { 3129,   6789 },
   { 2877,   6789 },
   { 2755,   7208 },
   { 3489,   7208 },
   { 2677,   7187 },
   { 3472,   7187 }
};
#endif


#ifndef DEFINE_GLOBALS
extern TripDelayElement bc2PackageRoundTripDelayArray[BC2_NUMBER_OF_INTERFACES*BC2_NUMBER_OF_PUP];
#else
/* package trace */
TripDelayElement bc2PackageRoundTripDelayArray[BC2_NUMBER_OF_INTERFACES*BC2_NUMBER_OF_PUP] =
{ 
     /*IF BUS DQ_DELYA CK_DELAY */
    {  362,    813 }, 
    {  458,    813 },
    {  405,    750 },
    {  446,    750 },
    {  393,    718 },
    {  331,    718 },
    {  316,    720 },
    {  366,    720 },
    {  512,    805 },
    {  454,    805 },
    {  514,    867 },
    {  380,    867 },
    {  498,    901 },
    {  586,    901 },
    {  523,    932 },
    {  489,    932 },
    {  375,    693 },
    {  400,    693 },
    {  380,    723 },
    {  358,    723 }
};
#endif

#ifndef DEFINE_GLOBALS
extern GT_32 bc2SiliconDelayOffset[];
#else
GT_32 bc2SiliconDelayOffset[] =
{
    /* board 0 */
    0,
    /* board 1 */
    0,
    /* board 2 */
    0
};
#endif

/************************* Functions Declarations ***********************************************/

/******************************************************************************
* Name:     ddr3TipInitBc2.
* Desc:     init Training SW DB and updates DDR topology.
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipInitBc2
(
    GT_U32  devNum,
    GT_U32  boardId
) ;

/******************************************************************************
 * Name:     ddr3GetSdramAssignment
 * Desc:     read S@R and return DDR3 assignment ( 0 = TM , 1 = MSYS )
 * Args:
 * Notes:
 * Returns:  required value
 */
DDR_IF_ASSIGNMENT ddr3GetSdramAssignment(GT_U8 devNum);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsDdr3_BC2_H */

