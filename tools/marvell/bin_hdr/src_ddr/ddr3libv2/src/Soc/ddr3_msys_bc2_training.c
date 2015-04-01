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
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#include "ddr3_hws_sil_training.h"
#include "mvSiliconIf.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvHwsDdr3Bc2.h"
#include "printf.h"

/************************** globals ***************************************/

extern MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr;
extern MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr;

GT_U32 serverBaseAddr = 0;
/* the following global variables are set in ddr3TipDynamicReadLeveling */


extern GT_U32 genericInitController;


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

/************************** Globals ******************************/
extern GT_STATUS    ddr3TipBc2SelectDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable
);

extern GT_STATUS ddr3TipBc2GetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
);

extern GT_STATUS ddr3TipBc2SetDivider
(
	GT_U8							devNum,
	GT_U32							interfaceId,
    MV_HWS_DDR_FREQ                 freq
);

extern GT_STATUS ddr3TipInitBc2Silicon
(
    GT_U32  devNum,
    GT_U32  boardId
);
extern GT_STATUS ddr3TipBc2GetInitFreq
(
    GT_STATUS       devNum,
	GT_U32			interfaceId,
    MV_HWS_DDR_FREQ *freq
);
extern GT_U32 debugBc2;
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
	MV_U32 uiReg;
	MV_U8 devNum = 0;

	/* initialize window to server */
	configureServerWindows();

	if (ddr3GetSdramAssignment(devNum) == TM_EN) {
		mvPrintf("Error: DDR3 interface is used by Traffic Manager\n", 0);
		return MV_FAIL;
	}

	mvPrintf("DDR3 interface is used by MSYS CPU\n", 0);

	/* set dupicate CS configuration */
	serverRegGet(0, 0xf8288, &uiReg);
	uiReg |= (1 << 17);
	serverRegSet(0, 0xf8288, uiReg);

    status = ddr3SiliconInit();
	if (MV_OK != status) {
		mvPrintf("DDR3 silicon init - FAILED 0x%x\n", status);
		return status;
	}

    CHECK_STATUS(mvHwsDdr3TipSelectDdrController(0, GT_FALSE)); // set mux to msys

	/* configure Dunit */
	status = ddr3DunitAccessInit(0, 1);
	if (MV_OK != status) {
		mvPrintf("DDR3 Dunit Access Init - FAILED 0x%x\n", status);
		return status;
	}

	return MV_OK;
}

MV_STATUS ddr3SiliconPostInit(void) 
{
    MV_STATUS status;	


	status = ddr3SiliconInit();
	if (MV_OK != status) {
		mvPrintf("DDR3 silicon init - FAILED 0x%x\n", status);
		return status;
	}


  	CHECK_STATUS(mvHwsDdr3TipSelectDdrController(0, GT_TRUE)); // set mux to tip                    

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

	//status = ddr3TipInitBc2Silicon(0, 1);
	status = ddr3TipInitBc2(0, 0);
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

    CHECK_STATUS(mvHwsDdr3TipSelectDdrController(0, GT_FALSE)); // set mux to msys

    MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR, gReadSampleDelay);
    MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR, gReadReadyDelay);
	return MV_OK;
}

/********************************************/
/*********** Msys D-Unit Access    **********/
/********************************************/


//configFuncInfo[devNum]->tipDunitReadReadFunc(devNum, interfaceAccess, interfaceId, regAddr, data, mask);
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
        if (debugBc2 >= 2)
        {
            mvPrintf("ddr3TipBc2IFWrite active interface = %d\n",interfaceId);
        }
        CHECK_STATUS(ddr3DunitAccessRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  dataRead, MASK_ALL_BITS));
        uiDataRead = dataRead[interfaceId];
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }

    if (debugBc2 >= 1)
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
	if (debugBc2 >= 2)
	{
		mvPrintf("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  *data, mask);
	}

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
    configFunc.tipDunitMuxSelectFunc = ddr3TipBc2SelectDdrController;
    configFunc.tipSetFreqDividerFunc = ddr3TipBc2SetDivider;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipBc2GetFreqConfig;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	return GT_OK;
}

MV_VOID configureServerWindows(void)
{
	static MV_BOOL configDone = MV_FALSE;

	if(configDone == MV_TRUE)
		return;

	MV_REG_WRITE(REG_XBAR_WIN_5_CTRL_ADDR, 0xF0081);

	serverBaseAddr = MV_REG_READ(REG_XBAR_WIN_5_BASE_ADDR);

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

	ddr3TipBc2GetInitFreq(0, 4, &freq); /* In BC2 interface 4 is the CPU interface*/

	return freq;

}



