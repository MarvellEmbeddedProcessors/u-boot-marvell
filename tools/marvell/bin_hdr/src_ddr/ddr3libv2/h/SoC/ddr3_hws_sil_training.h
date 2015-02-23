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
* ddr3_hws_sil_training.h
*
* DESCRIPTION: DDR3 adapter for ddr3_init to TrainingIp
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef _DDR3_HWS_SIL_TRAINING_H
#define _DDR3_HWS_SIL_TRAINING_H

#include "mv_os.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpPrvIf.h"

typedef struct {
	MV_U32 				uiActiveIf;
	MV_HWS_SPEED_BIN	uiSpeedBin; 
	MV_HWS_BUS_WIDTH	uiBusWidth;
	MV_HWS_MEM_SIZE		uiMemSize;
	MV_U32				uiCL;
    MV_U32				uiCWL;
}MV_DDR3_TOPOLOGY_INFO;

MV_STATUS	ddr3SiliconPreConfig(void);
MV_STATUS	ddr3SiliconInit(void);
MV_STATUS	ddr3SiliconGetDdrTargetFreq(MV_U32 *uiDdrFreq);       
MV_STATUS	ddr3SiliconGetTopologyUpdate(MV_DDR3_TOPOLOGY_INFO *psTopologyInfo);

#endif /* _DDR3_HWS_SIL_TRAINING_H */

