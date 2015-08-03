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
* ddr3TipDdr3TrainingPbs.c
*
* DESCRIPTION: DDR3 IF for galtis commands
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 48 $
******************************************************************************/

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3LoggingDef.h"


extern GT_U8  debugPbs;
extern GT_U32 effective_cs;

#define TYPICAL_PBS_VALUE   (12)

typedef enum
{
	PBS_TX_MODE = 0,
	PBS_RX_MODE,
	NUM_OF_PBS_MODES
}PBS_DIRECTION;

/************************** globals ***************************************/

GT_U32 nominalAdll[MAX_INTERFACE_NUM*MAX_BUS_NUM];
MV_HWS_TrainingIpStatus trainStatus[MAX_INTERFACE_NUM];
GT_U8 Result_MAT[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
GT_U8 Result_MAT_RX_DQS[MAX_INTERFACE_NUM][MAX_BUS_NUM][MAX_CS_NUM];
GT_U8 PupState[MAX_INTERFACE_NUM][MAX_BUS_NUM];/*4-EEWA,3-EWA,2-SWA,1-Fail,0-Pass*/
GT_U8 ResultAllBit[MAX_BUS_NUM*BUS_WIDTH_IN_BITS*MAX_INTERFACE_NUM];
GT_U8 MaxPBSPerPup[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8 MinPBSPerPup[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8 MaxADLLPerPup[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8 MinADLLPerPup[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8 pbsDelayPerPup[NUM_OF_PBS_MODES][MAX_INTERFACE_NUM][MAX_BUS_NUM][MAX_CS_NUM];
GT_U8 ADLL_SHIFT_Lock[MAX_INTERFACE_NUM][MAX_BUS_NUM] ;
GT_U8 ADLL_SHIFT_val[MAX_INTERFACE_NUM][MAX_BUS_NUM] ;
MV_HWS_PATTERN pbsPattern = PATTERN_VREF;

extern GT_U32 mvHwsDdr3TipMaxCSGet(GT_U32 devNum);
/************************** pre declarations ******************************/
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 maskResultsDqRegMap[];
extern MV_HWS_DDR_FREQ mediumFreq;
extern GT_U32 freqVal[];
extern MV_HWS_RESULT trainingResult[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM];
extern AUTO_TUNE_STAGE trainingStage;
extern GT_U32 debugMode;
extern GT_U32* dqMapTable;

/*****************************************************************************
Print PBS Result
******************************************************************************/
GT_STATUS    ddr3TipPrintPbsResult
(
    GT_U32		devNum,
    GT_U32		csNum,
    PBS_DIRECTION	pbsMode
);

/*****************************************************************************
Fixup PBS Result
******************************************************************************/
GT_STATUS    ddr3TipCleanPbsResult
(
    GT_U32                  devNum,
    PBS_DIRECTION			pbsMode
);

/******************************************************************************
* Name:     ddr3TipPbs
* Desc:     PBS
* Args:     TBD
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPbs
(
   GT_U32            devNum,
   PBS_DIRECTION     pbsMode
)
{
    GT_U32 Result0[MAX_INTERFACE_NUM] ;
    int ADLLTap = MEGA / freqVal[mediumFreq] / 64;
    int PadNum = 0;
    MV_HWS_SearchDirection SearchDirection = (pbsMode == PBS_RX_MODE) ? MV_HWS_High2Low : MV_HWS_Low2High;
    MV_HWS_DIRECTION Direction = (pbsMode == PBS_RX_MODE) ? OPER_READ:OPER_WRITE;
    int NumberOfIterations = (pbsMode == PBS_RX_MODE) ? 31:63;
    GT_U32 ResultValidationMask = (pbsMode == PBS_RX_MODE) ? 0x1F : 0x3f;
    int InitValue = (SearchDirection == MV_HWS_Low2High)?0:NumberOfIterations;
    MV_HWS_EdgeCompare SearchEDGE_ = EDGE_FP;
    GT_U32 pup = 0, bit = 0, interfaceId = 0, AllLock = 0 , csNum = 0;
    GT_32 regAddr = 0;
    GT_U32 validationVal = 0;
    GT_U32   csEnableRegVal[MAX_INTERFACE_NUM];
    GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
    GT_U8 temp = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* save current cs enable reg val */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* save current cs enable reg val */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal, MASK_ALL_BITS));
        /* enable single cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
    }

    regAddr = (pbsMode == PBS_RX_MODE ) ? (READ_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET)) : (WRITE_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET));
    readAdllValue(nominalAdll, regAddr , MASK_ALL_BITS );
    /* stage 1 shift ADLL */
    /*ddr3TipIpTrainingPerbitMultiCast( SearchDirection, Direction, InitValue, NumberOfIterations, pbsPattern,  SearchEDGE_ , MV_HWS_ControlElement_ADLL);*/

    ddr3TipIpTraining(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, RESULT_PER_BIT, MV_HWS_ControlElement_ADLL,
                      SearchDirection, Direction, topologyMap->interfaceActiveMask, InitValue, NumberOfIterations, pbsPattern, SearchEDGE_, CS_SINGLE, csNum, trainStatus);

    validationVal = (pbsMode == PBS_RX_MODE) ? 0x1f : 0;
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
		for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
		{
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
    		MinADLLPerPup[interfaceId][pup] = (pbsMode == PBS_RX_MODE) ? 0x1f : 0x3f;
            PupState[interfaceId][pup] = 0x3;
			ADLL_SHIFT_Lock[interfaceId][pup] = 1;
		    MaxADLLPerPup[interfaceId][pup] = 0x0;
        }
    }

    /* EBA */
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
		for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
		{
            CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[bit+pup*BUS_WIDTH_IN_BITS ], Result0, MASK_ALL_BITS));

			for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
			{
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
                 DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("FP I/F %d, bit:%d, pup:%d Result0 0x%x\n",interfaceId,bit,pup,Result0[interfaceId]));
				if(PupState[interfaceId][pup] != 3) { continue;} /* if not EBA state than move to next pup*/

				if((Result0[interfaceId] & 0x2000000) == 0 )
				{
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("-- Fail Training IP\n"));
					PupState[interfaceId][pup] = 1; /* training machine failed */
	     		    ADLL_SHIFT_Lock[interfaceId][pup] = 0;

					continue;
				}
                else if((Result0[interfaceId] & ResultValidationMask) == validationVal)
				{
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("-- FAIL EBA\n",interfaceId,bit,pup,Result0[interfaceId]));
					PupState[interfaceId][pup] = 4; /* this pup move to EEBA */
	     		    ADLL_SHIFT_Lock[interfaceId][pup] = 0;
					continue;
				}
                else
				{
					Result0[interfaceId]= (pbsMode == PBS_RX_MODE) ? ((Result0[interfaceId] & ResultValidationMask) + 1) :
																	 ((Result0[interfaceId] & ResultValidationMask) - 1);/* The search ended in Pass we need Fail */
					MaxADLLPerPup[interfaceId][pup]  = (MaxADLLPerPup[interfaceId][pup] < Result0[interfaceId]) ? (GT_U8)Result0[interfaceId] : MaxADLLPerPup[interfaceId][pup];
					MinADLLPerPup[interfaceId][pup]  = (Result0[interfaceId] > MinADLLPerPup[interfaceId][pup]) ? MinADLLPerPup[interfaceId][pup] : (GT_U8)Result0[interfaceId];
					ADLL_SHIFT_val[interfaceId][pup] = (pbsMode == PBS_RX_MODE) ? MaxADLLPerPup[interfaceId][pup] : MinADLLPerPup[interfaceId][pup];/*vs the Rx we are searching for the smallest value of DQ shift so all Bus would fail*/
				}
			}
		}
	}

	/* EEBA */
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
		{
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			if(PupState[interfaceId][pup] != 4) { continue;}/* if pup state different from EEBA than move to next pup */

            regAddr = (pbsMode == PBS_RX_MODE) ? (0x54 + effective_cs * 0x10) : (0x14 + effective_cs * 0x10);
            CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x1F));
            regAddr = (pbsMode == PBS_RX_MODE) ? (0x55 + effective_cs * 0x10) : (0x15 + effective_cs * 0x10);
            CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x1F));
		    ADLL_SHIFT_val[interfaceId][pup]=0;/* initilaze the Edge2 Max. */
		    MinADLLPerPup[interfaceId][pup] = (pbsMode == PBS_RX_MODE) ? 0x1F : 0x3f;
		    MaxADLLPerPup[interfaceId][pup] = 0x0;
		    /*ddr3TipIpTrainingPerbitMultiCast( SearchDirection, Direction, InitValue, NumberOfIterations, pbsPattern,  SearchEDGE_ , MV_HWS_ControlElement_ADLL);*/
            ddr3TipIpTraining(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, RESULT_PER_BIT, MV_HWS_ControlElement_ADLL,
                              SearchDirection, Direction, topologyMap->interfaceActiveMask, InitValue, NumberOfIterations, pbsPattern, SearchEDGE_, CS_SINGLE, csNum, trainStatus);

            DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("ADLL shift results:\n"));
		    for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
		    {
                CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[bit+pup*BUS_WIDTH_IN_BITS], Result0, MASK_ALL_BITS));

                DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("FP I/F %d, bit:%d, pup:%d Result0 0x%x\n",interfaceId,bit,pup, Result0[interfaceId]));

				if((Result0[interfaceId]&0x2000000) == 0)
				{
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,(" -- EEBA Fail\n"));
					bit = BUS_WIDTH_IN_BITS;/* exit bit loop */
					DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("-- EEBA Fail Training IP\n"));
					PupState[interfaceId][pup] = 2; /* training machine failed but pass before in the EBA so maybe the DQS shift change env. */
	     		    ADLL_SHIFT_Lock[interfaceId][pup] = 0;
                    regAddr = (pbsMode == PBS_RX_MODE) ? (0x54 + effective_cs * 0x10) : (0x14 + effective_cs * 0x10);
                    CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x0));
                    regAddr = (pbsMode == PBS_RX_MODE) ? (0x55 + effective_cs * 0x10) : (0x15 + effective_cs * 0x10);
                    CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x0));

					continue;
				} else if((Result0[interfaceId] & ResultValidationMask) == validationVal)
				{
					bit = BUS_WIDTH_IN_BITS;/* exit bit loop */
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("-- FAIL EEBA\n"));
					PupState[interfaceId][pup] = 2; /* this pup move to SBA */
	     		    ADLL_SHIFT_Lock[interfaceId][pup] = 0;
                    regAddr = (pbsMode == PBS_RX_MODE) ? (0x54 + effective_cs * 0x10) : (0x14 + effective_cs * 0x10);
                    CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x0));
                    regAddr = (pbsMode == PBS_RX_MODE) ? (0x55 + effective_cs * 0x10) : (0x15 + effective_cs * 0x10);
                    CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0x0));
					continue;
				}
				else
				{
					ADLL_SHIFT_Lock[interfaceId][pup] = 1;
					Result0[interfaceId]= (pbsMode == PBS_RX_MODE) ? ((Result0[interfaceId] & ResultValidationMask) + 1) :
																	 ((Result0[interfaceId] & ResultValidationMask) - 1);/* The search ended in Pass we need Fail */
					MaxADLLPerPup[interfaceId][pup]  = (MaxADLLPerPup[interfaceId][pup] <Result0[interfaceId]) ? (GT_U8)Result0[interfaceId] : MaxADLLPerPup[interfaceId][pup];
					MinADLLPerPup[interfaceId][pup]  = (Result0[interfaceId] > MinADLLPerPup[interfaceId][pup]) ? MinADLLPerPup[interfaceId][pup] : (GT_U8)Result0[interfaceId];
					ADLL_SHIFT_val[interfaceId][pup] = (pbsMode == PBS_RX_MODE) ? MaxADLLPerPup[interfaceId][pup] : MinADLLPerPup[interfaceId][pup];/*vs the Rx we are searching for the smallest value of DQ shift so all Bus would fail*/
				}
		    }
        }
	}

	/* Print Stage result */
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
		{
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
            DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("FP I/F %d, ADLL Shift for EBA: pup[%d] Lock status = %d Lock Val = %d,%d\n",interfaceId,pup, ADLL_SHIFT_Lock[interfaceId][pup], MaxADLLPerPup[interfaceId][pup], MinADLLPerPup[interfaceId][pup]));
        }
	}

    DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("Update ADLL Shift of all pups:\n"));
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
		{
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		    if(ADLL_SHIFT_Lock[interfaceId][pup] != 1) { continue;}/* if pup not locked continue to next pup */
            regAddr = (pbsMode == PBS_RX_MODE) ? (0x3 + effective_cs * 4) : (0x1 + effective_cs * 4);
            CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, ADLL_SHIFT_val[interfaceId][pup]));
            DEBUG_PBS_ENGINE(DEBUG_LEVEL_TRACE,("FP I/F %d, Pup[%d] = %d \n",interfaceId, pup, ADLL_SHIFT_val[interfaceId][pup]));
        }
	}

    /* PBS EEBA&EBA */
	/* Start the Per Bit Skew search */
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
	    {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	
			MaxPBSPerPup[interfaceId][pup] = 0x0;
			MinPBSPerPup[interfaceId][pup] = 0x1F;
			for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
			{
				/* reset result for PBS */
			    ResultAllBit[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*MAX_BUS_NUM*BUS_WIDTH_IN_BITS] = 0;
            }
		}
	}

    NumberOfIterations= 31;
	SearchDirection = MV_HWS_Low2High; 
	InitValue = 0; /* !!!!! ran sh (SearchDirection == MV_HWS_Low2High)?0:NumberOfIterations; */
	/*ddr3TipIpTrainingPerbitMultiCast( SearchDirection, Direction, 0x0 , NumberOfIterations, pbsPattern,  SearchEDGE_ , MV_HWS_ControlElement_DQ_SKEW);*/
    ddr3TipIpTraining(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, RESULT_PER_BIT, MV_HWS_ControlElement_DQ_SKEW,
                      SearchDirection, Direction, topologyMap->interfaceActiveMask, InitValue, NumberOfIterations, pbsPattern, SearchEDGE_, CS_SINGLE, csNum, trainStatus);
	
 	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		    if(ADLL_SHIFT_Lock[interfaceId][pup] != 1) { continue;}/* if pup not lock continue to next pup */
		    for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
		    {
                CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[bit+pup*BUS_WIDTH_IN_BITS], Result0, MASK_ALL_BITS));
                 DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("Per Bit Skew search, FP I/F %d, bit:%d, pup:%d Result0 0x%x\n",interfaceId,bit,pup, Result0[interfaceId]));

	
				if((Result0[interfaceId]&0x2000000) == 0)
				{
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("--EBA PBS Fail - Training IP machine\n"));

					bit = BUS_WIDTH_IN_BITS;/* exit the bit loop */
					ADLL_SHIFT_Lock[interfaceId][pup] = 0;/* ADLL is no long in lock need new search */
					PupState[interfaceId][pup] = 2;/* Move to SBA */
					MaxPBSPerPup[interfaceId][pup] = 0x0;
					MinPBSPerPup[interfaceId][pup] = 0x1F;
					continue;
				}
				else
				{
					temp = (GT_U8)(Result0[interfaceId] & ResultValidationMask);
					MaxPBSPerPup[interfaceId][pup] =(temp > MaxPBSPerPup[interfaceId][pup]) ? temp : MaxPBSPerPup[interfaceId][pup];
					MinPBSPerPup[interfaceId][pup] =(temp < MinPBSPerPup[interfaceId][pup]) ? temp : MinPBSPerPup[interfaceId][pup];
					ResultAllBit[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*MAX_BUS_NUM*BUS_WIDTH_IN_BITS] = temp;
				}
			}
		}
	}

    /* Check all Pup lock */
	AllLock = 1;
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			AllLock = AllLock*ADLL_SHIFT_Lock[interfaceId][pup];
        }
	}

	/* Only if not all Pups Lock */
	if(AllLock == 0)
	{
		DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("##########ADLL shift for SBA###########\n"));
		/* ADLL shift for SBA */
		SearchDirection = (pbsMode == PBS_RX_MODE) ? MV_HWS_Low2High : MV_HWS_High2Low; 
		InitValue = (SearchDirection == MV_HWS_Low2High)?0:NumberOfIterations;
		for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
		{
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
            for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
            {
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			    if(ADLL_SHIFT_Lock[interfaceId][pup] == 1) { continue;}/*if pup lock continue to next pup */
				ADLL_SHIFT_Lock[interfaceId][pup] = 0; /*init the var altogth init before*/
                regAddr = (pbsMode == PBS_RX_MODE) ? (0x54 + effective_cs * 0x10) : (0x14 + effective_cs * 0x10);
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0));
                regAddr = (pbsMode == PBS_RX_MODE) ? (0x55 + effective_cs * 0x10) : (0x15 + effective_cs * 0x10);
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0));
                regAddr = (pbsMode == PBS_RX_MODE) ? (0x5f + effective_cs * 0x10) : (0x1f + effective_cs * 0x10);
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0));
			    ADLL_SHIFT_val[interfaceId][pup]=0;/* initilaze the Edge2 Max. */
			    MinADLLPerPup[interfaceId][pup] = 0x1F;
			    MaxADLLPerPup[interfaceId][pup] = 0x0;
			    /*ddr3TipIpTrainingPerbitMultiCast( SearchDirection, Direction, InitValue, NumberOfIterations, pbsPattern,  SearchEDGE_ , MV_HWS_ControlElement_ADLL);*/
                ddr3TipIpTraining(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, RESULT_PER_BIT, MV_HWS_ControlElement_ADLL,
                          SearchDirection, Direction, topologyMap->interfaceActiveMask, InitValue, NumberOfIterations, pbsPattern, SearchEDGE_, CS_SINGLE, csNum, trainStatus);

			    for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
			    {
                    CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[bit+pup*BUS_WIDTH_IN_BITS], Result0, MASK_ALL_BITS));
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("FP I/F %d, bit:%d, pup:%d Result0 0x%x \n",interfaceId,bit,pup, Result0[interfaceId]));

					if((Result0[interfaceId]&0x2000000) == 0)
					{
						bit = BUS_WIDTH_IN_BITS;/* exit the bit loop */
						PupState[interfaceId][pup] = 1;/* Fail SBA --> Fail PBS */
                        DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,(" SBA Fail\n"));
						continue;
					}
					else
					{
						ADLL_SHIFT_Lock[interfaceId][pup]++;/* - increment to get all 8 bit lock.*/
					    Result0[interfaceId]= (pbsMode == PBS_RX_MODE) ? ((Result0[interfaceId] & ResultValidationMask) + 1) :
																		 ((Result0[interfaceId] & ResultValidationMask) - 1);/* The search ended in Pass we need Fail */
					    MaxADLLPerPup[interfaceId][pup]  = (MaxADLLPerPup[interfaceId][pup] <Result0[interfaceId]) ? (GT_U8)Result0[interfaceId] : MaxADLLPerPup[interfaceId][pup];
					    MinADLLPerPup[interfaceId][pup]  = (Result0[interfaceId] > MinADLLPerPup[interfaceId][pup]) ? MinADLLPerPup[interfaceId][pup] : (GT_U8)Result0[interfaceId];
					    ADLL_SHIFT_val[interfaceId][pup] = (pbsMode == PBS_RX_MODE) ? MaxADLLPerPup[interfaceId][pup] : MinADLLPerPup[interfaceId][pup];/*vs the Rx we are searching for the smallest value of DQ shift so all Bus would fail*/
					}
			    }
			    ADLL_SHIFT_Lock[interfaceId][pup] = (ADLL_SHIFT_Lock[interfaceId][pup] == 8)?1:0; /* 1 is lock */

            	regAddr = (pbsMode == PBS_RX_MODE) ? (0x3 + effective_cs * 4) : (0x1 + effective_cs * 4);
            	CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, ADLL_SHIFT_val[interfaceId][pup]));

				DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("ADLL_SHIFT_Lock[%x][%x] = %x \n",interfaceId,pup, ADLL_SHIFT_Lock[interfaceId][pup]));
            }
		}

		/* End ADLL Shift for SBA */
		/* Start the Per Bit Skew search */
		SearchEDGE_ = (pbsMode == PBS_RX_MODE) ? EDGE_PF : EDGE_FP; /* The ADLL shift finished with a Pass */
		SearchDirection = (pbsMode == PBS_RX_MODE) ? MV_HWS_Low2High : MV_HWS_High2Low;
        NumberOfIterations = 0x1F;
        InitValue = (pbsMode == PBS_RX_MODE) ? 0 : NumberOfIterations;/* - The initial value is different in Rx and Tx mode*/
		/*ddr3TipIpTrainingPerbitMultiCast( SearchDirection, Direction, 0x0 , NumberOfIterations, pbsPattern,  SearchEDGE_ , MV_HWS_ControlElement_DQ_SKEW);*/

	    ddr3TipIpTraining(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, RESULT_PER_BIT, MV_HWS_ControlElement_DQ_SKEW,
                      SearchDirection, Direction, topologyMap->interfaceActiveMask, InitValue, NumberOfIterations, pbsPattern, SearchEDGE_, CS_SINGLE, csNum, trainStatus);

		for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
		{
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
            for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
            {
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

				for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
				{
	                CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, maskResultsDqRegMap[bit+pup*BUS_WIDTH_IN_BITS], Result0, MASK_ALL_BITS));
					if(PupState[interfaceId][pup] != 2) {bit = BUS_WIDTH_IN_BITS; continue;}/*if pup is not SBA continue to next pup*/
                      DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("Per Bit Skew search, PF I/F %d, bit:%d, pup:%d Result0 0x%x \n",interfaceId,bit,pup, Result0[interfaceId]));
					if((Result0[interfaceId]&0x2000000) == 0)
					{
                        DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("SBA Fail\n"));
						/* bit = BUS_WIDTH_IN_BITS;*/ /*exit the bit loop*/
/*						bit = BUS_WIDTH_IN_BITS;*/ /*exit the bit loop*/
						/*ADLL_SHIFT_Lock[interfaceId][pup] = 0;*/ /* In this stage no need to change the ADLL shift lock there is a VW > 0 */
						/*PupState[interfaceId][pup] = 1; We want to continue learn on the bits Skew*/
						MaxPBSPerPup[interfaceId][pup] = 0x1F;
						ResultAllBit[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*MAX_BUS_NUM*BUS_WIDTH_IN_BITS] = 0x1F;
					} else
					{
						temp = (GT_U8)(Result0[interfaceId] & ResultValidationMask);
						MaxPBSPerPup[interfaceId][pup] =(temp > MaxPBSPerPup[interfaceId][pup]) ? temp : MaxPBSPerPup[interfaceId][pup];
						MinPBSPerPup[interfaceId][pup] =(temp < MinPBSPerPup[interfaceId][pup]) ? temp : MinPBSPerPup[interfaceId][pup];
						ResultAllBit[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*MAX_BUS_NUM*BUS_WIDTH_IN_BITS] = temp;
						ADLL_SHIFT_Lock[interfaceId][pup] = 1;
					}
				}
			}
		}
		/* Check all Pup state */
		AllLock = 1;
		for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
		{
          /*  DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("PupState[%d][%d] = %d\n",interfaceId,pup,PupState[interfaceId][pup])); */
		}
	}
	/* END OF SBA */
    /* Norm */
	for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
	{
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
		for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
		{
            for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
            {
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		        if(ADLL_SHIFT_Lock[interfaceId][pup] != 1) /* if pup not lock continue to next pup */
                { 
                    DEBUG_PBS_ENGINE(DEBUG_LEVEL_ERROR,("PBS failed for IF #%d\n",interfaceId));
                    trainingResult[trainingStage][interfaceId] = TEST_FAILED;
                  /*  if (debugMode == GT_FALSE)
                    {
                        return GT_FAIL; ?? don't want to fail all the Algorithm due to this fail. we will let the Centralization TH to limit us
                    }*/
                    /* continue; want to zero all PBS val on this pup that didn't converge in the ADLL Shift state */
					Result_MAT[interfaceId][pup][bit] = 0;
					MaxPBSPerPup[interfaceId][pup] = 0;
					MinPBSPerPup[interfaceId][pup] = 0;
                }
                else
                {
                    trainingResult[trainingStage][interfaceId] = (trainingResult[trainingStage][interfaceId] == TEST_FAILED) ? TEST_FAILED:TEST_SUCCESS;
					Result_MAT[interfaceId][pup][bit] = ResultAllBit[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*MAX_BUS_NUM*BUS_WIDTH_IN_BITS] - MinPBSPerPup[interfaceId][pup];
                }

                DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("The abs MinPBS[%d][%d] = %d\n" ,interfaceId ,pup , MinPBSPerPup[interfaceId][pup]));
			}
		}
	}

	/*Clean all results*/
	ddr3TipCleanPbsResult(devNum, pbsMode);

    /* DQ PBS register update with the final result */
    for( interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		for( pup = 0 ; pup <  octetsPerInterfaceNum ; pup++)
		{
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
			/*if(ADLL_SHIFT_Lock[interfaceId][pup] != 1) { continue;}*/ /* if pup not lock continue to next pup */
            DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("Final Results: interfaceId %d, pup %d, Pup State: %d\n", interfaceId, pup, PupState[interfaceId][pup]));

			for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
			{
				 if (dqMapTable == NULL)
				 {
					DEBUG_PBS_ENGINE(DEBUG_LEVEL_ERROR,("dqMapTable not initializaed\n"));
					return GT_FAIL;
				 }
				PadNum = dqMapTable[bit+pup*BUS_WIDTH_IN_BITS + interfaceId*BUS_WIDTH_IN_BITS*octetsPerInterfaceNum];
				DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,("Result_MAT: %d " ,Result_MAT[interfaceId][pup][bit]));
				regAddr = (pbsMode == PBS_RX_MODE) ? (PBS_RX_PHY_REG + effective_cs * 0x10) : (PBS_TX_PHY_REG + effective_cs * 0x10);
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr+PadNum, Result_MAT[interfaceId][pup][bit]));
			}

            temp = (MaxPBSPerPup[interfaceId][pup] == MinPBSPerPup[interfaceId][pup]) ?\
                TYPICAL_PBS_VALUE :\
                ((MaxADLLPerPup[interfaceId][pup] - MinADLLPerPup[interfaceId][pup]) * (GT_U8)ADLLTap / (MaxPBSPerPup[interfaceId][pup] - MinPBSPerPup[interfaceId][pup]));

			pbsDelayPerPup[pbsMode][interfaceId][pup][effective_cs] = temp;

			if( pbsMode == PBS_TX_MODE ){ /*RX results ready, write RX also*/
				/*Write TX results*/
		        regAddr = (0x14 + effective_cs * 0x10);
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, (MaxPBSPerPup[interfaceId][pup] - MinPBSPerPup[interfaceId][pup])/2));
		        regAddr = (0x15 + effective_cs * 0x10);
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, (MaxPBSPerPup[interfaceId][pup] - MinPBSPerPup[interfaceId][pup])/2));

				/*Write previously stored RX results*/
		        regAddr = (0x54 + effective_cs * 0x10);
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, Result_MAT_RX_DQS[interfaceId][pup][effective_cs]));
		        regAddr = (0x55 + effective_cs * 0x10);
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, Result_MAT_RX_DQS[interfaceId][pup][effective_cs]));

			}
			else{ /*RX results may affect RL results correctess, so just store the results that will written in TX stage*/
				Result_MAT_RX_DQS[interfaceId][pup][effective_cs] = (MaxPBSPerPup[interfaceId][pup] - MinPBSPerPup[interfaceId][pup])/2;
			}

			DEBUG_PBS_ENGINE(DEBUG_LEVEL_INFO,(", PBS tap=%d [psec] ==> skew observed = %d\n", temp, ((MaxPBSPerPup[interfaceId][pup] - MinPBSPerPup[interfaceId][pup])*temp)));
		}
    }
    /*Write back to the phy the default values */
    regAddr = (pbsMode == PBS_RX_MODE) ? (READ_CENTRALIZATION_PHY_REG + effective_cs * 4) : (WRITE_CENTRALIZATION_PHY_REG + effective_cs * 4);
	writeAdllValue(nominalAdll, regAddr);


   for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
   {
        regAddr = (pbsMode == PBS_RX_MODE) ? (0x5A + effective_cs * 0x10) : (0x1A + effective_cs * 0x10);
        CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr, 0));

        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)        /* restore cs enable value*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal[interfaceId], MASK_ALL_BITS));
   }
   /* exit test mode */
   CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_WRITE_READ_MODE_ENABLE_REG, 0xFFFF, MASK_ALL_BITS));

   for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
   {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		for( pup = 0 ; pup <  octetsPerInterfaceNum ; pup++)
		{
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
			/* if (trainingResult[trainingStage][interfaceId] == TEST_FAILED) */
			if ( PupState[interfaceId][pup] == 1) /* meaning that there is no VW exist at all (No lock at the EBA ADLL shift at EBS) */
			return GT_FAIL;
		}
   }

   return GT_OK;


}

/******************************************************************************
* Name:     ddr3TipPbsRx.
* Desc:     PBS TX
* Args:     TBD
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPbsRx(GT_U32               uidevNum)
{
   return ddr3TipPbs(uidevNum, PBS_RX_MODE);
}

/******************************************************************************
* Name:     ddr3TipPbsTx.
* Desc:     PBS TX
* Args:     TBD
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPbsTx(GT_U32               uidevNum)
{
   return ddr3TipPbs(uidevNum, PBS_TX_MODE);
}

#ifdef DDR_VIEWER_TOOL
/*****************************************************************************
Print PBS Result
******************************************************************************/
GT_STATUS    ddr3TipPrintAllPbsResult
(
    GT_U32      devNum
)
{
    GT_U32 currCs;
    GT_U32 max_cs = mvHwsDdr3TipMaxCSGet(devNum);

   for(currCs = 0; currCs < max_cs; currCs++)
   {
	   ddr3TipPrintPbsResult(devNum, currCs,PBS_RX_MODE);
	   ddr3TipPrintPbsResult(devNum, currCs,PBS_TX_MODE);
   }
   return GT_OK;
}

/*****************************************************************************
Print PBS Result
******************************************************************************/
GT_STATUS    ddr3TipPrintPbsResult
(
    GT_U32		devNum,
    GT_U32		csNum,
    PBS_DIRECTION	pbsMode
)
{
    GT_U32 dataValue = 0, bit = 0, interfaceId = 0, pup = 0;
    GT_U32 regAddr = (pbsMode == PBS_RX_MODE) ? (PBS_RX_PHY_REG + csNum * 0x10) : (PBS_TX_PHY_REG + csNum * 0x10);
    GT_U32 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    mvPrintf("%s,CS%d,PBS,ADLLRATIO,,,", (pbsMode == PBS_RX_MODE)?"Rx":"Tx", csNum);
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for( pup=0; pup < octetsPerInterfaceNum; pup++)
        {
                mvPrintf("%d,",pbsDelayPerPup[pbsMode][interfaceId][pup][csNum]);
        }
	}
    mvPrintf("\nCS%d, %s ,PBS \n", csNum ,(pbsMode==PBS_RX_MODE)? "Rx" : "Tx");
    for( bit = 0 ; bit < BUS_WIDTH_IN_BITS ; bit++)
    {
	mvPrintf("%s, DQ",(pbsMode==PBS_RX_MODE)? "Rx" : "Tx");
	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
	    mvPrintf("%d ,PBS,,, ",bit);
            for( pup=0; pup <=octetsPerInterfaceNum; pup++)
            {
		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
                CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId, ACCESS_TYPE_UNICAST, pup,  DDR_PHY_DATA,  regAddr+bit, &dataValue));
                mvPrintf("%d , ",dataValue);
            }
	}
        mvPrintf("\n");
    }
    mvPrintf("\n");
    return GT_OK;
}

#endif

/*****************************************************************************

Fixup PBS Result
******************************************************************************/
GT_STATUS    ddr3TipCleanPbsResult
(
    GT_U32                  devNum,
    PBS_DIRECTION			pbsMode
)
{
    GT_U32 interfaceId, pup, bit;
    GT_U32 regAddr = (pbsMode == PBS_RX_MODE) ? (PBS_RX_PHY_REG + effective_cs * 0x10) : (PBS_TX_PHY_REG + effective_cs * 0x10);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for( interfaceId = 0 ; interfaceId <= MAX_INTERFACE_NUM-1 ; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for( pup=0; pup <=octetsPerInterfaceNum; pup++)
        {
			for( bit = 0 ; bit <= BUS_WIDTH_IN_BITS+3; bit++)
			{
			CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId, ACCESS_TYPE_UNICAST,  pup, DDR_PHY_DATA, regAddr+bit, 0));
			}
		}
	}
	return GT_OK;
}


