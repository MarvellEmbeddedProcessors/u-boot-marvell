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
* mvCfgElementDb.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/

#ifndef __mvCfgElementDb_H
#define __mvCfgElementDb_H

#include "gtGenTypes.h"
#include "mvSiliconIf.h"

/* action type */
typedef enum {

    WRITE_OP,
    DELAY_OP,
    POLLING_OP,
    DUNIT_WRITE_OP,
    DUNIT_POLLING_OP,
    LAST_NON_OP

}MV_EL_DB_OPERATION;

typedef struct
{
    GT_U32   indexOffset;
    GT_U32   regOffset;
    GT_U32   data;
    GT_U32   mask;

}MV_WRITE_OP_PARAMS;

typedef struct
{
    GT_U32   indexOffset;
    GT_U32   regOffset;
    GT_U32   data;
    GT_U32   mask;
    GT_U32   waitTime;
    GT_U32   numOfLoops;

}MV_POLLING_OP_PARAMS;

typedef struct
{
    GT_U32   delay; /* msec */

}MV_DELAY_OP_PARAMS;

typedef struct
{
    MV_EL_DB_OPERATION op;
    void               *params;

}MV_CFG_ELEMENT;

typedef struct
{
    GT_U32          seqId;
    GT_U32          cfgSeqSize;
    MV_CFG_ELEMENT  *cfgSeq;

}MV_CFG_SEQ;


/********************** new format sequence DB ****************************************/
typedef struct
{
    GT_U16   unitId;
    GT_U32   regOffset;
    GT_U32   operData;
    GT_U32   mask;
    GT_U32   waitTime;
    GT_U32   numOfLoops;

}MV_OP_PARAMS;

/*******************************************************************************************/

typedef GT_STATUS (*MV_OPER_RUN_FUNCPTR)
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_U32           unitBaseAddr,
    MV_CFG_ELEMENT  *element
);

typedef GT_STATUS (*MV_PARALLEL_OPER_RUN_FUNCPTR)
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          numOfSer,
    GT_UOPT          *serdesArr,
    MV_CFG_ELEMENT   *element
);

typedef GT_STATUS (*MV_BULK_SERDES_OPER_RUN_FUNCPTR)
(
    GT_U32                  numOfSer,
    MV_HWS_ELEMENTS_ARRAY   *serdesArr,
    MV_CFG_ELEMENT          *element
);

typedef GT_STATUS (*MV_SERDES_OPER_RUN_FUNCPTR)
(
    GT_U8          devNum,
    GT_UOPT        portGroup,
    GT_UOPT        serdesNum,
    MV_OP_PARAMS   *params
);

typedef GT_STATUS (*MV_PARALLEL_SERDES_OPER_RUN_FUNCPTR)
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    MV_OP_PARAMS    *element
);

/*******************************************************************************
* mvCfgSeqExec
*
* DESCRIPTION:
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqExec
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          index,
    MV_CFG_ELEMENT  *element,
    GT_U32           elementSize
);

/*******************************************************************************
* mvCfgSerdesSeqExec
*
* DESCRIPTION:
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqExec
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       serdesNum,
    GT_U8         seqId
);

/*******************************************************************************
* mvCfgSerdesSeqparallelExec
*
* DESCRIPTION:
*      Run Serdes parallel configuration sequence (in the manner of lanes)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqParallelExec
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          numOfSer,
    GT_UOPT          *serdesArr,
    MV_CFG_ELEMENT   *element,
    GT_U32           elementSize
);

/*******************************************************************************
* mvCfgSerdesNewSeqParallelExec
*
* DESCRIPTION:
*      Run Serdes parallel configuration sequence (in the manner of lanes)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesNewSeqParallelExec
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    GT_UOPT         seqId
);

/*******************************************************************************
* mvCfgSeqBulkExec
*
* DESCRIPTION:
*      Run bulk configuration sequence (in the manner of lanes)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqBulkExec
(
    GT_U32                  numOfSer,
    MV_HWS_ELEMENTS_ARRAY   *serdesArr,
    MV_CFG_ELEMENT          *element,
    GT_U32                  elementSize
);

/*******************************************************************************
* mvCfgSeqExecInit
*
* DESCRIPTION:
*      Init configuration sequence executer
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvCfgSeqExecInit ();


/*******************************************************************************
* mvCfgSerdesSeqExecInit
*
* DESCRIPTION:
*      Init SERDES configuration sequence executer
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvCfgSerdesSeqExecInit
(
    MV_CFG_SEQ *seqDb,
    GT_U8       numberOfSeq
);

void hwsGenSeqInit(MV_CFG_SEQ *dbPtr, GT_U8 numOfSeq);


#endif /* __mvCfgElementDb_H */

