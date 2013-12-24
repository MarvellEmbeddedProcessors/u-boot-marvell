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


#ifndef __INCmvCtrlEnvLibh
#define __INCmvCtrlEnvLibh

/* includes */
#include "mvSysHwConfig.h"
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
/*#include "boardEnv/mvBoardEnvLib.h"*/

/* 0 for Auto scan mode, 1 for manual. */
#define MV_INTERNAL_SWITCH_SMI_SCAN_MODE	0

/* typedefs */
typedef MV_STATUS(*MV_WIN_GET_FUNC_PTR)(MV_U32, MV_U32, MV_UNIT_WIN_INFO*);


typedef enum {
	PEX_BUS_DISABLED	= 0,
	PEX_BUS_MODE_X1		= 1,
	PEX_BUS_MODE_X4		= 2,
	PEX_BUS_MODE_X8		= 3
} MV_PEX_UNIT_CFG;


/* BIOS Modes related defines */

#define SAR0_BOOTWIDTH_OFFSET	3
#define SAR0_BOOTWIDTH_MASK		(0x3 << SAR0_BOOTWIDTH_OFFSET)
#define SAR0_BOOTSRC_OFFSET		5
#define SAR0_BOOTSRC_MASK		(0xF << SAR0_BOOTSRC_OFFSET)

#define SAR0_L2_SIZE_OFFSET		19
#define SAR0_L2_SIZE_MASK		(0x3 << SAR0_L2_SIZE_OFFSET)
#define SAR0_CPU_FREQ_OFFSET	21
#define SAR0_CPU_FREQ_MASK		(0x7 << SAR0_CPU_FREQ_OFFSET)
#define SAR0_FABRIC_FREQ_OFFSET	24
#define SAR0_FABRIC_FREQ_MASK	(0xF << SAR0_FABRIC_FREQ_OFFSET)
#define SAR0_CPU0CORE_OFFSET	31
#define SAR0_CPU0CORE_MASK		(0x1 << SAR0_CPU0CORE_OFFSET)
#define SAR1_CPU0CORE_OFFSET	0
#define SAR1_CPU0CORE_MASK		(0x1 << SAR1_CPU0CORE_OFFSET)

#define PEX_CLK_100MHZ_OFFSET	2
#define PEX_CLK_100MHZ_MASK     (0x1 << PEX_CLK_100MHZ_OFFSET)

#define SAR1_CPU_CORE_OFFSET	3
#define SAR1_CPU_CORE_MASK		(0x3 << SAR1_CPU_CORE_OFFSET)
#define SAR1_FABRIC_MODE_OFFSET	19
#define SAR1_FABRIC_MODE_MASK	(0x1 << SAR1_FABRIC_MODE_OFFSET)
#define SAR1_CPU_MODE_OFFSET	20
#define SAR1_CPU_MODE_MASK		(0x1 << SAR1_CPU_MODE_OFFSET)

#define SAR_CPU_FAB_GET(cpu, fab)	(((cpu & 0x7) << 21) | ((fab & 0xF) << 24))
#define BIOS_MODES_NUM			4

typedef struct {
	char *name;
	MV_U16 confId;
	MV_U16 code;
	MV_U8 l2size;
	MV_U8 cpuFreq;
	MV_U8 cpuFreqMode;
	MV_U8 fabricFreq;
	MV_U8 AltfabricFreq;
	MV_U8 fabricFreqMode;
	MV_U8 cpuEna;
	MV_U8 cpuEndianess;
	MV_U8 dramBusWidth;
	MV_U8 bootSource;
	MV_U8 bootWidth;
} MV_BIOS_MODE;

/* mcspLib.h API list */
MV_U32 mvCtrlSocUnitInfoNumGet(MV_UNIT_ID unit);
MV_U32 mvCtrlGetCpuNum(MV_VOID);
MV_BOOL mvCtrlIsValidSatR(MV_VOID);

MV_STATUS mvCtrlEnvInit(MV_VOID);
MV_U32    mvCtrlMppRegGet(MV_U32 mppGroup);

#if defined(MV_INCLUDE_PEX)
MV_U32	mvCtrlPexMaxIfGet(MV_VOID);
MV_U32	mvCtrlPexMaxUnitGet(MV_VOID);
#else
#define mvCtrlPexMaxIfGet()	(0)
#endif

#if defined(MV_INCLUDE_PCI)
MV_U32	mvCtrlPciMaxIfGet(MV_VOID);
#else
#define mvCtrlPciIfMaxIfGet()	(mvCtrlPexMaxIfGet())
#endif

MV_U32	  mvCtrlEthMaxPortGet(MV_VOID);
MV_U8	  mvCtrlEthMaxCPUsGet(MV_VOID);
#if defined(MV_INCLUDE_IDMA)
MV_U32 mvCtrlIdmaMaxUnitGet(MV_VOID);
MV_U32 mvCtrlIdmaMaxChanGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_XOR)
MV_U32 mvCtrlXorMaxChanGet(MV_VOID);
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_USB)
MV_U32 	  mvCtrlUsbMaxGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_LEGACY_NAND)
MV_U32	  mvCtrlNandSupport(MV_VOID);
#endif
#if defined(MV_INCLUDE_SDIO)
MV_U32	  mvCtrlSdioSupport(MV_VOID);
#endif
#if defined(MV_INCLUDE_TDM)
MV_U32	  mvCtrlTdmSupport(MV_VOID);
MV_U32	  mvCtrlTdmMaxGet(MV_VOID);
MV_UNIT_ID mvCtrlTdmUnitTypeGet(MV_VOID);
MV_U32    mvCtrlTdmUnitIrqGet(MV_VOID);
#endif
MV_U32 mvCtrlDevFamilyIdGet(MV_U16 ctrlModel);
MV_U16    mvCtrlModelGet(MV_VOID);
MV_U8     mvCtrlRevGet(MV_VOID);
MV_STATUS mvCtrlNameGet(char *pNameBuff);
MV_U32    mvCtrlModelRevGet(MV_VOID);
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff);
MV_VOID   mvCtrlAddrDecShow(MV_VOID);
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target);
MV_U32	  ctrlSizeToReg(MV_U32 size, MV_U32 alignment);
MV_U32	  ctrlRegToSize(MV_U32 regSize, MV_U32 alignment);
MV_U32	  ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment);
MV_U32	  mvCtrlSysRstLengthCounterGet(MV_VOID);
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);

MV_VOID   mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable);
MV_BOOL	  mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index);
MV_BOOL	  mvCtrlIsBootFromNOR(MV_VOID);
MV_BOOL	  mvCtrlIsBootFromSPI(MV_VOID);
MV_BOOL	  mvCtrlIsBootFromNAND(MV_VOID);
MV_BOOL	  mvCtrlPwrMemGet(MV_UNIT_ID unitId, MV_U32 index);
MV_STATUS mvCtrlSerdesPhyConfig(MV_VOID);

MV_U32 mvCtrlDDRBudWidth(MV_VOID);
MV_BOOL mvCtrlDDRThruXbar(MV_VOID);
MV_BOOL mvCtrlDDRECC(MV_VOID);
MV_U32 mvCtrlGetJuncTemp(MV_VOID);
void mvCtrlGetPexActive(MV_BOOL *pPexActive, int size);
void mvCtrlNandClkSet(int nClock);

#endif /* __INCmvCtrlEnvLibh */
