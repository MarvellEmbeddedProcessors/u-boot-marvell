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
* ddr3_hws_hw_training.h
*
* DESCRIPTION: DDR3 adapter for ddr3_init to TrainingIp
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef _DDR3_HWS_HW_TRAINING_H
#define _DDR3_HWS_HW_TRAINING_H

#include "mv_os.h"

MV_STATUS ddr3HwsHwTraining(void);
MV_STATUS ddr3SiliconPreInit(void);
MV_STATUS ddr3SiliconPostInit(void);
MV_STATUS ddr3PostRunAlg(void);
MV_BOOL ddr3IfEccEnabled(void);
MV_STATUS ddr3NewTipEccScrub(void);

#endif /* _DDR3_HWS_HW_TRAINING_H */
