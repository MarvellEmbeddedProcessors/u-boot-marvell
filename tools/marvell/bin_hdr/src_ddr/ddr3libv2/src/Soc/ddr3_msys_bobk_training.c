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
* ddr3_msys_training.c
*
* DESCRIPTION: 
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
***************************************************************************/

#include "mv_os.h"
#include "ddr3_msys_bobk.h"
#include "ddr3_msys_bobk_config.h"
#include "ddr3_hws_sil_training.h"
#include "mvSiliconIf.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvHwsDdr3BobK.h"
#include "soc_spec.h"
#include "printf.h"

/************************** globals ***************************************/

extern MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr;
extern MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr;

GT_U32 serverBaseAddr = 0;
/* the following global variables are set in ddr3TipDynamicReadLeveling */


extern GT_U32 genericInitController;
extern GT_U32 debugBobK;

/************************** pre-declarations ******************************/
GT_STATUS    ddr3DunitAccessRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
);

GT_STATUS    ddr3DunitAccessWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
);

GT_STATUS ddr3DunitAccessInit
(
    GT_U32  devNum,
    GT_U32  boardId
);

MV_VOID configureServerWindows(void);

extern GT_STATUS ddr3TipBobKGetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
);

extern GT_STATUS ddr3TipBobKSetDivider
(
	GT_U8							devNum,
	GT_U32							interfaceId,
    MV_HWS_DDR_FREQ                 freq
);

extern GT_STATUS ddr3TipInitBobKSilicon
(
    GT_U32  devNum,
    GT_U32  boardId
);
extern GT_STATUS ddr3TipBobKGetInitFreq
(
    GT_STATUS       devNum,
	GT_U32			interfaceId,
    MV_HWS_DDR_FREQ *freq
);

GT_STATUS    ddr3TipBobKSelectCPUDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable
);

/***************************************************************************/

/******************************************************************************
* Name:     serverRegSet.
* Desc:     definition of server write prototype
* Args:     TBD
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS serverRegSet
(
    GT_U8 devNum,
    GT_U32 addr,
    GT_U32 data
)
{
	if(serverBaseAddr == 0) {
		mvPrintf("Server windows were not initialized\n", 0);
		return GT_NOT_INITIALIZED;
	}

	*((volatile unsigned int*)(serverBaseAddr+addr)) = data;
    //mvPrintf("serverRegSet 0x%x =0x%x\n",addr,data);

	return GT_OK;
}

/******************************************************************************
* Name:     serverRegGet.
* Desc:     definition of server read prototype
* Args:     TBD
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS serverRegGet
(
    GT_U8 devNum, 
    GT_U32 addr, 
    GT_U32 *data
)
{
	if(serverBaseAddr == 0) {
		mvPrintf("Server windows were not initialized\n", 0);
		return GT_NOT_INITIALIZED;
	}

	*data = (*((volatile unsigned int*)(serverBaseAddr+addr)));
	//mvPrintf("serverRegGet 0x%x =0x%x\n",addr,*data);

	return GT_OK;
}

MV_STATUS ddr3SiliconPreInit(void)
{
    MV_STATUS status;
	/* initialize window to server */
	configureServerWindows();

#if 0
	MV_U32 uiReg;
	/* set dupicate CS configuration */
	serverRegGet(0, 0xf8288, &uiReg);
	uiReg |= (1 << 17);
	serverRegSet(0, 0xf8288, uiReg);
#endif

    status = ddr3SiliconInit();
	if (MV_OK != status) {
		mvPrintf("DDR3 silicon init - FAILED 0x%x\n", status);
		return status;
	}

	return MV_OK;
}

MV_STATUS ddr3SiliconPostInit(void)
{
	return MV_OK;
}

/******************************************************************************
 * Name:     ddr3TipInitSilicon
 * Desc:     initiate silicon parameters
 * Args:
 * Notes:
 * Returns:  required value
 */
MV_STATUS ddr3SiliconInit(void) {

	MV_STATUS status;

	genericInitController = 1;

	status = ddr3TipInitBobK(0, 0);
	if (MV_OK != status) {
		mvPrintf("DDR3 Msys silicon init - FAILED 0x%x\n", status);
		return status;
	}

	return MV_OK;
}

MV_STATUS ddr3PostRunAlg(void)
{
    MV_U32 readDelay[5];
	GT_U32 gReadSampleDelay = 0x19191919;
	GT_U32 gReadReadyDelay = 0x19191919;

    CHECK_STATUS(mvHwsDdr3TipIFRead(0, ACCESS_TYPE_UNICAST, 4, REG_READ_DATA_SAMPLE_DELAYS_ADDR,
                                    readDelay, MASK_ALL_BITS));

    gReadSampleDelay = readDelay[4];

    CHECK_STATUS(mvHwsDdr3TipIFRead(0, ACCESS_TYPE_UNICAST, 4, REG_READ_DATA_READY_DELAYS_ADDR, 
                                    readDelay, MASK_ALL_BITS));

    gReadReadyDelay = readDelay[4];

    CHECK_STATUS(ddr3TipBobKSelectCPUDdrController(0, GT_FALSE)); // set mux to msys

    MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR, gReadSampleDelay);
    MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR, gReadReadyDelay);
	return MV_OK;
}

/********************************************/
/*********** Msys D-Unit Access    **********/
/********************************************/


GT_STATUS    ddr3DunitAccessWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
)
{
	GT_U32 uiDataRead;
    GT_U32 dataRead[MAX_INTERFACE_NUM];
#ifdef WIN32
    mvPrintf(DEBUG_LEVEL_TRACE,"ddr3DunitAccessWrite   0x%x: 0x%x (mask 0x%x)\n", regAddr, dataValue);
    return GT_OK;
#endif
    if (mask != MASK_ALL_BITS)
    {
        if (debugBobK >= 2)
        {
            mvPrintf("ddr3DunitAccessWrite active interface = %d\n",interfaceId);
        }
        CHECK_STATUS(ddr3DunitAccessRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  dataRead, MASK_ALL_BITS));
        uiDataRead = dataRead[interfaceId];
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }

    if (debugBobK >= 1)
    {
        mvPrintf("0x%x: 0x%x\n", regAddr, dataValue);
    }

    MV_REG_WRITE(regAddr, dataValue);

    return GT_OK;
}

GT_STATUS    ddr3DunitAccessRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                *data,
    GT_U32                mask
)
{
#ifdef WIN32
     mvPrintf("ddr3DunitAccessRead   0x%x: 0x%x (mask 0x%x)\n", regAddr, *data);
     return GT_OK;
#endif

	data[interfaceId] = MV_REG_READ(regAddr) & mask;

    return GT_OK;
}

GT_STATUS ddr3DunitAccessInit
(
    GT_U32  devNum,
    GT_U32  boardId
)
{
	MV_HWS_TIP_CONFIG_FUNC_DB configFunc;

    configFunc.tipDunitReadFunc = ddr3DunitAccessRead;
    configFunc.tipDunitWriteFunc = ddr3DunitAccessWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipBobKSelectCPUDdrController;
    configFunc.tipSetFreqDividerFunc = ddr3TipBobKSetDivider;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipBobKGetFreqConfig;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	return GT_OK;
}

MV_VOID configureServerWindows(void)
{
	static MV_BOOL configDone = MV_FALSE;

	if(configDone == MV_TRUE)
		return;

    serverBaseAddr = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(1));

    /* init server access */
	hwsServerRegSetFuncPtr = serverRegSet;
	hwsServerRegGetFuncPtr = serverRegGet;

	configDone = MV_TRUE;
}

/*****************************************************************************/
GT_U32 ddr3TipGetInitFreq()
{
    MV_HWS_DDR_FREQ freq;

	configureServerWindows();

	ddr3TipBobKGetInitFreq(0, 0, &freq); /* In BobK interface 0 is the CPU interface*/

	return freq;

}



