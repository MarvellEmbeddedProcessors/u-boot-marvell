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
#ifndef _INC_DDR_H
#define _INC_DDR_H

#include "mv_os.h"

#include "config_marvell.h"     /* Required to identify SOC and Board */

#ifdef MV88F78X60
#include "ddr3_axp.h"
#elif defined(MV88F67XX)
#include "ddr3_a370.h"
#elif defined(MV88F66XX)
#include "ddr3_alp.h"
#elif defined(MV88F672X)
#include "ddr3_a375.h"
#elif defined(MV_MSYS_BC2) || defined(MV_MSYS_AC3)
#include "ddr3_msys_bc2.h"
#endif

/*DRR training Error codes*/
/*Stage 0 errors*/
#define MV_DDR3_TRAINING_ERR_BAD_SAR            0xDD300001
/*Stage 1 errors*/
#define MV_DDR3_TRAINING_ERR_TWSI_FAIL              0xDD301001
#define MV_DDR3_TRAINING_ERR_DIMM_TYPE_NO_MATCH     0xDD301001
#define MV_DDR3_TRAINING_ERR_TWSI_BAD_TYPE          0xDD301003
#define MV_DDR3_TRAINING_ERR_BUS_WIDTH_NOT_MATCH    0xDD301004
#define MV_DDR3_TRAINING_ERR_BAD_DIMM_SETUP         0xDD301005
#define MV_DDR3_TRAINING_ERR_MAX_CS_LIMIT           0xDD301006
#define MV_DDR3_TRAINING_ERR_MAX_ENA_CS_LIMIT       0xDD301007
#define MV_DDR3_TRAINING_ERR_BAD_R_DIMM_SETUP       0xDD301008
/*Stage 2 errors*/
#define MV_DDR3_TRAINING_ERR_HW_FAIL_BASE           0xDD302000

typedef enum  _mvConfigType {
    CONFIG_ECC,
    CONFIG_MULTI_CS,
    CONFIG_BUS_WIDTH
} MV_CONFIG_TYPE;

typedef enum  {
    MV_LOG_LEVEL_0,
    MV_LOG_LEVEL_1,
    MV_LOG_LEVEL_2,
    MV_LOG_LEVEL_3
} MV_LOG_LEVEL;


MV_STATUS   ddr3HwTraining(MV_U32 uiTargetFreq, MV_U32 uiDdrWidth,
                        MV_BOOL bXorBypass, MV_U32 uiScrubOffs, MV_U32 uiScrubSize, MV_BOOL bDQSCLKAligned,
                        MV_BOOL bDebugMode, MV_BOOL bRegDimmSkipWL);

MV_VOID     ddr3PrintVersion(void);

MV_VOID     fixPLLValue(MV_U8 targetFabric);
MV_U8       ddr3GetEpromFabric(void);

MV_VOID     uDelay(MV_U32 uiDelay);

MV_U32      ddr3GetFabOpt(void);
MV_U32      ddr3GetCpuFreq(void);
MV_U32      ddr3GetVCOFreq(void);
MV_BOOL     ddr3CheckConfig(MV_U32 twsiAddr, MV_CONFIG_TYPE configType);
MV_U32 ddr3GetStaticMCValue(MV_U32 regAddr, MV_U32 offset1, MV_U32 mask1, MV_U32 offset2, MV_U32 mask2);

MV_U32 ddr3CLtoValidCL(MV_U32 uiCL);
MV_U32 ddr3ValidCLtoCL(MV_U32 uiValidCL);
MV_U32 ddr3GetCSNumFromReg(void);
MV_U32 ddr3GetCSEnaFromReg(void);
MV_U8 mvCtrlRevGet(MV_VOID);
MV_VOID     levelLogPrintS(char *str,MV_LOG_LEVEL eLogLevel);
MV_VOID     levelLogPrintD(MV_U32 dec_num,MV_U32 length,MV_LOG_LEVEL eLogLevel);
MV_VOID     levelLogPrintDD(MV_U32 dec_num,MV_U32 length,MV_LOG_LEVEL eLogLevel);
MV_VOID     printDunitSetup(void);
MV_VOID ddr3SetDqsResultsPrintStatus(MV_U32 status);
MV_U32      ddr3GetLogLevel(void);
MV_VOID		ddr3NewTipDlbConfig(void);
MV_VOID		ddr3GetAlpBusWidth(void);
MV_VOID		ddr3GetA375BusWidth(void);
#endif /* _INC_DDR_H */
