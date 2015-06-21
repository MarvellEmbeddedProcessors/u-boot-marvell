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
* mvHwsTrainingIpDdr3TrainingAdditional.c
*
* DESCRIPTION: DDR3 training IP configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
******************************************************************************/

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3TrainingIpCentralization.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingLeveling.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"


/************************** definitions ******************************/
#define VREF_INITIAL_STEP	(3)
#define VREF_SECOND_STEP	(1)
#define VREF_MAX_INDEX		(7)

#define MAX_VALUE               (1024-1)
#define MIN_VALUE               (-MAX_VALUE)
#define GET_RD_SAMPLE_DELAY(data,cs) ((data >> rdSampleMask[cs]) & 0xf)
#define GET_MAX(arg1,arg2) (arg1<arg2) ? (arg2) : (arg1);

GT_U32 caDelay;
GT_BOOL	ddr3TipCentralizationSkipMinWindowCheck = GT_FALSE;
GT_U8 currentVref[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U8 lastVref[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U16 currentValidWindow[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U16 lastValidWindow[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U8 limVref[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U8 interfaceState[MAX_INTERFACE_NUM] ;
GT_U8 pupState[MAX_BUS_NUM][MAX_INTERFACE_NUM] ;
GT_U8 vrefWindowSize[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8 vrefWindowSizeTh = 12;

extern GT_U32 ckDelay;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 csMaskReg[];
extern GT_U32 delayEnable;
extern GT_U32 startPattern, endPattern;
extern GT_U32 freqVal[DDR_FREQ_LIMIT];
extern GT_U8 debugTrainingHwAlg;
extern AUTO_TUNE_STAGE trainingStage;

/************************** globals ******************************/

static GT_U32 rdSampleMask[] =
{
    0,
    8,
    16,
    24
};

#define	VREF_STEP_1 	0
#define	VREF_STEP_2		1
#define	VREF_CONVERGE	2

/*****************************************************************************
ODT additional timing
******************************************************************************/
GT_STATUS    ddr3TipWriteAdditionalOdtSetting
(
    GT_U32                  devNum,
    GT_U32                  interfaceId
)
{
    GT_U32 csNum = 0, maxReadSample = 0, minReadSample = 0;
    GT_U32 dataRead[MAX_INTERFACE_NUM] = {0};
    GT_U32 ReadSample[MAX_CS_NUM];
    GT_U32 dataValue;
    GT_U32 pupIndex;
    GT_32 maxPhase = MIN_VALUE, currentPhase;
    MV_HWS_ACCESS_TYPE  accessType = ACCESS_TYPE_UNICAST;
    GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,accessType, interfaceId, DUNIT_ODT_CONTROL_REG, 0 << 8, 0x3 << 8));
    CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, accessType, interfaceId, READ_DATA_SAMPLE_DELAY, dataRead, MASK_ALL_BITS));
    dataValue = dataRead[interfaceId];

    for (csNum=0 ; csNum <MAX_CS_NUM ; csNum++)
    {
        ReadSample[csNum] = GET_RD_SAMPLE_DELAY(dataValue,csNum);

        /* find maximum of ReadSamples*/
        if (ReadSample[csNum] >= maxReadSample)
        {
            if (ReadSample[csNum] == maxReadSample)
            {
                maxPhase = MIN_VALUE;
            }else
			{
				maxReadSample = ReadSample[csNum];
			}
            for(pupIndex=0 ; pupIndex < octetsPerInterfaceNum; pupIndex++)
            {
                CHECK_STATUS(mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, pupIndex, DDR_PHY_DATA, RL_PHY_REG + CS_BYTE_GAP(csNum), &dataValue));
				currentPhase = ((GT_32)dataValue&0xE0)>>6;
                if ( currentPhase >= maxPhase )
                {
                    maxPhase = currentPhase;
                }
            }
        }
        /* find minimum */
        if (ReadSample[csNum] < minReadSample)
        {
            minReadSample = ReadSample[csNum];
        }
    }

    minReadSample = minReadSample-1 ;
    maxReadSample = maxReadSample + 4 + (maxPhase+1)/2 + 1;
    if (minReadSample >= 0xf)
        minReadSample = 0xf;
    if (maxReadSample >= 0x1f)
        maxReadSample = 0x1f;
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,accessType, interfaceId, ODT_TIMING_LOW, ((minReadSample-1) << 12) , 0xf << 12));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,accessType, interfaceId, ODT_TIMING_LOW, (maxReadSample << 16) , 0x1f << 16));

    return GT_OK;
}

GT_STATUS GetValidWinRx(GT_U32 devNum, GT_U32 interfaceId, GT_U8 res[4])
{
	GT_U32 RegPup = RESULT_DB_PHY_REG_ADDR;
	GT_U32 regData;
	GT_U32 Pupindex = 0, csNum;
 
	csNum = 0;/*TBD*/
	RegPup += csNum;

	for(Pupindex = 0; Pupindex < 4; Pupindex++)
	{
		CHECK_STATUS(mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, Pupindex, DDR_PHY_DATA, RegPup, &regData));

		res[Pupindex] = (regData >> RESULT_DB_PHY_REG_RX_OFFSET) & 0x1f;
	}
	return 0;
}

/* This algorithm deals with the vertical optimum from Voltage point of view of the sample signal.
   Voltage sample point can improve the Eye / window size of the bit and the pup.
   The problem is that it is tune for all DQ the same so there isn�t any �PBS� like code.
   It is more like centralization.
   But because we don�t have The training SM support we do it a bit more smart search to save time.*/
GT_STATUS ddr3TipVref
(
    GT_U32 devNum
)
{
	/*	The Vref register have non linear order. need to check what will be in futur  projects. */
	GT_U32 vrefMap[8] = {1,2,3,4,5,6,7,0};
	/*	state and parameter definitions */
	GT_U8  initialStep = VREF_INITIAL_STEP;
	GT_U8  secondStep = VREF_SECOND_STEP;/* need to be assign with minus ????? */
	GT_U32 algoRunFlag = 0, currrentVref = 0;
	GT_U32 whileCount = 0;
	GT_U32 pup = 0, interfaceId  = 0, numPup = 0;
    GT_U8  rep = 0;
	GT_U32 dataValue = 0;
	GT_U32 regAddr = 0xA8;
	GT_U32 copyStartPattern, copyEndPattern;
    MV_HWS_RESULT* flowResult = ddr3TipGetResultPtr(trainingStage);
	GT_U8 res[4];
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	CHECK_STATUS(ddr3TipSpecialRx(devNum));

	/* save start/end pattern */
	copyStartPattern = startPattern;
	copyEndPattern = endPattern;

	/* set vref as centralization pattern */
	startPattern = PATTERN_VREF;
	endPattern = PATTERN_VREF;

	/*	init params */
	for(interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
	{
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		for(pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
		{
			currentVref[pup][interfaceId] = 0;
			lastVref[pup][interfaceId] = 0;
			limVref[pup][interfaceId] = 0;
			currentValidWindow[pup][interfaceId] = 0;
			lastValidWindow[pup][interfaceId] = 0;
			if(vrefWindowSize[interfaceId][pup] > vrefWindowSizeTh){
				pupState[pup][interfaceId] = VREF_CONVERGE;
	    		DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_INFO, ("VREF config,IF[ %d ]pup[ %d ] - Vref tune not requered (%d)\n", interfaceId, pup, __LINE__));
			} else{
				pupState[pup][interfaceId] = VREF_STEP_1;
		    	CHECK_STATUS(mvHwsDdr3TipBUSRead( devNum, interfaceId,  ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
		    	CHECK_STATUS(mvHwsDdr3TipBUSWrite( devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup ,DDR_PHY_DATA,regAddr, (dataValue&(~0xF)) | vrefMap[0]));
		    	DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_INFO, ("VREF config,IF[ %d ]pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, (dataValue&(~0xF)) | vrefMap[0],__LINE__));
			}
		}
		interfaceState[interfaceId] = 0;
	}

	numPup = octetsPerInterfaceNum*MAX_INTERFACE_NUM;/*TODO set number of active interfaces*/
	while((algoRunFlag <= numPup)&(whileCount<10)  )
	{
		whileCount++;

		for(rep = 1 ;  rep < 4 ; rep++)
		{
			ddr3TipCentralizationSkipMinWindowCheck = GT_TRUE;
			ddr3TipCentralizationRx(devNum);
			ddr3TipCentralizationSkipMinWindowCheck = GT_FALSE;
			/* Read Valid window results only for non converge pups */
			for(interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
			{
        		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
				if(interfaceState[interfaceId] != 4)
				{
					GetValidWinRx(devNum, interfaceId,  res);
					for( pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
					{
       					VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
						if(pupState[pup][interfaceId] == VREF_CONVERGE)
						{
							continue;
						}
						currentValidWindow[pup][interfaceId] = (currentValidWindow[pup][interfaceId]*(rep-1) + 1000*res[pup])/rep;
					}
				}
			}
		}

		for(interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
		{
       		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("currentValidWindow: IF[ %d ] - ", interfaceId ));
			for(pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
   				DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("%d ", currentValidWindow[pup][interfaceId]));
			}
			DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("\n"));
		}
		/*	Compare results and respond as function of state */
		for(interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
		{
			VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
			for(pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
			{
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
				DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] STATE #%d (%d)\n", interfaceId, pup,
							pupState[pup][interfaceId], __LINE__));
				if(pupState[pup][interfaceId] == VREF_CONVERGE)
				{
					continue;
				}
 DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] CHECK progress - Current %d Last %d, limit VREF %d (%d)\n", interfaceId, pup,
							currentValidWindow[pup][interfaceId],lastValidWindow[pup][interfaceId], limVref[pup][interfaceId], __LINE__));
				if(currentValidWindow[pup][interfaceId] + 200 >= (lastValidWindow[pup][interfaceId]))/* The -1 is for solution resolution +/- 1 tap of ADLL */
				{
					if(pupState[pup][interfaceId] == VREF_STEP_1)
					{
						/* we stay in the same state and step just update the window size( take the max ) and Vref  */
						if(currentVref[pup][interfaceId] == VREF_MAX_INDEX)
						{
							/* if we step to the end and didn't converge to some particular better Vref value define the pup as converge and step back to nominal Vref. */
							pupState[pup][interfaceId] = VREF_CONVERGE;
							algoRunFlag++;
							interfaceState[interfaceId]++;
				        DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] VREF_CONVERGE - Vref = %X (%d)\n", interfaceId, pup, currentVref[pup][interfaceId],__LINE__));
						}else
						{
							/* continue to update the Vref index */
							currentVref[pup][interfaceId] = ((currentVref[pup][interfaceId]+initialStep) > VREF_MAX_INDEX)?VREF_MAX_INDEX:(currentVref[pup][interfaceId]+initialStep);
							if(currentVref[pup][interfaceId] == VREF_MAX_INDEX)
							{
								pupState[pup][interfaceId] = VREF_STEP_2;
							}

							limVref[pup][interfaceId] = lastVref[pup][interfaceId] = currentVref[pup][interfaceId];
						}

						lastValidWindow[pup][interfaceId] = GET_MAX(currentValidWindow[pup][interfaceId],lastValidWindow[pup][interfaceId]);
						/* update the Vref for next stage */
						currrentVref = currentVref[pup][interfaceId];
				        CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
					    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup ,DDR_PHY_DATA,regAddr, (dataValue&(~0xF)) | vrefMap[currrentVref]));
					    DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("VREF config,IF[ %d ]pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, (dataValue&(~0xF)) | vrefMap[currrentVref],__LINE__));

					}else if(pupState[pup][interfaceId] == VREF_STEP_2)
					{
						/* we keep on search back with the same step size. */
						lastValidWindow[pup][interfaceId] = GET_MAX(currentValidWindow[pup][interfaceId],lastValidWindow[pup][interfaceId]);
						lastVref[pup][interfaceId] = currentVref[pup][interfaceId];
						if((currentVref[pup][interfaceId] - secondStep)  == limVref[pup][interfaceId])/* we finish all search space */
						{/* if we step to the end and didn't converge to some particular better Vref value define the pup as converge and step back to nominal Vref.*/
							pupState[pup][interfaceId] = VREF_CONVERGE;
							algoRunFlag++;
							interfaceState[interfaceId]++;
							currentVref[pup][interfaceId] = (currentVref[pup][interfaceId]- secondStep);
				        	DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] VREF_CONVERGE - Vref = %X (%d)\n", interfaceId, pup, currentVref[pup][interfaceId],__LINE__));
						}else if(currentVref[pup][interfaceId] == limVref[pup][interfaceId])/* we finish all search space */
						{/* if we step to the end and didn't converge to some particular better Vref value define the pup as converge and step back to nominal Vref.*/
							pupState[pup][interfaceId] = VREF_CONVERGE;
							algoRunFlag++;
							interfaceState[interfaceId]++;
				        DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] VREF_CONVERGE - Vref = %X (%d)\n", interfaceId, pup, currentVref[pup][interfaceId],__LINE__));

						}else
						{
							currentVref[pup][interfaceId] = currentVref[pup][interfaceId] - secondStep;
						}

						/* update the Vref for next stage */
						currrentVref = currentVref[pup][interfaceId];
				        CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
					    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup ,DDR_PHY_DATA,regAddr, (dataValue&(~0xF)) | vrefMap[currrentVref]));
	    				DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("VREF config,IF[ %d ]pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, (dataValue&(~0xF)) | vrefMap[currrentVref],__LINE__));
					}
				}else
				{
					if(pupState[pup][interfaceId] == VREF_STEP_1)
					{/* we change state and change step */
						pupState[pup][interfaceId] = VREF_STEP_2;
						limVref[pup][interfaceId] = currentVref[pup][interfaceId] - initialStep;
						lastValidWindow[pup][interfaceId] = currentValidWindow[pup][interfaceId];
						lastVref[pup][interfaceId] = currentVref[pup][interfaceId];
						currentVref[pup][interfaceId] = lastVref[pup][interfaceId] - secondStep;
						/* update the Vref for next stage */
				        CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
					    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup ,DDR_PHY_DATA,regAddr, (dataValue&(~0xF)) | vrefMap[currentVref[pup][interfaceId]] ));
	    				DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("VREF config,IF[ %d ]pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, (dataValue&(~0xF)) | vrefMap[currentVref[pup][interfaceId]],__LINE__));
					}
					else if(pupState[pup][interfaceId] == VREF_STEP_2)
					{/* the last search was the max point set value and exit */
				        CHECK_STATUS(mvHwsDdr3TipBUSRead(   devNum, interfaceId,  ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
					    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup ,DDR_PHY_DATA,regAddr, (dataValue&(~0xF)) | vrefMap[lastVref[pup][interfaceId]] ));
	    				DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("VREF config,IF[ %d ]pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, (dataValue&(~0xF)) | vrefMap[lastVref[pup][interfaceId]],__LINE__));

						pupState[pup][interfaceId] = VREF_CONVERGE;
						algoRunFlag++;
						interfaceState[interfaceId]++;
				        DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE, ("I/F[ %d ],pup[ %d ] VREF_CONVERGE - Vref = %X (%d)\n", interfaceId, pup, currentVref[pup][interfaceId],__LINE__));
					}
				}
			}
		}
	}
	for(interfaceId = 0 ; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
   		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		for(pup = 0 ; pup < octetsPerInterfaceNum ; pup++)
		{
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
   			CHECK_STATUS(mvHwsDdr3TipBUSRead( devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, regAddr, &dataValue));
	        DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_INFO, ("FINAL values: I/F[ %d ],pup[ %d ] - Vref = %X (%d)\n", interfaceId, pup, dataValue,__LINE__));
		}
	}

	flowResult[interfaceId] = TEST_SUCCESS;

	/* restore start/end pattern */
	startPattern = copyStartPattern;
	endPattern = copyEndPattern;

	return 0;
}


/*****************************************************************************
CK/CA Delay
******************************************************************************/
GT_STATUS    ddr3TipCmdAddrInitDelay
(
	GT_U32		devNum,
	GT_U32		adllTap
)
{
	GT_U32 interfaceId = 0;
	GT_U32 ckNumADLLTap = 0, caNumADLLTap = 0, data = 0;
	/*ckDelayTable is delaying the of the Clock signal only.(to overcome timing issues betweenCK & command/address signals)*/
	/* CA_delay is delaying the of the entire command & Address signals (include Clock signal � to overcome DGL error on the Clock versus the DQS). */
	/*Calc ADLL Tap*/

	if(ckDelay == MV_PARAMS_UNDEFINED)
	{
		DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_ERROR,("ERROR: One of ckDelay values not initialized!!!\n"));
	}

	for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
	{
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		/*Calc Delay ps in ADLL tap*/
		ckNumADLLTap = ckDelay/adllTap;
		caNumADLLTap = caDelay/adllTap;

		data = (ckNumADLLTap & 0x3f) + ((caNumADLLTap & 0x3f) << 10);
		/* Set the ADLL number to the CK ADLL for Interfaces for all Pup */
		DEBUG_TRAINING_HW_ALG(DEBUG_LEVEL_TRACE,("ckNumADLLTap %d caNumADLLTap %d adllTap %d\n",ckNumADLLTap,caNumADLLTap,adllTap));
		CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0x0, data));
		/*CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0x1, caNumADLLTap));*/
	}
	return GT_OK;
}
