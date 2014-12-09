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

/*struct used for DLB configuration array*/
typedef struct __mvDramDLBConfig {
	MV_U32 regAddr;
	MV_U32 regData;
} MV_DRAM_DLB_CONFIG;

/*Topology update structure*/
typedef struct topologyUpdateInfo{
	MV_BOOL 	mvUpdateECC;
	MV_U8 		mvECC;
	MV_BOOL 	mvUpdateWidth;
	MV_U8 		mvWidth;
} MV_TOPOLOGY_UPDATE_INFO;

/*Topology update defines*/
#define MV_TOPOLOGY_UPDATE_WIDTH_16BIT 			1
#define MV_TOPOLOGY_UPDATE_WIDTH_32BIT 			0
#define MV_TOPOLOGY_UPDATE_WIDTH_ALL_BIT_MASK	0xF
#define MV_TOPOLOGY_UPDATE_WIDTH_32BIT_MASK		0xF
#define MV_TOPOLOGY_UPDATE_WIDTH_16BIT_MASK		0x3

#define MV_TOPOLOGY_UPDATE_ECC_ON				1
#define MV_TOPOLOGY_UPDATE_ECC_OFF				0
#define MV_TOPOLOGY_UPDATE_ECC_OFFSET			4

#define L2_FILTER_FOR_MAX_MEMORY_SIZE 0xD0000000
#define ADDRESS_FILTERING_END_REGISTER 0x8c04

#define SUB_VERSION	0

#endif /* _DDR3_HWS_HW_TRAINING_H */
