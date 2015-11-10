/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, exresults, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvDdr3TrainingIpCentralization.c
*
* DESCRIPTION: DDR3 Centralization flow
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 54 $
******************************************************************************/

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpCentralization.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3LoggingDef.h"


extern GT_U8  debugCentralization;

#define VALIDATE_WIN_LENGTH(e1,e2,maxsize)      ((e2+1 > e1 + (GT_U8) MIN_WINDOW_SIZE) && (e2+1 < e1 + (GT_U8) maxsize))
#define IS_WINDOW_OUT_BOUNDARY(e1,e2,maxsize)   ((e1==0 && e2 != 0) || (e1!=(maxsize-1) && e2==(maxsize-1)))
#define CENTRAL_TX           (0)
#define CENTRAL_RX           (1)
#define NUM_OF_CENTRAL_TYPES (2)

#ifdef CONFIG_DDR4
GT_U32 startPattern = PATTERN_KILLER_DQ0, endPattern = PATTERN_VREF_INV;
#else
GT_U32 startPattern = PATTERN_KILLER_DQ0, endPattern = PATTERN_KILLER_DQ7;
#endif
GT_U32 startIf = 0 , endIf = (MAX_INTERFACE_NUM - 1);
GT_U8  busEndWindow[NUM_OF_CENTRAL_TYPES][MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8  busStartWindow[NUM_OF_CENTRAL_TYPES][MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8  centralizationState[MAX_INTERFACE_NUM][MAX_BUS_NUM];
GT_U8  vrefWindowSize[MAX_INTERFACE_NUM][MAX_BUS_NUM];

extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 debugMode;
extern GT_U32 effective_cs;
extern GT_BOOL	ddr3TipCentralizationSkipMinWindowCheck;
extern GT_U32* dqMapTable;
extern AUTO_TUNE_STAGE trainingStage;

/******************************************************************************
* Name:     ddr3TipDDR3Centralization.
* Desc:     Centralization RX
* Args:     devNum
* Notes:
* Returns:  OK if success, other error code if fail.
*/

static GT_STATUS    ddr3TipCentralization
(
    GT_U32   devNum,
    GT_U32   mode
);

/*****************************************************************************
Centralization RX Flow
******************************************************************************/
GT_STATUS    ddr3TipCentralizationRx
(
    GT_U32    devNum
)
{
	CHECK_STATUS(ddr3TipSpecialRx(devNum));
	CHECK_STATUS(ddr3TipCentralization(devNum, CENTRAL_RX));
	return GT_OK;
}

/*****************************************************************************
Centralization TX Flow
******************************************************************************/
GT_STATUS    ddr3TipCentralizationTx
(
    GT_U32    devNum
)
{
    CHECK_STATUS(ddr3TipCentralization(devNum, CENTRAL_TX));
    return GT_OK;
}

/*****************************************************************************
Centralization Flow
******************************************************************************/
static GT_STATUS    ddr3TipCentralization
(
    GT_U32   devNum,
    GT_U32   mode
)
{
    MV_HWS_TrainingIpStatus trainingResult[MAX_INTERFACE_NUM];
    GT_U32 interfaceId, patternId, bitId;
	GT_U8  busId;
    GT_U8  currentStartWindow[BUS_WIDTH_IN_BITS];
    GT_U8  centralizationResult[MAX_INTERFACE_NUM][BUS_WIDTH_IN_BITS];
    GT_U8  currentEndWindow[BUS_WIDTH_IN_BITS];
    GT_U8  currentWindow[BUS_WIDTH_IN_BITS];
    GT_U8  optWindow, wasteWindow ,startWindowSkew , endWindowSkew;
    GT_U8  finalPupWindow[MAX_INTERFACE_NUM][BUS_WIDTH_IN_BITS];

   /* initialize parameters */
    MV_HWS_TRAINING_RESULT resultType = RESULT_PER_BIT;
    MV_HWS_DIRECTION direction;
    GT_U32 *result[MV_HWS_SearchDirLimit];
    GT_U32 regPhyOffset, regData;
	GT_U8 maxWinSize;
    GT_BOOL lockSuccess = GT_TRUE;
    GT_U8 currentEndWindowMin, currentStartWindowMax;
	GT_U32   csEnableRegVal[MAX_INTERFACE_NUM];
    GT_BOOL isIfFail = GT_FALSE;
    MV_HWS_RESULT* flowResult = ddr3TipGetResultPtr(trainingStage);
	GT_U32 pupWinLength = 0;
    MV_HWS_SearchDirection searchDirId;
	GT_U8 consTap = (mode == CENTRAL_TX)?(64):(0);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* save current cs enable reg val */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal, MASK_ALL_BITS));
        /* enable single cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
    }

    if (mode == CENTRAL_TX)
    {
        maxWinSize = MAX_WINDOW_SIZE_TX;
        regPhyOffset = WRITE_CENTRALIZATION_PHY_REG + (effective_cs * 4);
        direction = OPER_WRITE;
    }
    else
    {
        maxWinSize = MAX_WINDOW_SIZE_RX;
        regPhyOffset = READ_CENTRALIZATION_PHY_REG + (effective_cs * 4);
        direction = OPER_READ;
    }

    /* DB initialization */
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busId = 0; busId < octetsPerInterfaceNum ; busId++)
        {
        	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
            centralizationState[interfaceId][busId] = 0;
            busEndWindow[mode][interfaceId][busId] = (maxWinSize-1) + consTap;
            busStartWindow[mode][interfaceId][busId] = 0;
            centralizationResult[interfaceId][busId] = 0;
        }
    }

    /* start flow */
    for(patternId = startPattern; patternId <= endPattern ; patternId++)
    {
        ddr3TipIpTrainingWrapper(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST, 
                                 PARAM_NOT_CARE, resultType, MV_HWS_ControlElement_ADLL,
                                 PARAM_NOT_CARE, direction, topologyMap->interfaceActiveMask, 
                                 0x0, maxWinSize-1, maxWinSize-1, patternId, EDGE_FPF, CS_SINGLE, 
                                 PARAM_NOT_CARE, trainingResult);

        for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
            for(busId = 0; busId < octetsPerInterfaceNum ; busId++)
            {
        		VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)

                for(searchDirId = MV_HWS_Low2High ; searchDirId <= MV_HWS_High2Low ; searchDirId++)
                {
                    CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceId, ACCESS_TYPE_UNICAST,
                                busId, ALL_BITS_PER_PUP, searchDirId,  direction,
                                resultType, TrainingLoadOperation_UNLOAD,
                                CS_SINGLE, &result[searchDirId] , GT_TRUE, 0, GT_FALSE));
                    DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("%s pat %d IF %d pup %d Regs: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", ((mode == CENTRAL_TX)? "TX" : "RX") ,patternId, interfaceId, busId,  result[searchDirId][0],result[searchDirId][1],result[searchDirId][2],result[searchDirId][3],result[searchDirId][4],result[searchDirId][5],result[searchDirId][6],result[searchDirId][7]));
                }
                for(bitId = 0; bitId < BUS_WIDTH_IN_BITS  ; bitId++)
                {
                    /* check if this code is valid for 2 edge, probably not :( */
                    currentStartWindow[bitId] = GET_TAP_RESULT(result[MV_HWS_Low2High][bitId], EDGE_1);
                    currentEndWindow[bitId] = GET_TAP_RESULT(result[MV_HWS_High2Low][bitId], EDGE_1);
                    /* window length */
                    currentWindow[bitId] = currentEndWindow[bitId] - currentStartWindow[bitId] + 1;
                    DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_TRACE,("cs %x patern %d IF %d pup %d curStartWin %d currentEndWindow %d currentWindow %d\n",effective_cs, patternId, interfaceId, busId, currentStartWindow[bitId],currentEndWindow[bitId],currentWindow[bitId]));
                }
                if ((ddr3TipIsPupLock(result[MV_HWS_Low2High], resultType)) && (ddr3TipIsPupLock(result[MV_HWS_High2Low], resultType)))
                {
                    /* read result success */
                    DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Pup locked, pat %d IF %d pup %d \n",patternId, interfaceId, busId));
                }
                else
                {
                    /* read result failure */
                    DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO, ("fail Lock, pat %d IF %d pup %d \n",patternId, interfaceId, busId));
                    if (centralizationState[interfaceId][busId] == 1)
                    {
                        /* continue with next pup */
                        DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_TRACE,("continue to next pup\n",interfaceId,busId));
                        continue;
                    }
                    for(bitId = 0; bitId < BUS_WIDTH_IN_BITS  ; bitId++)
                    {
                        /* the next check is relevant only when using search machine 2 edges */
                        if (currentStartWindow[bitId] > 0 &&
                            currentEndWindow[bitId] == 0)
                        {
                            currentEndWindow[bitId] = maxWinSize-1;
                            DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_TRACE,("fail, IF %d pup %d bit %d fail #1\n",interfaceId,busId,bitId));
                            /* the next bit */
                            continue;
                        }
                        else
                        {
                            centralizationState[interfaceId][busId] = 1;
                            DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_TRACE,("fail, IF %d pup %d bit %d fail #2\n",interfaceId, busId, bitId));
                        }
                    }
                    if (centralizationState[interfaceId][busId] == 1)
                    {
                       /* going to next pup */
                        continue;
                    }
                }/*bit*/

                optWindow = ddr3TipGetBufMin(currentWindow);
                /* final pup window length */
                finalPupWindow[interfaceId][busId] = ddr3TipGetBufMin(currentEndWindow) - ddr3TipGetBufMax(currentStartWindow) + 1;
                wasteWindow = optWindow - finalPupWindow[interfaceId][busId];
                startWindowSkew = ddr3TipGetBufMax(currentStartWindow) - ddr3TipGetBufMin(currentStartWindow);
                endWindowSkew = ddr3TipGetBufMax(currentEndWindow) - ddr3TipGetBufMin(currentEndWindow);
                /* min/max updated with pattern change */
                currentEndWindowMin = ddr3TipGetBufMin(currentEndWindow);
                currentStartWindowMax = ddr3TipGetBufMax(currentStartWindow);
                busEndWindow[mode][interfaceId][busId] =   GET_MIN(busEndWindow[mode][interfaceId][busId], currentEndWindowMin);
                busStartWindow[mode][interfaceId][busId] = GET_MAX(busStartWindow[mode][interfaceId][busId],currentStartWindowMax);
                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("pat %d IF %d pup %d optWin %d finalWin %d wasteWin %d stWinSkew %d endWinSkew %d curStWinMax %d curEndWinMin %d busStWin %d busEndWin %d \n",patternId, interfaceId, busId,optWindow,finalPupWindow[interfaceId][busId],wasteWindow,startWindowSkew,endWindowSkew,currentStartWindowMax,currentEndWindowMin,busStartWindow[mode][interfaceId][busId],busEndWindow[mode][interfaceId][busId]));
                /* check if window is valid */
				if(ddr3TipCentralizationSkipMinWindowCheck == GT_FALSE){
		            if ((VALIDATE_WIN_LENGTH(busStartWindow[mode][interfaceId][busId],busEndWindow[mode][interfaceId][busId],maxWinSize) == GT_TRUE) ||
		                (IS_WINDOW_OUT_BOUNDARY(busStartWindow[mode][interfaceId][busId],busEndWindow[mode][interfaceId][busId],maxWinSize) == GT_TRUE))
		            {
		                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("win valid, pat %d IF %d pup %d \n",patternId, interfaceId, busId));
		                /* window is valid */
		            }
		            else
		            {
		                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("fail win, pat %d IF %d pup %d busStWin %d busEndWin %d\n",patternId, interfaceId, busId, busStartWindow[mode][interfaceId][busId],busEndWindow[mode][interfaceId][busId]));
		                centralizationState[interfaceId][busId] = 1;
		                if (debugMode == GT_FALSE)
		                {
							flowResult[interfaceId] = TEST_FAILED;
		                    return GT_FAIL;
		                }
		            }
				}/*if ddr3TipCentralizationSkipMinWindowCheck*/
            }/*pup*/
        }/*interface*/
    }/*pattern*/
    for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)
    {
		VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

        isIfFail = GT_FALSE;
        flowResult[interfaceId] = TEST_SUCCESS;
        for(busId = 0; busId < octetsPerInterfaceNum ; busId++)
        {
        	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)

            /*continue only if lock*/
            if (centralizationState[interfaceId][busId] != 1)
            {
				if(ddr3TipCentralizationSkipMinWindowCheck == GT_FALSE){
		            if ((busEndWindow[mode][interfaceId][busId] == (maxWinSize-1)) &&
		                ((busEndWindow[mode][interfaceId][busId]-busStartWindow[mode][interfaceId][busId]) < MIN_WINDOW_SIZE)&&
		                ((busEndWindow[mode][interfaceId][busId]-busStartWindow[mode][interfaceId][busId]) > 2)) /*prevent false lock */
		            {
		                centralizationState[interfaceId][busId] = 2; /*TBD change to enum */
		            }
		            if ((busEndWindow[mode][interfaceId][busId] == 0) &&
		                ((busEndWindow[mode][interfaceId][busId]-busStartWindow[mode][interfaceId][busId]) < MIN_WINDOW_SIZE)&&
		                ((busEndWindow[mode][interfaceId][busId]-busStartWindow[mode][interfaceId][busId]) > 2)) /*prevent false lock */
		            {
		                centralizationState[interfaceId][busId] = 3;
		            }
				}
				if ((busEndWindow[mode][interfaceId][busId] > (maxWinSize-1)) && direction == OPER_WRITE)
				{
					DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Tx special pattern \n"));
                    consTap   = 64;
				}
            }

            /* check states */
            if (centralizationState[interfaceId][busId] == 3)
            {
                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("SSW - TBD IF %d pup %d\n", interfaceId, busId));
                lockSuccess = GT_TRUE;
            }
            else if (centralizationState[interfaceId][busId] == 2)
            {
                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO, ("SEW - TBD IF %d pup %d\n", interfaceId, busId));
                lockSuccess = GT_TRUE;
            }
            else if (centralizationState[interfaceId][busId] == 0)
            {
                lockSuccess = GT_TRUE;
            }
            else
            {
                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_ERROR, ("fail, IF %d pup %d\n", interfaceId, busId));
                lockSuccess = GT_FALSE;
            }

            if (lockSuccess == GT_TRUE)
            {
                centralizationResult[interfaceId][busId] = (busEndWindow[mode][interfaceId][busId] + busStartWindow[mode][interfaceId][busId])/2 - consTap;
                DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_TRACE, (" busId %d Res= %d\n",busId,centralizationResult[interfaceId][busId]));
                /* copy results to registers  */
				pupWinLength = busEndWindow[mode][interfaceId][busId]-busStartWindow[mode][interfaceId][busId]+1;

				mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, busId, DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR+effective_cs, &regData);
				regData = (regData & (~0x1F << ((mode == CENTRAL_TX)?(RESULT_DB_PHY_REG_TX_OFFSET):(RESULT_DB_PHY_REG_RX_OFFSET)))) | pupWinLength << ((mode == CENTRAL_TX)?(RESULT_DB_PHY_REG_TX_OFFSET):(RESULT_DB_PHY_REG_RX_OFFSET));
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId,
                                             ACCESS_TYPE_UNICAST,  busId, DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR+effective_cs,
                                             regData));
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId,
                                             ACCESS_TYPE_UNICAST,  busId, DDR_PHY_DATA, regPhyOffset, /* offset per CS is calculated earlier */ 
                                             centralizationResult[interfaceId][busId]));
            }
            else
            {
                isIfFail = GT_TRUE;
            }
       }

       if (isIfFail == GT_TRUE)
       {
           flowResult[interfaceId] = TEST_FAILED;
       }
   }

   /* restore cs enable value*/
   for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
   {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal[interfaceId], MASK_ALL_BITS));
   }

   return isIfFail;
}

/*****************************************************************************
Centralization Flow
******************************************************************************/
GT_U8 ddr3TipSpecialRxRunOnceFlag = 0;
GT_STATUS    ddr3TipSpecialRx
(
    GT_U32   devNum
)
{
    MV_HWS_TrainingIpStatus trainingResult[MAX_INTERFACE_NUM];
    GT_U32 interfaceId, pupId, patternId, bitId;
    GT_U8  currentStartWindow[BUS_WIDTH_IN_BITS];
    GT_U8  currentEndWindow[BUS_WIDTH_IN_BITS];
    MV_HWS_TRAINING_RESULT resultType = RESULT_PER_BIT;
    MV_HWS_DIRECTION direction;
    MV_HWS_SearchDirection searchDirId;
    GT_U32 *result[MV_HWS_SearchDirLimit];
    GT_U32 maxWinSize;
    GT_U8 currentEndWindowMin, currentStartWindowMax;
   	GT_U32   csEnableRegVal[MAX_INTERFACE_NUM];
	GT_U32 temp = 0;
    int PadNum = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	if( (ddr3TipSpecialRxRunOnceFlag&(1<<effective_cs)) == 1<<effective_cs )
		return GT_OK;
	ddr3TipSpecialRxRunOnceFlag |= 1<<effective_cs;

    for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* save current cs enable reg val */
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, csEnableRegVal, MASK_ALL_BITS));
        /* enable single cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
    }

    maxWinSize = MAX_WINDOW_SIZE_RX;
    direction = OPER_READ;
	patternId = PATTERN_FULL_SSO1;

    /* start flow */
    ddr3TipIpTrainingWrapper(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST,
				              PARAM_NOT_CARE, resultType, MV_HWS_ControlElement_ADLL,
				             PARAM_NOT_CARE, direction, topologyMap->interfaceActiveMask,
				             0x0, maxWinSize-1, maxWinSize-1, patternId, EDGE_FPF, CS_SINGLE,
				             PARAM_NOT_CARE, trainingResult);
    for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(pupId = 0; pupId <= octetsPerInterfaceNum; pupId++)
        {
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupId)

            for(searchDirId = MV_HWS_Low2High ; searchDirId <= MV_HWS_High2Low ; searchDirId++)
            {
				CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceId, ACCESS_TYPE_UNICAST,
				             pupId, ALL_BITS_PER_PUP, searchDirId,  direction,
				            resultType, TrainingLoadOperation_UNLOAD,
				            CS_SINGLE, &result[searchDirId] , GT_TRUE, 0, GT_FALSE));
				DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Special: pat %d IF %d pup %d Regs: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",patternId, interfaceId, pupId,  result[searchDirId][0],result[searchDirId][1],result[searchDirId][2],result[searchDirId][3],result[searchDirId][4],result[searchDirId][5],result[searchDirId][6],result[searchDirId][7]));
            }
            for(bitId = 0; bitId < BUS_WIDTH_IN_BITS  ; bitId++)
            {
				/* check if this code is valid for 2 edge, probably not :( */
				currentStartWindow[bitId] = GET_TAP_RESULT(result[MV_HWS_Low2High][bitId], EDGE_1);
				currentEndWindow[bitId] = GET_TAP_RESULT(result[MV_HWS_High2Low][bitId], EDGE_1);
            }
            if( !((ddr3TipIsPupLock(result[MV_HWS_Low2High], resultType)) && (ddr3TipIsPupLock(result[MV_HWS_High2Low], resultType))))
            {
				DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_ERROR,("Special: Pup lock fail, pat %d IF %d pup %d \n",patternId, interfaceId, pupId));
				return GT_FAIL;
            }

            currentEndWindowMin = ddr3TipGetBufMin(currentEndWindow);
            currentStartWindowMax = ddr3TipGetBufMax(currentStartWindow);

			if( currentStartWindowMax <= 1 ) /*Align left*/
			{
				for( bitId = 0 ; bitId < BUS_WIDTH_IN_BITS ; bitId++)
				{
					PadNum = dqMapTable[bitId+pupId*BUS_WIDTH_IN_BITS + interfaceId*BUS_WIDTH_IN_BITS*MAX_BUS_NUM];
                    CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, pupId,  DDR_PHY_DATA,  PBS_RX_PHY_REG+PadNum +  effective_cs * 0x10, &temp));
					temp = (temp + 0xA > 31)?(31):(temp + 0xA);
					CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,ACCESS_TYPE_UNICAST,interfaceId, ACCESS_TYPE_UNICAST,pupId, DDR_PHY_DATA, PBS_RX_PHY_REG+PadNum + effective_cs * 0x10, temp));
				}
				DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Special: PBS:: I/F# %d , Bus# %d fix align to the Left \n", interfaceId,pupId));
			}

			if( currentEndWindowMin > 30 )/*Align rigth*/
			{
				CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, pupId,  DDR_PHY_DATA,  PBS_RX_PHY_REG+4 + effective_cs * 0x10, &temp));
				temp += 0xA;
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,ACCESS_TYPE_UNICAST,interfaceId, ACCESS_TYPE_UNICAST,pupId, DDR_PHY_DATA, PBS_RX_PHY_REG+4 + effective_cs * 0x10, temp));
				CHECK_STATUS(mvHwsDdr3TipBUSRead(  devNum, interfaceId, ACCESS_TYPE_UNICAST, pupId,  DDR_PHY_DATA,  PBS_RX_PHY_REG+5 + effective_cs * 0x10, &temp));
				temp += 0xA;
				CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,ACCESS_TYPE_UNICAST,interfaceId, ACCESS_TYPE_UNICAST,pupId, DDR_PHY_DATA, PBS_RX_PHY_REG+5 + effective_cs * 0x10, temp));
				DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Special: PBS:: I/F# %d , Bus# %d fix align to the right \n", interfaceId,pupId));
			}

			vrefWindowSize[interfaceId][pupId] = currentEndWindowMin - currentStartWindowMax + 1;
			DEBUG_CENTRALIZATION_ENGINE(DEBUG_LEVEL_INFO,("Special: Winsize I/F# %d , Bus# %d is %d\n", interfaceId,pupId, vrefWindowSize[interfaceId][pupId]));
		}/*pup*/
	}/*end of interface*/

	return GT_OK;
}
/*****************************************************************************
Read Centralization Result
******************************************************************************/
GT_STATUS    mvHwsDdr3TipReadCentralizationResult
(
    GT_U32          devNum,
    GT_U8*  result
)
{
	devNum = devNum;

    if (result == NULL)
    {
        return GT_BAD_PARAM;
    }
    osMemCpy(result, busStartWindow, sizeof(busStartWindow));
    osMemCpy(((GT_U8*)result)+sizeof(busStartWindow), busEndWindow, sizeof(busEndWindow));

    return GT_OK;
}

/*****************************************************************************
Print Centralization Result
******************************************************************************/
GT_STATUS    ddr3TipPrintCentralizationResult
(
    GT_U32      devNum
)
{
    GT_U32 interfaceId = 0, busId = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

	devNum = devNum;

    mvPrintf("Centralization Results\n");
    mvPrintf("I/F0 Result[0 - success 1-fail 2 - state_2 3 - state_3] ...\n");
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busId = 0; busId < octetsPerInterfaceNum; busId++)
        {
        	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busId)
            mvPrintf("%d ,\n",centralizationState[interfaceId][busId]);
        }
    }
    return GT_OK;
}

