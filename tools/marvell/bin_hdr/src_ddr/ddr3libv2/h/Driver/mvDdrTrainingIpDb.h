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
* mvDdrTrainingIpDb.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/




#ifndef _MV_DDR_TRAININGIP_DB_H_
#define _MV_DDR_TRAININGIP_DB_H_

#include "mvDdrTopologyDef.h"
#ifdef CONFIG_DDR3
typedef enum
{
   PATTERN_PBS1,
   PATTERN_PBS2,
   PATTERN_PBS3,
   PATTERN_TEST,
   PATTERN_RL,
   PATTERN_RL2,
   PATTERN_STATIC_PBS,
   PATTERN_KILLER_DQ0,
   PATTERN_KILLER_DQ1,
   PATTERN_KILLER_DQ2,
   PATTERN_KILLER_DQ3,
   PATTERN_KILLER_DQ4,
   PATTERN_KILLER_DQ5,
   PATTERN_KILLER_DQ6,
   PATTERN_KILLER_DQ7,
   PATTERN_VREF,
   PATTERN_FULL_SSO0,
   PATTERN_FULL_SSO1,
   PATTERN_FULL_SSO2,
   PATTERN_FULL_SSO3,
   PATTERN_SSO_FULL_XTALK_DQ0,
   PATTERN_SSO_FULL_XTALK_DQ1,
   PATTERN_SSO_FULL_XTALK_DQ2,
   PATTERN_SSO_FULL_XTALK_DQ3,
   PATTERN_SSO_FULL_XTALK_DQ4,
   PATTERN_SSO_FULL_XTALK_DQ5,
   PATTERN_SSO_FULL_XTALK_DQ6,
   PATTERN_SSO_FULL_XTALK_DQ7,
   PATTERN_SSO_XTALK_FREE_DQ0,
   PATTERN_SSO_XTALK_FREE_DQ1,
   PATTERN_SSO_XTALK_FREE_DQ2,
   PATTERN_SSO_XTALK_FREE_DQ3,
   PATTERN_SSO_XTALK_FREE_DQ4,
   PATTERN_SSO_XTALK_FREE_DQ5,
   PATTERN_SSO_XTALK_FREE_DQ6,
   PATTERN_SSO_XTALK_FREE_DQ7,
   PATTERN_ISI_XTALK_FREE,
   PATTERN_LIMIT
} MV_HWS_PATTERN;
#elif defined(CONFIG_DDR4)
typedef enum
{
   PATTERN_PBS1,/*0*/
   PATTERN_PBS2,
   PATTERN_PBS3,
   PATTERN_TEST,
   PATTERN_RL,
   PATTERN_RL2,
   PATTERN_STATIC_PBS,
   PATTERN_KILLER_DQ0,
   PATTERN_KILLER_DQ1,
   PATTERN_KILLER_DQ2,
   PATTERN_KILLER_DQ3,/*10*/
   PATTERN_KILLER_DQ4,
   PATTERN_KILLER_DQ5,
   PATTERN_KILLER_DQ6,
   PATTERN_KILLER_DQ7,
   PATTERN_KILLER_DQ0_INV,
   PATTERN_KILLER_DQ1_INV,
   PATTERN_KILLER_DQ2_INV,
   PATTERN_KILLER_DQ3_INV,
   PATTERN_KILLER_DQ4_INV,
   PATTERN_KILLER_DQ5_INV,/*20*/
   PATTERN_KILLER_DQ6_INV,
   PATTERN_KILLER_DQ7_INV,
   PATTERN_VREF,
   PATTERN_VREF_INV,
   PATTERN_FULL_SSO0,
   PATTERN_FULL_SSO1,
   PATTERN_FULL_SSO2,
   PATTERN_FULL_SSO3,
   PATTERN_SSO_FULL_XTALK_DQ0,
   PATTERN_SSO_FULL_XTALK_DQ1,/*30*/
   PATTERN_SSO_FULL_XTALK_DQ2,
   PATTERN_SSO_FULL_XTALK_DQ3,
   PATTERN_SSO_FULL_XTALK_DQ4,
   PATTERN_SSO_FULL_XTALK_DQ5,
   PATTERN_SSO_FULL_XTALK_DQ6,
   PATTERN_SSO_FULL_XTALK_DQ7,
   PATTERN_SSO_XTALK_FREE_DQ0,
   PATTERN_SSO_XTALK_FREE_DQ1,
   PATTERN_SSO_XTALK_FREE_DQ2,
   PATTERN_SSO_XTALK_FREE_DQ3,/*40*/
   PATTERN_SSO_XTALK_FREE_DQ4,
   PATTERN_SSO_XTALK_FREE_DQ5,
   PATTERN_SSO_XTALK_FREE_DQ6,
   PATTERN_SSO_XTALK_FREE_DQ7,
   PATTERN_ISI_XTALK_FREE,
   PATTERN_RESONANCE_1T,
   PATTERN_RESONANCE_2T,
   PATTERN_RESONANCE_3T,
   PATTERN_RESONANCE_4T,
   PATTERN_RESONANCE_5T,/*50*/
   PATTERN_RESONANCE_6T,
   PATTERN_RESONANCE_7T,
   PATTERN_RESONANCE_8T,
   PATTERN_RESONANCE_9T,
   PATTERN_LIMIT
}MV_HWS_PATTERN;
#else
#error "CONFIG_DDR3 or CONFIG_DDR4 must be defined !!!"
#endif


#ifdef __cplusplus
   extern "C"
   {
#endif

typedef enum
{
    WriteLevelingSupp_RegMode,
    WriteLevelingSupp_ECCMode_DataPups,
    WriteLevelingSupp_ECCMode_ECCPup4,
    WriteLevelingSupp_ECCMode_ECCPup3
}MV_HWS_WL_SUPP_MODE;

typedef enum
{
    MV_ATTR_TIP_REV,
    MV_ATTR_PHY_EDGE,
    MV_ATTR_OCTET_PER_INTERFACE,
    MV_ATTR_PLL_BEFORE_INIT,
    MV_ATTR_TUNE_MASK,
    MV_ATTR_INIT_FREQ,
    MV_ATTR_MID_FREQ,
    MV_ATTR_DFS_LOW_FREQ,
    MV_ATTR_DFS_LOW_PHY,
    MV_ATTR_DELAY_ENABLE,
    MV_ATTR_CK_DELAY,
    MV_ATTR_CA_DELAY,
    MV_ATTR_INTERLEAVE_WA,
    MV_ATTR_LAST
} MV_DDR_DEV_ATTRIBUTE;

typedef enum
{
    MV_TIP_REV_NA,
    MV_TIP_REV_1, /* NP5 */
    MV_TIP_REV_2, /* BC2 */
    MV_TIP_REV_3, /* AC3 */
    MV_TIP_REV_4, /* A-380/A-390 */
    MV_TIP_REV_LAST
} MV_DDR_TIP_REVISION;

typedef enum
{
    MV_DDR_TRAINING_CONTROLLER_TIP,
    MV_DDR_TRAINING_CONTROLLER_CPU
} MV_DDR_TRAINING_CONTROLLER;

typedef enum
{
    MV_DDR_PHY_EDGE_POSITIVE,
    MV_DDR_PHY_EDGE_NEGATIVE
} MV_DDR_PHY_EDGE;


/************************* Declarations ***********************************************/

GT_U32 speedBinTable
(
    GT_U8 index,
    speedBinTableElements element
);

INLINE GT_U32 patternTableGetWord
(
    GT_U32  devNum,
    MV_HWS_PATTERN type,
    GT_U8 index
);

/* Device topology functionality */
MV_HWS_TOPOLOGY_MAP* ddr3TipGetTopologyMap
(
    GT_U32  devNum
);

void ddr3TipSetTopologyMap
(
    GT_U32  devNum,
    MV_HWS_TOPOLOGY_MAP* topology
);

/******************************************************************************
* Name:     ddr3TipDevAttrInit.
* Desc:     Init device attributes structures
* Args:
* Notes:
* Returns:  none.
*/
void    ddr3TipDevAttrInit
(
    GT_U32  devNum
);

/******************************************************************************
* Name:     ddr3TipDevAttrGet.
* Desc:     get specified device attribute value
* Args:
* Notes:
* Returns:  none.
*/
GT_U32    ddr3TipDevAttrGet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId
);

/******************************************************************************
* Name:     ddr3TipDevAttrSet.
* Desc:     set specified device attribute value
* Args:
* Notes:
* Returns:  none.
*/
void    ddr3TipDevAttrSet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId,
    GT_U32                  value
);

#endif /* _MV_DDR3_TRAININGIP_DB_H_ */


