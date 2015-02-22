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
* ddr3_a380_training.c
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
#include "ddr3_a38x.h"
#include "ddr3_hws_sil_training.h"
#include "printf.h"

/************************** globals ***************************************/

extern ClValuePerFreq casLatencyTable[];
extern ClValuePerFreq casWriteLatencyTable[];

/************************** Enume *****************************************/

/************************** pre-declarations ******************************/

MV_STATUS ddr3SiliconGetDdrTargetFreq(MV_U32 *uiDdrFreq);

extern GT_STATUS ddr3TipInitA38x
(
    GT_U32  devNum,
    GT_U32  boardId
);

/************************** Globals ******************************/

/***************************************************************************/

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
	status = ddr3TipInitA38x(0, 0);
	if (MV_OK != status) {
		mvPrintf("DDR3 A38x silicon init - FAILED 0x%x\n", status);
		return status;
	}

	initDone = MV_TRUE;

	return MV_OK;
}






