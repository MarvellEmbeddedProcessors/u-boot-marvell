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
#include "mvDdr3TrainingIpDb.h"
#elif defined(CONFIG_DDR4)
#include "mvDdr4TrainingIpDb.h"
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
    MV_ATTR_PLL_BEFORE_INIT,
    MV_ATTR_TUNE_MASK,
    MV_ATTR_INIT_FREQ,
    MV_ATTR_MID_FREQ,
    MV_ATTR_DFS_LOW_FREQ,
    MV_ATTR_DFS_LOW_PHY,
    MV_ATTR_DELAY_ENABLE,
    MV_ATTR_CK_DELAY,
    MV_ATTR_CA_DELAY,
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

/* Device attributes functionality */
GT_U32    ddr3TipDevAttrGet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId
);

void    ddr3TipDevAttrSet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId,
    GT_U32                  value
);

#endif /* _MV_DDR3_TRAININGIP_DB_H_ */


