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
* mvHwsTrainingIpDdr3TrainingIf.c
*
* DESCRIPTION: DDR3 training IP configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 42 $
******************************************************************************/

#include "mvDdr3TrainingIpBist.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpEngine.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3LoggingDef.h"


extern GT_U8  debugTrainingBist;
MV_HWS_PATTERN sweepPattern = PATTERN_KILLER_DQ0;
GT_U32 bistOffset = 32;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_STATUS mvHwsDdr3CsBaseAdrCalc(GT_U32 interfaceId, GT_U32 uiCs, GT_U32 *csBaseAddr);
/************************** Globals ******************************/
GT_U32  bistPollTime = 25;

static
GT_STATUS    ddr3TipBistOperation
(
    GT_U32                  devNum,
    MV_HWS_ACCESS_TYPE      accessType,
    GT_U32                  interfaceId,
    MV_HWS_BIST_OPERATION   operType
);


extern GT_U32 maxtPollTries;
extern GT_U32 isBistResetBit;

/*****************************************************************************
BIST activate 
******************************************************************************/
GT_STATUS    ddr3TipBistActivate
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
	GT_U32					csNum,
    GT_U32                  patternAddrLength
)
{
    GT_U32    txBurstSize;
    GT_U32    delayBetweenBurst;
    GT_U32    rdMode, dataValue;
    GT_U32    pollCnt = 0, MaxPoll = 1000, interfaceNum, startIf, endIf;
    PatternInfo* patternTable = ddr3TipGetPatternTable();
    GT_U32 readData[MAX_INTERFACE_NUM];
    /*CHECK_STATUS(mvHwsDdr3TipSelectDdrController(devNum, GT_TRUE));*/

    /* ODPG Write enable from BIST */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType,  ifNum, ODPG_DATA_CONTROL_REG,  0x1, 0x1));
    /* ODPG Read enable/disable from BIST */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, ifNum, ODPG_DATA_CONTROL_REG,  (direction==OPER_READ) ? 0x2 : 0, 0x2));
    CHECK_STATUS(ddr3TipLoadPatternToOdpg(devNum, accessType, ifNum, pattern, offset));

    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,   accessType, ifNum,   ODPG_DATA_BUF_SIZE_REG,  patternAddrLength, MASK_ALL_BITS));
    txBurstSize = (direction == OPER_WRITE) ? patternTable[pattern].txBurstSize : 0;
    delayBetweenBurst = (direction == OPER_WRITE) ? 2 : 0;
    rdMode = (direction == OPER_WRITE) ? 1 : 0;
    CHECK_STATUS(ddr3TipConfigureOdpg(devNum, accessType, ifNum, direction, patternTable[pattern].numOfPhasesTx,
                                      txBurstSize, patternTable[pattern].numOfPhasesRx, delayBetweenBurst,
                                      rdMode,  csNum, addrStressJump, duration));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, ifNum,  ODPG_PATTERN_ADDR_OFFSET_REG,  offset, MASK_ALL_BITS));
    if (operType == BIST_STOP)
    {
        CHECK_STATUS(ddr3TipBistOperation(devNum, accessType,  ifNum,  BIST_STOP));
    }
    else
    {
        CHECK_STATUS(ddr3TipBistOperation(devNum, accessType,  ifNum,  BIST_START));
        if (duration != DURATION_CONT)
        {
            /* This pdelay is a WA, becuase polling fives "done" also the odpg did nmot finish its task */
#if 0
#ifndef ASIC_SIMULATION
            CHECK_STATUS(hwsOsExactDelayPtr(devNum, devNum, bistPollTime));   
#endif
#endif
            if (accessType == ACCESS_TYPE_MULTICAST)
            {
                startIf = 0;
                endIf = MAX_INTERFACE_NUM-1;
            }
            else
            {
                startIf = ifNum;
                endIf = ifNum;
            }

            for(interfaceNum = startIf; interfaceNum <= endIf; interfaceNum++)
            {
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceNum)

                for (pollCnt = 0; pollCnt < MaxPoll ; pollCnt++)
                {
                    CHECK_STATUS(mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, ifNum, ODPG_BIST_DONE, readData, MASK_ALL_BITS));
					dataValue = readData[interfaceNum];
					if (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
						if ((dataValue & 0x1) == 0x0)/*in SOC type devices this bit is self clear so, if it was cleared all good*/
		                    break;
					}
					else{
						if ((dataValue & 0x1) == 0x1)
		                {
		                    if (isBistResetBit != 0)
		                    {
		                        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,ACCESS_TYPE_UNICAST, ifNum, ODPG_BIST_DONE, (dataValue & 0xFFFFFFFE), MASK_ALL_BITS));
		                    }
		                    break;
		                }
					}
                }
                if (pollCnt >= MaxPoll)
                {
                    DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_ERROR,("Bist poll failure 2\n"));
                    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, ifNum, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
                    return GT_FAIL;
                }
            }

            CHECK_STATUS(ddr3TipBistOperation(devNum, accessType, ifNum, BIST_STOP));
        }
    }
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, ifNum, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));

    return GT_OK;
}



/*****************************************************************************
BIST read result 
******************************************************************************/
GT_STATUS    ddr3TipBistReadResult
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    BistResult  *pstBistResult
)
{
    GT_STATUS retVal;
    GT_U32 readData[MAX_INTERFACE_NUM];
    /*mvHwsDdr3TipSelectDdrController(devNum,GT_TRUE);*/

    if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
        return GT_NOT_SUPPORTED;
    DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_TRACE, ("ddr3TipBistReadResult interfaceId %d\n",interfaceId));
    retVal = mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_FAILED_DATA_HI_REG, readData, MASK_ALL_BITS);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    pstBistResult->bistFailHigh = readData[interfaceId];
    retVal = mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_FAILED_DATA_LOW_REG, readData, MASK_ALL_BITS);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    pstBistResult->bistFailLow = readData[interfaceId];

    retVal = mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_LAST_FAIL_ADDR_REG, readData, MASK_ALL_BITS);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    pstBistResult->bistLastFailAddr = readData[interfaceId];
    retVal = mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_DATA_ERROR_COUNTER_REG, readData, MASK_ALL_BITS);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    pstBistResult->bistErrorCnt = readData[interfaceId];
    return GT_OK;
}


/*****************************************************************************
BIST flow - Activate & read result
******************************************************************************/
GT_STATUS    mvHwsDdr3RunBist
(
	GT_U32	        devNum,
	MV_HWS_PATTERN  pattern,
	GT_U32          *result,
	GT_U32          csNum
)
{
    GT_STATUS retVal;
    GT_U32 i = 0;
    GT_U32 winBase;
    BistResult stBistResult;

     for(i = 0; i < MAX_INTERFACE_NUM; i++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, i)
	mvHwsDdr3CsBaseAdrCalc(i,csNum,&winBase);
        retVal = ddr3TipBistActivate(devNum, pattern, ACCESS_TYPE_UNICAST, i, OPER_WRITE, STRESS_NONE, DURATION_SINGLE,  BIST_START, (bistOffset + winBase),csNum, 15 );
        if (retVal != GT_OK)
	    {
		    mvPrintf("ddr3TipBistActivate failed (0x%x)\n", retVal);
		    return retVal;
	    }
        retVal = ddr3TipBistActivate(devNum, pattern, ACCESS_TYPE_UNICAST, i, OPER_READ, STRESS_NONE, DURATION_SINGLE, BIST_START, (bistOffset + winBase),csNum, 15 );
        if (retVal != GT_OK)
	    {
		    mvPrintf("ddr3TipBistActivate failed (0x%x)\n", retVal);
		    return retVal;
	    }

        retVal = ddr3TipBistReadResult(devNum, i, &stBistResult);
        if (retVal != GT_OK)
        {
            mvPrintf("ddr3TipBistReadResult failed\n");
            return retVal;
        }
        result[i] = stBistResult.bistErrorCnt;
    }


    return GT_OK;
}

/******************************************************************************
* Set BIST Operation */

static
GT_STATUS    ddr3TipBistOperation
(
    GT_U32                  devNum,
    MV_HWS_ACCESS_TYPE      accessType,
    GT_U32                  interfaceId,
    MV_HWS_BIST_OPERATION   operType
)
{

   if (operType == BIST_STOP)
   {
		if (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,  accessType, interfaceId,  ODPG_BIST_DONE,  (1 << 8), (1 << 8)));
		}
		else{
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,  accessType, interfaceId,  ODPG_DATA_CONTROL_REG,  (1 << 30)  , (GT_U32)(0x3 << 30)));
		}
   }
   else
   {
		if (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,  accessType, interfaceId,  ODPG_BIST_DONE,  1, 1));
		}
		else{
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum,  accessType, interfaceId,  ODPG_DATA_CONTROL_REG,  (GT_U32)(1 << 31), (GT_U32)(1 << 31)));
		}
   }
   return GT_OK;
}


/*****************************************************************************
Print BIST result
******************************************************************************/
void    ddr3TipPrintBistRes()
{
    GT_U32      devNum = 0;
    GT_U32      i;
    BistResult  stBistResult[MAX_INTERFACE_NUM];
    GT_STATUS   res;

    for (i=0; i<MAX_INTERFACE_NUM; i++)
    {
        if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, i) ==  GT_FALSE)
            continue;
        res = ddr3TipBistReadResult(devNum, i, &stBistResult[i]);
        if (res != GT_OK)
        {
            DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_ERROR,("ddr3TipBistReadResult failed\n"));
            return;
        }
    }
    DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_INFO,("interface | error_cnt | fail_low | fail_high | fail_addr\n"));
    for (i=0; i<MAX_INTERFACE_NUM; i++)
    {
        if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, i) ==  GT_FALSE)
            continue;
        DEBUG_TRAINING_BIST_ENGINE(DEBUG_LEVEL_INFO, ("%d |  0x%08x  |  0x%08x  |  0x%08x  | 0x%08x\n", 
                i, stBistResult[i].bistErrorCnt, stBistResult[i].bistFailLow, stBistResult[i].bistFailHigh, stBistResult[i].bistLastFailAddr));
    }
}




