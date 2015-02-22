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
* mvHwsDdr3TrainingIpIf.c
*
* DESCRIPTION:
*       Training IP function interface
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/

#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3TrainingIpBist.h"

static MV_HWS_TRAINING_IP_FUNC_PTRS hwsTrainingIpFuncsPtr;


/*******************************************************************************
* mvHwsTrainingIpIfWrite
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpIfWrite
(
	GT_U32				devNum,
	MV_HWS_ACCESS_TYPE	accessType,
	GT_U32				dunitId, 
	GT_U32				regAddr, 
	GT_U32				data,
	GT_U32				mask
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpIfWrite == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsTrainingIpFuncsPtr.trainingIpIfWrite(devNum, accessType, dunitId, regAddr, data, mask);
}

/*******************************************************************************
* mvHwsTrainingIpIfRead
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpIfRead
(
	GT_U32				devNum,
	MV_HWS_ACCESS_TYPE	accessType,
	GT_U32				dunitId, 
	GT_U32				regAddr, 
	GT_U32				*data,
	GT_U32				mask
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpIfRead == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpIfRead(devNum, accessType, dunitId, regAddr, data, mask);
}



/*******************************************************************************
* mvHwsTrainingIpBusRead
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpBusRead
( 
	GT_U32				devNum,
	GT_U32				uiInterfaceId, 
	MV_HWS_ACCESS_TYPE	phyAccessType,
	GT_U32				phyId, 
	MV_HWS_DDR_PHY		phyType, 
	GT_U32				regAddr, 
	GT_U32				*data 
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpBusRead == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsTrainingIpFuncsPtr.trainingIpBusRead(devNum, uiInterfaceId, phyAccessType, phyId, 
												   phyType, regAddr, data);
}

/*******************************************************************************
* mvHwsTrainingIpBusWrite
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpBusWrite
(
	GT_U32             devNum,
	MV_HWS_ACCESS_TYPE  dunitAccessType,
	GT_U32             interfaceId, 
	MV_HWS_ACCESS_TYPE  phyAccessType,
	GT_U32             phyId, 
	MV_HWS_DDR_PHY     phyType, 
	GT_U32             regAddr, 
	GT_U32             data 
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpBusWrite == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpBusWrite(devNum, dunitAccessType, interfaceId, phyAccessType,
													phyId, phyType, regAddr, data);
}

/*******************************************************************************
* mvHwsTrainingIpAlgoRun
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpAlgoRun
(
	GT_U32            devNum,
    MV_HWS_ALGO_TYPE  algoType
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpAlgoRun == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpAlgoRun(devNum, algoType);
}

/*******************************************************************************
* mvHwsTrainingIpSetFreq
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpSetFreq
(
	GT_U32				devNum, 
	MV_HWS_ACCESS_TYPE   accessType, 
	GT_U32              interfaceId, 
    MV_HWS_DDR_FREQ	frequency
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpSetFreq == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpSetFreq(devNum, accessType, interfaceId, frequency);
}

/*******************************************************************************
* mvHwsTrainingIpInitController
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpInitController
(
	GT_U32   devNum,
    InitCntrParam   *initCntrPrm
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpInitController == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpInitController(devNum, initCntrPrm);
}

/*******************************************************************************
* mvHwsTrainingIpPbsRx
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpPbsRx
(
	GT_U32   devNum
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpPbsRx == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpPbsRx(devNum);
}

/*******************************************************************************
* mvHwsTrainingIpPbsTx
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpPbsTx
(
	GT_U32   devNum
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpPbsTx == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpPbsTx(devNum);
}

/*******************************************************************************
* mvHwsTrainingIpSelectController
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpSelectController
(
	GT_U32    devNum,
    GT_BOOL   enable 
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpSelectController == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpSelectController(devNum, enable);
}



/*******************************************************************************
* mvHwsTrainingIpExtRead
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpExtRead
(
	GT_U32    devNum,
    GT_U32    InterfcaeID,
	GT_U32    DdrAddr,
	GT_U32    numBursts,
	GT_U32    *Addr
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpExternalRead == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
	return hwsTrainingIpFuncsPtr.trainingIpExternalRead(devNum, InterfcaeID, DdrAddr, numBursts, Addr);
}


/*******************************************************************************
* mvHwsTrainingIpExtWrite
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpExtWrite
(
	GT_U32    devNum,
    GT_U32    InterfcaeID,
	GT_U32    DdrAddr,
	GT_U32    numBursts,
	GT_U32    *Addr
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpExternalWrite == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
	return hwsTrainingIpFuncsPtr.trainingIpExternalWrite(devNum, InterfcaeID, DdrAddr, numBursts, Addr);
}


/*******************************************************************************
* mvHwsTrainingIpTopologyMapLoad
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpTopologyMapLoad
( 
	GT_U32                devNum,
    MV_HWS_TOPOLOGY_MAP   *topologyMap 
	)
{
	if (hwsTrainingIpFuncsPtr.trainingIpTopologyMapLoad == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpTopologyMapLoad(devNum, topologyMap);
}

/*******************************************************************************
* mvHwsTrainingIpIfWrite
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpStaticConfig
( 
	GT_U32						devNum,
	MV_HWS_DDR_FREQ			frequency, 
	MV_HWS_STATIC_COFIG_TYPE	staticConfigType,
	GT_U32						interfaceId
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpStaticConfig == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpStaticConfig(devNum, frequency, staticConfigType, interfaceId);
}

/*******************************************************************************
* mvHwsTrainingIpBistActivate
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpBistActivate
(
    GT_U32                  devNum,
    MV_HWS_PATTERN          pattern,
    MV_HWS_ACCESS_TYPE      accessType,
    GT_U32                  ifNum,
    MV_HWS_DIRECTION        direction,
    MV_HWS_STRESS_JUMP      addrStressJump,
    MV_HWS_PATTERN_DURATION duration,
    MV_HWS_BIST_OPERATION   operType,
    GT_U32                  offset,
    GT_U32                  csNum,
    GT_U32                  patternAddrLength
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpBistActivate == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsTrainingIpFuncsPtr.trainingIpBistActivate(devNum, pattern, accessType, ifNum,
                                                        direction, addrStressJump, duration, operType,
                                                        offset, csNum, patternAddrLength);
}

/*******************************************************************************
* mvHwsTrainingIpBistReadResult
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsTrainingIpBistReadResult
(
    GT_U32          devNum,
    GT_U32          interfaceId,
    BistResult      *pstBistResult
)
{
	if (hwsTrainingIpFuncsPtr.trainingIpBistReadResult == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
	
    return hwsTrainingIpFuncsPtr.trainingIpBistReadResult(devNum, interfaceId, pstBistResult);
}

/*******************************************************************************
* mvHwsTrainingIpGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvHwsTrainingIpGetFuncPtr(MV_HWS_TRAINING_IP_FUNC_PTRS **hwsFuncsPtr)
{
    *hwsFuncsPtr = &hwsTrainingIpFuncsPtr;
}



/*******************************************************************************
* mvHwsDdr3TipIfInit
*
* DESCRIPTION:
*       Register DDR Training functions
*
* INPUTS:
*       MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvHwsDdr3TipIfInit(MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray)
{
    funcPtrArray->trainingIpInitController = mvHwsDdr3TipInitController;
    funcPtrArray->trainingIpAlgoRun= mvHwsDdr3TipRunAlg;
    funcPtrArray->trainingIpBusRead = mvHwsDdr3TipBUSRead;
    funcPtrArray->trainingIpBusWrite = mvHwsDdr3TipBUSWrite;
    funcPtrArray->trainingIpIfRead = mvHwsDdr3TipIFRead;
    funcPtrArray->trainingIpIfWrite = mvHwsDdr3TipIFWrite;
    funcPtrArray->trainingIpPbsRx = ddr3TipPbsRx;
    funcPtrArray->trainingIpPbsTx = ddr3TipPbsTx;
    funcPtrArray->trainingIpSelectController = mvHwsDdr3TipSelectDdrController;
    funcPtrArray->trainingIpSetFreq = ddr3TipFreqSet;
    funcPtrArray->trainingIpStaticConfig = NULL;
    funcPtrArray->trainingIpTopologyMapLoad = mvHwsDdr3TipLoadTopologyMap;
    funcPtrArray->trainingIpExternalRead = ddr3TipExtRead;
    funcPtrArray->trainingIpExternalWrite = ddr3TipExtWrite;
	funcPtrArray->trainingIpBistActivate = ddr3TipBistActivate;
	funcPtrArray->trainingIpBistReadResult = ddr3TipBistReadResult;
}

