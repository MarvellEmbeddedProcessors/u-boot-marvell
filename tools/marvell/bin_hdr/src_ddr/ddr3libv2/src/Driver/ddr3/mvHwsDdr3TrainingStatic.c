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
* mvHwsDdr3TrainingStatic.c
*
* DESCRIPTION: DDR3 static leveling
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 58 $
******************************************************************************/


#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"

extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 clampTbl[MAX_INTERFACE_NUM];

#ifdef STATIC_ALGO_SUPPORT
/************************** definitions ******************************/

extern GT_U8  debugTrainingStatic;
extern GT_U32 firstActiveIf;

#define PARAM_NOT_CARE (0)
#define MAX_STATIC_SEQ (48)
/************************** globals ***************************************/
GT_U32 siliconDelay[HWS_MAX_DEVICE_NUM];
MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig[HWS_MAX_DEVICE_NUM];
static RegData* staticInitControllerConfig[HWS_MAX_DEVICE_NUM];

/* silicon delay (used in static leveling) */
/* M silicon delay (used in static leveling) */
GT_32 s_uiMSiliconDelay = 1450;
/* EZ silicon delay (used in static leveling) */
GT_32 s_uiEZSiliconDelay = 950;
/* debug delay in write leveling */ 
GT_32 wlDebugDelay = 0;
/* pup register #3 for functional board */
GT_32 functionRegValue = 8;
/* pup register #3 for M/Customer board */
GT_32 s_uiMRegValue = 12;
GT_U32 silicon;

GT_U32 readReadyDelayPhaseOffset[] = { 4, 4, 4, 4, 6, 6, 6, 6 };

extern ClValuePerFreq casLatencyTable[];
extern GT_U32 targetFreq;
extern MV_HWS_TIP_CONFIG_FUNC_DB configFuncInfo[HWS_MAX_DEVICE_NUM];
extern GT_U32 isPllBeforeInit, isAdllCalibBeforeInit;
extern GT_U32 clampTbl[];
extern GT_U32 initFreq;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
/* list of allowed frequency listed in order of MV_HWS_DDR_FREQ */
extern GT_U32 freqVal[];

/**************************** internal function header ************************/
GT_STATUS    ddr3TipStaticRoundTripArrBuild
(
    GT_U32              devNum,
    TripDelayElement    *tablePtr,
    GT_BOOL             isWl,
    GT_U32              *roundTripDelayArr 
);

/*****************************************************************************
Register static init controller DB
******************************************************************************/
GT_STATUS ddr3TipInitSpecificRegConfig
(
    GT_U32              devNum,
	RegData				*regConfigArr
)
{
    staticInitControllerConfig[devNum] = regConfigArr;
    return GT_OK;
}

/*****************************************************************************
Register static info DB
******************************************************************************/
GT_STATUS ddr3TipInitStaticConfigDb
(
    GT_U32                          devNum,
	MV_HWS_TIP_STATIC_CONFIG_INFO*	staticConfigInfo
)
{
    staticConfig[devNum].boardTraceArr = staticConfigInfo->boardTraceArr;
    staticConfig[devNum].packageTraceArr = staticConfigInfo->packageTraceArr;
    siliconDelay[devNum] = staticConfigInfo->siliconDelay;
    return GT_OK;
}
/************************** global data ******************************/
static CsElement chipSelectMap[] =
{
   /* CS Value
   (single only)  Num_CS */
   {  0,            0},  
   {  0,            1},  
   {  1,            1},  
   {  0,            2},  
   {  2,            1},  
   {  0,            2},  
   {  0,            2},  
   {  0,            3},  
   {  3,            1},  
   {  0,            2},  
   {  0,            2},  
   {  0,            3},  
   {  0,            2},  
   {  0,            3},  
   {  0,            3},  
   {  0,            4}  
};

/*****************************************************************************
Static round trip flow - Calculates the total round trip delay.
******************************************************************************/
GT_STATUS    ddr3TipStaticRoundTripArrBuild
(
    GT_U32              devNum,
    TripDelayElement*  tablePtr,
    GT_BOOL             isWl,
    GT_U32              *roundTripDelayArr 
)
{
    GT_U32 busIndex, globalBus;
    GT_U32 interfaceId;
    GT_U32 busPerInterface;
    GT_32  sign;
    GT_U32 temp;
    GT_U32 boardTrace;
    TripDelayElement* pkgDelayPtr;
	GT_U8 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    sign = (isWl) ? -1 : 1; /* in WL we calc the diff between Clock to DQs in RL we sum the round trip of Clock and DQs */

    busPerInterface = octetsPerInterfaceNum;

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busIndex = 0; busIndex < busPerInterface; busIndex++)
        {
       		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busIndex)
            globalBus = (interfaceId * busPerInterface) + busIndex;
            /* calculate total trip delay (package and board) */
            boardTrace = (tablePtr[globalBus].dqsDelay * sign) + tablePtr[globalBus].ckDelay;
            /* fix: multiply and than divide instead of using floating point ;*/
            temp = (boardTrace*163)/1000;
            /* Convert the length to delay in psec units */
            pkgDelayPtr = staticConfig[devNum].packageTraceArr;
            roundTripDelayArr[globalBus] = temp +  (GT_32) (pkgDelayPtr[globalBus].dqsDelay * sign) + (GT_32) pkgDelayPtr[globalBus].ckDelay +  (GT_32) ((isWl == GT_TRUE) ? wlDebugDelay : (GT_32) siliconDelay[devNum]);
            DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("Round Trip Build roundTripDelayArr[0x%x]: 0x%x    temp 0x%x \n", globalBus, roundTripDelayArr[globalBus], temp));   
        }
    }

   return GT_OK;
}


/*****************************************************************************
Write leveling for static flow - calculating the round trip delay of the DQS signal.
******************************************************************************/
GT_STATUS    ddr3TipWriteLevelingStaticConfig
(
    GT_U32				devNum,
    GT_U32				interfaceId,
    MV_HWS_DDR_FREQ     frequency,
    GT_U32				*roundTripDelayArr 
)
{
    GT_U32 busIndex;   /* index to the bus loop */
    GT_U32 busStartIndex;
    GT_U32 busPerInterface;
    GT_U32 phase = 0;
    GT_U32 adll = 0, adll_cen, adll_inv, adll_final;
    GT_U32 adllPeriod = MEGA / freqVal[frequency] / 64;
	GT_U8 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("ddr3TipWriteLevelingStaticConfig\n"));
    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("devNum 0x%x IF 0x%x freq %d (adllPeriod 0x%x)\n", devNum, interfaceId, frequency, adllPeriod));   


	busPerInterface = octetsPerInterfaceNum;
	busStartIndex = interfaceId * busPerInterface;
	for(busIndex = busStartIndex; busIndex < (busStartIndex + busPerInterface); busIndex++)
	{
   		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busIndex)
		phase = roundTripDelayArr[busIndex] / (32 * adllPeriod);
		adll = (roundTripDelayArr[busIndex] - (phase * 32 * adllPeriod)) / adllPeriod; 
		adll = (adll > 31) ? 31 : adll;
		adll_cen = 16 + adll;
		adll_inv = adll_cen/32;
		adll_final = adll_cen - (adll_inv * 32);
		adll_final = (adll_final > 31) ? 31 : adll_final;
		/*
		cs = chipSelectMap[topologyMap->interfaceParams[interfaceId].asBusParams[busIndex % 4].csBitmask].csNum;
		*/
		/*cs = 0;*/ /* TBD - For NP5 Only */
		DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("\t%d - phase 0x%x adll 0x%x\n", busIndex, phase, adll));   
		/* Writing to all 4 phy of Interface number, bit 0 \96 4 \96 ADLL, bit 6-8 phase*/
		CHECK_STATUS(ddr3TipBusReadModifyWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, (busIndex % 4), DDR_PHY_DATA, PHY_WRITE_DELAY(cs), ((phase << 6) + (adll & 0x1F)),0x1DF));
		CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST,(busIndex % 4),DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG, ((adll_inv & 0x1) << 5) + adll_final));
	}

	return GT_OK;
}

/*****************************************************************************
Read leveling for static flow
******************************************************************************/
GT_STATUS    ddr3TipReadLevelingStaticConfig(GT_U32             devNum,
                                             GT_U32             interfaceId,
                                             MV_HWS_DDR_FREQ    frequency,
                                             GT_U32             *totalRoundTripDelayArr)
{
	GT_U32 cs, data0, data1, data3 = 0;
	GT_U32 busIndex;   /* index to the bus loop */
	GT_U32 busStartIndex;
	GT_U32 phase0, phase1, maxPhase;
	GT_U32 adll0, adll1;
	GT_U32 clValue;
	GT_U32 minDelay;
	GT_U32 sdrPeriod = MEGA / freqVal[frequency];
	GT_U32 ddrPeriod = MEGA / freqVal[frequency] / 2;
	GT_U32 adllPeriod = MEGA / freqVal[frequency] / 64;
	MV_HWS_SPEED_BIN speedBinIndex;
	GT_U32 rdSampleDly[MAX_CS_NUM] = { 0 };
	GT_U32 rdReadyDel[MAX_CS_NUM]  = { 0 };
	GT_U8 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("ddr3TipReadLevelingStaticConfig\n"));
    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("devNum 0x%x ifc 0x%x freq %d\n", devNum,interfaceId, frequency));   
    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("SdrPeriod 0x%x DdrPeriod 0x%x adllPeriod 0x%x\n", sdrPeriod, ddrPeriod, adllPeriod));   
    if(topologyMap->interfaceParams[firstActiveIf].memoryFreq == frequency)
    {
        clValue = topologyMap->interfaceParams[firstActiveIf].casL;
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("clValue 0x%x\n", clValue)); 
    }
    else
    {
        speedBinIndex = topologyMap->interfaceParams[interfaceId].speedBinIndex;
        clValue = casLatencyTable[speedBinIndex].clVal[frequency];
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("clValue 0x%x speedBinIndex %d\n", clValue, speedBinIndex)); 
    }
  

    busStartIndex = interfaceId * octetsPerInterfaceNum;

    for(busIndex=busStartIndex; busIndex < (busStartIndex + octetsPerInterfaceNum); busIndex+=2)
    {
   		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busIndex)
        cs = chipSelectMap[topologyMap->interfaceParams[interfaceId].asBusParams[(busIndex % 4)].csBitmask].csNum;
        /* read sample delay calculation */
        minDelay = (totalRoundTripDelayArr[busIndex] < totalRoundTripDelayArr[busIndex + 1]) ? totalRoundTripDelayArr[busIndex] : totalRoundTripDelayArr[busIndex + 1]; 
        rdSampleDly[cs] = 2 * (minDelay / (sdrPeriod*2)); /* round down */
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("\t%d - minDelay 0x%x cs 0x%x rdSampleDly[cs] 0x%x\n", busIndex, minDelay, cs, rdSampleDly[cs]));   

        /* phase calculation */
        phase0 = (totalRoundTripDelayArr[busIndex] - (sdrPeriod * rdSampleDly[cs])) / (ddrPeriod);
        phase1 = (totalRoundTripDelayArr[busIndex + 1] - (sdrPeriod * rdSampleDly[cs])) / (ddrPeriod);
        maxPhase  =  (phase0 > phase1) ? phase0 : phase1;
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("\tphase0 0x%x phase1 0x%x maxPhase 0x%x\n", phase0, phase1, maxPhase));   
        /* ADLL calculation */
        adll0 = (GT_U32)((totalRoundTripDelayArr[busIndex] - (sdrPeriod * rdSampleDly[cs]) - 
                                            (ddrPeriod * phase0)) / adllPeriod);
        adll0 = (adll0 > 31) ? 31 : adll0;
        adll1 =  (GT_U32)((totalRoundTripDelayArr[busIndex + 1] - (sdrPeriod * rdSampleDly[cs]) - 
                                            (ddrPeriod * phase1)) / adllPeriod);
        adll1 = (adll1 > 31) ? 31 : adll1;
        /* The Read delay close the Read FIFO*/
        rdReadyDel[cs] = rdSampleDly[cs] + readReadyDelayPhaseOffset[maxPhase];
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("\tadll0 0x%x adll1 0x%x rdReadyDel[cs] 0x%x\n", adll0, adll1, rdReadyDel[cs]));   
        /* Write to the phy of Interface (bit 0 \96 4 \96 ADLL, bit 6-8 phase) */
		data0 =  ((phase0 << 6) + (adll0 & 0x1F));
		data1 = ((phase1 << 6) + (adll1 & 0x1F));

		CHECK_STATUS(ddr3TipBusReadModifyWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, (busIndex % 4), DDR_PHY_DATA, PHY_READ_DELAY(cs), data0,0x1DF));
		CHECK_STATUS(ddr3TipBusReadModifyWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ((busIndex + 1) % 4), DDR_PHY_DATA, PHY_READ_DELAY(cs), data1, 0x1DF));
	}

    for (busIndex = 0; busIndex < octetsPerInterfaceNum; busIndex++)
    {
   		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busIndex)
        CHECK_STATUS(ddr3TipBusReadModifyWrite(  devNum, ACCESS_TYPE_UNICAST, interfaceId, busIndex, DDR_PHY_DATA, 0x3, data3, 0x1F));
    }
	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_SAMPLE_DELAY, 
                               (rdSampleDly[0] + clValue) + (rdSampleDly[1] << 8), MASK_ALL_BITS));
	/*ReadReadyDel0 bit 0-4 , CS bits 8-12 */
	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_READY_DELAY, 
							   rdReadyDel[0]  + (rdReadyDel[1] << 8) + clValue , MASK_ALL_BITS));
	return GT_OK;
}

/*****************************************************************************
DDR3 Static flow
******************************************************************************/
GT_STATUS    ddr3TipRunStaticAlg
(
    GT_U32  devNum,
    MV_HWS_DDR_FREQ freq
)
{
    GT_U32   interfaceId = 0;
    TripDelayElement* tablePtr;
    GT_U32   auiWlTotalRoundTripDelayArr[MAX_TOTAL_BUS_NUM];
    GT_U32   auiRlTotalRoundTripDelayArr[MAX_TOTAL_BUS_NUM];
    InitCntrParam   initCntrPrm;
    GT_STATUS retVal;

    DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("ddr3TipRunStaticAlg"));

	initCntrPrm.doMrsPhy = GT_TRUE;
 	initCntrPrm.isCtrl64Bit = GT_FALSE;
    initCntrPrm.initPhy = GT_TRUE;
    retVal = mvHwsDdr3TipInitController(devNum, &initCntrPrm);
    if (retVal != GT_OK)
    {
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_ERROR, ("mvHwsDdr3TipInitController failure \n")); 
    }
	/* calculate the round trip delay for Write Leveling */
    tablePtr = staticConfig[devNum].boardTraceArr;
	CHECK_STATUS(ddr3TipStaticRoundTripArrBuild(devNum, tablePtr, GT_TRUE, auiWlTotalRoundTripDelayArr));
	/* calculate the round trip delay  for Read Leveling */
	CHECK_STATUS(ddr3TipStaticRoundTripArrBuild(devNum, tablePtr, GT_FALSE, auiRlTotalRoundTripDelayArr));
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
	{
		/* check if the interface is enabled */
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* static frequency is defined according to init-frequency (not target) */
        DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("Static IF %d freq %d \n", interfaceId, freq));   
		CHECK_STATUS(ddr3TipWriteLevelingStaticConfig(devNum, interfaceId, freq,auiWlTotalRoundTripDelayArr));
		CHECK_STATUS(ddr3TipReadLevelingStaticConfig(devNum, interfaceId, freq,auiRlTotalRoundTripDelayArr));
	}

	return GT_OK;
}

/*****************************************************************************
Init controller for static flow
******************************************************************************/
GT_STATUS    ddr3TipStaticInitController
(
    GT_U32    devNum 
)
{
   GT_U32 indexCnt = 0;

   DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("ddr3TipStaticInitController\n")); 

	while(staticInitControllerConfig[devNum][indexCnt].regAddr != 0)
   {
       	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 
								staticInitControllerConfig[devNum][indexCnt].regAddr, 								   
								staticInitControllerConfig[devNum][indexCnt].regData, 
								staticInitControllerConfig[devNum][indexCnt].regMask));
        
       	DEBUG_TRAINING_STATIC_IP(DEBUG_LEVEL_TRACE, ("InitController indexCnt %d\n", indexCnt)); 
		indexCnt++;
   }
   return GT_OK;
}

/*****************************************************/
GT_STATUS    ddr3TipStaticPhyInitController
(
    GT_U32    devNum
)
{
    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xA4, 0x3dfe));
    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xA6, 0xCB2));
    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xA9, 0));
	CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xD0, 0x1F));
    CHECK_STATUS(mvHwsDdr3TipBUSWrite(  devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xA8, 0x434));
     
    return GT_OK;
}

#endif


