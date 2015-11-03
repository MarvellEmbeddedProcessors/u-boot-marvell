
/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
		used to endorse or promote products derived from this software without
		specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#ifndef _DDR3_LOGGING_CONFIG_H
#define _DDR3_LOGGING_CONFIG_H


#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
#include <common/os/hwsEnv.h>
#define mvPrintf	hwsPrintf
#elif defined(WIN32)
#define mvPrintf printf
#else /* RTOS or uBoot */
#include "printf.h"
#define osPrintf mvPrintf
#endif


#ifdef SILENT_LIB
#define DEBUG_TRAINING_BIST_ENGINE(level,s)
#define DEBUG_TRAINING_IP(level,s)
#define DEBUG_CENTRALIZATION_ENGINE(level,s)
#define DEBUG_TRAINING_HW_ALG(level,s)
#define DEBUG_TRAINING_IP_ENGINE(level,s)
#define DEBUG_LEVELING(level,s)
#define DEBUG_PBS_ENGINE(level,s)
#define DEBUG_TRAINING_STATIC_IP(level,s)
#define DEBUG_TRAINING_ACCESS(level,s)
#else
#ifdef LIB_FUNCTIONAL_DEBUG_ONLY
#define DEBUG_TRAINING_BIST_ENGINE(level,s)
#define DEBUG_TRAINING_IP_ENGINE(level,s)
#define DEBUG_TRAINING_IP(level,s) if (level >= debugTraining) {osPrintf s;}
#define DEBUG_CENTRALIZATION_ENGINE(level,s)	if (level >= debugCentralization) {osPrintf s;}
#define DEBUG_TRAINING_HW_ALG(level,s) if (level >= debugTrainingHwAlg) {osPrintf s;}
#define DEBUG_LEVELING(level,s) if (level >= debugLeveling) {osPrintf s;}
#define DEBUG_PBS_ENGINE(level,s) if (level >= debugPbs) {osPrintf s;}
#define DEBUG_TRAINING_STATIC_IP(level,s) if (level >= debugTrainingStatic) {osPrintf s;}
#define DEBUG_TRAINING_ACCESS(level,s) if (level >= debugTrainingAccess) {osPrintf s;}
#else
#define DEBUG_TRAINING_BIST_ENGINE(level,s) if (level >= debugTrainingBist) {osPrintf s;} 
#define DEBUG_TRAINING_IP_ENGINE(level,s) if (level >= debugTrainingIp) {osPrintf s;} 
#define DEBUG_TRAINING_IP(level,s) if (level >= debugTraining) {osPrintf s;} 
#define DEBUG_CENTRALIZATION_ENGINE(level,s) if (level >= debugCentralization) {osPrintf s;} 
#define DEBUG_TRAINING_HW_ALG(level,s) if (level >= debugTrainingHwAlg) {osPrintf s;}
#define DEBUG_LEVELING(level,s) if (level >= debugLeveling) {osPrintf s;} 
#define DEBUG_PBS_ENGINE(level,s) if (level >= debugPbs) {osPrintf s;} 
#define DEBUG_TRAINING_STATIC_IP(level,s) if (level >= debugTrainingStatic) {osPrintf s;} 
#define DEBUG_TRAINING_ACCESS(level,s) if (level >= debugTrainingAccess) {osPrintf s;} 

#endif
#endif

#ifdef CONFIG_DDR4

#ifdef SILENT_LIB
#define DEBUG_TAP_TUNING_ENGINE(level,s)
#define DEBUG_CALIBRATION(level,s)
#define DEBUG_DDR4_CENTRALIZATION(level,s)
#else
#define DEBUG_TAP_TUNING_ENGINE(level,s) if (level >= debugTapTuning) {osPrintf s;}
#define DEBUG_CALIBRATION(level,s)	if (level >= debugCalibration) {osPrintf s;}
#define DEBUG_DDR4_CENTRALIZATION(level,s)	if (level >= debugDdr4Centralization) {osPrintf s;}
#endif

#endif

/**********Logging defines***************/
#define DEBUG_LEVEL_TRACE (1)
#define DEBUG_LEVEL_INFO  (2)
#define DEBUG_LEVEL_ERROR (3)

typedef enum
{
    MV_DEBUG_BLOCK_STATIC,
    MV_DEBUG_BLOCK_TRAINING_MAIN,
    MV_DEBUG_BLOCK_LEVELING,
    MV_DEBUG_BLOCK_CENTRALIZATION,
    MV_DEBUG_BLOCK_PBS,
    MV_DEBUG_BLOCK_IP,
    MV_DEBUG_BLOCK_BIST,
    MV_DEBUG_BLOCK_ALG,
	MV_DEBUG_BLOCK_DEVICE,
	MV_DEBUG_BLOCK_ACCESS,
	MV_DEBUG_STAGES_REG_DUMP,
#ifdef CONFIG_DDR4
	MV_DEBUG_TAP_TUNING,
    MV_DEBUG_BLOCK_CALIBRATION,
    MV_DEBUG_BLOCK_DDR4_CENTRALIZATION,
#endif
    MV_DEBUG_BLOCK_ALL /*All excluding IP and REG_DUMP, should be enabled separatelly*/
}MV_DDR_LIB_DEBUG_BLOCK;
/***************************************/

GT_STATUS ddr3TipPrintLog(GT_U32 devNum, GT_U32 memAddr);
GT_STATUS	ddr3TipPrintStabilityLog(GT_U32 devNum);

#endif /* _DDR3_LOGGING_CONFIG_H */

