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
* mvHwsTrainingIpDdr3TrainingLeveling.c
*
* DESCRIPTION: DDR3 training IP configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
******************************************************************************/

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingLeveling.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3TrainingIpCentralization.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3TrainingHwAlgo.h"
#include "mvDdr3LoggingDef.h"
#include "mvDdrTopologyDef.h"

#define WL_ITERATION_NUM            (10)
#define MATRIX_MEM_ADDRESS      (0xfd000)

#define NO_PHASE_SHIFT				(0)
#define ONE_CLOCK_ERROR_SHIFT		(2)
#define TWO_CLOCK_ERROR_SHIFT		(4)
#define THREE_CLOCK_ERROR_SHIFT		(6)
#define ALIGN_ERROR_SHIFT			(-2)

static GT_U32 pupMaskTable[]=
{
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0xFF000000
};

extern GT_U8 debugLeveling;
WriteSuppResultStruct writeSuppResultTable[MAX_INTERFACE_NUM][MAX_BUS_NUM];

extern MV_HWS_RESULT trainingResult[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM];
extern AUTO_TUNE_STAGE trainingStage;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern ClValuePerFreq casLatencyTable[];
extern GT_U32  startXsbOffset;
extern GT_U32 csMaskReg[];
extern GT_U32 debugMode;
extern GT_U32 odtConfig;
extern GT_U32 effective_cs;
extern GT_U32  PhyReg1Val;

static GT_STATUS    ddr3TipDynamicWriteLevelingSeq
(
    GT_U32  devNum
);

static GT_STATUS    ddr3TipDynamicReadLevelingSeq
(
    GT_U32   devNum
);
static GT_STATUS    ddr3TipDynamicPerBitReadLevelingSeq
(
    GT_U32   devNum
);

static GT_STATUS    ddr3TipWlSuppAlignPhaseShift
(
	GT_U32 devNum,
    GT_U32 interfaceId,
    GT_U32 busId
);

static GT_STATUS    ddr3TipXsbCompareTest
(
    GT_U32 devNum,
    GT_U32 interfaceId,
    GT_U32 busId,
    GT_8 offset
);

/*****************************************************************************
mvHwsDdr3TipMaxCSGet
******************************************************************************/
GT_U32 mvHwsDdr3TipMaxCSGet(GT_U32 devNum)
{
  GT_U32 c_cs,interfaceId=0,busId=0;
  GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);
  static GT_U32 max_cs=0;

  if (!max_cs)
  {
      CHECK_STATUS(ddr3TipGetFirstActiveIf((GT_U8)devNum, topologyMap->interfaceActiveMask, &interfaceId));
 	  for(busId=0; busId<octetsPerInterfaceNum; busId++)
      {
          VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
          break;
      }

      for(c_cs = 0;c_cs < NUM_OF_CS; c_cs++)
      {
		VALIDATE_IF_ACTIVE(topologyMap->interfaceParams[interfaceId].asBusParams[busId].csBitmask, c_cs)
		max_cs++;
	  }
  }
  return max_cs;
}

/*****************************************************************************
Dynamic read leveling
******************************************************************************/
GT_STATUS    ddr3TipDynamicReadLeveling
(
    GT_U32  devNum,
    GT_U32  freq
)
{
    GT_U32 data, mask;
	GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);
	GT_U32 busNum, interfaceId, clVal;
    MV_HWS_SPEED_BIN speedBinIndex;
	GT_U32 csEnableRegVal[MAX_INTERFACE_NUM] = {0}; /* save current CS value */
    GT_BOOL isAnyPupFail = GT_FALSE;
    GT_U32   dataRead[MAX_INTERFACE_NUM+1] = {0};
	GT_U8   RLValues[NUM_OF_CS][MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
	PatternInfo *patternTable = ddr3TipGetPatternTable();
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	for(effective_cs = 0; effective_cs < NUM_OF_CS; effective_cs++)
		for(busNum = 0; busNum < MAX_BUS_NUM; busNum++)
			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
					RLValues[effective_cs][busNum][interfaceId] =0;

	for(effective_cs = 0; effective_cs < max_cs; effective_cs++){

		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			trainingResult[trainingStage][interfaceId] = TEST_SUCCESS;

			/* save current cs enable reg val */
			CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal, MASK_ALL_BITS));
			/* enable single cs */
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
		}

		ddr3TipResetFifoPtr(devNum);

		/************************************************************************/
		/*     Phase 1: Load pattern (using ODPG)                               */
		/************************************************************************/
		/* enter Read Leveling mode */
		/* only 27 bits are masked */
		/* assuming non multi-CS configuration */
		/* write to CS = 0 for the non multi CS configuration, note that the results shall be read back to the required CS !!! */
		/* BUS count is 0 shifted 26 */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_DATA_CONTROL_REG, 0x3 , 0x3));
		CHECK_STATUS(ddr3TipConfigureOdpg(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0,
										patternTable[PATTERN_RL].numOfPhasesTx, 0,
										patternTable[PATTERN_RL].numOfPhasesRx, 0,
										0,  effective_cs, STRESS_NONE, DURATION_SINGLE));

	    /* load pattern to ODPG */
		ddr3TipLoadPatternToOdpg(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, PATTERN_RL, patternTable[PATTERN_RL].startAddr);
		/************************************************************************/
		/*     Phase 2: ODPG to Read Leveling mode                              */
		/************************************************************************/
		/* General Training Opcode register */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_WRITE_READ_MODE_ENABLE_REG,  0, MASK_ALL_BITS));

		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_CONTROL_REG,( 0x301b01 | effective_cs <<2), 0x3C3FEF));
		 /*
		for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
		 {
			if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  GT_FALSE)
            continue;

			CHECK_STATUS(mvHwsDdr3TipIFRead(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_TRAINING_CONTROL_REG,  dataRead, (0x3 << 20)));
			if (dataRead[interfaceId] == 0x300000)
			 {
			    continue;
			 }
		}
    */
    /* Object1 opcode register 0 & 1 */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        speedBinIndex = topologyMap->interfaceParams[interfaceId].speedBinIndex;
        clVal = casLatencyTable[speedBinIndex].clVal[freq];
        /*DEBUG_LEVELING("clVal %d (IF %d speed bin %d freq %d)\n", clVal, interfaceId, speedBinIndex, freq);*/
        /*CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, 0, ODPG_OBJ1_OPCODE_REG, ((clVal << 17) | (0x3 << 25)), ((0xFF << 9) | (clVal << 17) | (0x3 << 25))));*/
        data = (clVal << 17) | (0x3 << 25);
        mask = (0xFF << 9) |(0x1F << 17) | (0x3 << 25);
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_OBJ1_OPCODE_REG, data, mask));
    }
    /* Set iteration count to max value */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_OPCODE_1_REG, 0xD00, 0xD00));
    /************************************************************************/
    /*     Phase 2: Mask config                                             */
    /************************************************************************/
    ddr3TipDynamicReadLevelingSeq(devNum);
    /************************************************************************/
    /*     Phase 3: Read Leveling execution                                   */
    /************************************************************************/
    /* temporary jira dunit=14751 */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_DBG_1_REG, 0, (GT_U32)(1 << 31)));
    /* configure phy reset value */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_DBG_3_REG, (0x7F << 24), (GT_U32)(0xFF << 24)));
    /* data pup rd reset enable  */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, SDRAM_CONFIGURATION_REG, 0, (1 << 30)));
    /* data pup rd reset disable */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, SDRAM_CONFIGURATION_REG, (1 << 30), (1 << 30)));
    /* training SW override & training RL mode */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x1, 0x9));
    /* training enable */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_REG, (1 << 24) | (1 << 20), (1 << 24) | (1 << 20)));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_REG, (GT_U32)(1 << 31), (GT_U32)(1 << 31)));
    /********* trigger training *******************/
	if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_TRIGGER_REG, 0x1, 0x1));

		  /*check for training done + results pass*/
		  if (ddr3TipIfPolling(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x2, 0x2, ODPG_TRAINING_STATUS_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			  {
				  DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("Training Done Failed\n"));
				  return GT_FAIL;
			  }
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
		    VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		  CHECK_STATUS(mvHwsDdr3TipIFRead(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_TRAINING_TRIGGER_REG,  dataRead, 0x4));
		data = dataRead[interfaceId];
		if(data != 0x0) {
		  DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("Training Result Failed\n"));
		  }
		}
		  /*disable ODPG - Back to functional mode*/
		  CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_ENABLE_REG, 0x1 << ODPG_DISABLE_OFFS,  (0x1 << ODPG_DISABLE_OFFS)));
		  if (ddr3TipIfPolling(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x0, 0x1, ODPG_ENABLE_REG, MAX_POLLING_ITERATIONS) != GT_OK)
				  {
					  DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("ODPG disable failed "));
					  return GT_FAIL;
				  }
		  CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
	}
	else {
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_STATUS_REG, 0x1, 0x1));
	}
		/************ double loop on bus, pup *********/
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			/* check training done */
			isAnyPupFail = GT_FALSE;
			for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
			{
       			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
				if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId, (1 << 25), (1 << 25), maskResultsPupRegMap[busNum], MAX_POLLING_ITERATIONS) != GT_OK)
				{
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("\nRL: DDR3 poll failed(2) for IF %d CS %d bus %d", interfaceId, effective_cs, busNum));
					isAnyPupFail = GT_TRUE;
				}
				else
				{
					/* read result per pup */
					/* CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsPupRegMap[busNum], dataRead, MASK_ALL_BITS)); */
					CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsPupRegMap[busNum], dataRead, 0xff));
					RLValues[effective_cs][busNum][interfaceId] = (GT_U8)dataRead[interfaceId];
				}
			}
			if (isAnyPupFail == GT_TRUE)
			{
				trainingResult[trainingStage][interfaceId] = TEST_FAILED;
				if (debugMode == GT_FALSE)
				{
					return GT_FAIL;
				}
			}
		}

		DEBUG_LEVELING(DEBUG_LEVEL_INFO, ("RL exit read leveling \n"));
		/************************************************************************/
		/*     Phase 3: Exit Read Leveling                                      */
		/************************************************************************/
		/************** ********************/
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, (1 << 3),  (1 << 3)));
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_1_REG, (1 << 16), (1 << 16)));
		/* set ODPG to functional */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));
		/* Copy the result from the effective CS search to the real Functional CS */
		/*ddr3TipWriteCsResult(devNum, RL_PHY_REG);*/
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));

	}
	for(effective_cs = 0; effective_cs < max_cs; effective_cs++){
		/************ double loop on bus, pup *********/
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
				/* read result per pup from arry */
				data = RLValues[effective_cs][busNum][interfaceId];
				data = (data & 0x1f) | (((data & 0xE0) >> 5) << 6);
				mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, RL_PHY_REG + ((effective_cs == 0)? 0x0:0x4), data);
			}
		}
	}
	effective_cs = 0;/*Set to 0 after each loop to avoid illegal value may be used*/

	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* restore cs enable value*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal[interfaceId], MASK_ALL_BITS));
        if (odtConfig != 0)
        {
            CHECK_STATUS(ddr3TipWriteAdditionalOdtSetting(devNum, interfaceId));   
        }
    }
	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (trainingResult[trainingStage][interfaceId] == TEST_FAILED)
			return GT_FAIL;
    }
    return GT_OK;
}

/*****************************************************************************
Legacy Dynamic write leveling
******************************************************************************/
GT_STATUS    ddr3TipLegacyDynamicWriteLeveling
(
    GT_U32  devNum
)
{
	GT_U32 c_cs, interfaceId, cs_mask = 0;
	GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);

/*	in TRAINIUNG reg (0x15b0) write 0x80000008 | cs_mask:
	TrnStart
	cs_mask = 0x1 <<20 Trn_CS0 - CS0 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<21 Trn_CS1 - CS1 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<22 Trn_CS2 - CS2 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<23 Trn_CS3 - CS3 is included in the DDR3 training sequence.
	TrnAutoSeq =  write leveling */
	for (c_cs = 0; c_cs < max_cs; c_cs++)
		cs_mask = cs_mask | 1 << (20 + c_cs);

	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++) {
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, 0,TRAINING_REG, (0x80000008 | cs_mask), 0xFFFFFFFF));
		CHECK_STATUS(hwsOsExactDelayPtr((GT_U8)devNum, devNum,  20));
		if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId,0, (GT_U32)0x80000000, TRAINING_REG, MAX_POLLING_ITERATIONS) != GT_OK)
		{
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("polling failed for Old WL result\n"));
			return GT_FAIL;
		}
	}
	return GT_OK;
}

/*****************************************************************************
Legacy Dynamic read leveling
******************************************************************************/
GT_STATUS    ddr3TipLegacyDynamicReadLeveling
(
    GT_U32  devNum
)
{
	GT_U32 c_cs, interfaceId, cs_mask = 0;
	GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);

	/* in TRAINIUNG reg (0x15b0) write 0x80000040 | cs_mask:
	TrnStart
	cs_mask = 0x1 <<20 TTrn_CS0 - CS0 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<21 TTrn_CS1 - CS1 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<22 TTrn_CS2 - CS2 is included in the DDR3 training sequence.
	cs_mask = 0x1 <<23 TTrn_CS3 - CS3 is included in the DDR3 training sequence.
	TrnAutoSeq =  Read Leveling using training pattern  */

	for (c_cs = 0; c_cs < max_cs; c_cs++)
		cs_mask = cs_mask | 1 << (20 + c_cs);

	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, 0,TRAINING_REG, (0x80000040 | cs_mask), 0xFFFFFFFF));
	CHECK_STATUS(hwsOsExactDelayPtr((GT_U8)devNum, devNum,  20));
	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++) {
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId,0, (GT_U32)0x80000000, TRAINING_REG, MAX_POLLING_ITERATIONS) != GT_OK)
		{
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("polling failed for Old RL result\n"));
			return GT_FAIL;
		}
	}
	return GT_OK;
}


/*****************************************************************************
Dynamic per bit read leveling
******************************************************************************/
GT_STATUS    ddr3TipDynamicPerBitReadLeveling
(
    GT_U32  devNum,
    GT_U32  freq
)
{
    GT_U32 data, mask;
    GT_U32 busNum, interfaceId, clVal, bit_numb;
	GT_U32 curr_numb,currMinDelay;

	int adll_array[3] = {0,-0xA,0x14};
	GT_U32 phyreg3_arr[MAX_INTERFACE_NUM][MAX_BUS_NUM];
    MV_HWS_SPEED_BIN speedBinIndex;
	GT_BOOL isAnyPupFail = GT_FALSE;
	GT_BOOL breakLoop = GT_FALSE;
	GT_U32 csEnableRegVal[MAX_INTERFACE_NUM]; /* save current CS value */
	GT_U32  dataRead[MAX_INTERFACE_NUM];
	GT_BOOL perBitRLPupStatus[MAX_INTERFACE_NUM][MAX_BUS_NUM];
    GT_U32  data2Write[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	PatternInfo *patternTable = ddr3TipGetPatternTable();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busNum = 0; busNum <=octetsPerInterfaceNum; busNum++)
		{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
				perBitRLPupStatus[interfaceId][busNum] = 0;	
				data2Write[interfaceId][busNum] = 0;	
				/* read current value of phy register 0x3 */
				CHECK_STATUS(mvHwsDdr3TipBUSRead (devNum,interfaceId, ACCESS_TYPE_UNICAST,busNum,DDR_PHY_DATA, READ_CENTRALIZATION_PHY_REG, &phyreg3_arr[interfaceId][busNum]));
		}
	}
    /* NEW RL machine */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        trainingResult[trainingStage][interfaceId] = TEST_SUCCESS;

        /* save current cs enable reg val */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, &csEnableRegVal[interfaceId], MASK_ALL_BITS));
        /* enable single cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
    }

   
	ddr3TipResetFifoPtr(devNum);
	for ( curr_numb = 0; curr_numb <3; curr_numb++)
	{

		/************************************************************************/
		/*     Phase 1: Load pattern (using ODPG)                               */
		/************************************************************************/
		/* enter Read Leveling mode */
		/* only 27 bits are masked */
		/* assuming non multi-CS configuration */
		/* write to CS = 0 for the non multi CS configuration, note that the results shall be read back to the required CS !!! */
		/* BUS count is 0 shifted 26 */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_DATA_CONTROL_REG, 0x3 , 0x3));
		CHECK_STATUS(ddr3TipConfigureOdpg(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0, 
										patternTable[PATTERN_TEST].numOfPhasesTx, 0,
										patternTable[PATTERN_TEST].numOfPhasesRx, 0,
										0,  0, STRESS_NONE, DURATION_SINGLE));

		/* load pattern to ODPG */
		ddr3TipLoadPatternToOdpg(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, PATTERN_TEST, patternTable[PATTERN_TEST].startAddr);
		/************************************************************************/
		/*     Phase 2: ODPG to Read Leveling mode                              */
		/************************************************************************/
		/* General Training Opcode register */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_WRITE_READ_MODE_ENABLE_REG,  0, MASK_ALL_BITS));
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_CONTROL_REG,  0x301B01, 0x3C3FEF));
 
		/* Object1 opcode register 0 & 1 */
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			speedBinIndex = topologyMap->interfaceParams[interfaceId].speedBinIndex;
			clVal = casLatencyTable[speedBinIndex].clVal[freq];
			/*DEBUG_LEVELING("clVal %d (IF %d speed bin %d freq %d)\n", clVal, interfaceId, speedBinIndex, freq);*/
			/*CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, 0, ODPG_OBJ1_OPCODE_REG, ((clVal << 17) | (0x3 << 25)), ((0xFF << 9) | (clVal << 17) | (0x3 << 25))));*/
			data = (clVal << 17) | (0x3 << 25);
			mask = (0xFF << 9) |(0x1F << 17) | (0x3 << 25);
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_OBJ1_OPCODE_REG, data, mask));
		}
		/* Set iteration count to max value */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_OPCODE_1_REG, 0xD00, 0xD00));
		/************************************************************************/
		/*     Phase 2: Mask config                                             */
		/************************************************************************/
		ddr3TipDynamicPerBitReadLevelingSeq(devNum);
		/************************************************************************/
		/*     Phase 3: Read Leveling execution                                   */
		/************************************************************************/
		/* temporary jira dunit=14751 */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_DBG_1_REG, 0, (GT_U32)(1 << 31)));
		/* configure phy reset value */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_DBG_3_REG, (0x7F << 24), (GT_U32)(0xFF << 24)));
		/* data pup rd reset enable  */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, SDRAM_CONFIGURATION_REG, 0, (1 << 30)));
		/* data pup rd reset disable */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, SDRAM_CONFIGURATION_REG, (1 << 30), (1 << 30)));
		/* training SW override & training RL mode */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x1, 0x9));
		/* training enable */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_REG, (1 << 24) | (1 << 20), (1 << 24) | (1 << 20)));
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_REG, (GT_U32)(1 << 31), (GT_U32)(1 << 31)));
		/********* trigger training *******************/
		if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_TRIGGER_REG, 0x1, 0x1));

			/*check for training done + results pass*/
			if (ddr3TipIfPolling(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x2, 0x2, ODPG_TRAINING_STATUS_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			{
				 DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("Training Done Failed\n"));
				 return GT_FAIL;
			}
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
		    VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_TRAINING_TRIGGER_REG,  dataRead, 0x4));
			data = dataRead[interfaceId];
			if(data != 0x0) {
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("Training Result Failed\n"));
			}
		}
			/*disable ODPG - Back to functional mode*/
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_ENABLE_REG, 0x1 << ODPG_DISABLE_OFFS,  (0x1 << ODPG_DISABLE_OFFS)));
			if (ddr3TipIfPolling(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x0, 0x1, ODPG_ENABLE_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			{
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("ODPG disable failed "));
				return GT_FAIL;
			}
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
		}
		else {
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_TRAINING_STATUS_REG, 0x1, 0x1));
		}

		/************ double loop on bus, pup *********/
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			/* check training done */
			for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
				if (perBitRLPupStatus[interfaceId][busNum] == GT_FALSE)
				{
					currMinDelay = 0;
					for (bit_numb=0; bit_numb < 8; bit_numb++)
					{
						if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId, (1 << 25), (1 << 25), maskResultsDqRegMap[busNum*8 +bit_numb], MAX_POLLING_ITERATIONS) != GT_OK)
						{
							DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("\nRL: DDR3 poll failed(2) for bus %d bit %d\n", busNum, bit_numb));
						}
						else
						{
							/* read result per pup */
							CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsDqRegMap[busNum *8 + bit_numb], dataRead, MASK_ALL_BITS));
							data = (dataRead[interfaceId]  & 0x1f) | ((dataRead[interfaceId]  & 0xE0) << 1);
							if (currMinDelay == 0)
								currMinDelay = data;
							else
								if	(data < currMinDelay)
									currMinDelay = data;
							if ((data > data2Write[interfaceId][busNum])) /* && (data < MAX_DQ_READ_LEVELING_DELAY)) */
							{
								data2Write[interfaceId][busNum] = data;
							}
						}

					}
					if (data2Write[interfaceId][busNum] <= (currMinDelay + MAX_DQ_READ_LEVELING_DELAY))
					{
						perBitRLPupStatus[interfaceId][busNum] = GT_TRUE;
					}

				}
			}
		}
		/* check if there is need to search new phyreg3 value */
		if (curr_numb < 2) /* if there is DLL that is not checked yet */
		{
			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
			{
				VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
				for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
				{
	   				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
					if (perBitRLPupStatus[interfaceId][busNum] != GT_TRUE)
					{
						/* go to next ADLL value */
						CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, READ_CENTRALIZATION_PHY_REG, (phyreg3_arr[interfaceId][busNum] + adll_array[curr_numb])));
						breakLoop = GT_TRUE;
						break;	
					}
				}
				if (breakLoop)
					break;
			}
		} /*if (curr_numb < 2) */
		if (!breakLoop)
			break;
	} /*for ( curr_numb = 0; curr_numb <3; curr_numb++) */

	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
        {
	   		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
			if (perBitRLPupStatus[interfaceId][busNum] == GT_TRUE)
				mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, RL_PHY_REG + CS_BYTE_GAP(effective_cs), data2Write[interfaceId][busNum]);
			else
				isAnyPupFail = GT_TRUE;
		}
				/* TBD flow does not support multi CS */
				/* csBitmask = topologyMap->interfaceParams[interfaceId].asBusParams[busNum].csBitmask; */
				/*divide by 4 is used for retrieving the CS number*/
				/* TBD BC2 - what is the PHY address for other CS ddr3TipWriteCsResult() ??? */
				/* find what should be written to PHY  - max delay that is less than threshold*/
	
		if (isAnyPupFail == GT_TRUE)
		{
			trainingResult[trainingStage][interfaceId] = TEST_FAILED;
			if (debugMode == GT_FALSE)
	        return GT_FAIL; 
		}

	}
    DEBUG_LEVELING(DEBUG_LEVEL_INFO, ("RL exit read leveling \n"));
    /************************************************************************/
    /*     Phase 3: Exit Read Leveling                                      */
    /************************************************************************/
    /************** ********************/
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, (1 << 3),  (1 << 3)));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_1_REG, (1 << 16), (1 << 16)));
    /* set ODPG to functional */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));
	/* Copy the result from the effective CS search to the real Functional CS */
    ddr3TipWriteCsResult(devNum, RL_PHY_REG);
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* restore cs enable value*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal[interfaceId], MASK_ALL_BITS));
        if (odtConfig != 0)
        {
            CHECK_STATUS(ddr3TipWriteAdditionalOdtSetting(devNum, interfaceId));   
        }
    }
	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (trainingResult[trainingStage][interfaceId] == TEST_FAILED) 
			return GT_FAIL;
    }
    return GT_OK;
}

GT_STATUS    ddr3TipCalcCsMask
(
	GT_U32    devNum,
	GT_U32    interfaceId,
	GT_U32    effectiveCs,
	GT_U32	  *csMask
)
{
	GT_U32 allBusCs = 0, sameBusCs;
	GT_U32 busCnt;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	devNum = devNum; /* avoid warnings */

	*csMask = sameBusCs = CS_BIT_MASK;

	/* in some of the devices (such as BC2), the CS is per pup and there for mixed mode is valid
	   on like other devices where CS configuration is per interface.
	   In order to know that, we do 'Or' and 'And' operation between all CS (of the pups).
	   if they are they are not the same then it's mixed mode so all CS should be configured
	   (when configuring the MRS)*/

	for (busCnt=0; busCnt<octetsPerInterfaceNum; busCnt++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)

		allBusCs |= topologyMap->interfaceParams[interfaceId].asBusParams[busCnt].csBitmask;
		sameBusCs &= topologyMap->interfaceParams[interfaceId].asBusParams[busCnt].csBitmask;

		/* cs enable is active low */
		*csMask &= ~topologyMap->interfaceParams[interfaceId].asBusParams[busCnt].csBitmask;
	}

	if(allBusCs == sameBusCs)
	{
		*csMask = (*csMask | (~(1<<effectiveCs))) & CS_BIT_MASK;
	}
	return GT_OK;
}

/*****************************************************************************
Dynamic write leveling
******************************************************************************/
GT_STATUS    ddr3TipDynamicWriteLeveling(GT_U32    devNum)
{
    GT_U32   regData = 0, iter, interfaceId, busCnt, triggerRegAddr;
	GT_U32   csEnableRegVal[MAX_INTERFACE_NUM] = {0};
    GT_U32   csMask[MAX_INTERFACE_NUM];
    GT_U32   readDataSampleDelayVals[MAX_INTERFACE_NUM] = {0};
    GT_U32   readDataReadyDelayVals[MAX_INTERFACE_NUM] = {0};
    GT_U32   resValues[MAX_INTERFACE_NUM * MAX_BUS_NUM] = {0}; /* 0 for failure */
    GT_U32   testRes = 0; /* 0 - success for all pup */
    GT_U32   dataRead[MAX_INTERFACE_NUM];
	GT_U8   WLValues[NUM_OF_CS][MAX_BUS_NUM][MAX_INTERFACE_NUM];
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U32 csMask0[MAX_INTERFACE_NUM]={0};
	GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

		trainingResult[trainingStage][interfaceId] = TEST_SUCCESS;

        /* save Read Data Sample Delay */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_SAMPLE_DELAY, readDataSampleDelayVals, MASK_ALL_BITS));
        /* save Read Data Ready Delay */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_READY_DELAY, readDataReadyDelayVals, MASK_ALL_BITS));
        /* save current cs reg val */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal, MASK_ALL_BITS));

		if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) < MV_TIP_REV_3)
		{
		    /* enable multi cs */
		    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, 0, (1 << 3)));
		}
    }

	/************************************************************************/
	/*     Phase 1: DRAM 2 Write Leveling mode                              */
	/************************************************************************/
	/*Assert 10 refresh commands to DRAM to all CS */
		for(iter=0; iter<WL_ITERATION_NUM; iter++)
		{
			for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
			{
				VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
				CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, SDRAM_OPERATION_REG, (GT_U32)((~(0xF) << 8) | 0x2), 0xf1f));
			}
		}
		/* check controller back to normal */
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0, 0x1F, SDRAM_OPERATION_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			{
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WL: DDR3 poll failed(3)"));
			}
		}

	for(effective_cs = 0; effective_cs < max_cs; effective_cs++)
	{
		/*enable write leveling to all cs  - Q off , WL n*/
		CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask0, MRS1_CMD, 0x1000, 0x1080));		/* calculate interface cs mask */

		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			/* cs enable is active low */

			ddr3TipCalcCsMask(devNum, interfaceId, effective_cs, &csMask[interfaceId]);
		}

		if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)
		{
			/*Enable Output buffer to relevant CS - Q on , WL on*/
			CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask, MRS1_CMD, 0x80, 0x1080));

			/*enable odt for relevant CS*/
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x1498, (0x3<<(effective_cs*2)) , 0xf));
		}
		else {
			CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask, MRS1_CMD, 0xC0, 0x12C4)); /*FIXME should be same as _CPU case*/
		}

		/************************************************************************/
		/*     Phase 2: Set training IP to write leveling mode                  */
		/************************************************************************/
		CHECK_STATUS(ddr3TipDynamicWriteLevelingSeq(devNum));

		/************************************************************************/
		/*     Phase 3: Trigger training                                        */
		/************************************************************************/
		triggerRegAddr = (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) < MV_TIP_REV_3)?(ODPG_TRAINING_STATUS_REG):(ODPG_TRAINING_TRIGGER_REG);
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, triggerRegAddr, 0x1, 0x1));


	if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)
	{
	   for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

			/* training done */
			if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId,  (1 << 1), (1 << 1), ODPG_TRAINING_STATUS_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			{
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WL: DDR3 poll (4) failed (Data: 0x%x)\n", regData));
			}
			else
			{
                #if defined(CONFIG_ARMADA_38X) /* JIRA #1498 for 16 bit with ECC */
                if(topologyMap->activeBusMask == 0xB)
                {
                    break;
                }
                #endif
				CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_TRAINING_TRIGGER_REG, dataRead, (1 << 2)));
				if (dataRead[interfaceId] != 0)
				{
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WL 1: WL failed IF %d regData=0x%x\n",interfaceId,dataRead[interfaceId]));
				}
			}
		}
	}
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			/* training done */
			if (ddr3TipIfPolling(devNum, ACCESS_TYPE_UNICAST, interfaceId,  (1 << 1), (1 << 1), ODPG_TRAINING_STATUS_REG, MAX_POLLING_ITERATIONS) != GT_OK)
			{
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WL: DDR3 poll (4) failed (Data: 0x%x)\n", regData));
			}
			else
			{
				CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_TRAINING_STATUS_REG, dataRead, (1 << 2)));
				regData = dataRead[interfaceId];
                #if defined(CONFIG_ARMADA_38X) /* JIRA #1498 for 16 bit with ECC */
                if(topologyMap->activeBusMask == 0xB)
                {
                    /* set data to 0 in order to skip the check */
                    regData = 0;
                }
                #endif
				if (regData != 0)
				{
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WL 2:  WL failed IF %d regData=0x%x\n",interfaceId,regData));
				}

				/* check for training completion per bus */
				for (busCnt=0; busCnt<octetsPerInterfaceNum; busCnt++)
				{
       				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)
					/* training status */
					CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsPupRegMap[busCnt], dataRead, MASK_ALL_BITS));
    				regData = dataRead[interfaceId];
					DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL: IF %d BUS %d reg 0x%x\n", interfaceId, busCnt,regData));
					if((regData & (1 << 25)) == 0 )
					{
						resValues[(interfaceId * octetsPerInterfaceNum) + busCnt] = GT_TRUE;
					}
					CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsPupRegMap[busCnt], dataRead, 0xff));
					WLValues[effective_cs][busCnt][interfaceId] = (GT_U8)dataRead[interfaceId]; /* save the read value that should be write to PHY register */
				}
			}
		}

		/************************************************************************/
		/*     Phase 3.5: Validate result phase                                 */
		/************************************************************************/
		for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			for (busCnt=0; busCnt<octetsPerInterfaceNum; busCnt++){

				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)
				/*read result control register according to pup */
				regData = WLValues[effective_cs][busCnt][interfaceId] + 16 ;/*16 is half a phase*/
				/* write into write leveling register ([4:0] ADLL, [8:6] Phase, [15:10] (centralization) ADLL + 0x10) */
				regData = (regData & 0x1f) | (((regData & 0xE0) >> 5) << 6) | (((regData & 0x1f) + PhyReg1Val) << 10);
				mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA,
									WL_PHY_REG + 0*CS_REGISTER_ADDR_OFFSET, regData);/*we always work with CS0 so the search is with WL-CS0 register in the phy*/

				/*Check if data read from DRAM not changed, if so - fix the result*/
				CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, TRAINING_WRITE_LEVELING_REG,
									dataRead, MASK_ALL_BITS));
				if((dataRead[interfaceId]&(1<<(20+busCnt)))>>(20+busCnt) == 0)
				{
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("WLValues was changed from 0x%X", WLValues[effective_cs][busCnt][interfaceId]));
					WLValues[effective_cs][busCnt][interfaceId] = WLValues[effective_cs][busCnt][interfaceId] + 32;
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR, ("to 0x%X\n", WLValues[effective_cs][busCnt][interfaceId]));
				}
			}
		}

		/************************************************************************/
		/*     Phase 4: Exit write leveling mode                                */
		/************************************************************************/
		/* disable DQs toggling */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  WR_LEVELING_DQS_PATTERN_REG, 0x0, 0x1));

		/* Update MRS 1 (WL off) */
		if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)
		{
			CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask0, MRS1_CMD , 0x1000, 0x1080));
		}
		else {
			CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask0, MRS1_CMD, 0x1000, 0x12C4)); /*FIXME should be same as _CPU case*/
		}

		/* Update MRS 1 (return to functional mode - Q on , WL off) */
		CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask0, MRS1_CMD, 0x0, 0x1080));

		/* set phy to normal mode */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x5,0x7));

		/* exit sw override mode  */
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x4, 0x7));
	}
    /************************************************************************/
    /*     Phase 5: Load WL values to each PHY                              */
    /************************************************************************/
	for(effective_cs = 0; effective_cs < max_cs; effective_cs++){
		for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			testRes = 0;
			for (busCnt=0; busCnt<octetsPerInterfaceNum; busCnt++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)
				/* check if result == pass */
				if (resValues[(interfaceId * octetsPerInterfaceNum) + busCnt] == 0)
				{
					/*read result control register according to pup */
					regData = WLValues[effective_cs][busCnt][interfaceId];
					/* write into write leveling register ([4:0] ADLL, [8:6] Phase, [15:10] (centralization) ADLL + 0x10) */
					regData = (regData & 0x1f) | (((regData & 0xE0) >> 5) << 6) | (((regData & 0x1f) + PhyReg1Val) << 10);
					mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, regData);
				}
				else
				{
					testRes = 1;
					/*read result control register according to pup */
					CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, maskResultsPupRegMap[busCnt], dataRead, 0xFF));
					regData = dataRead[interfaceId];
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR,  ("WL: IF %d BUS %d failed, reg 0x%x\n", interfaceId, busCnt,regData));
				}
			}
			if (testRes != 0)
			{
				trainingResult[trainingStage][interfaceId] = TEST_FAILED;
			}
		}
	}
	effective_cs = 0;/*Set to 0 after each loop to avoid illegal value may be used*/

	/* Copy the result from the effective CS search to the real Functional CS */
    /* ddr3TipWriteCsResult(devNum, WL_PHY_REG); */
    /* restore saved values */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* restore Read Data Sample Delay*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_SAMPLE_DELAY, readDataSampleDelayVals[interfaceId], MASK_ALL_BITS));

        /* restore Read Data Ready Delay*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_READY_DELAY, readDataReadyDelayVals[interfaceId], MASK_ALL_BITS));

        /* enable multi cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal[interfaceId], MASK_ALL_BITS));
    }

	if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
		/*disable modt0 for CS0 training - need to adjust for multy CS*/
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x1498, 0x0 , 0xf));
	}
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (trainingResult[trainingStage][interfaceId] == TEST_FAILED) 
			return GT_FAIL;
	}
    return GT_OK;
}


/*****************************************************************************
Dynamic write leveling supplementary
******************************************************************************/
GT_STATUS    ddr3TipDynamicWriteLevelingSupp
(
	GT_U32 devNum
)
{
    GT_32 adllOffset;
    GT_U32 interfaceId, busId, data, dataTmp;
    GT_BOOL isIfFail = GT_FALSE;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        isIfFail = GT_FALSE;
        for(busId = 0; busId < octetsPerInterfaceNum; busId++)
        {
       		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
            writeSuppResultTable[interfaceId][busId].isPupFail = GT_TRUE;
            CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, &data));
            DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: adllOffset=0 data delay = %d \n", data));
            if (ddr3TipWlSuppAlignPhaseShift(devNum, interfaceId, busId) == GT_OK)
            {
                DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: IF %d busId %d adllOffset=0 Success !\n", interfaceId, busId));
                continue;
            }
            /* change adll */
            adllOffset = 5;
            CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, data + adllOffset));
            CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, &dataTmp));
            DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: adllOffset= %d data delay = %d \n", adllOffset, dataTmp));

            if (ddr3TipWlSuppAlignPhaseShift(devNum, interfaceId, busId) == GT_OK)
            {
                DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: IF %d busId %d adllOffset= %d Success !\n", interfaceId, busId, adllOffset));
                continue;
            }
            /* change adll */
            adllOffset = -5;
            CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, data + adllOffset));
            CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, &dataTmp));
            DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: adllOffset= %d data delay = %d \n", adllOffset, dataTmp));
            if (ddr3TipWlSuppAlignPhaseShift(devNum, interfaceId, busId) == GT_OK)
            {
                DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("WL Supp: IF %d busId %d adllOffset= %d Success !\n", interfaceId, busId, adllOffset));
                continue;
            }
            else
            {
                DEBUG_LEVELING(DEBUG_LEVEL_ERROR,  ("WL Supp: IF %d busId %d Failed !\n", interfaceId, busId));
                isIfFail = GT_TRUE;
            }
        }

        if (isIfFail == GT_TRUE)
        {
            DEBUG_LEVELING(DEBUG_LEVEL_ERROR,  ("WL Supp CS# %d: IF %d failed\n", effective_cs ,interfaceId));
            trainingResult[trainingStage][interfaceId] = TEST_FAILED;
        }
        else
        {
            trainingResult[trainingStage][interfaceId] = TEST_SUCCESS;
        }

    }
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		if (trainingResult[trainingStage][interfaceId] == TEST_FAILED)
			return GT_FAIL;
	}
    return GT_OK;
}

/*****************************************************************************
Phase Shift
******************************************************************************/
static GT_STATUS    ddr3TipWlSuppAlignPhaseShift
(
	GT_U32 devNum,
    GT_U32 interfaceId,
    GT_U32 busId
)
{
	GT_U32 originalPhase;
    GT_U32 data, writeData;

    writeSuppResultTable[interfaceId][busId].stage = PHASE_SHIFT;
	if( GT_OK == ddr3TipXsbCompareTest(devNum, interfaceId, busId, 0))
		return GT_OK;

	/*Read the current phase */
    CHECK_STATUS(mvHwsDdr3TipBUSRead(devNum, interfaceId,  ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, &data));
	originalPhase = data >> 6 & 0x7;

	/*--- set phase(0x0[6-8]) -2  ---*/
	if( originalPhase >= 1){
		if( originalPhase == 1) writeData = (data & ~0x1DF);
		else writeData = (data & ~0x1C0) | ( (originalPhase - 2) << 6);
		mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, writeData);

		CHECK_STATUS(ddr3TipXsbCompareTest(devNum, interfaceId, busId, -2 ))
	}

	/*--- set phase(0x0[6-8]) +2  ---*/
	if( originalPhase <= 5){
		writeData = (data & ~0x1C0) | ( (originalPhase + 2) << 6);
		mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, writeData);

		CHECK_STATUS(ddr3TipXsbCompareTest(devNum, interfaceId, busId, 2))
	}

	/*--- set phase(0x0[6-8]) +4  ---*/
	if( originalPhase <= 3){
		writeData = (data & ~0x1C0) | ( (originalPhase + 4) << 6);
		mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, writeData);

		CHECK_STATUS(ddr3TipXsbCompareTest(devNum, interfaceId, busId, 4))
	}

	/*--- set phase(0x0[6-8]) +6  ---*/
	if( originalPhase <= 1){
		writeData = (data & ~0x1C0) | ( (originalPhase + 6) << 6);
		mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, writeData);

		CHECK_STATUS(ddr3TipXsbCompareTest(devNum, interfaceId, busId, 6))
	}

	/*Nothing sucess, go ahead*/

	/*Write original WL result back*/
	mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, WL_PHY_REG + effective_cs*CS_REGISTER_ADDR_OFFSET, data);
	writeSuppResultTable[interfaceId][busId].isPupFail = GT_TRUE;
	return GT_FAIL;
}

/*****************************************************************************
Compare Test
******************************************************************************/
static GT_STATUS    ddr3TipXsbCompareTest
(
    GT_U32 devNum,
    GT_U32 interfaceId,
    GT_U32 busId,
    GT_8 edgeOffset
)
{
    GT_U32 numOfSuccByteCompare, wordInPattern;
    GT_U32 wordOffset,i,numOfWordMult;
    GT_U32 readPattern[TEST_PATTERN_LENGTH*2];
	PatternInfo *patternTable = ddr3TipGetPatternTable();
	GT_U32 patternTestPatternTable[8];

    numOfWordMult = (topologyMap->activeBusMask == 3 /*INTERFACE_BUS_MASK_16BIT*/) ? 1:2; 

    	for(i = 0; i < 8; i++) {
		patternTestPatternTable[i] = patternTableGetWord(devNum, PATTERN_TEST, (GT_U8)i);
	}


    /* extern write, than read and compare */
	CHECK_STATUS(ddr3TipLoadPatternToMem(devNum, PATTERN_TEST));	

    CHECK_STATUS(ddr3TipResetFifoPtr(devNum));

    CHECK_STATUS(ddr3TipExtRead(devNum, interfaceId, ((patternTable[PATTERN_TEST].startAddr<<3) + ((SDRAM_CS_SIZE + 1)  * effective_cs)), 1, readPattern));

    DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("XsbCompt CS#%d: IF %d busId %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",effective_cs,
										interfaceId, busId, readPattern[0],readPattern[1],readPattern[2],readPattern[3],
										readPattern[4],readPattern[5],readPattern[6],readPattern[7]));
    /* compare byte per pup */
    numOfSuccByteCompare = 0;
    for(wordInPattern = startXsbOffset; wordInPattern < (TEST_PATTERN_LENGTH*numOfWordMult) ; wordInPattern++)
    {
        wordOffset = wordInPattern;
        if (wordOffset > (TEST_PATTERN_LENGTH*2 - 1))
            continue;
        if ((readPattern[wordInPattern] & pupMaskTable[busId]) == (patternTestPatternTable[wordOffset] & pupMaskTable[busId]))
        {
            /*DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("ddr3TipDynamicWriteLevelingSupp equal to Ofsset pattern !! \n"));*/
            numOfSuccByteCompare++;
        }
    }
    if (numOfSuccByteCompare == ((TEST_PATTERN_LENGTH*numOfWordMult) - startXsbOffset))
    {
		writeSuppResultTable[interfaceId][busId].stage = edgeOffset;
		DEBUG_LEVELING(DEBUG_LEVEL_TRACE,("supplementary: shift to %d for if %d pup %d success\n",
											edgeOffset, interfaceId, busId));
		writeSuppResultTable[interfaceId][busId].isPupFail = GT_FALSE;
        return GT_OK;
    }
    else
    {
        DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("XsbCompt CS# %d: IF %d busId %d numOfSuccByteCompare %d - Fail ! \n",effective_cs, 
											interfaceId, busId, numOfSuccByteCompare));
        DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("XsbCompt: expected 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", 
											patternTestPatternTable[0],patternTestPatternTable[1],
                                            patternTestPatternTable[2],patternTestPatternTable[3],patternTestPatternTable[4],
											patternTestPatternTable[5],patternTestPatternTable[6],patternTestPatternTable[7]));
        DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("XsbCompt: received 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", 
											readPattern[0],readPattern[1],readPattern[2],readPattern[3],
											readPattern[4],readPattern[5],readPattern[6],readPattern[7]));
        DEBUG_LEVELING(DEBUG_LEVEL_TRACE,  ("XsbCompt CS# %d: IF %d busId %d numOfSuccByteCompare %d - Fail ! \n", effective_cs,
											interfaceId, busId, numOfSuccByteCompare));
        return GT_FAIL;
    }
}

/*****************************************************************************
Dynamic write leveling sequence
******************************************************************************/
static GT_STATUS    ddr3TipDynamicWriteLevelingSeq(GT_U32 devNum)
{
    GT_U32 busId, dqId;
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  TRAINING_SW_2_REG, 0x1,      0x5));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  TRAINING_WRITE_LEVELING_REG,  0x50,     0xFF));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  TRAINING_WRITE_LEVELING_REG,  0x5C,     0xFF));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_TRAINING_CONTROL_REG,  0x381B82, 0x3C3FAF));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_OBJ1_OPCODE_REG, (0x3 << 25), (0x3ffff << 9)));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_OBJ1_ITER_CNT_REG,  0x80,     0xffff));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  ODPG_WRITE_LEVELING_DONE_CNTR_REG,  0x14,     0xff));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  TRAINING_WRITE_LEVELING_REG,  0xFF5C,    0xFFFF));

    /* mask PBS */
    for (dqId=0; dqId<MAX_DQ_NUM; dqId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[dqId], 0x1<<24, 0x1<<24));
    }

	/*Mask all results*/
    for (busId=0; busId < octetsPerInterfaceNum; busId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  maskResultsPupRegMap[busId], 0x1<<24, 0x1<<24));
    }

	/*Unmask only wanted*/
    for (busId=0; busId < octetsPerInterfaceNum; busId++)
    {
       	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  maskResultsPupRegMap[busId], 0, 0x1<<24));
    }

    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  WR_LEVELING_DQS_PATTERN_REG, 0x1, 0x1));

    return GT_OK;
}


/*****************************************************************************
Dynamic read leveling sequence
******************************************************************************/
static GT_STATUS    ddr3TipDynamicReadLevelingSeq( GT_U32            devNum) 
{
    GT_U32 busId, dqId;
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* mask PBS */
    for (dqId=0; dqId<MAX_DQ_NUM; dqId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[dqId], 0x1<<24, 0x1<<24));
    }

	/*Mask all results*/
    for (busId=0; busId < octetsPerInterfaceNum; busId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  maskResultsPupRegMap[busId], 0x1<<24, 0x1<<24));
    }

	/*Unmask only wanted*/
    for (busId=0; busId<octetsPerInterfaceNum; busId++)
    {
       	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  maskResultsPupRegMap[busId], 0, 0x1<<24));
    }
    
    return GT_OK;
}

/*****************************************************************************
Dynamic read leveling sequence
******************************************************************************/
static GT_STATUS    ddr3TipDynamicPerBitReadLevelingSeq( GT_U32            devNum)
{
    GT_U32 busId, dqId;
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* mask PBS */
    for (dqId=0; dqId<MAX_DQ_NUM; dqId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[dqId], 0x1<<24, 0x1<<24));
    }

	/*Mask all results*/
    for (busId=0; busId < octetsPerInterfaceNum; busId++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,  maskResultsPupRegMap[busId], 0x1<<24, 0x1<<24));
    }

	/*Unmask only wanted*/
     for (dqId=0; dqId<MAX_DQ_NUM; dqId++)
    {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, dqId/8)
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[dqId], 0x0<<24, 0x1<<24));
    }

    return GT_OK;
}

/*****************************************************************************
Print write leveling supplementary Results
******************************************************************************/
GT_BOOL ddr3TipPrintWLSuppResult(GT_U32 devNum)
{
    GT_U32 busId = 0,interfaceId = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    DEBUG_LEVELING(DEBUG_LEVEL_INFO,("I/F0 PUP0 Result[0 - success, 1-fail] ...\n"));

    devNum = devNum; /* avoid warnings */

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
         for(busId=0; busId<octetsPerInterfaceNum; busId++)
        {
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
            DEBUG_LEVELING(DEBUG_LEVEL_INFO,("%d ,", writeSuppResultTable[interfaceId][busId].isPupFail));
        }
    }
	DEBUG_LEVELING(DEBUG_LEVEL_INFO,("I/F0 PUP0 Stage[0-phase_shift, 1-clock_shift, 2-align_shift] ...\n"));
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
         for(busId=0; busId<octetsPerInterfaceNum; busId++)
        {
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
            DEBUG_LEVELING(DEBUG_LEVEL_INFO,("%d ,", writeSuppResultTable[interfaceId][busId].stage));
        }
    }
    return GT_OK;
}

