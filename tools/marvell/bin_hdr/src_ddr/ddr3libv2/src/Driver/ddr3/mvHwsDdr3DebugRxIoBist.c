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
* mvHwsDdr3DebugRxIoBist.c.c
*
* DESCRIPTION: RX IO BIST Test
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 204 $
******************************************************************************/

#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3LoggingDef.h"

#if defined(MV_HWS_RX_IO_BIST) || defined(MV_HWS_RX_IO_BIST_ETP)

/************************** definitions ******************************/

#define MV_IO_BIST_PATTERN_SIZE     4

/************************** enums ************************************/

typedef enum
{
    TEST_STOP,
    TEST_STAR,
    TEST_STOP_AND_START,
} MV_TEST_ENABLE;

/************************** globals **********************************/

GT_U32 ioBistCounter;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;

/************************** Pre-Declaration **************************/

void mvHwsInitDataAgrressor(GT_U8 devNum);
void mvHwsInitCaAgressor(GT_U8 devNum);
GT_BOOL mvHwsDataAndSyncSelect(GT_U8 devNum, GT_U32 interfaceId, GT_U32 pup);
void mvHwsSetRxPbsDelay(GT_U8 devNum, GT_U32 DqValue, GT_U32 DqsValue);
void mvHwsTestEnable(GT_U8 devNum, GT_U32 interfaceId, MV_HWS_ACCESS_TYPE phyAccess, GT_U32 pup, MV_TEST_ENABLE enMode);
void mvHwsPhyAndTestReset(GT_U8 devNum, GT_U32 interfaceId, MV_HWS_ACCESS_TYPE phyAccess, GT_U32 pup);
void nvHwsIoBistTest(GT_U8 devNum, GT_U32 interfaceId);
void mvHwsSetVictimPattern(GT_U8 devNum, GT_U32 interfaceId);
void mvHwsSetNoisePattern(GT_U8 devNum, GT_U32 interfaceId);
void mvHwsIoBistTestRx(GT_U8 devNum, GT_U32 interfaceId, GT_U32 pup,GT_U32 victBit, GT_U32 agrBits, GT_U32 rxAdll);
GT_U32 mvHwsConfigureTxAdllOpt(GT_U8 devNum,GT_U32 interfaceId, GT_U32 pup, GT_U32 VictBit, GT_U32 AgrBits, GT_U32 dirrection);
void mvHwsFillPattern(GT_U8 devNum, GT_U32 interfaceId, GT_BOOL isNoise, GT_U32 pattern[MV_IO_BIST_PATTERN_SIZE]);

/************************** functions ********************************/

GT_STATUS mvHwsIoBistTest(GT_U8 devNum)
{
    GT_U32 interfaceId;

    mvHwsSetRxPbsDelay(devNum, 0x1F, 0xF);

#ifdef MV_HWS_RX_IO_BIST_ETP
    mvHwsInitCaAgressor(devNum);
#endif /* MV_HWS_RX_IO_BIST_ETP */

    mvHwsInitDataAgrressor(devNum);

    for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        nvHwsIoBistTest(devNum, interfaceId);
    }

    return GT_OK;
}

void mvHwsSetRxPbsDelay(GT_U8 devNum, GT_U32 DqValue, GT_U32 DqsValue)
{
    GT_U32 interfaceId;
    GT_U32 busNum;
    GT_U32 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for (busNum=0; busNum<octetsPerInterfaceNum; busNum++)
        {
            VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busNum)
            mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, 0x5f, DqValue);
            mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, 0x54, DqsValue);
            mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, 0x55, DqsValue);
            mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, busNum, DDR_PHY_DATA, 0x2, 0xDF);
        }
    }
}

void mvHwsInitCaAgressor(GT_U8 devNum)
{
    GT_U32 interfaceId;

    /*aggressive pattern for all C/A signals*/
                                  /* 0-15   16-31   32-47   48-63 */
    GT_U32 addrKillerPattern[MV_IO_BIST_PATTERN_SIZE] = { 0xcfc0, 0x330c, 0x3333, 0xc0cc };

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)

        /*Tx ETP mode*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0xB0, 0x2);

        /*Continuous mode*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0xC8, 0x1);

        /* PHY & Test reset/un-reset*/
        mvHwsPhyAndTestReset(devNum, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE);

        /*fill FIFO with aggressive pattern*/
        mvHwsFillPattern(devNum, interfaceId, GT_FALSE, addrKillerPattern);

        /*clock N invert*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0xB6, 0xC);

        /*Select Noise FIFO*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL, 0xB4, 0x0);

        /*Stop & Start Test*/
        mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TEST_STOP_AND_START);
    }
}

void mvHwsInitDataAgrressor(GT_U8 devNum)
{
    GT_U32 interfaceId;

    /*aggressive pattern for all Data signals*/
                                  /* 0-15   16-31   32-47   48-63 */
    GT_U32 dataKillerPattern[4] = { 0x52b8, 0x8a55, 0xa6b3, 0x066d };

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /*Tx ETP mode*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xB0, 0x2);

        /*Continuous mode*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xC8, 0x1);

        /* PHY & Test reset/un-reset*/
        mvHwsPhyAndTestReset(devNum, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE);

        /*fill FIFO with aggressive pattern*/
        mvHwsFillPattern(devNum, interfaceId, GT_FALSE, dataKillerPattern);

        /*Select Noise FIFO*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, 0xB4, 0x30);

        /*Stop & Start Test*/
        mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, TEST_STOP_AND_START);
    }
}

void nvHwsIoBistTest(GT_U8 devNum, GT_U32 interfaceId)
{
    GT_U32 dq;
    GT_U32 AgrBits;
    GT_U32 VictBit;
    GT_U32 pup;
    GT_U32 txAdll;
    GT_U32 rxAdll;
    GT_U32 mask = 0x7ff;
    GT_U32 results[MAX_INTERFACE_NUM][MAX_BUS_NUM][MAX_DQ_NUM];

    GT_U32 octetsPerInterfaceNum = ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    mvPrintf("octetsPerInterfaceNum: %d\n",octetsPerInterfaceNum);

    mvHwsSetVictimPattern(devNum, interfaceId);

    for(dq = 0; dq < octetsPerInterfaceNum * 8 ; dq++)
    {
        pup = dq/8;

        VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
        VictBit = dq%8;
        AgrBits = (VictBit > 3) ? (~(1<<(VictBit + 2)) & mask) : (~(1<<VictBit) & mask); /* each pup=11 Bits  8 dq bits, Bits 4 & 5 are DQS, Bit 10=DM */

        mvHwsSetNoisePattern(interfaceId, pup);
        hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);

        /* PHY & Test reset/un-reset*/
        mvHwsPhyAndTestReset(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup);

        /*Stop Test*/
        mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, TEST_STOP);

        txAdll = mvHwsConfigureTxAdllOpt(devNum, interfaceId, pup, VictBit, AgrBits, 1) + 1;
        if ((txAdll > 0) && (txAdll < 5)) {
            txAdll = mvHwsConfigureTxAdllOpt(devNum, interfaceId, pup, VictBit, AgrBits, 0) - 1;
        }
        mvPrintf("txAdll: %d\n",txAdll);

        /*Set Optimal DQS_WR*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x1, txAdll);

        /*Set Optimal Receiver Calibration*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xDB, 0x3F);

        for(rxAdll = 0; rxAdll < 32; rxAdll++)   /* Search RX Window */
        {
            /* PHY & Test reset/un-reset*/
            mvHwsPhyAndTestReset(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup);

            /*Stop Test*/
            mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, TEST_STOP);

            /*Set Current Rx ADLL*/
            mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x3, rxAdll);
            mvHwsIoBistTestRx(devNum,interfaceId,pup,VictBit, AgrBits, rxAdll);
        }
        /*Reset DQS_WR*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x1, 0x0);
        /*Reset DQS_RD*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x3, 0x0);
        mvPrintf("\n ########## Rx - win size: %d #########\n",ioBistCounter);
        results[interfaceId][pup][dq%8] = ioBistCounter;
        ioBistCounter = 0;
    }
    /*print final results*/
    mvPrintf("Interface     PUP     BIT     X_WIN_SIZE\n");
    mvPrintf("-----------------------------------------\n");
    for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for (pup=0; pup<octetsPerInterfaceNum; pup++)
        {
            VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pup)
            for(dq = 0; dq < 8 ; dq++)
            {
                mvPrintf("%d        %d      %d      %d\n",interfaceId,pup,dq,results[interfaceId][pup][dq%8]);
            }
        }
    }
}

void mvHwsTestEnable(GT_U8 devNum, GT_U32 interfaceId, MV_HWS_ACCESS_TYPE phyAccess, GT_U32 pup, MV_TEST_ENABLE enMode)
{
    if((enMode == TEST_STOP) || (enMode == TEST_STOP_AND_START))
    {
        /*Stop Test*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0xB1, 0x0);
        hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
    }

    if((enMode == TEST_STAR) || (enMode == TEST_STOP_AND_START))
    {
        /*Start Test*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0xB1, 0x1);
        hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
    }
}

void mvHwsPhyAndTestReset(GT_U8 devNum, GT_U32 interfaceId, MV_HWS_ACCESS_TYPE phyAccess, GT_U32 pup)
{
    /*Phy Reset*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0x90, 0x2);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);

    /*Phy Un-Reset*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0x90, 0x2002);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);

    /*Test Reset*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0xBD, 0x1);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);

    /*Test Un-Reset*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, phyAccess, pup, DDR_PHY_DATA, 0xBD, 0x0);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
}

void mvHwsFillPattern(GT_U8 devNum, GT_U32 interfaceId, GT_BOOL isNoise, GT_U32 pattern[MV_IO_BIST_PATTERN_SIZE])
{
    GT_U32 offset = (isNoise) ? 0xC4 : 0xC0;
    GT_U8 i;

    for(i = 0; i < MV_IO_BIST_PATTERN_SIZE; i++)
    {
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA, offset + i, pattern[i]);
    }
}

void mvHwsSetVictimPattern(GT_U8 devNum, GT_U32 interfaceId)
{
                                   /* 0-15   16-31   32-47   48-63 */
    GT_U32 killerVictPattern[MV_IO_BIST_PATTERN_SIZE] = { 0xB9A6, 0x45A6, 0x5819, 0xDB9A };

    /*fill FIFO with aggressive pattern*/
    mvHwsFillPattern(devNum, interfaceId, GT_TRUE, killerVictPattern);

    /*TBD - need to extend for BC2*/
}

void mvHwsSetNoisePattern(GT_U8 devNum, GT_U32 interfaceId)
{
                                /* 0-15   16-31   32-47   48-63 */
    GT_U32 allZeroPattern[MV_IO_BIST_PATTERN_SIZE] = { 0x0000, 0x0000, 0x0000, 0x0000 };

    /*fill FIFO with aggressive pattern*/
    mvHwsFillPattern(devNum, interfaceId, GT_FALSE, allZeroPattern);

    /*TBD - need to extend for BC2*/
}

void mvHwsIoBistTestRun(GT_U8 devNum, GT_U32 interfaceId, GT_U32 pup, GT_U32 victBit, GT_U32 agrBits)
{
    GT_U32 timeout = 0;
    GT_U32 dataVal;

    /*Configure I/O bist Mode*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB0, 0x3);

    /*Set Repeat to 31*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xBE, 0xF);

    /*Configure continuous mode to active*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xC8, 0x1);

    /*Configure Sync Pattern to 0x7*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xBC, 0x7);

    /* PHY & Test reset/un-reset*/
    mvHwsPhyAndTestReset(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup);

    /*Select Rx Bit*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB8, victBit);

    /*Select Aggressor*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB4, agrBits);

    /*Stop & Start Test*/
    mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, TEST_STOP_AND_START);

    do
    {
        mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB1, &dataVal);
        hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
        if (timeout++>16)
        {
            /*Stop Test*/
            mvHwsTestEnable(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, TEST_STOP);
        }
    } while (dataVal == 1);
}

GT_BOOL mvHwsDataAndSyncSelect(GT_U8 devNum, GT_U32 interfaceId, GT_U32 pup)
{
    GT_U32 dataResult;
    GT_U32 syncResult;
    GT_U32 dataVal;

    /*Data Select*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB7, 0x1);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
    mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB2, &dataVal);
    dataResult = dataVal;

    /*Sync Select*/
    mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB7, 0x0);
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 10);
    mvHwsDdr3TipBUSRead(devNum, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0xB2, &dataVal);
    syncResult = dataVal;

    return ((dataResult == 0) && (syncResult == 0)) ? GT_TRUE : GT_FALSE;
}

GT_U32 mvHwsConfigureTxAdllOpt(GT_U8 devNum,GT_U32 interfaceId, GT_U32 pup, GT_U32 victBit, GT_U32 agrBits, GT_U32 dirrection)
{
	GT_U32 txAdll;
	GT_U32 endLoop;
    GT_BOOL isPass;

	mvPrintf("direction = %d\n",dirrection);
	if (dirrection) {
		endLoop = 64;
		txAdll = 5;
	} else {
		endLoop = 0;
		txAdll = 64;
	}

	/*Set Rx Adll to min*/
	mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x3, 0x0);

	while(txAdll != endLoop)
	{
        if (dirrection) {
            txAdll ++;
        } else {
            txAdll --;
        }

        /*Set Tx Adll*/
        mvHwsDdr3TipBUSWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ACCESS_TYPE_UNICAST, pup, DDR_PHY_DATA, 0x1, txAdll);

        /* Run I/O BIST test */
        mvHwsIoBistTestRun(devNum, interfaceId, pup, victBit, agrBits);

        /* Check if Sync and/or Data pass */
        isPass = mvHwsDataAndSyncSelect(devNum, interfaceId, pup);
        if(isPass)
        {
            /* looking for failure so continue the search */
            mvPrintf(".");
        }
        else
        {
            return txAdll;
        }
    }

    /* we shouldn't reach this point. if so, it's en error */
    mvPrintf("mvHwsConfigureTxAdllOpt: Error: couldn't find failure point\n");

    return 0;
}

void mvHwsIoBistTestRx(GT_U8 devNum, GT_U32 interfaceId, GT_U32 pup,GT_U32 victBit, GT_U32 agrBits, GT_U32 rxAdll)
{
    GT_BOOL isPass;

    /* Run I/O BIST test */
    mvHwsIoBistTestRun(devNum, interfaceId, pup, victBit, agrBits);

    mvPrintf("interfaceId: %d   PUP: %d   VictBit: %d  AgrBits: 0x%x  rxAdll: %d --> ",interfaceId, pup, victBit, agrBits, rxAdll);

    /* Check if Sync and/or Data pass */
    isPass = mvHwsDataAndSyncSelect(devNum, interfaceId, pup);
    if(isPass)
    {
        mvPrintf("PASS\n");
        ioBistCounter++;
    }
    else
    {
        mvPrintf("-----\n");
    }
}

#endif /* defined(MV_HWS_RX_IO_BIST) || defined(MV_HWS_RX_IO_BIST_ETP) */


