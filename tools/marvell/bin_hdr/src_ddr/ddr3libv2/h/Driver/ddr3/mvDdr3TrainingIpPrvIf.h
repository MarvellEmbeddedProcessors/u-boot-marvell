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
* mvDdr3TrainingIpPrvIf.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/

#ifndef __mvDdr3TrainingIpPrvIf_H
#define __mvDdr3TrainingIpPrvIf_H



/* General H Files */
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpBist.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************** enums ***************************************/

typedef enum
{
   WRITE_LEVELING_STATIC,
   READ_LEVELING_STATIC

}MV_HWS_STATIC_COFIG_TYPE;


/****************************************************************************/
typedef struct {
	GT_U32							deviceId;
	GT_U32				            ckDelay;

}MV_DDR3_DEVICE_INFO;

/******************************** functores ***************************************/
/************************* Function Definition ***********************************************/

typedef GT_STATUS (*MV_HWS_TIP_DUNIT_MUX_SELECT_FUNC_PTR)
(
	GT_U8                   devNum,
	GT_BOOL                 enable
);

typedef GT_STATUS (*MV_HWS_TIP_DUNIT_REG_READ_FUNC_PTR)
(
	GT_U8                   devNum,
    MV_HWS_ACCESS_TYPE      interfaceAccess,
    GT_U32                  interfaceId,
	GT_U32					offset,
	GT_U32					*data,
	GT_U32					mask
);

typedef GT_STATUS (*MV_HWS_TIP_DUNIT_REG_WRITE_FUNC_PTR)
(
	GT_U8                   devNum,
    MV_HWS_ACCESS_TYPE      interfaceAccess,
    GT_U32                  interfaceId,
	GT_U32					offset,
	GT_U32					data,
	GT_U32					mask
);

typedef GT_STATUS (*MV_HWS_TIP_GET_FREQ_CONFIG_INFO)
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
);

typedef GT_STATUS (*MV_HWS_TIP_GET_DEVICE_INFO)
(
	GT_U8				  devNum,
    MV_DDR3_DEVICE_INFO * infoPtr

);

typedef GT_STATUS (*MV_HWS_GET_CS_CONFIG_FUNC_PTR)
(
	GT_U8							devNum,
    GT_U32                          csMask,
	MV_HWS_CS_CONFIG_INFO		    *csInfo
);

typedef GT_STATUS (*MV_HWS_SET_FREQ_DIVIDER_FUNC_PTR)
(
	GT_U8							devNum,
	GT_U32                  		interfaceId,
    MV_HWS_DDR_FREQ                 freq
);

typedef GT_STATUS (*MV_HWS_GET_INIT_FREQ)
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 *freq
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_IF_WRITE_FUNC_PTR)
(
	GT_U32              devNum,
	MV_HWS_ACCESS_TYPE  accessType,
	GT_U32              dunitId, 
	GT_U32              regAddr, 
	GT_U32              data,
	GT_U32              mask 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_IF_READ_FUNC_PTR)
( 
	GT_U32              devNum,
	MV_HWS_ACCESS_TYPE  accessType,
	GT_U32              dunitId, 
	GT_U32              regAddr, 
	GT_U32              *data,
	GT_U32              mask 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_BUS_WRITE_FUNC_PTR)
(
	GT_U32              devNum,
	MV_HWS_ACCESS_TYPE  dunitAccessType,
	GT_U32              interfaceId, 
	MV_HWS_ACCESS_TYPE  phyAccessType,
	GT_U32              phyId, 
	MV_HWS_DDR_PHY      phyType, 
	GT_U32              regAddr, 
	GT_U32              data 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_BUS_READ_FUNC_PTR)
( 
	GT_U32				devNum,
	GT_U32				interfaceId, 
	MV_HWS_ACCESS_TYPE	phyAccessType,
	GT_U32				phyId, 
	MV_HWS_DDR_PHY		phyType, 
	GT_U32				regAddr, 
	GT_U32				*data 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_ALGO_RUN_FUNC_PTR)
(
	GT_U32            devNum,
    MV_HWS_ALGO_TYPE  algoType
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_SET_FREQ_FUNC_PTR)
(
	GT_U32				devNum, 
	MV_HWS_ACCESS_TYPE  accessType, 
	GT_U32              interfaceId, 
    MV_HWS_DDR_FREQ	frequency
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_INIT_CONTROLLER_FUNC_PTR)
(
	GT_U32   devNum,
    InitCntrParam   *initCntrPrm
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_PBS_RX_FUNC_PTR)
(
	GT_U32   devNum
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_PBS_TX_FUNC_PTR)
(
	GT_U32   devNum
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_SELECT_CONTROLLER_FUNC_PTR)
(
	GT_U32    devNum,
    GT_BOOL   enable 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_TOPOLOGY_MAP_LOAD_FUNC_PTR)
( 
	GT_U32                devNum,
    MV_HWS_TOPOLOGY_MAP   *topologyMap 
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_STATIC_CONFIG_FUNC_PTR)
( 
	GT_U32						devNum,
	MV_HWS_DDR_FREQ			frequency,
	MV_HWS_STATIC_COFIG_TYPE	staticConfigType,
	GT_U32						interfaceId
);


typedef GT_STATUS (*MV_HWS_TRAINING_IP_EXTERNAL_READ_PTR)
(
	GT_U32    devNum,
    GT_U32    InterfcaeID,
	GT_U32    DdrAddr,
	GT_U32    numBursts,
	GT_U32    *data
);



typedef GT_STATUS (*MV_HWS_TRAINING_IP_EXTERNAL_WRITE_PTR)
(
	GT_U32    devNum,
    GT_U32    InterfcaeID,
	GT_U32    DdrAddr,
	GT_U32    numBursts,
	GT_U32    *data
);


typedef GT_STATUS (*MV_HWS_TRAINING_IP_BIST_ACTIVATE)
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
);


typedef GT_STATUS (*MV_HWS_TRAINING_IP_BIST_READ_RESULT)
(
    GT_U32          devNum,
    GT_U32          interfaceId,
    BistResult      *pstBistResult
);


typedef GT_STATUS (*MV_HWS_TRAINING_IP_LOAD_TOPOLOGY)
(
	GT_U32    devNum,
    GT_U32    configNum
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_READ_LEVELING)
(
	GT_U32    devNum,
    GT_U32    configNum
);

typedef GT_STATUS (*MV_HWS_TRAINING_IP_WRITE_LEVELING)
(
	GT_U32    devNum,
    GT_U32    configNum
);

typedef GT_U32 (*MV_HWS_TRAINING_IP_GET_TEMP)
(
	GT_U8    devNum
);

typedef GT_U8 (*MV_HWS_TRAINING_IP_GET_RATIO)
(
	GT_U32    freq
);


/******************************** Functions Strut ***************************************/
typedef struct
{
	MV_HWS_TIP_DUNIT_MUX_SELECT_FUNC_PTR	tipDunitMuxSelectFunc;
	MV_HWS_TIP_DUNIT_REG_READ_FUNC_PTR		tipDunitReadFunc;
	MV_HWS_TIP_DUNIT_REG_WRITE_FUNC_PTR		tipDunitWriteFunc;
	MV_HWS_TIP_GET_FREQ_CONFIG_INFO			tipGetFreqConfigInfoFunc;
	MV_HWS_TIP_GET_DEVICE_INFO			    tipGetDeviceInfoFunc;
    MV_HWS_SET_FREQ_DIVIDER_FUNC_PTR        tipSetFreqDividerFunc;
    MV_HWS_GET_CS_CONFIG_FUNC_PTR           tipGetCsConfigInfo;
	MV_HWS_TRAINING_IP_GET_TEMP				tipGetTemperature;
	MV_HWS_TRAINING_IP_GET_RATIO			tipGetClockRatio;
    MV_HWS_TRAINING_IP_EXTERNAL_READ_PTR	tipExternalRead;
    MV_HWS_TRAINING_IP_EXTERNAL_WRITE_PTR	tipExternalWrite;

} MV_HWS_TIP_CONFIG_FUNC_DB;


typedef struct
{
	MV_HWS_TRAINING_IP_IF_WRITE_FUNC_PTR			trainingIpIfWrite;
	MV_HWS_TRAINING_IP_IF_READ_FUNC_PTR				trainingIpIfRead;
	MV_HWS_TRAINING_IP_BUS_WRITE_FUNC_PTR			trainingIpBusWrite;
	MV_HWS_TRAINING_IP_BUS_READ_FUNC_PTR			trainingIpBusRead;
	MV_HWS_TRAINING_IP_ALGO_RUN_FUNC_PTR			trainingIpAlgoRun;
	MV_HWS_TRAINING_IP_SET_FREQ_FUNC_PTR			trainingIpSetFreq;
	MV_HWS_TRAINING_IP_INIT_CONTROLLER_FUNC_PTR		trainingIpInitController;
	MV_HWS_TRAINING_IP_PBS_RX_FUNC_PTR				trainingIpPbsRx;
	MV_HWS_TRAINING_IP_PBS_TX_FUNC_PTR				trainingIpPbsTx;
	MV_HWS_TRAINING_IP_SELECT_CONTROLLER_FUNC_PTR	trainingIpSelectController;
	MV_HWS_TRAINING_IP_TOPOLOGY_MAP_LOAD_FUNC_PTR	trainingIpTopologyMapLoad;
	MV_HWS_TRAINING_IP_STATIC_CONFIG_FUNC_PTR		trainingIpStaticConfig;
	MV_HWS_TRAINING_IP_EXTERNAL_READ_PTR			trainingIpExternalRead;
	MV_HWS_TRAINING_IP_EXTERNAL_WRITE_PTR			trainingIpExternalWrite;
    MV_HWS_TRAINING_IP_BIST_ACTIVATE                trainingIpBistActivate;
    MV_HWS_TRAINING_IP_BIST_READ_RESULT             trainingIpBistReadResult;
    MV_HWS_TRAINING_IP_LOAD_TOPOLOGY                trainingIpLoadTopology;

}MV_HWS_TRAINING_IP_FUNC_PTRS;

/******************************** declarations ***************************************/

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
);

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
);

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
	GT_U32				interfaceId, 
	MV_HWS_ACCESS_TYPE	phyAccessType,
	GT_U32				phyId, 
	MV_HWS_DDR_PHY		phyType, 
	GT_U32				regAddr, 
	GT_U32				*data 
);

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
	GT_U32              devNum,
	MV_HWS_ACCESS_TYPE  dunitAccessType,
	GT_U32              interfaceId, 
	MV_HWS_ACCESS_TYPE  phyAccessType,
	GT_U32              phyId, 
	MV_HWS_DDR_PHY      phyType, 
	GT_U32              regAddr, 
	GT_U32              data 
);

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
);

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
	MV_HWS_ACCESS_TYPE  accessType, 
	GT_U32              interfaceId, 
    MV_HWS_DDR_FREQ	frequency
);

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
);

/*****************************************************************************
Print device informtion
******************************************************************************/
/******************************************************************************
* Name:     printDeviceInfo
* Desc:     print device info
* Args:     device number
*
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS printDeviceInfo( GT_U8      devNum);
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
);

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
);

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
);




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
	GT_U32    *data
);



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
	GT_U32    *data
);

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
);

/*******************************************************************************
* mvHwsTrainingIpStaticConfig
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
	
);

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
);

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
);

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
void mvHwsTrainingIpGetFuncPtr(MV_HWS_TRAINING_IP_FUNC_PTRS **hwsFuncsPtr);


/*******************************************************************************
* mvHwsDdr3BistWrapper
*
* DESCRIPTION:
*       Test DDR functionality
*
* INPUTS:
*       devNum.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BistWrapper
( 
	GT_U32			 devNum,
    MV_HWS_PATTERN   pattern
);




/******************************************************************************
* Name:     mvHwsDdr3TipIfInit.
* Desc:     register ddr training functions
* Args:     
* Notes:
* Returns:  OK if success, other error code if fail.
*/
void mvHwsDdr3TipIfInit
(
    MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray
);


/******************************************************************************
* Name:     mvHwsDdr3TipInitConfigFunc.
* Desc:     register access func registration
* Args:    
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS mvHwsDdr3TipInitConfigFunc
(
	GT_U32						devNum, 
	MV_HWS_TIP_CONFIG_FUNC_DB	*configFunc
);

/******************************************************************************
* Name:     ddr3TipRegisterXsbInfo.
* Desc:     register XSB Info
* Args:     
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipRegisterXsbInfo
(
    GT_U32					devNum, 
    MV_HWS_XSB_INFO         *xsbInfoTable
);

/******************************************************************************
* Name:     ddr3TipGetResultPtr.
* Desc:     Get Training Results
* Args:     
* Notes:
* Returns:  OK if success, other error code if fail.
*/
MV_HWS_RESULT* ddr3TipGetResultPtr
(
    GT_U32 stage
);

/******************************************************************************
* Name:     ddr3SetFreqConfigInfo.
* Desc:     Set frequency table
* Args:     
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3SetFreqConfigInfo
(
    MV_HWS_TIP_FREQ_CONFIG_INFO* table
);


#ifdef __cplusplus
}
#endif

#endif /* __mvDdr3TrainingIpPrvIf_H */

