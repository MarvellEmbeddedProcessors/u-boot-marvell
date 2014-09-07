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

#include "mvSysHwConfig.h"
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"


/* typedefs */
typedef MV_STATUS (*MV_WIN_GET_FUNC_PTR)(MV_U32, MV_U32, MV_UNIT_WIN_INFO*);

/* This enumerator describes the possible HW cache coherency policies the
 * controllers supports.
 */
typedef enum _mvCachePolicy {
	NO_COHERENCY,   /* No HW cache coherency support                        */
	WT_COHERENCY,   /* HW cache coherency supported in Write Through policy */
	WB_COHERENCY    /* HW cache coherency supported in Write Back policy    */
} MV_CACHE_POLICY;

typedef enum {
	SERDES_UNIT_NA		= 0x0,
	SERDES_UNIT_PEX		= 0x10,
	SERDES_UNIT_SATA	= 0x20,
	SERDES_UNIT_GBE		= 0x30,
	SERDES_UNIT_USB_H	= 0x40,
	SERDES_UNIT_USB		= 0x50,
	SERDES_UNIT_QSGMII	= 0x60,
	SERDES_UNIT_XAUI	= 0x70,
} MV_SERDES_UNIT_INDX;

typedef enum {
	PEX_BUS_DISABLED        = 0,
	PEX_BUS_MODE_X1         = 1,
	PEX_BUS_MODE_X4         = 2,
	PEX_BUS_MODE_X8         = 3
} MV_PEX_UNIT_CFG;
/*
	This enum should reflect the units numbers in register
	space which we will need when accessing the HW
*/

typedef enum {
	PEX0_IF,
	PEX1_IF,
	PEX2_IF,
	PEX3_IF,
	PEXIF_MAX
} MV_PEXIF_INDX;

/*
	This structure refrect registers:
	Serdes 0-6 selectors		0x183fc
*/

#ifdef CONFIG_ARMADA_38X
#define SERDES_CFG {	\
/* Lane 0 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_SATA | 0,	SERDES_UNIT_GBE  | 0,\
	      SERDES_UNIT_NA,		SERDES_UNIT_NA,		    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 1 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_PEX | PEX1_IF,	SERDES_UNIT_SATA | 0,\
	      SERDES_UNIT_GBE | 0,	SERDES_UNIT_GBE | 1,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 2 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_PEX | PEX2_IF,	SERDES_UNIT_SATA | 1,\
	      SERDES_UNIT_GBE | 1,	SERDES_UNIT_NA,		    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 3 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX3_IF, SERDES_UNIT_PEX | PEX3_IF,	SERDES_UNIT_SATA | 3,\
	      SERDES_UNIT_GBE | 2,	SERDES_UNIT_USB_H | 1,	    SERDES_UNIT_USB,		SERDES_UNIT_NA},     \
/* Lane 4 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_SATA | 1,	SERDES_UNIT_GBE  | 1,\
	      SERDES_UNIT_USB_H | 0,	SERDES_UNIT_USB,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 5 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX2_IF, SERDES_UNIT_SATA | 2,	SERDES_UNIT_GBE  | 2,\
	      SERDES_UNIT_USB_H | 1,	SERDES_UNIT_USB,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
}
#elif defined CONFIG_ARMADA_39X
#define SERDES_CFG {    \
/* Lane 0 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_SATA | 0,	SERDES_UNIT_GBE  | 0,\
	      SERDES_UNIT_GBE,		SERDES_UNIT_NA,		   SERDES_UNIT_NA,		SERDES_UNIT_NA,\
	      SERDES_UNIT_NA,		SERDES_UNIT_NA,		   SERDES_UNIT_NA},     \
/* Lane 1 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_PEX | PEX1_IF,	SERDES_UNIT_SATA | 0,\
	      SERDES_UNIT_GBE | 0,	SERDES_UNIT_GBE | 1,	   SERDES_UNIT_USB_H | 0,	SERDES_UNIT_QSGMII | 0,\
	      SERDES_UNIT_GBE | 0,	SERDES_UNIT_GBE | 0,	   SERDES_UNIT_NA},     \
/* Lane 2 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_PEX | PEX2_IF,	SERDES_UNIT_SATA | 1,\
	      SERDES_UNIT_GBE | 1,	SERDES_UNIT_GBE | 1,	   SERDES_UNIT_NA,		SERDES_UNIT_NA,\
	      SERDES_UNIT_NA,		SERDES_UNIT_NA,            SERDES_UNIT_NA},     \
/* Lane 3 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX3_IF, SERDES_UNIT_PEX | PEX3_IF,  SERDES_UNIT_SATA | 3,\
	      SERDES_UNIT_GBE | 2,	SERDES_UNIT_USB_H | 1,     SERDES_UNIT_USB,            SERDES_UNIT_GBE | 2,\
	      SERDES_UNIT_XAUI | 3,	SERDES_UNIT_NA,            SERDES_UNIT_NA},     \
/* Lane 4 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_NA,             SERDES_UNIT_GBE  | 1,\
	      SERDES_UNIT_USB_H | 0,	SERDES_UNIT_USB,           SERDES_UNIT_SATA | 2,       SERDES_UNIT_PEX | 2,\
	      SERDES_UNIT_GBE | 3,	SERDES_UNIT_XAUI | 2,      SERDES_UNIT_NA},     \
/* Lane 5 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX2_IF, SERDES_UNIT_SATA | 2,       SERDES_UNIT_GBE  | 2,\
	      SERDES_UNIT_USB_H | 1,	SERDES_UNIT_USB,           SERDES_UNIT_GBE | 2,        SERDES_UNIT_NA,\
	      SERDES_UNIT_XAUI | 1,	SERDES_UNIT_NA,		   SERDES_UNIT_NA},	\
/* Lane 6 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_GBE | 3,       SERDES_UNIT_NA,\
	      SERDES_UNIT_XAUI | 0,	SERDES_UNIT_NA,            SERDES_UNIT_NA,            SERDES_UNIT_NA,\
	      SERDES_UNIT_NA,		SERDES_UNIT_NA,            SERDES_UNIT_NA},     \
}
#endif


/* Termal Sensor Registers */
#define TSEN_STATE_REG						0xE4070
#define TSEN_STATE_OFFSET					31
#define TSEN_STATE_MASK						(0x1 << TSEN_STATE_OFFSET)

#define TSEN_CONF_REG						0xE4074
#define TSEN_CONF_RST_OFFSET					8
#define TSEN_CONF_RST_MASK					(0x1 << TSEN_CONF_RST_OFFSET)

#define TSEN_STATUS_REG						0xE4078
#define TSEN_STATUS_READOUT_VALID_OFFSET			10
#define TSEN_STATUS_READOUT_VALID_MASK				(0x1 << TSEN_STATUS_READOUT_VALID_OFFSET)
#define TSEN_STATUS_TEMP_OUT_OFFSET				0
#define TSEN_STATUS_TEMP_OUT_MASK				(0x3FF << TSEN_STATUS_TEMP_OUT_OFFSET)


/* BIOS Modes related defines */
#define SAR_CPU_FAB_GET(cpu, fab)       (((cpu & 0x7) << 21) | ((fab & 0xF) << 24))

/* NSS MPP related defines */
#define MPP_GROUP0_SMI_MDC_MASK					0x00FF0000
#define MPP_GROUP0_SMI_MDC_LEGACY_MODE				0x00110000
#define MPP_GROUP0_SMI_MDC_NSS_MODE				0x00770000

#define MPP_GROUP3_GE1_MASK					0xFFFFF000
#define MPP_GROUP3_GE1_LEGACY_MODE				0x22222000
#define MPP_GROUP3_GE1_NSS_MODE					0x88888000

#define MPP_GROUP4_GE1_MASK					0xFFF0000F
#define MPP_GROUP4_GE1_NSS_MODE					0x88800008

#define MPP_GROUP5_GE1_MASK					0x000000FF
#define MPP_GROUP5_GE1_NSS_MODE					0x00000088

/* mcspLib.h API list */
MV_STATUS mvCtrlCpuDdrL2FreqGet(MV_FREQ_MODE *freqMode);
MV_U32 mvCtrlbootSrcGet(void);
MV_BOOL mvCtrlPortIsSerdesSgmii(MV_U32 ethPort);
MV_U32 mvCtrlGetCpuNum(MV_VOID);
MV_U32 mvCtrlGetQuadNum(MV_VOID);
MV_STATUS mvCtrlUpdatePexId(MV_VOID);
MV_BOOL mvCtrlIsEepromEnabled(MV_VOID);
MV_STATUS mvCtrlEepromEnable(MV_BOOL enable);
MV_U32 mvCtrlSocUnitInfoNumGet(MV_UNIT_ID unit);
MV_U32 mvCtrlSocUnitInfoNumSet(MV_UNIT_ID unit, MV_U32 maxValue);
MV_STATUS mvCtrlEnvInit(MV_VOID);
MV_U32    mvCtrlMppRegGet(MV_U32 mppGroup);
MV_U32 mvCtrlGetJuncTemp(MV_VOID);
void mvCtrlNandClkSet(int nClock);

#if defined(MV_INCLUDE_PEX)
MV_U32 mvCtrlPexMaxIfGet(MV_VOID);
MV_U32 mvCtrlPexMaxUnitGet(MV_VOID);
#else
#define mvCtrlPciMaxIfGet()             1
#endif
MV_U32 mvCtrlPexActiveUnitNumGet(MV_VOID);

#if defined(MV_INCLUDE_PCI)
#define mvCtrlPciIfMaxIfGet()           1
#else
#define mvCtrlPciIfMaxIfGet()           (mvCtrlPexMaxIfGet())
#endif

MV_U32 mvCtrlEthMaxPortGet(MV_VOID);
MV_U8 mvCtrlEthMaxCPUsGet(MV_VOID);

#if defined(MV_INCLUDE_SATA)
MV_U32 mvCtrlSataMaxPortGet(MV_VOID);
MV_U32 mvCtrlSataMaxUnitGet(MV_VOID);
MV_BOOL mvCtrlIsActiveSataUnit(MV_U32 unitNumber);
#endif
#if defined(MV_INCLUDE_XOR)
MV_U32 mvCtrlXorMaxChanGet(MV_VOID);
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_USB)
MV_U32 mvCtrlUsbMaxGet(MV_VOID);
MV_U32 mvCtrlUsb3MaxGet(MV_VOID);
MV_VOID mvCtrlUtmiPhySelectorSet(MV_U32 usbUnitId);
#endif
#if defined(MV_INCLUDE_SDIO)
MV_U32 mvCtrlSdioSupport(MV_VOID);
#endif
MV_U32 mvCtrlTdmSupport(MV_VOID);
MV_U32 mvCtrlTdmMaxGet(MV_VOID);
MV_TDM_UNIT_TYPE mvCtrlTdmUnitTypeGet(MV_VOID);
MV_U32 mvCtrlTdmUnitIrqGet(MV_VOID);
MV_U32 mvCtrlDevFamilyIdGet(MV_U16 ctrlModel);
MV_U16 mvCtrlModelGet(MV_VOID);
MV_U8 mvCtrlRevGet(MV_VOID);
MV_STATUS mvCtrlNameGet(char *pNameBuff);
MV_U32 mvCtrlModelRevGet(MV_VOID);
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff);
MV_VOID mvCtrlAddrDecShow(MV_VOID);
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target);
MV_U32    ctrlSizeToReg(MV_U32 size, MV_U32 alignment);
MV_U32    ctrlRegToSize(MV_U32 regSize, MV_U32 alignment);
MV_U32    ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment);
MV_U32 mvCtrlSysRstLengthCounterGet(MV_VOID);
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable);
MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index);
MV_BOOL mvCtrlIsBootFromNOR(MV_VOID);
MV_BOOL mvCtrlIsBootFromSPI(MV_VOID);
MV_BOOL mvCtrlIsBootFromNAND(MV_VOID);
MV_BOOL mvCtrlIsDLBEnabled(MV_VOID);
MV_U32 mvCtrlDDRBudWidth(MV_VOID);
MV_BOOL mvCtrlDDRThruXbar(MV_VOID);
MV_BOOL mvCtrlDDRECC(MV_VOID);
MV_BOOL mvCtrlDDRECCPUP3(MV_VOID);
void mvCtrlNSSMppSwitch(void);

#endif /* __INCmvCtrlEnvLibh */
