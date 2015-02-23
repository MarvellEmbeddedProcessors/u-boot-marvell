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
* ddr3_msys_ac3_training.c
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
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#include "ddr3_hws_sil_training.h"
#include "mvSiliconIf.h"
#include "soc_spec.h"
#include "printf.h"

/************************** globals ***************************************/

extern MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr;
extern MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr;

GT_U32 serverBaseAddr = 0;

/************************** pre-declarations ******************************/

MV_STATUS ddr3SiliconGetDdrTargetFreq(MV_U32 *uiDdrFreq);

extern GT_STATUS ddr3TipInitAc3
(
    GT_U32  devNum,
    GT_U32  boardId
);

extern MV_HWS_TOPOLOGY_MAP*    ddr3TipGetTopologyMap
(
    GT_U32  devNum
);

/************************** Globals ******************************/

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
		/* save server base address */
		serverBaseAddr = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(1));
		//mvPrintf("configureServerWindows: Server base is 0x%x\n", serverBaseAddr);
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
		/* save server base address */
		serverBaseAddr = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(1));
		//mvPrintf("configureServerWindows: Server base is 0x%x\n", serverBaseAddr);
	}

	*data = (*((volatile unsigned int*)(serverBaseAddr+addr)));
	//mvPrintf("serverRegGet 0x%x =0x%x\n",addr,*data);

	return GT_OK;
}
MV_STATUS ddr3SiliconPreConfig(void) {

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
	static MV_BOOL initDone = MV_FALSE;

	if (initDone == MV_TRUE) {
		return MV_OK;
	}

	/* init server access */
	hwsServerRegSetFuncPtr = serverRegSet;
	hwsServerRegGetFuncPtr = serverRegGet;

	status = ddr3TipInitAc3(0, 0);
	if (MV_OK != status) {
		mvPrintf("DDR3 Ac3 silicon init - FAILED 0x%x\n", status);
		return status;
	}

	initDone = MV_TRUE;

	return MV_OK;
}

/********************************************/
/*********** Ac3 SoC init          **********/
/********************************************/

MV_STATUS ddr3SiliconPreInit(void)
{
	MV_STATUS result;

	result = ddr3SiliconInit();

	return result;
}
MV_STATUS ddr3PostRunAlg(void)
{
	return MV_OK;
}

MV_STATUS ddr3SiliconPostInit(void)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(0);

	/*Set half bus width*/
	if(DDR3_IS_16BIT_DRAM_MODE(topologyMap->activeBusMask)){
		CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, REG_SDRAM_CONFIG_ADDR, 0x0, 0x8000));
	}

	return MV_OK;
}
