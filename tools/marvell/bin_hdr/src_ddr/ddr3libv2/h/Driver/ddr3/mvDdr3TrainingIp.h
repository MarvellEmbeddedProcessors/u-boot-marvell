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
*     Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*      Neither the name of Marvell nor the names of its contributors may be
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
* mvDdr3TrainingIp.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 67 $
*
*******************************************************************************/

#ifndef _MV_DDR3_TRAININGIP_H_
#define _MV_DDR3_TRAININGIP_H_

#include "mvDdr3TrainingIpDef.h"
#include "mvDdrTopologyDef.h"
#include "mvDdrTrainingIpDb.h"

#ifdef __cplusplus
   extern "C"
   {
#endif

#ifdef CONFIG_DDR4
#define DDR3_TIP_VERSION_STRING "DDR4 Training Sequence - Ver TIP-0.23."
#else
#define DDR3_TIP_VERSION_STRING "DDR3 Training Sequence - Ver TIP-1.55."
#endif

#define MAX_CS_NUM         (4)
#define MAX_TOTAL_BUS_NUM  (MAX_INTERFACE_NUM * MAX_BUS_NUM)
#define MAX_DQ_NUM         (40)

#define GET_MIN(arg1,arg2) (arg1<arg2) ? (arg1) : (arg2);
#define GET_MAX(arg1,arg2) (arg1<arg2) ? (arg2) : (arg1);

#define INIT_CONTROLLER_MASK_BIT          0x00000001
#define STATIC_LEVELING_MASK_BIT          0x00000002
#define SET_LOW_FREQ_MASK_BIT             0x00000004
#define LOAD_PATTERN_MASK_BIT             0x00000008
#define SET_MEDIUM_FREQ_MASK_BIT          0x00000010
#define WRITE_LEVELING_MASK_BIT           0x00000020
#define LOAD_PATTERN_2_MASK_BIT           0x00000040
#define READ_LEVELING_MASK_BIT            0x00000080
#define SW_READ_LEVELING_MASK_BIT         0x00000100
#define WRITE_LEVELING_SUPP_MASK_BIT      0x00000200
#define PBS_RX_MASK_BIT                   0x00000400
#define PBS_TX_MASK_BIT                   0x00000800
#define SET_TARGET_FREQ_MASK_BIT          0x00001000
#define ADJUST_DQS_MASK_BIT               0x00002000
#define WRITE_LEVELING_TF_MASK_BIT        0x00004000
#define LOAD_PATTERN_HIGH_MASK_BIT		  0x00008000
#define READ_LEVELING_TF_MASK_BIT         0x00010000
#define WRITE_LEVELING_SUPP_TF_MASK_BIT   0x00020000
#define DM_PBS_TX_MASK_BIT                0x00040000
#define CENTRALIZATION_RX_MASK_BIT        0x00100000
#define CENTRALIZATION_TX_MASK_BIT        0x00200000
#define TX_EMPHASIS_MASK_BIT              0x00400000
#define PER_BIT_READ_LEVELING_TF_MASK_BIT 0x00800000
#define VREF_CALIBRATION_MASK_BIT		  0x01000000

/* DDR4 Specific Training Mask bits */
#ifdef CONFIG_DDR4
#define RECEIVER_CALIBRATION_MASK_BIT     0x04000000
#define WL_PHASE_CORRECTION_MASK_BIT      0x08000000
#define DQ_VREF_CALIBRATION_MASK_BIT      0x10000000
#define DQ_MAPPING_MASK_BIT               0x20000000
#endif

typedef enum
{ 
    TEST_FAILED  =  0,
    TEST_SUCCESS =  1,
    NO_TEST_DONE =  2
}MV_HWS_RESULT;


typedef enum
{
    RESULT_PER_BIT,
    RESULT_PER_BYTE
} MV_HWS_TRAINING_RESULT;


/************************* Definitions ***********************************************/
typedef enum
{
	INIT_CONTROLLER,
	STATIC_LEVELING,
	SET_LOW_FREQ,
	LOAD_PATTERN,
	SET_MEDIUM_FREQ,
	WRITE_LEVELING,
	LOAD_PATTERN_2,
	READ_LEVELING,
#ifdef CONFIG_DDR4
	SW_READ_LEVELING,
#endif
	WRITE_LEVELING_SUPP,
	PBS_RX,
	PBS_TX,
	SET_TARGET_FREQ,
	ADJUST_DQS,
	WRITE_LEVELING_TF,
	READ_LEVELING_TF,
	WRITE_LEVELING_SUPP_TF,
	DM_PBS_TX,
	VREF_CALIBRATION,
	CENTRALIZATION_RX,
	CENTRALIZATION_TX,
	TX_EMPHASIS,
	LOAD_PATTERN_HIGH,
	PER_BIT_READ_LEVELING_TF,
#ifdef CONFIG_DDR4
	RECEIVER_CALIBRATION,
	WL_PHASE_CORRECTION,
	DQ_VREF_CALIBRATION,
	DQ_MAPPING,
#endif
	MAX_STAGE_LIMIT
}AUTO_TUNE_STAGE;


/************************* Enums ***********************************************/


typedef enum 
{
   ACCESS_TYPE_UNICAST = 0,
   ACCESS_TYPE_MULTICAST = 1
} MV_HWS_ACCESS_TYPE;

typedef enum 
{
   ALGO_TYPE_DYNAMIC,
   ALGO_TYPE_STATIC

} MV_HWS_ALGO_TYPE;

/************************* Structures ***********************************************/
typedef struct
{
    GT_BOOL isCtrl64Bit;
    GT_BOOL doMrsPhy;
    GT_BOOL initPhy;
    GT_BOOL msysInit;
}InitCntrParam;


/***********************************************/
typedef struct 
{
   GT_U8   numOfPhasesTx;
   GT_U8   txBurstSize;
   GT_U8   delayBetweenBursts;
   GT_U8   numOfPhasesRx;
   GT_U32   startAddr;
   GT_U8   patternLen;

}PatternInfo;


/***********************************/

/* CL value for each frequency */
typedef struct
{
  GT_U8 clVal[DDR_FREQ_LIMIT];

}ClValuePerFreq;


typedef struct
{
    GT_U8                  csNum;
    GT_U8                  numOfCs;
} CsElement;

typedef struct
{
   /*32 bits representing MRS bits*/
   GT_U32                  regMR0[MAX_INTERFACE_NUM];
   GT_U32                  regMR1[MAX_INTERFACE_NUM];
   GT_U32                  regMR2[MAX_INTERFACE_NUM];
   GT_U32                  regMR3[MAX_INTERFACE_NUM];
   /*each element in array represent read_data_sample register delay for a specific interface.
   each register, 4 bits [0+CS*8 to 4+CS*8] represent Number of DDR cycles from read command 
   until data is ready to be fetched from the PHY, when accessing CS.*/
   GT_U32                  readDataSample[MAX_INTERFACE_NUM];
   /* each element in array represent read_data_sample register delay for a specific interface.
      each register, 4 bits [0+CS*8 to 4+CS*8] represent the total delay from read command until 
      opening the read mask, when accessing CS. This field defines the delay 
      in DDR cycles granularity.*/
   GT_U32                  uiReadDataReady[MAX_INTERFACE_NUM];
} Mode;


typedef struct
{
	GT_U8	            isSupported;
	GT_U8	            bwPerFreq;
    GT_U8	            ratePerFreq;
} MV_HWS_TIP_FREQ_CONFIG_INFO;

typedef struct
{
    GT_U32 csRegValue;
    GT_U32 csCbeValue;
} MV_HWS_CS_CONFIG_INFO;



typedef struct {
	GT_U8	pipe;
	GT_U8	client;
} MV_DFX_ACCESS;


typedef struct
{
	MV_DFX_ACCESS	        *dfxTable;
} MV_HWS_XSB_INFO;

/************************* Declarations ***********************************************/
/******************************************************************************
* Name:     ddr3TipRegisterDqTable
* Desc:     Register DQ table
* Args:     devNum     - device number
*           table        - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipRegisterDqTable
(
    GT_U32          devNum,
	GT_U32			*table
);

/******************************************************************************
* Name:     mvHwsDdr3TipSelectDdrController.
* Desc:     Enable/Disable access to Marvell's server.
* Args:     devNum     - device number
*           enable        - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    mvHwsDdr3TipSelectDdrController
(
    GT_U32   devNum,
    GT_BOOL  enable 
);

/******************************************************************************
* Name:     mvHwsDdr3TipInitController.
* Desc:     Initialize DDR Controller
* Args:     devNum - device number 
*           initCntrPrm - init controller parameters
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipInitController
( 
    GT_U32          devNum,
    InitCntrParam   *initCntrPrm    
);

/******************************************************************************
* Name:     mvHwsDdr3TipLoadTopologyMap.
* Desc:     Load  topology map
* Args:     devNum     - device number.
*           topology - topology map pointer
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipLoadTopologyMap
( 
    GT_U32                  devNum,
    MV_HWS_TOPOLOGY_MAP     *topology 
);

/******************************************************************************
* Name:     mvHwsDdr3TipRunAlg.
* Desc:     Execute the DDR3 algorithm.
* Args:     devNum     - device number.
*           algoType - algorithm type
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipRunAlg
(
    GT_U32              devNum,
    MV_HWS_ALGO_TYPE    algoType
);

/*****************************************************************************
Get DDR version
******************************************************************************/
/******************************************************************************
* Name:     mvHwsDdr3TipVersionGet
* Desc:     Return DDR version string
* Args:
*
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/
const GT_CHAR* mvHwsDdr3TipVersionGet(void);

/******************************************************************************
* Name:     mvHwsDdr3TipModeRead.
* Desc:     read Mode registers
* Args:     devNum - Device number
*           modeInfo - pointer to mode info struct
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    mvHwsDdr3TipModeRead
( 
    GT_U32            devNum,
    Mode              *modeInfo
);

/******************************************************************************
* Name:     mvHwsDdr3TipReadTrainingResult.
* Desc:     read training result
* Args:     devNum
*           result
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipReadTrainingResult
(
    GT_U32          devNum,
    MV_HWS_RESULT   result[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM]
);

/*****************************************************************************
Check if pup search is locked
******************************************************************************/
/******************************************************************************
* Name:     ddr3TipIsPupLock
* Desc:     Check if pup is lock
* Args:     pupBuf
*           readMode
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_BOOL   ddr3TipIsPupLock
(
    GT_U32                   *pupBuf,
    MV_HWS_TRAINING_RESULT   readMode
);

/*****************************************************************************
Get minimum buffer value
******************************************************************************/
/******************************************************************************
* Name:     ddr3TipGetBufMin
* Desc:     Return buffer's minimum value
* Args:     bufPtr
*
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_U8 ddr3TipGetBufMin
(
    GT_U8* bufPtr
);

/*****************************************************************************
Get maximum buffer value
******************************************************************************/
/******************************************************************************
* Name:     ddr3TipGetBufMax
* Desc:     Return buffer's maximum value
* Args:     bufPtr
*
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/

GT_U8 ddr3TipGetBufMax
(
    GT_U8* bufPtr
);

#ifdef __cplusplus
   }
#endif

/*****************************************************************************/

#endif /* _MV_DDR3_TRAININGIP_H_ */


