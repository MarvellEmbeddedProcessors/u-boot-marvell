/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsTrainingIpDdr3Training.c
*
* DESCRIPTION: DDR3 training IP configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 204 $
******************************************************************************/

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3TrainingIpCentralization.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingHwAlgo.h"
#include "mvDdr3TrainingLeveling.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"
#include "mvDdrTrainingIpDb.h"


#ifdef DDR_VIEWER_TOOL
GT_STATUS printAdll(GT_U32 devNum, GT_U32 adll[MAX_INTERFACE_NUM*MAX_BUS_NUM]);
GT_STATUS printPh(GT_U32 devNum, GT_U32 adll[MAX_INTERFACE_NUM*MAX_BUS_NUM]);

GT_U8 sweepPatternIndexStart = PATTERN_KILLER_DQ0, sweepPatternIndexEnd = PATTERN_LIMIT;
static char* ConvertFreq(MV_HWS_DDR_FREQ freq);
#if defined(EXCLUDE_SWITCH_DEBUG)
extern MV_HWS_PATTERN sweepPattern;
GT_U32 ctrlSweepres[ADLL_LENGTH][MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U32 ctrlADLL[MAX_CS_NUM*MAX_INTERFACE_NUM*MAX_BUS_NUM];
GT_U32 ctrlADLL1[MAX_CS_NUM*MAX_INTERFACE_NUM*MAX_BUS_NUM];
GT_U32 ctrlLevelPhase[MAX_CS_NUM*MAX_INTERFACE_NUM*MAX_BUS_NUM];
#endif
#endif

extern GT_U32 ckDelay;
extern GT_U32 dminPhyRegTable[MAX_BUS_NUM*MAX_CS_NUM][2];

extern GT_STATUS ddr3TipRestoreDunitRegs
(
    GT_U32               devNum
);
GT_U8 	isRegDump = 0;
GT_U32  debugPbs = DEBUG_LEVEL_ERROR;
/*****************************************************************************
API to change flags outside of the lib
******************************************************************************/
#ifndef SILENT_LIB
/*Debug flags for other Training modules*/
GT_U8  debugTrainingStatic = DEBUG_LEVEL_ERROR;
GT_U8  debugTraining = DEBUG_LEVEL_ERROR;
GT_U8  debugLeveling = DEBUG_LEVEL_ERROR;
GT_U8  debugCentralization = DEBUG_LEVEL_ERROR;
GT_U8  debugTrainingIp = DEBUG_LEVEL_ERROR;
GT_U8  debugTrainingBist = DEBUG_LEVEL_ERROR;
GT_U8  debugTrainingHwAlg = DEBUG_LEVEL_ERROR;
GT_U8  debugTrainingAccess = DEBUG_LEVEL_ERROR;
GT_U8  debugTrainingA38x = DEBUG_LEVEL_ERROR;
#ifdef CONFIG_DDR4
GT_U8  debugTapTuning = DEBUG_LEVEL_ERROR;
GT_U8  debugCalibration = DEBUG_LEVEL_ERROR;
GT_U8  debugDdr4Centralization = DEBUG_LEVEL_ERROR;
#endif

void printTopology(MV_HWS_TOPOLOGY_MAP *pTopologyDB);

GT_VOID ddr3HwsSetLogLevel(
	MV_DDR_LIB_DEBUG_BLOCK 	block,
	GT_U8					level
)
{
	switch(block){
	case MV_DEBUG_BLOCK_STATIC:
		debugTrainingStatic = level;
		break;
    case MV_DEBUG_BLOCK_TRAINING_MAIN:
		debugTraining = level;
		break;
    case MV_DEBUG_BLOCK_LEVELING:
		debugLeveling = level;
		break;
    case MV_DEBUG_BLOCK_CENTRALIZATION:
		debugCentralization = level;
		break;
    case MV_DEBUG_BLOCK_PBS:
		debugPbs = level;
		break;
    case MV_DEBUG_BLOCK_ALG:
		debugTrainingHwAlg = level;
		break;
    case MV_DEBUG_BLOCK_DEVICE:
		debugTrainingA38x = level;
		break;
    case MV_DEBUG_BLOCK_ACCESS:
		debugTrainingAccess = level;
		break;
    case MV_DEBUG_STAGES_REG_DUMP:
		if( level == DEBUG_LEVEL_TRACE)
			isRegDump = 1;
		else
			isRegDump = 0;
		break;
#ifdef CONFIG_DDR4
	case MV_DEBUG_TAP_TUNING:
		debugTapTuning = level;
		break;
    case MV_DEBUG_BLOCK_CALIBRATION:
        debugCalibration = level;
        break;
    case MV_DEBUG_BLOCK_DDR4_CENTRALIZATION:
        debugDdr4Centralization = level;
        break;
#endif
    case MV_DEBUG_BLOCK_ALL:
	default:
		debugTrainingStatic = level;
		debugTraining = level;
		debugLeveling = level;
		debugCentralization = level;
		debugPbs = level;
		debugTrainingHwAlg = level;
		debugTrainingAccess = level;
		debugTrainingA38x = level;
#ifdef CONFIG_DDR4
		debugTapTuning = level;
        debugCalibration = level;
        debugDdr4Centralization = level;
#endif
	}
}
#else
GT_VOID ddr3HwsSetLogLevel(
	MV_DDR_LIB_DEBUG_BLOCK 	block,
	GT_U8					level
)
{
	return;
}
#endif

extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern MV_HWS_RESULT trainingResult[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM];
extern GT_U32 firstActiveIf;
extern GT_U32 maskTuneFunc;
extern GT_U32 freqVal[];
MV_HWS_TIP_CONFIG_FUNC_DB configFuncInfo[HWS_MAX_DEVICE_NUM];
GT_U8 isDefaultCentralization = 0 , isTuneResult = 0, isValidateWindowPerIf = 0, isValidateWindowPerPup = 0, sweepCnt = 1, isBistResetBit = 1, isRunLevelingSweepTests = 0;
static MV_HWS_XSB_INFO xsbInfo[HWS_MAX_DEVICE_NUM];


/*****************************************************************************
Dump Dunit & Phy registers
******************************************************************************/
GT_STATUS ddr3TipRegDump
(
    GT_U32 devNum
)
{
    GT_U32 interfaceId, regAddr, dataValue, busId;
	GT_U32 readData[MAX_INTERFACE_NUM];
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);
    mvPrintf("-- dunit registers --\n");
    for(regAddr = 0x1400; regAddr < 0x19F0; regAddr+=4)
    {
        mvPrintf("0x%x ",regAddr);
        for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
    	    CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, regAddr,  readData,  MASK_ALL_BITS));
            mvPrintf("0x%x ",  readData[interfaceId]);
       }
       mvPrintf("\n");
    }
    mvPrintf("-- Phy registers --\n");

    for(regAddr = 0; regAddr <= 0xFF; regAddr++)
    {
        mvPrintf("0x%x ",regAddr);
        for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
            for(busId = 0; busId < octetsPerInterfaceNum; busId++)
            {
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
				CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId, ACCESS_TYPE_UNICAST, busId,  DDR_PHY_DATA,  regAddr, &dataValue));
                mvPrintf("0x%x ",  dataValue);
            }
            for(busId = 0; busId < octetsPerInterfaceNum; busId++)
            {
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
                CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId, ACCESS_TYPE_UNICAST, busId,  DDR_PHY_CONTROL,  regAddr, &dataValue));
                mvPrintf("0x%x ",  dataValue);
            }
       }
       mvPrintf("\n");
    }

	return GT_OK;
}

/*****************************************************************************
Register access func registration
******************************************************************************/
GT_STATUS mvHwsDdr3TipInitConfigFunc
(
	GT_U32						devNum, 
	MV_HWS_TIP_CONFIG_FUNC_DB	*configFunc
)
{
    if (configFunc == NULL)
    {
        return GT_BAD_PARAM;
    }
    osMemCpy(&configFuncInfo[devNum], configFunc, sizeof(MV_HWS_TIP_CONFIG_FUNC_DB));
   
    return GT_OK;
}

/*****************************************************************************
Read training result table
******************************************************************************/
GT_STATUS    mvHwsDdr3TipReadTrainingResult
(
    GT_U32          devNum,
    MV_HWS_RESULT   result[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM]
)
{
   devNum = devNum;

   if (result == NULL)
   {
       return GT_BAD_PARAM;
   }
   osMemCpy(result, trainingResult, sizeof(result));
   return GT_OK;
}


/*****************************************************************************
Get training result info pointer
******************************************************************************/
MV_HWS_RESULT* ddr3TipGetResultPtr
(
    GT_U32 stage
)
{
    return trainingResult[stage];
}

/*****************************************************************************
Device info read
******************************************************************************/
GT_STATUS    ddr3TipGetDeviceInfo
(
    GT_U32                devNum,
    MV_DDR3_DEVICE_INFO * infoPtr
)
{
    if (configFuncInfo[devNum].tipGetDeviceInfoFunc != NULL)
    {
        return configFuncInfo[devNum].tipGetDeviceInfoFunc((GT_U8)devNum, infoPtr);
    }
    return GT_FAIL;
}


#ifdef DDR_VIEWER_TOOL
/*****************************************************************************
Convert freq to character string
******************************************************************************/
static char* ConvertFreq(MV_HWS_DDR_FREQ freq)
{
  switch (freq)
    {
        case DDR_FREQ_LOW_FREQ:
            return "DDR_FREQ_LOW_FREQ";
#ifdef CONFIG_DDR3
        case DDR_FREQ_400:
            return "400";

        case DDR_FREQ_533:
            return "533";
#endif
        case DDR_FREQ_667:
            return "667";

        case DDR_FREQ_800:
            return "800";

        case DDR_FREQ_933:
            return "933";

        case DDR_FREQ_1066:
            return "1066";
#ifdef CONFIG_DDR3
        case DDR_FREQ_311:
            return "311";

        case DDR_FREQ_333:
            return "333";

        case DDR_FREQ_467:
            return "467";

        case DDR_FREQ_850:
            return "850";

        case DDR_FREQ_900:
            return "900";

        case DDR_FREQ_360:
            return "DDR_FREQ_360";

        case DDR_FREQ_1000:
            return "DDR_FREQ_1000";
#endif
        default:
            return "Unknown Frequency";

  }
}
/*****************************************************************************
Convert device ID to character string
******************************************************************************/
static char* ConvertDevId(GT_U32 devId)
{
  switch (devId)
    {
        case 0x6800:
            return "A38xx";
        case 0x6900:
            return "A39XX";
		case 0xF400:
            return "AC3";
	   case 0xFC00:
            return "BC2";

        default:
            return "Unknown Device";

  }
}
/*****************************************************************************
Convert device ID to character string
******************************************************************************/
static char* ConvertMemSize (GT_U32 devId)
{
  switch (devId)
    {
        case 0:
            return "512 MB";
        case 1:
            return "1 GB";
		case 2:
            return "2 GB";
	   case  3:
            return "4 GB";
	   case  4:
            return "8 GB";

        default:
            return "wrong mem size";

  }
}

GT_STATUS printDeviceInfo( GT_U8     devNum)
{
	MV_DDR3_DEVICE_INFO infoPtr;
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	CHECK_STATUS(ddr3TipGetDeviceInfo (devNum,&infoPtr));
	mvPrintf("=== DDR setup START===\n");
	mvPrintf("\tDevice ID: %s\n" ,ConvertDevId(infoPtr.deviceId));
	mvPrintf("\tDDR3  CK delay: %d\n",infoPtr.ckDelay);
	printTopology(topologyMap);
	mvPrintf("=== DDR setup END===\n");
	return GT_OK;
}
void mvHwsDdr3TipSweepTest(GT_BOOL enable)
{
	if (enable){
		isValidateWindowPerIf = 1;
		isValidateWindowPerPup = 1;
		debugTraining = DEBUG_LEVEL_TRACE;
	}
	else{
		isValidateWindowPerIf = 0;
		isValidateWindowPerPup = 0;
	}
}
#endif

char* ddr3TipConvertTuneResult(MV_HWS_RESULT TuneResult)
{
	switch(TuneResult)
	{
	case TEST_FAILED:
		return "FAILED";
	case TEST_SUCCESS:
		return "PASS";
	case NO_TEST_DONE:
		return "NOT COMPLETED";
	default:
		return "Un-KNOWN";
	}
}

/*****************************************************************************
Print log info
******************************************************************************/
GT_STATUS ddr3TipPrintLog(GT_U32 devNum, GT_U32 memAddr)
{
    GT_U32 interfaceId = 0;

    memAddr = memAddr; /* avoid warnings */

#ifndef DDR_VIEWER_TOOL
    devNum = devNum; /* avoid warnings */
#else
    if (( isValidateWindowPerIf != 0) || ( isValidateWindowPerPup != 0))

    {
    	GT_U32 isPupLog = 0;
		MV_HWS_DDR_FREQ freq;

		freq = topologyMap->interfaceParams[firstActiveIf].memoryFreq;

        isPupLog = (isValidateWindowPerPup != 0) ? 1:0;
		mvPrintf("===VALIDATE WINDOW LOG START===\n");
		mvPrintf("DDR Frequency: %s   ======\n",ConvertFreq(freq));
        /* print sweep windows */
        ddr3TipRunSweepTest(devNum, sweepCnt, 1 , isPupLog);
        ddr3TipRunSweepTest(devNum, sweepCnt, 0 , isPupLog);

#if defined(EXCLUDE_SWITCH_DEBUG)
		if( isRunLevelingSweepTests == 1 ){
			ddr3TipRunLevelingSweepTest(devNum, sweepCnt, 0 , isPupLog);
			ddr3TipRunLevelingSweepTest(devNum, sweepCnt, 1 , isPupLog);
		}
#endif
		ddr3TipPrintAllPbsResult(devNum);
		ddr3TipPrintWLSuppResult(devNum);
		mvPrintf("===VALIDATE WINDOW LOG END ===\n");
		CHECK_STATUS(ddr3TipRestoreDunitRegs(devNum));
		ddr3TipRegDump(devNum);
    }
#endif

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

		DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("IF %d Status:\n", interfaceId));

        if  (maskTuneFunc & INIT_CONTROLLER_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tInit Controller: %s\n", ddr3TipConvertTuneResult(trainingResult[INIT_CONTROLLER][interfaceId])));
        }
        if  (maskTuneFunc & SET_LOW_FREQ_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tLow freq Config: %s\n", ddr3TipConvertTuneResult(trainingResult[SET_LOW_FREQ][interfaceId])));
        }
        if  (maskTuneFunc & LOAD_PATTERN_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tLoad Pattern: %s\n", ddr3TipConvertTuneResult(trainingResult[LOAD_PATTERN][interfaceId])));
        }
        if  (maskTuneFunc & SET_MEDIUM_FREQ_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tMedium freq Config: %s\n", ddr3TipConvertTuneResult(trainingResult[SET_MEDIUM_FREQ][interfaceId])));
        }
        if  (maskTuneFunc & WRITE_LEVELING_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tWL: %s\n", ddr3TipConvertTuneResult(trainingResult[WRITE_LEVELING][interfaceId])));
        }
        if  (maskTuneFunc & LOAD_PATTERN_2_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tLoad Pattern: %s\n", ddr3TipConvertTuneResult(trainingResult[LOAD_PATTERN_2][interfaceId])));
        }
        if  (maskTuneFunc & READ_LEVELING_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tRL: %s\n", ddr3TipConvertTuneResult(trainingResult[READ_LEVELING][interfaceId])));
        }
        if  (maskTuneFunc & WRITE_LEVELING_SUPP_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tWL Supp: %s\n", ddr3TipConvertTuneResult(trainingResult[WRITE_LEVELING_SUPP][interfaceId])));
        }
        if  (maskTuneFunc & PBS_RX_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tPBS RX: %s\n", ddr3TipConvertTuneResult(trainingResult[PBS_RX][interfaceId])));
        }
        if  (maskTuneFunc & PBS_TX_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tPBS TX: %s\n", ddr3TipConvertTuneResult(trainingResult[PBS_TX][interfaceId])));
        }
		if  (maskTuneFunc & SET_TARGET_FREQ_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tTarget freq Config: %s\n", ddr3TipConvertTuneResult(trainingResult[SET_TARGET_FREQ][interfaceId])));
        }
        if  (maskTuneFunc & WRITE_LEVELING_TF_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tWL TF: %s\n", ddr3TipConvertTuneResult(trainingResult[WRITE_LEVELING_TF][interfaceId])));
        }
#ifdef CONFIG_DDR3
        if  (maskTuneFunc & READ_LEVELING_TF_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tRL TF: %s\n", ddr3TipConvertTuneResult(trainingResult[READ_LEVELING_TF][interfaceId])));
        }
#endif
        if  (maskTuneFunc & WRITE_LEVELING_SUPP_TF_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tWL TF Supp: %s\n", ddr3TipConvertTuneResult(trainingResult[WRITE_LEVELING_SUPP_TF][interfaceId])));
        }
        if  (maskTuneFunc & CENTRALIZATION_RX_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tCentr RX: %s\n", ddr3TipConvertTuneResult(trainingResult[CENTRALIZATION_RX][interfaceId])));
        }
        if  (maskTuneFunc & VREF_CALIBRATION_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tVREF_CALIBRATION: %s\n", ddr3TipConvertTuneResult(trainingResult[VREF_CALIBRATION][interfaceId])));
        }
        if  (maskTuneFunc & CENTRALIZATION_TX_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tCentr TX: %s\n", ddr3TipConvertTuneResult(trainingResult[CENTRALIZATION_TX][interfaceId])));
        }
#ifdef CONFIG_DDR4
        if  (maskTuneFunc & SW_READ_LEVELING_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tSW RL TF: %s\n", ddr3TipConvertTuneResult(trainingResult[SW_READ_LEVELING][interfaceId])));
        }
        if  (maskTuneFunc & RECEIVER_CALIBRATION_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tRX CAL: %s\n", ddr3TipConvertTuneResult(trainingResult[RECEIVER_CALIBRATION][interfaceId])));
        }
        if  (maskTuneFunc & WL_PHASE_CORRECTION_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tWL PHASE CORRECT: %s\n", ddr3TipConvertTuneResult(trainingResult[WL_PHASE_CORRECTION][interfaceId])));
        }
        if  (maskTuneFunc & DQ_VREF_CALIBRATION_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tDQ VREF CAL: %s\n", ddr3TipConvertTuneResult(trainingResult[DQ_VREF_CALIBRATION][interfaceId])));
        }
        if  (maskTuneFunc & DQ_MAPPING_MASK_BIT)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("\tDQ MAP: %s\n", ddr3TipConvertTuneResult(trainingResult[DQ_MAPPING][interfaceId])));
        }
#endif
   }
    return GT_OK;
}
#ifndef MV_HWS_EXCLUDE_DEBUG_PRINTS
/*****************************************************************************
Print stability log info
******************************************************************************/
GT_STATUS	ddr3TipPrintStabilityLog(GT_U32 devNum)
{
	GT_U8 interfaceId=0,csindex = 0,busId=0,padIndex=0;
	/*GT_U8 frequency = topologyMap->interfaceParams[firstActiveIf].memoryFreq;
	GT_U8 adllTap =  MEGA/(freqVal[frequency]*64);
	GT_U32 Ph = adllTap*32;
	GT_U32 CC = Ph*2;*/
	GT_U32 regData;
#ifdef CONFIG_DDR4
	GT_U32 regData1;
#endif
	GT_U32 readData[MAX_INTERFACE_NUM];
	GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);

	/*Title print*/
	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		mvPrintf("Title: I/F# , Tj,CalibrationN0,CalibrationP0,CalibrationN1,CalibrationP1,CalibrationN2,CalibrationP2,");
		for(csindex = 0; csindex < max_cs; csindex++)
		{
			mvPrintf("CS%d , ",csindex);
				mvPrintf("\n");
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
				#ifdef CONFIG_DDR3/*DDR3*/
					mvPrintf("VWTx,VWRx,WL_tot,WL_ADLL,WL_PH,RL_Tot,RL_ADLL,RL_PH,RL_Smp,CenTx,CenRx,Vref,DQVref,");
				#else/*DDR4*/
					mvPrintf("DminTx,AreaTx,DminRx,AreaRx,WL_tot,WL_ADLL,WL_PH,RL_Tot,RL_ADLL,RL_PH,RL_Smp,CenTx,CenRx,Vref,DQVref,");
				#endif
				#ifdef CONFIG_DDR3
				#else
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvPrintf("DC-Pad%d,",padIndex);
				}
				#endif
				mvPrintf("\t\t");
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvPrintf("PBSTx-Pad%d,",padIndex);
				}
				mvPrintf("\t\t");
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvPrintf("PBSRx-Pad%d,",padIndex);
				}
		}
	}
	mvPrintf("\n");

	/*Data print*/
	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

		mvPrintf("Data: %d,%d,",interfaceId, (configFuncInfo[devNum].tipGetTemperature != NULL)?(configFuncInfo[devNum].tipGetTemperature((GT_U8)devNum)):(0));

		CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x14C8, readData, MASK_ALL_BITS));
		mvPrintf("%d,%d,",((readData[interfaceId]&0x3F0)>>4),((readData[interfaceId]&0xFC00)>>10));
		CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x17C8, readData, MASK_ALL_BITS));
		mvPrintf("%d,%d,",((readData[interfaceId]&0x3F0)>>4),((readData[interfaceId]&0xFC00)>>10));
		CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1DC8, readData, MASK_ALL_BITS));
		mvPrintf("%d,%d,",((readData[interfaceId]&0x3F0000)>>16),((readData[interfaceId]&0xFC00000)>>22));

		for(csindex = 0; csindex < max_cs; csindex++)
		{
			mvPrintf("CS%d , ",csindex);
			for(busId = 0; busId < MAX_BUS_NUM; busId++)
			{
				mvPrintf("\n");
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
				#ifdef CONFIG_DDR3/*DDR3*/
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR+csindex, &regData);
				mvPrintf("%d,%d,",(regData&0x1F),((regData&0x3E0)>>5));
				#else/*DDR4*/
				/*DminTx, areaTX*/
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR+csindex, &regData);
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, dminPhyRegTable[csindex*5+busId][0], DDR_PHY_CONTROL, dminPhyRegTable[csindex*5+busId][1], &regData1);
				mvPrintf("%d,%d,",2*(regData1&0xFF),regData);
				/*DminRx, areaRX*/
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR+csindex + 4, &regData);
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, dminPhyRegTable[csindex*5+busId][0], DDR_PHY_CONTROL, dminPhyRegTable[csindex*5+busId][1], &regData1);
				mvPrintf("%d,%d,",2*(regData1>>8),regData);
				#endif
				/*WL*/
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG+csindex*4, &regData);
				mvPrintf("%d,%d,%d,",(regData&0x1F)+((regData&0x1C0)>>6)*32,(regData&0x1F),(regData&0x1C0)>>6);
				/*RL*/
				CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_SAMPLE_DELAY, readData, MASK_ALL_BITS));
				readData[interfaceId] = (readData[interfaceId]&(0x1F<<(8*csindex))) >> (8*csindex);
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, RL_PHY_REG+csindex*4, &regData);
				mvPrintf("%d,%d,%d,%d,",(regData&0x1F)+((regData&0x1C0)>>6)*32 + readData[interfaceId]*64,(regData&0x1F),((regData&0x1C0)>>6),readData[interfaceId]);
				/*Centralization*/
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG+csindex*4, &regData);
				mvPrintf("%d,",(regData&0x3F));
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, READ_CENTRALIZATION_PHY_REG+csindex*4, &regData);
				mvPrintf("%d,",(regData&0x1F));
				/*Vref */
				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, PAD_CONFIG_PHY_REG, &regData);
				mvPrintf("%d,",(regData&0x7));
				/*DQVref*/
				/* Need to add the Read Function from device*/
				mvPrintf("%d,",0);
				#ifndef CONFIG_DDR3
				mvPrintf("\t\t");
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, 0xD0+12*csindex+padIndex, &regData);
					mvPrintf("%d,",(regData&0x3F));
				}
				#endif
				mvPrintf("\t\t");
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, 0x10+16*csindex+padIndex, &regData);
					mvPrintf("%d,",(regData&0x3F));
				}
				mvPrintf("\t\t");
				for(padIndex = 0; padIndex < 11; padIndex++)
				{
					mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, 0x50+16*csindex+padIndex, &regData);
					mvPrintf("%d,",(regData&0x3F));
				}
			}
		}
	}
	mvPrintf("\n");

	return GT_OK;
}

#endif
/*****************************************************************************
Register XSB information
******************************************************************************/
GT_STATUS ddr3TipRegisterXsbInfo
(
    GT_U32					devNum,
    MV_HWS_XSB_INFO         *xsbInfoTable
)
{
    osMemCpy(&xsbInfo[devNum], xsbInfoTable, sizeof(MV_HWS_XSB_INFO));
    return GT_OK;
}

/*****************************************************************************
Read ADLL Value
******************************************************************************/
GT_BOOL mvHwsDdr3TipReadAdllValue(GT_U32 devNum, GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr, GT_U32 mask)
{
    GT_U32  dataValue;
    GT_U32 interfaceId = 0, busId = 0;
    GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* multi CS support - regAddr is calucalated in calling function with CS offset */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	    for(busId = 0 ; busId < octetsPerInterfaceNum ; busId++)
	    {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
                CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId, ACCESS_TYPE_UNICAST, busId,  DDR_PHY_DATA,  regAddr, &dataValue));
                PupValues[interfaceId*octetsPerInterfaceNum + busId] = dataValue & mask;
        }
	}

	return 0;
}

/*****************************************************************************
Write ADLL Value
******************************************************************************/
GT_BOOL mvHwsDdr3TipWriteAdllValue(GT_U32 devNum, GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr)
{
    GT_U32 interfaceId = 0, busId = 0;
    GT_U32 data;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* multi CS support - regAddr is calucalated in calling function with CS offset */

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	    for(busId = 0 ; busId < octetsPerInterfaceNum ; busId++)
	    {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
		data = PupValues[interfaceId*octetsPerInterfaceNum+busId];
		CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, regAddr, data));
	    }

    }
    return 0;
}


/*****************************************************************************
Read Phase Value
******************************************************************************/
GT_BOOL readPhaseValue(GT_U32 devNum,GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr, GT_U32 mask)
{
    GT_U32  dataValue;
    GT_U32 interfaceId = 0, busId = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* multi CS support - regAddr is calucalated in calling function with CS offset */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	    for(busId = 0 ; busId < octetsPerInterfaceNum ; busId++)
	    {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
                CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId, ACCESS_TYPE_UNICAST, busId,  DDR_PHY_DATA,  regAddr, &dataValue));
                PupValues[interfaceId*octetsPerInterfaceNum + busId] = dataValue & mask;
        }
	}

	return 0;
}

/*****************************************************************************
Write Leveling Value
******************************************************************************/
GT_BOOL writeLevelingValue(GT_U32 devNum,GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], GT_U32 PupPhValues[MAX_INTERFACE_NUM*MAX_BUS_NUM],int regAddr)
{
    GT_U32 interfaceId = 0, busId = 0;
    GT_U32 data;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* multi CS support - regAddr is calucalated in calling function with CS offset */

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	    for(busId = 0 ; busId < octetsPerInterfaceNum ; busId++)
	    {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
		data = PupValues[interfaceId*octetsPerInterfaceNum+busId] + (PupPhValues[interfaceId*octetsPerInterfaceNum+busId]);
		CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, regAddr, data));
	    }

    }
    return 0;
}

#ifndef EXCLUDE_SWITCH_DEBUG
extern MV_HWS_DDR_FREQ initFreq;
extern MV_HWS_DDR_FREQ lowFreq;
extern MV_HWS_DDR_FREQ mediumFreq;
extern GT_U8 genericInitController;
extern AUTO_TUNE_STAGE trainingStage;
extern GT_U8  isPllBeforeInit;
extern GT_U8  isAdllCalibBeforeInit;
extern GT_U8  isDfsInInit;
extern GT_32 wlDebugDelay;
extern GT_U32 siliconDelay[HWS_MAX_DEVICE_NUM];
extern 	GT_U32 vrefInitialValue;
extern 	GT_U32 gZpriData;
extern 	GT_U32 gZnriData;
extern 	GT_U32 gZpriCtrl;
extern 	GT_U32 gZnriCtrl;
extern 	GT_U32 gZpodtData;
extern 	GT_U32 gZnodtData;
extern GT_U32 freqVal[DDR_FREQ_LIMIT];
extern GT_U32 startPattern, endPattern;
extern GT_U32  PhyReg0Val;
extern GT_U32  PhyReg1Val;
extern GT_U32  PhyReg2Val;
extern GT_U32  PhyReg3Val;
extern MV_HWS_PATTERN sweepPattern;
extern MV_HWS_PATTERN pbsPattern;
extern GT_U8  debugTrainingAccess;
extern GT_U32 fingerTest, pFingerStart,  pFingerEnd,  nFingerStart, nFingerEnd, pFingerStep, nFingerStep;
extern GT_U32 mode2T;
extern GT_U32 xsbValidateType;
extern GT_U32 xsbValidationBaseAddress;
extern GT_U32 odtAdditional;
extern GT_U32 debugMode;
extern GT_U32 delayEnable;
extern GT_U32 caDelay;
extern GT_U32 debugDunit;
extern GT_U32 clampTbl[MAX_INTERFACE_NUM];
extern GT_U32 freqMask[HWS_MAX_DEVICE_NUM][DDR_FREQ_LIMIT];
extern GT_U32 startPattern, endPattern;

/*Declarations*/
MV_HWS_TIP_CONFIG_FUNC_DB configFuncInfo[HWS_MAX_DEVICE_NUM];
GT_U32  startXsbOffset = 0;
GT_U8 isRlOld = 0;
GT_U8 isFreqOld = 0;
GT_U8 isDfsDisabled = 0;
GT_U32 defaultCentrlizationValue = 0x12;
GT_U32 activateSelectBeforeRunAlg= 1, activateDeselectAfterRunAlg = 1, rlTest = 0, resetReadFifo = 0;
GT_BOOL debugAcc = GT_FALSE;
GT_U32 ctrlSweepres[ADLL_LENGTH][MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U32 ctrlADLL[MAX_CS_NUM*MAX_INTERFACE_NUM*MAX_BUS_NUM];
GT_U32 xsbTestTable[][8] =
{
    {0x00000000,         0x11111111,         0x22222222,         0x33333333,         0x44444444,         0x55555555,         0x66666666,         0x77777777},
    {0x88888888,         0x99999999,         0xAAAAAAAA,         0xBBBBBBBB,         0xCCCCCCCC,         0xDDDDDDDD,         0xEEEEEEEE,         0xFFFFFFFF},
    {0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF},
    {0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF},
    {0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF},
    {0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF},
    {0x00000000,         0x00000000,         0xFFFFFFFF,         0xFFFFFFFF,         0x00000000,         0x00000000,         0xFFFFFFFF,         0xFFFFFFFF},
    {0x00000000,         0x00000000,         0x00000000,         0xFFFFFFFF,         0x00000000,         0x00000000,         0x00000000,         0x00000000},
    {0xFFFFFFFF,         0xFFFFFFFF,         0xFFFFFFFF,         0xFFFFFFFF,         0x00000000,         0xFFFFFFFF,         0xFFFFFFFF,         0xFFFFFFFF}
};
GT_U32 debugSweepVal = 100;

static GT_STATUS  ddr3TipAccessAtr
(
    GT_U32 devNum,
    GT_U32 flagId, 
	GT_U32 value,
    GT_U32** ptr
);

GT_STATUS    ddr3TipPrintAdll()
{
    GT_U32  busCnt = 0,  interfaceId,  dataP1, dataP2, uiData3, devNum = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busCnt = 0; busCnt < octetsPerInterfaceNum; busCnt++)
        {
        	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)
            CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, 0x1, &dataP1));
            CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, 0x2, &dataP2));
            CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, 0x3, &uiData3));
            DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, (" IF %d busCnt %d  phy_reg_1_data 0x%x phy_reg_2_data 0x%x phy_reg_3_data 0x%x\n",interfaceId, busCnt, dataP1, dataP2, uiData3));
        }
    }
    return GT_OK; 
}

#if 0
/*****************************************************************************
Get attribute value
******************************************************************************/
GT_STATUS  ddr3TipGetAtr
(
    GT_U32 devNum,
    GT_U32 flagId, 
    GT_U32* value
)
{
    GT_STATUS retVal; 
    GT_U32 *ptrFlag = NULL;
    retVal = ddr3TipAccessAtr(devNum, flagId, &ptrFlag);
	*value = *ptrFlag;

	if(ptrFlag == NULL)
	{
		mvPrintf("flag 0x%x address cannot be retrieved. use ddr3TipSetAtr to set the value\n",flagId);
		*value = *ptrFlag;

		return GT_BAD_PARAM;
	}

    return retVal; 
}
#endif

/*****************************************************************************
Set attribute value
******************************************************************************/
GT_STATUS  ddr3TipSetAtr
(
    GT_U32 devNum,
    GT_U32 flagId,
    GT_U32 value
)
{
    GT_STATUS retVal;
    GT_U32 *ptrFlag = NULL;
    retVal = ddr3TipAccessAtr(devNum, flagId, value, &ptrFlag);
	if(ptrFlag != NULL)
	{
		mvPrintf("ddr3TipSetAtr Flag ID 0x%x value is set to 0x%x (was 0x%x)\n", flagId, value, *ptrFlag);
		*ptrFlag = value;
	}
	else
	{
		mvPrintf("ddr3TipSetAtr Flag ID 0x%x value is set to 0x%x\n", flagId, value);
	}

    return retVal;
}


/*****************************************************************************
Access attribute
******************************************************************************/
static GT_STATUS  ddr3TipAccessAtr
(
    GT_U32   devNum,
    GT_U32   flagId,
	GT_U32   value,
    GT_U32** ptr
)
 {
    GT_U32 tmpVal = 0, interfaceId = 0, pupId = 0;

	devNum = devNum;
	*ptr = NULL;

    switch(flagId)
    {

    case 0:
        *ptr = (GT_U32*) &(topologyMap->interfaceActiveMask);
        break;

    case 0x1:
        *ptr = (GT_U32*) &maskTuneFunc;
        break;

    case 0x2:
        lowFreq = (MV_HWS_DDR_FREQ)value;
	    break;

    case 0x3:
        mediumFreq = (MV_HWS_DDR_FREQ)value;
	    break;

    case 0x4:
        *ptr = (GT_U32*) &genericInitController;
        break;

    case 0x8:
        *ptr = (GT_U32*) &startXsbOffset;
        break;

   case 0x20:
        *ptr = (GT_U32*) &isRlOld;
        break;

   case 0x21:
       *ptr = (GT_U32*) &isFreqOld;
       break;

   case 0x23:
       *ptr = (GT_U32*) &isDfsDisabled;
       break;

   case 0x24:
       *ptr = (GT_U32*) &isPllBeforeInit;
       break;

   case 0x25:
       *ptr = (GT_U32*) &isAdllCalibBeforeInit;
       break;
#ifdef STATIC_ALGO_SUPPORT
   case 0x26:
       *ptr = (GT_U32*) &(siliconDelay[0]);
       break;

   case 0x27:
       *ptr = (GT_U32*) &wlDebugDelay;
       break;
#endif
   case 0x28:
       *ptr = (GT_U32*) &isTuneResult;
       break;

   case 0x29:
       *ptr = (GT_U32*) &isValidateWindowPerIf;
       break;

   case 0x2a:
       *ptr = (GT_U32*) &isValidateWindowPerPup;
       break;

   case 0x30:
       *ptr = (GT_U32*) &sweepCnt;
       break;

   case 0x31:
       *ptr = (GT_U32*) &isBistResetBit;
       break;

   case 0x32:
       *ptr = (GT_U32*) &isDfsInInit;
       break;
   case 0x33:
       *ptr = (GT_U32*) &gZnodtData;
       break;

   case 0x34:
       *ptr = (GT_U32*) &gZpodtData;
       break;

   case 0x35:
       initFreq = (MV_HWS_DDR_FREQ)value;
       break;

   case 0x36:
       *ptr = (GT_U32*) &(freqVal[DDR_FREQ_LOW_FREQ]);
       break;

   case 0x37:
       *ptr = (GT_U32*) &startPattern;
       break;

   case 0x38:
       *ptr = (GT_U32*) &endPattern;
       break;

   case 0x39:
       *ptr = (GT_U32*) &PhyReg0Val;
       break;

   case 0x4a:
       *ptr = (GT_U32*) &PhyReg1Val;
       break;

   case 0x4b:
       *ptr = (GT_U32*) &PhyReg2Val;
       break;

   case 0x4c:
       *ptr = (GT_U32*) &PhyReg3Val;
       break;

   case 0x4e:
       sweepPattern = (MV_HWS_PATTERN)value;
       break;
/*
   case 0x4f:
       *ptr = (GT_U32*) &(phy1ValTable[DDR_FREQ_LOW_FREQ]);
       break;
*/
   case 0x51:
       *ptr = (GT_U32*) &gZnriData;
       break;

   case 0x52:
       *ptr = (GT_U32*) &gZpriData;
       break;

   case 0x53:
       *ptr = (GT_U32*) &fingerTest;
       break;

   case 0x54:
       *ptr = (GT_U32*) &nFingerStart;
       break;

   case 0x55:
       *ptr = (GT_U32*) &nFingerEnd;
       break;

   case 0x56:
       *ptr = (GT_U32*) &pFingerStart;
       break;

   case 0x57:
       *ptr = (GT_U32*) &pFingerEnd;
       break;

   case 0x58:
       *ptr = (GT_U32*) &pFingerStep;
       break;

   case 0x59:
       *ptr = (GT_U32*) &nFingerStep;
       break;

   case 0x5A:
       *ptr = (GT_U32*) &gZnriCtrl;
       break;

   case 0x5B:
       *ptr = (GT_U32*) &gZpriCtrl;
       break;
   case 0x5C:
       *ptr = (GT_U32*) &isRegDump;
       break;

   case 0x5D:
       *ptr = (GT_U32*) &vrefInitialValue;
       break;

   case 0x5E:
       *ptr = (GT_U32*) &mode2T;
       break;

   case 0x5F:
       *ptr = (GT_U32*) &xsbValidateType;
       break;

   case 0x60:
       *ptr = (GT_U32*) &xsbValidationBaseAddress;
       break;

   case 0x67:
       *ptr = (GT_U32*) &activateSelectBeforeRunAlg;
       break;

   case 0x68:
       *ptr = (GT_U32*) &activateDeselectAfterRunAlg;
       break;

   case 0x69:
       *ptr = (GT_U32*) &odtAdditional;
       break;

   case 0x70:
       *ptr = (GT_U32*) &debugMode;
       break;

   case 0x71:
       pbsPattern = (MV_HWS_PATTERN)value;
       break;

   case 0x72:
       *ptr = (GT_U32*) &delayEnable;
       break;

   case 0x73:
       *ptr = (GT_U32*) &ckDelay;
       break;

   case 0x75:
       *ptr = (GT_U32*) &caDelay;
       break;

   case 0x100:
        *ptr = (GT_U32*) &debugDunit;
        break;

    case 0x101:
		debugAcc = (GT_BOOL)value;
        break;

    case 0x102:
		debugTraining = (GT_U8)value;
        break;
        
    case 0x103:
		debugTrainingBist = (GT_U8)value;
        break;
       
    case 0x104:
		debugCentralization = (GT_U8)value;
   	    break;

    case 0x105:
		debugTrainingIp = (GT_U8)value;
        break;

    case 0x106:
		debugLeveling = (GT_U8)value;
        break;
		
    case 0x107:
		debugPbs = (GT_U8)value;
        break;
    
    case 0x108:
		debugTrainingStatic = (GT_U8)value;
        break;

    case 0x109:
		debugTrainingAccess = (GT_U8)value;
        break; 

#ifdef CONFIG_DDR4
    case 0x110:
		debugTapTuning = (GT_U8)value;
        break;

    case 0x111:
		debugCalibration = (GT_U8)value;
        break;
#endif
   case 0x112:
	   *ptr = &startPattern;
        break;

    case 0x113:
		*ptr = &endPattern;
        break;

    default:
        if ((flagId >=0x200) && (flagId < 0x210))
        {
            interfaceId = flagId - 0x200;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].memoryFreq);
        }
        else if ((flagId >=0x210) && (flagId < 0x220))
        {
            interfaceId = flagId - 0x210;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].speedBinIndex);
        }
        else if ((flagId >=0x220) && (flagId < 0x230))
        {
            interfaceId = flagId - 0x220;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].busWidth);
        }
        else if ((flagId >=0x230) && (flagId < 0x240))
        {
            interfaceId = flagId - 0x230;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].memorySize);
        }
        else if ((flagId >=0x240) && (flagId < 0x250))
        {
            interfaceId = flagId - 0x240;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].casL);
        }
        else if ((flagId >=0x250) && (flagId < 0x260))
        {
            interfaceId = flagId - 0x250;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].casWL);
        }
        else if ((flagId >=0x270) && (flagId < 0x2cf))
        {
            interfaceId = (flagId - 0x270)/MAX_BUS_NUM;
			pupId = (flagId - 0x270)%MAX_BUS_NUM;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].asBusParams[pupId].isCkSwap);
        }
        else if ((flagId >=0x2d0) && (flagId < 0x32f))
        {
            interfaceId = (flagId - 0x2d0)/MAX_BUS_NUM;
			pupId = (flagId - 0x2d0)%MAX_BUS_NUM;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].asBusParams[pupId].isDqsSwap);
        }
        else if ((flagId >=0x330) && (flagId < 0x38f))
        {
            interfaceId = (flagId - 0x330)/MAX_BUS_NUM;
			pupId = (flagId - 0x330)%MAX_BUS_NUM;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].asBusParams[pupId].csBitmask);
        }
        else if ((flagId >=0x390) && (flagId < 0x3ef))
        {
            interfaceId = (flagId - 0x390)/MAX_BUS_NUM;
			pupId = (flagId - 0x390)%MAX_BUS_NUM;
            *ptr = (GT_U32*) &(topologyMap->interfaceParams[interfaceId].asBusParams[pupId].mirrorEnableBitmask);
        }
       /* else if ((flagId >=0x3f0) && (flagId < 0x3ff))
        {
            interfaceId = flagId - 0x3f0;
            *ptr = (GT_U32*) &ckDelay;
        }
        else if ((flagId >=0x400) && (flagId < 0x40f))
        {
            interfaceId = flagId - 0x400;
            *ptr = (GT_U32*) &caDelay;
        }*/
       /* else if ((flagId >=0x400) && (flagId<=0x4ff))
        {
            freq = (flagId-0x400)/16;
            field = (flagId-0x400)%16;
            *ptr = (GT_U32*) &(freqParams[freq][field]);
        }*/
        else if ((flagId >=0x500) && (flagId<=0x50f))
        {
            tmpVal = flagId-0x320;
            *ptr = (GT_U32*) &(clampTbl[tmpVal]);
        }
        else
        {
			DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("flagId out of boundary %d \n",flagId));
            return GT_BAD_PARAM;
        }
    }
    return GT_OK;
 }
#endif /* #ifndef EXCLUDE_SWITCH_DEBUG */
#ifdef DDR_VIEWER_TOOL
/*****************************************************************************
Print ADLL
******************************************************************************/
GT_STATUS printAdll(GT_U32 devNum, GT_U32 adll[MAX_INTERFACE_NUM*MAX_BUS_NUM])
{
    GT_U32 i,j;
    GT_U32 octetsPerInterfaceNum;

    devNum = devNum;
    octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(j=0; j< octetsPerInterfaceNum; j++)
    {
        VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, j)
        for(i=0;i<MAX_INTERFACE_NUM; i++)
        {
            mvPrintf("%d ,",adll[i*octetsPerInterfaceNum+j]);
        }
    }
    mvPrintf("\n");
    return GT_OK;
}
GT_STATUS printPh(GT_U32 devNum, GT_U32 adll[MAX_INTERFACE_NUM*MAX_BUS_NUM])
{
    GT_U32 i,j;
    GT_U32 octetsPerInterfaceNum;

    devNum = devNum;
    octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(j=0; j< octetsPerInterfaceNum; j++)
    {
        VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, j)
        for(i=0;i<MAX_INTERFACE_NUM; i++)
        {
            mvPrintf("%d ,",adll[i*octetsPerInterfaceNum+j]>>6);
        }
    }
    mvPrintf("\n");
    return GT_OK;
}
#endif

#ifndef EXCLUDE_SWITCH_DEBUG
/* uiByteIndex - only byte 0,1,2, or 3, oxff - test all bytes */
static
GT_U32 ddr3TipCompare(GT_U32 interfaceId, GT_U32 *pSrc, GT_U32 *pDst, GT_U32 uiByteIndex)
{
   GT_U32 burstCnt = 0, addrOffset,iId;
   GT_BOOL bIsFail = GT_FALSE;

   addrOffset = (uiByteIndex == 0xff) ? (GT_U32) 0xffffffff: (GT_U32) (0xff << (uiByteIndex*8));
   for(burstCnt = 0; burstCnt < EXT_ACCESS_BURST_LENGTH; burstCnt++)
   {
      if ((pSrc[burstCnt] & addrOffset) != (pDst[interfaceId] & addrOffset))
      {
          bIsFail = GT_TRUE;
      }
   }
   if (bIsFail == GT_TRUE)
   {
        DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("IF %d exp: ",interfaceId));
	for(iId = 0; iId <= MAX_INTERFACE_NUM-1; iId++)
	{
		 DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("0x%8x ",pSrc[iId]));
        }
        DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("\nIF %d rcv: ",interfaceId));
	for(iId = 0; iId <= MAX_INTERFACE_NUM-1; iId++)
	{
		 DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("(0x%8x ",pDst[iId]));
        }
	DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("\n "));
   }
   return bIsFail;

}

#endif /* EXCLUDE_SWITCH_DEBUG*/
#if defined(DDR_VIEWER_TOOL)
/*****************************************************************************
Sweep validation
******************************************************************************/
GT_BOOL ddr3TipRunSweepTest(GT_32 devNum, GT_U32 RepeatNum, GT_U32 direction, GT_U32 mode)
{
    GT_U32  pup = 0, startPup = 0, endPup = 0;
    GT_U32 adll = 0,rep = 0,sweepPatternIndex = 0;
    GT_U32 res[MAX_INTERFACE_NUM] = {0};
    GT_32  interfaceId = 0;
    GT_U32 adllValue = 0;
    GT_32 reg = (direction == 0) ? WRITE_CENTRALIZATION_PHY_REG : READ_CENTRALIZATION_PHY_REG;
    MV_HWS_ACCESS_TYPE  pupAccess;
    GT_U32 uiCs;
    GT_U32 maxCs = mvHwsDdr3TipMaxCSGet(devNum);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    RepeatNum = RepeatNum;
	RepeatNum = 2;
    if ( mode == 1)
    {
        /* per pup */
        startPup = 0;
        endPup = octetsPerInterfaceNum-1;
        pupAccess = ACCESS_TYPE_UNICAST;
    }
    else
    {
        startPup = 0;
        endPup = 0;
        pupAccess = ACCESS_TYPE_MULTICAST;
    }
    for(uiCs=0; uiCs < maxCs; uiCs++){
	for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
	{
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			for( pup = startPup ;pup <= endPup ; pup++)
			{
				ctrlSweepres[adll][interfaceId][pup] = 0;
			}
		}
	}

	for(adll = 0 ; adll < (MAX_INTERFACE_NUM * MAX_BUS_NUM) ; adll++)
	{
		ctrlADLL[adll] = 0;
	}
	/*Save DQS value(after algorithm run)*/
	mvHwsDdr3TipReadAdllValue(devNum,ctrlADLL, (reg + (uiCs * CS_REGISTER_ADDR_OFFSET)) , MASK_ALL_BITS );

	/*Sweep ADLL  from 0:31 on all I/F on all Pup and perform BIST on each stage.*/
	for(pup=startPup; pup <=endPup; pup++)
	{
		for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
		{
			for(rep = 0; rep < RepeatNum ; rep++)
			{
				for(sweepPatternIndex = sweepPatternIndexStart ; sweepPatternIndex < sweepPatternIndexEnd ; sweepPatternIndex++)
				{
					adllValue = (direction == 0) ? (adll*2):adll;
					CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, 0, pupAccess, pup, DDR_PHY_DATA, reg + CS_BYTE_GAP(uiCs), adllValue));
					mvHwsDdr3RunBist(devNum, sweepPattern, res ,uiCs);
					/*ddr3TipResetFifoPtr(devNum);*/
					for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
					{
						VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
						ctrlSweepres[adll][interfaceId][pup] += res[interfaceId];
						if (mode == 1)
						{
							CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, reg + CS_BYTE_GAP(uiCs),  ctrlADLL[interfaceId*uiCs*octetsPerInterfaceNum+pup]));
						}
					}
				}
			}
		}
	}
	mvPrintf("Final,CS %d,%s,Sweep,Result,Adll,", uiCs,((direction==0) ? "TX":"RX"));
	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
	{
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (mode == 1)
		{
			for(pup=startPup; pup <=endPup; pup++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
				mvPrintf("I/F%d-PHY%d , ",interfaceId,pup);
			}
		}
		else
		{
			mvPrintf("I/F%d , ",interfaceId);
		}
	}
	mvPrintf("\n");
	for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
	{
		adllValue = (direction == 0) ? (adll*2):adll;
		mvPrintf("Final,%s,Sweep,Result, %d ,",((direction==0) ? "TX":"RX"),adllValue);

		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++){
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			for( pup=startPup; pup <=endPup; pup++)
			{
				mvPrintf("%8d , ",ctrlSweepres[adll][interfaceId][pup]);
			}
		}
		mvPrintf("\n");
	}
	/*Write back to the phy the Rx DQS value, we store in the begging. */
	mvHwsDdr3TipWriteAdllValue(devNum,ctrlADLL, (reg +  uiCs * CS_REGISTER_ADDR_OFFSET));
	/* print adll results */
	mvHwsDdr3TipReadAdllValue(devNum,ctrlADLL, (reg + uiCs * CS_REGISTER_ADDR_OFFSET), MASK_ALL_BITS);
	mvPrintf("%s,DQS,ADLL,,,",(direction==0) ? "Tx":"Rx");
	printAdll(devNum, ctrlADLL);
    }
    ddr3TipResetFifoPtr(devNum);
    return 0;
}

#if defined(EXCLUDE_SWITCH_DEBUG)
GT_BOOL ddr3TipRunLevelingSweepTest(GT_32 devNum, GT_U32 RepeatNum, GT_U32 direction, GT_U32 mode)
{
    GT_U32  pup = 0, startPup = 0, endPup = 0, startAdll = 0;
    GT_U32 adll = 0,rep = 0,sweepPatternIndex = 0;
	GT_U32  readData[MAX_INTERFACE_NUM];
    GT_U32 res[MAX_INTERFACE_NUM] = {0};
    GT_32  interfaceId = 0,gap = 0;
    GT_32 adllValue = 0;
    GT_32 reg = (direction == 0) ? WL_PHY_REG : RL_PHY_REG;
    MV_HWS_ACCESS_TYPE  pupAccess;
    GT_U32 uiCs;
    GT_U32 maxCs = mvHwsDdr3TipMaxCSGet(devNum);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);
    RepeatNum = RepeatNum;
	RepeatNum = 3;
    if ( mode == 1)
    {
        /* per pup */
        startPup = 0;
        endPup = octetsPerInterfaceNum-1;
        pupAccess = ACCESS_TYPE_UNICAST;
    }
    else
    {
        startPup = 0;
        endPup = 0;
        pupAccess = ACCESS_TYPE_MULTICAST;
    }
    for(uiCs=0; uiCs < maxCs; uiCs++){
		for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
		{
			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
			{
				VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
				for( pup = startPup ;pup <= endPup ; pup++)
				{
					ctrlSweepres[adll][interfaceId][pup] = 0;
				}
			}
		}

		for(adll = 0 ; adll < (MAX_INTERFACE_NUM * MAX_BUS_NUM) ; adll++)
		{
			ctrlADLL[adll] = 0;
			ctrlLevelPhase[adll] = 0;
			ctrlADLL1[adll] = 0;
		}
		/*Save Leveling value(after algorithm run)*/
		mvHwsDdr3TipReadAdllValue(devNum,ctrlADLL, (reg + (uiCs * CS_REGISTER_ADDR_OFFSET)) , 0x1F );
		readPhaseValue(devNum,ctrlLevelPhase, (reg + (uiCs * CS_REGISTER_ADDR_OFFSET)) , 0x7<<6 );
		if(direction == 0)
		{
			mvHwsDdr3TipReadAdllValue(devNum,ctrlADLL1, (0x1 + (uiCs * CS_REGISTER_ADDR_OFFSET)) , MASK_ALL_BITS );
		}
		/*Sweep ADLL  from 0:31 on all I/F on all Pup and perform BIST on each stage.*/
		for(pup=startPup; pup <=endPup; pup++)
		{
			for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
			{
				for(rep = 0; rep < RepeatNum ; rep++)
				{
					adllValue = (direction == 0) ? (adll*2):(adll*3);
					for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
					{
						startAdll = ctrlADLL[interfaceId*uiCs*octetsPerInterfaceNum+pup] + (ctrlLevelPhase[interfaceId*uiCs*octetsPerInterfaceNum+pup]>>6)*32 ;
						if(direction == 0)
						{
							startAdll = (startAdll>32)?(startAdll-32):0;
						}else
						{
							startAdll = (startAdll>48)?(startAdll-48):0;
						}
						if(direction == 0)
						{
							adllValue = startAdll + adllValue;
						}else
						{
							adllValue = startAdll + adllValue;
						}
						gap = ctrlADLL1[interfaceId*uiCs*octetsPerInterfaceNum+pup] - ctrlADLL[interfaceId*uiCs*octetsPerInterfaceNum+pup];
						gap = (((adllValue%32) + gap)%64);
						adllValue = ((adllValue%32) + (((adllValue- (adllValue%32))/32)<<6));

						CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, pupAccess, pup, DDR_PHY_DATA, reg + CS_BYTE_GAP(uiCs), adllValue));
						if(direction == 0)
						{
							CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, pupAccess, pup, DDR_PHY_DATA, 0x1 + CS_BYTE_GAP(uiCs), gap));
						}
					}
					for(sweepPatternIndex = sweepPatternIndexStart ; sweepPatternIndex < sweepPatternIndexEnd ; sweepPatternIndex++)
					{
						mvHwsDdr3RunBist(devNum, sweepPattern, res ,uiCs);
						ddr3TipResetFifoPtr(devNum);
						for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
						{
							VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
							if(pup != 4)
							{
								ctrlSweepres[adll][interfaceId][pup] += res[interfaceId];
							}else
							{
								CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1458, readData, MASK_ALL_BITS));
								ctrlSweepres[adll][interfaceId][pup] += readData[interfaceId];

								CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1458, 0x0 , 0xFFFFFFFF));
								CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x145C, 0x0 , 0xFFFFFFFF));
							}
						}
					}
				}
			}
			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
			{
				startAdll = ctrlADLL[interfaceId*uiCs*octetsPerInterfaceNum+pup] + ctrlLevelPhase[interfaceId*uiCs*octetsPerInterfaceNum+pup] ;
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, pupAccess, pup, DDR_PHY_DATA, reg + CS_BYTE_GAP(uiCs), startAdll));
				if(direction == 0)
				{
					CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, pupAccess, pup, DDR_PHY_DATA, 0x1 + CS_BYTE_GAP(uiCs), ctrlADLL1[interfaceId*uiCs*octetsPerInterfaceNum+pup]));
				}
			}
		}
		mvPrintf("Final,CS %d,%s,Leveling,Result,Adll,", uiCs,((direction==0) ? "TX":"RX"));
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			if (mode == 1)
			{
				for(pup=startPup; pup <=endPup; pup++)
				{
					VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
					mvPrintf("I/F%d-PHY%d , ",interfaceId,pup);
				}
			}
			else
			{
				mvPrintf("I/F%d , ",interfaceId);
			}
		}
		mvPrintf("\n");
		for(adll = 0 ; adll < ADLL_LENGTH ; adll++)
		{
			adllValue = (direction == 0) ? ((adll*2)-32):((adll*3)-48);
			mvPrintf("Final,%s,LevelingSweep,Result, %d ,",((direction==0) ? "TX":"RX"),adllValue);

			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++){
				VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
				for( pup=startPup; pup <=endPup; pup++)
				{
					mvPrintf("%8d , ",ctrlSweepres[adll][interfaceId][pup]);
				}
			}
			mvPrintf("\n");
		}
		/*Write back to the phy the Rx DQS value, we store in the begging. */
		writeLevelingValue(devNum,ctrlADLL,ctrlLevelPhase, (reg +  uiCs * CS_REGISTER_ADDR_OFFSET));
		if(direction == 0)
		{
			mvHwsDdr3TipWriteAdllValue(devNum,ctrlADLL1, (0x1 + (uiCs * CS_REGISTER_ADDR_OFFSET)));
		}
		/* print adll results */
		mvHwsDdr3TipReadAdllValue(devNum,ctrlADLL, (reg + uiCs * CS_REGISTER_ADDR_OFFSET), MASK_ALL_BITS);
		mvPrintf("%s,DQS,Leveling,,,",(direction==0) ? "Tx":"Rx");
		printAdll(devNum, ctrlADLL);
		printPh(devNum, ctrlLevelPhase);
	}/*End of CS*/

    ddr3TipResetFifoPtr(devNum);
    return 0;
}
#endif


void printTopology(MV_HWS_TOPOLOGY_MAP *pTopologyDB)
{
	GT_U32 ui, uj;

	mvPrintf("\tinterfaceMask: 0x%x\n",   pTopologyDB->interfaceActiveMask);
	mvPrintf("\tNum Bus:  %d\n",   ddr3TipDevAttrGet(0, MV_ATTR_OCTET_PER_INTERFACE));
	mvPrintf("\tactiveBusMask: 0x%x\n",   pTopologyDB->activeBusMask);

	for( ui = 0; ui < MAX_INTERFACE_NUM; ui++ )
	{
		VALIDATE_IF_ACTIVE(pTopologyDB->interfaceActiveMask, ui)
		mvPrintf ("\n\tInterface ID: %d\n",ui);
		mvPrintf("\t\tDDR Frequency: %s\n",   ConvertFreq(pTopologyDB->interfaceParams[ ui ].memoryFreq));
		mvPrintf("\t\tSpeedBin: %d\n",   pTopologyDB->interfaceParams[ ui ].speedBinIndex);
		mvPrintf("\t\tBusWidth: %d\n", (4 << pTopologyDB->interfaceParams[ ui ].busWidth));
		mvPrintf("\t\tMemSize: %s\n",   ConvertMemSize(pTopologyDB->interfaceParams[ ui ].memorySize));
		mvPrintf("\t\tCasWL: %d\n",   pTopologyDB->interfaceParams[ ui ].casWL);
		mvPrintf("\t\tCasL: %d\n",   pTopologyDB->interfaceParams[ ui ].casL);
		mvPrintf("\t\tTemperature: %d\n",   pTopologyDB->interfaceParams[ ui ].interfaceTemp);
		mvPrintf("\n");
		for( uj = 0; uj < 4; uj++ )
		{
			mvPrintf("\t\tBUS %d parameters- CS Mask: 0x%x\t", uj, pTopologyDB->interfaceParams[ ui ].asBusParams[ uj ].csBitmask);
			mvPrintf("Mirror: 0x%x\t",   pTopologyDB->interfaceParams[ ui ].asBusParams[ uj ].mirrorEnableBitmask);
			mvPrintf("DQS Swap is %s \t", (pTopologyDB->interfaceParams[ ui ].asBusParams[ uj ].isDqsSwap == 1)? "enabled": "disabled");
			mvPrintf("Ck Swap:%s\t",   (pTopologyDB->interfaceParams[ ui ].asBusParams[ uj ].isCkSwap == 1)? "enabled": "disabled");
			mvPrintf("\n");
		}
	}
}
#endif
#ifndef EXCLUDE_SWITCH_DEBUG
/*****************************************************************************
Execute XSB Test transaction (rd/wr/both)
******************************************************************************/
GT_STATUS RunXsbTest(GT_U32 devNum, GT_U32 uiMemAddress, GT_U32 writeType, GT_U32 readType, GT_U32 burstLength)
{
    GT_U32 seq = 0, interfaceId = 0, uiAddr, cnt;
    GT_STATUS retVal = GT_OK, retTmp;
	GT_U32 dataRead[MAX_INTERFACE_NUM];



    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        uiAddr = uiMemAddress;
        /*DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_TRACE, " interfaceIdIndex %d numIfTested %d interfaceId %d \n",interfaceId, numIfTested, interfaceId);*/
        for(cnt = 0; cnt <= burstLength; cnt++)
        {
            seq = (seq + 1)%8;
            if (writeType != 0)
            {
                CHECK_STATUS(ddr3TipExtWrite(devNum, interfaceId, uiAddr, 1, xsbTestTable[seq]));
            }
            if (readType != 0)
            {
                CHECK_STATUS(ddr3TipExtRead(  devNum, interfaceId, uiAddr, 1, dataRead));
            }
            if ((readType != 0)&&(writeType != 0))
            {
                retTmp = ddr3TipCompare(interfaceId, xsbTestTable[seq], dataRead, 0xff);
                uiAddr += (EXT_ACCESS_BURST_LENGTH*4);
                retVal = (retVal != GT_OK) ? retVal:retTmp;
            }
        }
    }
    return retVal;
}

#else /*EXCLUDE_SWITCH_DEBUG*/
GT_U32  startXsbOffset = 0;
GT_STATUS RunXsbTest(GT_U32 devNum, GT_U32 uiMemAddress, GT_U32 writeType, GT_U32 readType, GT_U32 burstLength)
{
	return GT_OK;
}

#endif

