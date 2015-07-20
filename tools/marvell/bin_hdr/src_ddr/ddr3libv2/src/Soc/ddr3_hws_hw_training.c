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
* ddr3_hws_hw_training.c
*
* DESCRIPTION: DDR3 adapter for ddr3_init to TrainingIp
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#include "ddr3_init.h"
#include "ddr3_hws_hw_training.h"
#include "ddr3_hws_hw_training_def.h"
#include "ddr3_hws_sil_training.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvSiliconIf.h"
#include "printf.h"

#define REG_READ_DATA_SAMPLE_DELAYS_ADDR		0x1538
#define REG_READ_DATA_SAMPLE_DELAYS_MASK		0x1F
#define REG_READ_DATA_SAMPLE_DELAYS_OFFS		8

#define REG_READ_DATA_READY_DELAYS_ADDR			0x153C
#define REG_READ_DATA_READY_DELAYS_MASK			0x1F
#define REG_READ_DATA_READY_DELAYS_OFFS			8

/************************** globals **************************************/
extern MV_HWS_TOPOLOGY_MAP*    ddr3TipGetTopologyMap
( 
    GT_U32  devNum
);

MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr; 
MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr; 

extern MV_U32 mvBoardIdGet(MV_VOID);

MV_BOOL ddr3IfEccEnabled(void)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap;
	MV_U8 	devNum = 0;

	topologyMap = ddr3TipGetTopologyMap(devNum);;

	if( DDR3_IS_ECC_PUP4_MODE(topologyMap->activeBusMask) || DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask) )
		return MV_TRUE;
	else
		return MV_FALSE;
}

MV_STATUS ddr3PreAlgoConfig(void)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap;
	MV_U8 	devNum = 0;

	topologyMap = ddr3TipGetTopologyMap(devNum);;

	/*Set Bus3 ECC training mode*/
	if(DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask)){
		/*Set Bus3 ECC MUX*/
    	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, REG_SDRAM_PINS_MUX, 0x100, 0x100));
	}

	/*Set regular ECC training mode (bus4 and bus 3)*/
	if( (DDR3_IS_ECC_PUP4_MODE(topologyMap->activeBusMask)) || (DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask))){
		/*Enable ECC Write MUX*/
    	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x100, 0x100));
		/*General ECC enable*/
    	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, REG_SDRAM_CONFIG_ADDR, 0x40000, 0x40000));
		/*Disable Read Data ECC MUX*/
    	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x0, 0x2));
	}

	return MV_OK;
}

MV_STATUS ddr3PostAlgoConfig(void)
{
	MV_STATUS status;
	MV_HWS_TOPOLOGY_MAP* topologyMap;
	MV_U8 	devNum = 0;

	topologyMap = ddr3TipGetTopologyMap(devNum);;

    status = ddr3PostRunAlg();  
	if (MV_OK != status) {
		mvPrintf("DDR3 Post Run Alg - FAILED 0x%x\n", status);
		return status;
	}	

	/*UnSet ECC training mode*/
	if( (DDR3_IS_ECC_PUP4_MODE(topologyMap->activeBusMask)) || (DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask))){
		/*Disable ECC Write MUX*/
    	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, TRAINING_SW_2_REG, 0x0, 0x100));
		/*General ECC and Bus3 ECC MUX remains enabled*/
	}

	return MV_OK;
}

MV_STATUS ddr3HwsHwTraining(void)
{
	MV_HWS_ALGO_TYPE algoMode = ALGO_TYPE_DYNAMIC;
	MV_STATUS status;
	InitCntrParam initParam;

	status = ddr3SiliconPreInit();
	if (MV_OK != status) {
		mvPrintf("DDR3 Pre silicon Config - FAILED 0x%x\n", status);
		return status;
	}

	initParam.doMrsPhy = GT_TRUE;
#if !defined (CONFIG_BOBCAT2)
	initParam.isCtrl64Bit = GT_FALSE;
#else
	initParam.isCtrl64Bit = GT_TRUE;
#endif
#if !defined (CONFIG_BOBCAT2)
    initParam.initPhy = GT_TRUE;
#else
    initParam.initPhy = GT_FALSE;
#endif
    initParam.msysInit = GT_TRUE;
    status = mvHwsDdr3TipInitController(0, &initParam);
	if (MV_OK != status) {
		mvPrintf("DDR3 init controller - FAILED 0x%x\n", status);
		return status;
	}

    status = ddr3SiliconPostInit();
	if (MV_OK != status) {
		mvPrintf("DDR3 Post Init - FAILED 0x%x\n", status);
		return status;
	}

    status = ddr3PreAlgoConfig();
	if (MV_OK != status) {
		mvPrintf("DDR3 Pre Algo Config - FAILED 0x%x\n", status);
		return status;
	}		

	/* run algorithm in order to configure the PHY */
	status = mvHwsDdr3TipRunAlg(0, algoMode);
	if (MV_OK != status) {
		mvPrintf("DDR3 run algorithm - FAILED 0x%x\n", status);
		return status;
	}

    status = ddr3PostAlgoConfig();  
	if (MV_OK != status) {
		mvPrintf("DDR3 Post Algo Config - FAILED 0x%x\n", status);
		return status;
	}		

	return MV_OK;
   
}

