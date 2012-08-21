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

/* includes */
#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "mvSysEthConfig.h"

#if defined(MV_INCLUDE_PEX)
#include "pex/mvPex.h"
#include "pex/mvPexRegs.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(MV_ETH_LEGACY)
#include "eth/mvEth.h"
#else
#include "neta/gbe/mvNeta.h"
#endif /* MV_ETH_LEGACY or MV_ETH_NETA */
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_SATA)
#include "sata/CoreDriver/mvSata.h"
#endif
#if defined(MV_INCLUDE_USB)
#include "usb/mvUsb.h"
#endif

#if defined(MV_INCLUDE_TDM)
#include "mvSysTdmConfig.h"
#endif

#include "ddr2_3/mvDramIfRegs.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif
MV_U32 dummyFlavour = 0;
MV_BIOS_MODE bios_modes[BIOS_MODES_NUM] = {
#ifdef MV88F78X60_Z1
/*	DBConf ConfID Code L2Size CPUFreq CpuFreqMode FabricFreq FabricFreqMode CPU1/2/3Enable cpuMode dramBusWidth*/
/*	0x4d/[1:0] 0x4d/[4:2] 0x4e[0] 0x4e/[4:1] 0x4f[0] 0x4f/[2:1] 0x4f/[4:3]	*/
       {"78130", 0x10, 0x7813, 0x1, 0x2, 0x0, 0xC, 0x0, 0x0, 0x1, 0x1},
       {"6710" , 0x11, 0x6710, 0x0, 0x2, 0x0, 0x5, 0x0, 0x0, 0x1, 0x0},
       {"78160", 0x12, 0x7816, 0x1, 0x2, 0x0, 0x5, 0x0, 0x0, 0x1, 0x0},
       {"78230", 0x13, 0x7823, 0x1, 0x2, 0x0, 0xC, 0x0, 0x2, 0x2, 0x1},
       {"78260", 0x14, 0x7826, 0x1, 0x2, 0x0, 0x5, 0x0, 0x2, 0x2, 0x0},
       {"78460", 0x15, 0x7846, 0x3, 0x2, 0x0, 0x5, 0x0, 0x3, 0x2, 0x0},
       {"78480", 0x16, 0x7846, 0x3, 0x2, 0x0, 0x5, 0x0, 0x3, 0x2, 0x0}
};
#else
/*	DBConf ConfID   Code 	L2Size	   CPUFreq    CpuFreqMode  FabricFreq	FabricFreqMode   CPU1/2/3Enable  cpuEndianess dramBusWidth */
/*	                       0x4d/[1:0]  0x4d/[4:2]  0x4e[0]      0x4e/[4:1]  	0x4f[0]   0x4f/[2:1]      0x4f/[3]   	  */
	{"78130",0x10, 0x7813,	0x1,	   0x3,		0x0,	      0x5,		0x0,		0x0,		0x1,		0x1},
/*	{"6710" ,0x11, 0x6710,	0x0,	   0x3,		0x0,	      0x5, 		0x0,		0x0,		0x1,		0x0},     */
	{"78160",0x12, 0x7816,	0x1,	   0x3,		0x0,	      0x5, 		0x0,		0x0,		0x1,	 	0x0},
	{"78230",0x13, 0x7823,	0x1,	   0x3,		0x0,	      0x5, 		0x0,		0x1,		0x0,		0x1},
	{"78260",0x14, 0x7826,	0x1,	   0x3,		0x0,	      0x5,		0x0,		0x1,		0x0,		0x0},
	{"78460",0x15, 0x7846,	0x3,	   0x3,		0x0,	      0x5, 		0x0,		0x3,		0x0,		0x0},
	{"78480",0x16, 0x7846,	0x3,	   0x3,		0x0,	      0x5, 		0x0,		0x3,		0x0,		0x0}
};
#endif

#if 0
table below before moving all flavour to 1333/667 mode only
MV_BIOS_MODE bios_modes[BIOS_MODES_NUM] = {
/*	DB Conf		Code		L2 size		CPU Freq	Fabric Freq		CPU1/2/3 Enable		CPU Mode v6UP/v6MP
	0x4d/[1:0]	0x4d/[4:2]	0x4e/[4:1]		0x4f/[2:1]				4f/[4:3]*/
	{"78130", 	0x7813, 	0x1, 		0x1, 		0x1, 			0x0,					0x0},
	{"KW40", 	0x6710, 	0x1, 		0x1, 		0x1, 			0x0,					0x0},
	{"78160", 	0x7816, 	0x1, 		0x3, 		0x5, 			0x0,					0x0},
	{"78230", 	0x7823, 	0x1, 		0x1, 		0x1, 			0x2,					0x2},
	{"78260", 	0x7826, 	0x1, 		0x3, 		0x5, 			0x2,					0x2},
	{"78460", 	0x7846, 	0x3, 		0x3, 		0x5, 			0x3,					0x2},
	{"78480", 	0x7846, 	0x3, 		0x5, 		0x5, 			0x3,					0x2}
};
#endif
MV_U32 mvCtrlGetCpuNum(MV_VOID)
{
	return ((MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET);
}
MV_U32 mvCtrlGetQuadNum(MV_VOID)
{
	return ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_L2_SIZE_MASK) >> SAR0_L2_SIZE_OFFSET);
}
MV_BOOL mvCtrlIsValidSatR(MV_VOID)
{
	int i = 0;
	MV_U32 tmpSocCores;
	MV_U8 cpuEna = 0;
	MV_U8 l2size;
	MV_U8 cpuFreq;
	MV_U8 fabricFreq;
	MV_U8 cpuFreqMode;
	MV_U8 fabricFreqMode;

	MV_U32 confId = mvBoardConfIdGet();

	l2size = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_L2_SIZE_MASK) >> SAR0_L2_SIZE_OFFSET;
	cpuFreq = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_CPU_FREQ_MASK) >> SAR0_CPU_FREQ_OFFSET;
	fabricFreq = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_FABRIC_FREQ_MASK) >> SAR0_FABRIC_FREQ_OFFSET;
	tmpSocCores = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET;
	cpuFreqMode = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU_MODE_MASK) >> SAR1_CPU_MODE_OFFSET;
	fabricFreqMode = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_FABRIC_MODE_MASK) >> SAR1_FABRIC_MODE_OFFSET;
	/* Bug fix in HW, bit0 & bit1 are swapped */
	cpuEna |= (tmpSocCores & 0x2) >> 1;
	cpuEna |= (tmpSocCores & 0x1) << 1;

	/* Find out what is programmed in SAR and change device ID accordingly */
	for (i = 0; i < BIOS_MODES_NUM; i++) {
		if (bios_modes[i].confId == confId) {
			DB(mvOsPrintf("confId = 0x%x\n", confId));
			DB(mvOsPrintf("cpuFreq [0x%x] = 0x%x\n", cpuFreq, bios_modes[i].cpuFreq));
			DB(mvOsPrintf("fabricFreq [0x%x] = 0x%x\n", fabricFreq, bios_modes[i].fabricFreq));
			DB(mvOsPrintf("cpuEna [0x%x] = 0x%x\n", cpuEna, bios_modes[i].cpuEna));
			DB(mvOsPrintf("cpuFreqMode [0x%x] = 0x%x\n", cpuFreqMode, bios_modes[i].cpuFreqMode));
			DB(mvOsPrintf("fabricFreqMode [0x%x] = 0x%x\n", fabricFreqMode, bios_modes[i].fabricFreqMode));
			DB(mvOsPrintf("l2size [0x%x] = 0x%x\n", l2size, bios_modes[i].l2size));
			if ((cpuFreq == bios_modes[i].cpuFreq) &&
				(fabricFreq ==  bios_modes[i].fabricFreq) &&
				(cpuEna == bios_modes[i].cpuEna) &&
				(cpuFreqMode == bios_modes[i].cpuFreqMode) &&
				(fabricFreqMode == bios_modes[i].fabricFreqMode) &&
				(l2size == bios_modes[i].l2size)) {
				return MV_TRUE;
			} else {
				return MV_FALSE;
			}
		}
	}
	return MV_FALSE;
}
MV_STATUS mvCtrlUpdatePexId(MV_VOID)
{
	/* MV_U32 socFreq, tmpSocCores;		*/
	/* MV_U32 socCores = 0;				*/
	MV_U32 pmCtrl;
#if defined(DB_88F78X60) || defined(RD_88F78460_SERVER) || defined (DB_88F78X60_REV2)
	MV_U32 devVendId;
	int i;
	int j;
	MV_U16 confId;
	MV_U32 tmp;
	MV_U32 NewVal;
#endif

	/* if PEX0 clock is disabled - enable it for reading the device ID */
	pmCtrl = MV_REG_READ(POWER_MNG_CTRL_REG);
	if ((pmCtrl & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0)) {
		MV_REG_WRITE(POWER_MNG_CTRL_REG,
			(pmCtrl & ~PMC_PEXSTOPCLOCK_MASK(0)) | PMC_PEXSTOPCLOCK_EN(0));
	}
#if defined(DB_88F78X60) || defined (DB_88F78X60_REV2)
	devVendId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));
	/* socFreq   = MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_DDR3_FREQ_MASK; */
	/* tmpSocCores  = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET; */
	confId = mvBoardConfIdGet();

	/* Bug fix in HW, bit0 & bit1 are swapped */
/*	socCores |= (tmpSocCores & 0x2) >> 1;
	socCores |= (tmpSocCores & 0x1) << 1;
*/
	/* Find out what is programmed in SAR and change device ID accordingly */
/*	if ((socFreq == SAR_CPU_FAB_GET(bios_modes[i].cpuFreq, bios_modes[i].fabricFreq)) &&
			(socCores == bios_modes[i].cpuEna)) {
*/
	tmp = MV_REG_READ(SOC_CTRL_REG); /*Saving old value of 0x18204 to tmp*/
	NewVal = tmp;
	NewVal |=0x00000080; /* writing 1 to bit 7 */
	MV_REG_WRITE(SOC_CTRL_REG, NewVal); /* writing '1' to bit 7 in order to get an access to PEX registers */
	for (i = 0; i < BIOS_MODES_NUM; i++) {
		if (bios_modes[i].confId == confId) {
			devVendId &= 0x0000FFFF;
			devVendId |= bios_modes[i].code << 16;
			for (j=0;j<mvCtrlPexMaxIfGet();j++){
				MV_REG_WRITE(MV_PEX_IF_REGS_OFFSET(j), devVendId);
			}
		}
	}
	MV_REG_WRITE(SOC_CTRL_REG, tmp);/*returing 0x18204 to it's previous value. */

#elif defined(RD_88F78460_SERVER)
	devVendId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));
	devVendId &= 0x0000FFFF;
	devVendId |= 0x7846 << 16;
	MV_REG_WRITE(MV_PEX_IF_REGS_OFFSET(0), devVendId);
#endif
	/* Reset the original value of PEX0 clock */
	if ((pmCtrl & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0))
		MV_REG_WRITE(POWER_MNG_CTRL_REG, pmCtrl);

	return MV_OK;
}
/*******************************************************************************
* mvCtrlEnvInit - Initialize Marvell controller environment.
*
* DESCRIPTION:
*       This function get environment information and initialize controller
*       internal/external environment. For example
*       1) MPP settings according to board MPP macros.
*		NOTE: It is the user responsibility to shut down all DMA channels
*		in device and disable controller sub units interrupts during
*		boot process.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvCtrlEnvInit(MV_VOID)
{
	MV_U32 mppGroup;
	MV_U32 mppVal;
	MV_BOARD_PEX_INFO  *boardPexInfo = mvBoardPexInfoGet();
	MV_U32 pexUnit = 0;
	MV_U32 i, gppMask;

	/* Disable MBus Error Propagation */
	MV_REG_BIT_RESET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT8);

	/* Use Background sync barrier (polling) for I/O cache coherency */
	MV_REG_BIT_SET(SOC_CIB_CTRL_CFG_REG, BIT8);

	/* MPP Init - scan which modeule is connected */
	mvBoardMppModulesScan();

	/* Read MPP config values from board level and write MPP options to HW */
	for (mppGroup = 0; mppGroup < MV_MPP_MAX_GROUP; mppGroup++) {
		mppVal = mvBoardMppGet(mppGroup);	/* get pre-defined values */
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* disable all GPIO interrupts */
	for (i = 0; i < MV_GPP_MAX_GROUP; i++) {
		MV_REG_WRITE(GPP_INT_MASK_REG(i), 0x0);
		MV_REG_WRITE(GPP_INT_LVL_REG(i), 0x0);
	}

	/* clear all int */
	for (i = 0; i < MV_GPP_MAX_GROUP; i++)
		MV_REG_WRITE(GPP_INT_CAUSE_REG(i), 0x0);

	/* Set gpp interrupts as needed */
	for (i = 0; i < MV_GPP_MAX_GROUP; i++) {
		gppMask = mvBoardGpioIntMaskGet(i);
		mvGppTypeSet(i, gppMask , (MV_GPP_IN & gppMask));
		mvGppPolaritySet(i, gppMask , (MV_GPP_IN_INVERT & gppMask));
	}

	/* Scan for other modules (SERDES/LVDS/...) */
	mvBoardOtherModulesScan();

	/* Update interfaces configuration based on above scan */
	if (MV_OK != mvCtrlSerdesPhyConfig())
		mvOsPrintf("mvCtrlEnvInit: Can't init some or all SERDES lanes\n");

	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		/* PEX enabling */
		if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg != PEX_BUS_DISABLED)
			MV_REG_BIT_SET(SOC_CTRL_REG, SCR_PEX_ENA_MASK(pexUnit));
		else
			MV_REG_BIT_RESET(SOC_CTRL_REG, SCR_PEX_ENA_MASK(pexUnit));
	}

#ifndef MV88F78X60_Z1
	MV_REG_BIT_SET(PUP_EN_REG,0x17); /* Enable GBE0, GBE1, LCD and NFC PUP */
#endif
	mvOsDelay(100);

	return MV_OK;
}

/*******************************************************************************
* mvCtrlMppRegGet - return reg address of mpp group
*
* DESCRIPTION:
*
* INPUT:
*       mppGroup - MPP group.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 - Register address.
*
*******************************************************************************/
MV_U32 mvCtrlMppRegGet(MV_U32 mppGroup)
{
	MV_U32 ret;

	if (mppGroup >= MV_MPP_MAX_GROUP)
		mppGroup = 0;

	ret = MPP_CONTROL_REG(mppGroup);

	return ret;
}

#if defined(MV_INCLUDE_PEX)
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_78130_DEV_ID:
	case MV_6710_DEV_ID:
	case MV_78230_DEV_ID:
		return 7;
/* TODO: alior fix PexMaxIfGet for KW40... */

	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
	case MV_78460_DEV_ID:
	case MV_78000_DEV_ID:
		return MV_PEX_MAX_IF;

	default:
		return 0;
	}
}
/*******************************************************************************
* mvCtrlPexMaxUnitGet - Get Marvell controller number of PEX units.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX units.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX units. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexMaxUnitGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_78130_DEV_ID:
	case MV_6710_DEV_ID:
	case MV_78230_DEV_ID:
		return 2;

	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
		return 3;

	case MV_78460_DEV_ID:
	case MV_78000_DEV_ID:
		return MV_PEX_MAX_UNIT;

	default:
		return 0;
	}
}

#endif

#if defined(MV_INCLUDE_PCI)
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPciMaxIfGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_FPGA_DEV_ID:
		return 1;

	default:
		return 0;
	}
}
#endif

/*******************************************************************************
* mvCtrlEthMaxPortGet - Get Marvell controller number of etherent ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of etherent port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of etherent port.
*
*******************************************************************************/
MV_U32 mvCtrlEthMaxPortGet(MV_VOID)
{
	MV_U32 devId;

	devId = mvCtrlModelGet();
#if 0
	/* FIXME - assuming MV_78460_DEV_ID */
	devId = MV_78460_DEV_ID;
#endif
	switch (devId) {
	case MV_78130_DEV_ID:
	case MV_6710_DEV_ID:
	case MV_78230_DEV_ID:
		return MV_78130_ETH_MAX_PORT;

	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
	case MV_78460_DEV_ID:
	case MV_78000_DEV_ID:
		return MV_78460_ETH_MAX_PORT;

	default:
		return 0;
	}
}

/*******************************************************************************
* mvCtrlEthMaxCPUsGet - Get Marvell controller number of CPUs.
*
* DESCRIPTION:
*       This function returns Marvell controller number of CPUs.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of CPUs.
*
*******************************************************************************/
MV_U8 mvCtrlEthMaxCPUsGet(MV_VOID)
{
	MV_U32 devId;

	devId = mvCtrlModelGet();

	/* FIXME - assuming MV_78460_DEV_ID */
	devId = MV_78460_DEV_ID;

	switch (devId) {
	case MV_78130_DEV_ID:
	case MV_78230_DEV_ID:
	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
	case MV_78460_DEV_ID:
		return 4;

	default:
		return 0;
	}
}


#if defined(MV_INCLUDE_SATA)
/*******************************************************************************
* mvCtrlSataMaxPortGet - Get Marvell controller number of Sata ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of Sata ports.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of Sata ports.
*
*******************************************************************************/
MV_U32 mvCtrlSataMaxPortGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	default:
		res = MV_SATA_MAX_CHAN;
		break;
	}
	return res;
}
#endif

#if defined(MV_INCLUDE_XOR)
/*******************************************************************************
* mvCtrlXorMaxChanGet - Get Marvell controller number of XOR channels.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR channels.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR channels.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxChanGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	default:
		res = MV_XOR_MAX_CHAN;
		break;
	}
	return res;
}

/*******************************************************************************
* mvCtrlXorMaxUnitGet - Get Marvell controller number of XOR units.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR units.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR units.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	default:
		res = MV_XOR_MAX_UNIT;
		break;
	}
	return res;
}

#endif

#if defined(MV_INCLUDE_USB)
/*******************************************************************************
* mvCtrlUsbHostMaxGet - Get number of Marvell Usb  controllers
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       returns number of Marvell USB  controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsbMaxGet(void)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_FPGA_DEV_ID:
		res = 0;
		break;

	default:
		res = ARMADA_XP_MAX_USB_PORTS;
		break;
	}

	return res;
}
#endif

#if defined(MV_INCLUDE_LEGACY_NAND)
/*******************************************************************************
* mvCtrlNandSupport - Return if this controller has integrated NAND flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if NAND is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlNandSupport(MV_VOID)
{
	return ARMADA_XP_NAND;
}
#endif

#if defined(MV_INCLUDE_SDIO)
/*******************************************************************************
* mvCtrlSdioSupport - Return if this controller has integrated SDIO flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if SDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlSdioSupport(MV_VOID)
{
	return ARMADA_XP_SDIO;
}
#endif

#if defined(MV_INCLUDE_TDM)
/*******************************************************************************
* mvCtrlTdmSupport - Return if this controller has integrated TDM flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if TDM is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlTdmSupport(MV_VOID)
{
	return ARMADA_XP_TDM;
}

/*******************************************************************************
* mvCtrlTdmMaxGet - Return the maximum number of TDM ports.
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The number of TDM ports in device.
*
*******************************************************************************/
MV_U32 mvCtrlTdmMaxGet(MV_VOID)
{
	return ARMADA_XP_MAX_TDM_PORTS;
}

/*******************************************************************************
* mvCtrlTdmTypeGet
*
* DESCRIPTION:
*	Return the TDM unit type being compiled in.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TDM unit type.
*
*******************************************************************************/
MV_UNIT_ID mvCtrlTdmUnitTypeGet(MV_VOID)
{
	return TDM_UNIT_32CH;
}

/*******************************************************************************
* mvCtrlTdmUnitIrqGet
*
* DESCRIPTION:
*	Return the TDM unit IRQ number depending on the TDM unit compilation
*	options.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
******************************************************************************/
MV_U32 mvCtrlTdmUnitIrqGet(MV_VOID)
{
	return MV_TDM_IRQ_NUM;
}

#endif /* if defined(MV_INCLUDE_TDM) */

/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit desscribing Marvell controller ID
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	MV_U32 devId;
	MV_U16 model = 0;
	MV_U32 reg, reg2;

	/* if PEX0 clocks are disabled - enabled it to read */
	reg = MV_REG_READ(POWER_MNG_CTRL_REG);
	if ((reg & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0)) {
		reg2 = ((reg & ~PMC_PEXSTOPCLOCK_MASK(0)) | PMC_PEXSTOPCLOCK_EN(0));
		MV_REG_WRITE(POWER_MNG_CTRL_REG, reg2);
	}

	devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));

	/* Reset the original value of the PEX0 clock */
	if ((reg & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0))
		MV_REG_WRITE(POWER_MNG_CTRL_REG, reg);


	model = (MV_U16) ((devId >> 16) & 0xFFFF);
#if 0
	/* FIXME */
	/* Temperarely override model numebr for Z1 */
	switch (model) {
	case MV_78130_DEV_ID:
	case MV_78160_DEV_ID:
	case MV_78230_DEV_ID:
	case MV_78260_DEV_ID:
	case MV_78460_DEV_ID:
		model = MV_78460_DEV_ID;
		break;

	/* Fallback: assume 78460 */
	default:
		model = MV_78460_DEV_ID;
	}
#endif
	return model;
}

/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U8 revNum;
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID, 0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	revNum = (MV_U8) MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PCI_CLASS_CODE_AND_REVISION_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return ((revNum & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS);
}

/*******************************************************************************
* mvCtrlNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlNameGet(char *pNameBuff)
{
	if (mvCtrlModelGet() == 0x7800)
		mvOsSPrintf(pNameBuff, "%s78XX", SOC_NAME_PREFIX);
	else
		mvOsSPrintf(pNameBuff, "%s%x Rev %d", SOC_NAME_PREFIX, mvCtrlModelGet(), mvCtrlRevGet());
	return MV_OK;
}

/*******************************************************************************
* mvCtrlModelRevGet - Get Controller Model (Device ID) and Revision
*
* DESCRIPTION:
*       This function returns 32bit value describing both Device ID and Revision
*       as defined in PCI Express Device and Vendor ID Register and device revision
*	    as defined in PCI Express Class Code and Revision ID Register.

*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing both controller device ID and revision number
*
*******************************************************************************/
MV_U32 mvCtrlModelRevGet(MV_VOID)
{
	return ((mvCtrlModelGet() << 16) | mvCtrlRevGet());
}

/*******************************************************************************
* mvCtrlModelRevNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff)
{
	switch (mvCtrlModelRevGet()) {
	case MV_78130_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_78130_Z1_NAME);
		break;

	case MV_6710_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6710_Z1_NAME);
		break;

	case MV_78230_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_78230_Z1_NAME);
		break;
	case MV_78160_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_78160_Z1_NAME);
		break;
	case MV_78260_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_78260_Z1_NAME);
		break;
	case MV_78460_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_78460_Z1_NAME);
		break;
	default:
		mvCtrlNameGet(pNameBuff);
		break;
	}

	return MV_OK;
}

static const char *cntrlName[] = TARGETS_NAME_ARRAY;

/*******************************************************************************
* mvCtrlTargetNameGet - Get Marvell controller target name
*
* DESCRIPTION:
*       This function convert the trget enumeration to string.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Target name (const MV_8 *)
*******************************************************************************/
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target)
{
	if (target >= MAX_TARGETS)
		return "target unknown";

	return cntrlName[target];
}

/*******************************************************************************
* mvCtrlPexAddrDecShow - Print the PEX address decode map (BARs and windows).
*
* DESCRIPTION:
*		This function print the PEX address decode map (BARs and windows).
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static MV_VOID mvCtrlPexAddrDecShow(MV_VOID)
{
	MV_PEX_BAR pexBar;
	MV_PEX_DEC_WIN win;
	MV_U32 pexIf;
	MV_U32 bar, winNum;
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
	MV_U32 pexHWInf = 0;

	for (pexIf = 0; pexIf < boardPexInfo->boardPexIfNum; pexIf++) {
		pexHWInf = boardPexInfo->pexMapping[pexIf];


		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexHWInf))
			continue;
		mvOsOutput("\n");
		mvOsOutput("PEX%d:\n", pexHWInf);
		mvOsOutput("-----\n");

		mvOsOutput("\nPex Bars \n\n");

		for (bar = 0; bar < PEX_MAX_BARS; bar++) {
			memset(&pexBar, 0, sizeof(MV_PEX_BAR));

			mvOsOutput("%s ", pexBarNameGet(bar));

			if (mvPexBarGet(pexHWInf, bar, &pexBar) == MV_OK) {
				if (pexBar.enable) {
					mvOsOutput("base %08x, ", pexBar.addrWin.baseLow);
					if (pexBar.addrWin.size == 0)
						mvOsOutput("size %3dGB ", 4);
					else
						mvSizePrint(pexBar.addrWin.size);
					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}
		mvOsOutput("\nPex Decode Windows\n\n");

		for (winNum = 0; winNum < PEX_MAX_TARGET_WIN - 2; winNum++) {
			memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

			mvOsOutput("win%d - ", winNum);

			if (mvPexTargetWinRead(pexHWInf, winNum, &win) == MV_OK) {
				if (win.winInfo.enable) {
					mvOsOutput("%s base %08x, ",
						   mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win.winInfo)),
						   win.winInfo.addrWin.baseLow);
					mvOsOutput("....");
					mvSizePrint(win.winInfo.addrWin.size);

					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}

		memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

		mvOsOutput("default win - ");

		if (mvPexTargetWinRead(pexHWInf, MV_PEX_WIN_DEFAULT, &win) == MV_OK) {
			mvOsOutput("%s ", mvCtrlTargetNameGet(win.target));
			mvOsOutput("\n");
		}
		memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

		mvOsOutput("Expansion ROM - ");

		if (mvPexTargetWinRead(pexHWInf, MV_PEX_WIN_EXP_ROM, &win) == MV_OK) {
			mvOsOutput("%s ", mvCtrlTargetNameGet(win.target));
			mvOsOutput("\n");
		}
	}
}

/*******************************************************************************
* mvUnitAddrDecShow - Print the Unit's address decode map.
*
* DESCRIPTION:
*       This is a generic function for printing the different unit's address
*	decode map.
*
* INPUT:
*       unit	- The unit to print the address decode for.
*	name	- The unit's name.
*	winGetFuncPtr - A pointer to the HAL's window get function.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static void mvUnitAddrDecShow(MV_U8 numUnits, MV_UNIT_ID unitId, const char *name, MV_WIN_GET_FUNC_PTR winGetFuncPtr)
{
	MV_UNIT_WIN_INFO win;
	MV_U32 unit, i;

	for (unit = 0; unit < numUnits; unit++) {

		if (MV_FALSE == mvCtrlPwrClckGet(unitId, unit))
			continue;
		mvOsOutput("\n");
		mvOsOutput("%s %d:\n", name, unit);
		mvOsOutput("----\n");

		for (i = 0; i < 16; i++) {
			memset(&win, 0, sizeof(MV_UNIT_WIN_INFO));

			mvOsOutput("win%d - ", i);

			if (winGetFuncPtr(unit, i, &win) == MV_OK) {
				if (win.enable) {
					mvOsOutput("%s base %08x, ",
						   mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win)),
						   win.addrWin.baseLow);
					mvOsOutput("....");
					mvSizePrint(win.addrWin.size);
					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}
	}
	return;
}

/*******************************************************************************
* mvCtrlAddrDecShow - Print the Controller units address decode map.
*
* DESCRIPTION:
*		This function the Controller units address decode map.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvCtrlAddrDecShow(MV_VOID)
{
	mvCpuIfAddDecShow();
	mvAhbToMbusAddDecShow();
#if defined(MV_INCLUDE_PEX)
	mvCtrlPexAddrDecShow();
#endif
#if defined(MV_INCLUDE_USB)
	mvUnitAddrDecShow(mvCtrlUsbMaxGet(), USB_UNIT_ID, "USB", mvUsbWinRead);
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(MV_ETH_LEGACY)
	mvUnitAddrDecShow(mvCtrlEthMaxPortGet(), ETH_GIG_UNIT_ID, "ETH", mvEthWinRead);
#else
	mvUnitAddrDecShow(mvCtrlEthMaxPortGet(), ETH_GIG_UNIT_ID, "ETH", mvNetaWinRead);
#endif /* MV_ETH_LEGACY */
#endif /* MV_INCLUDE_GIG_ETH */

#if defined(MV_INCLUDE_XOR)
	mvUnitAddrDecShow(mvCtrlXorMaxChanGet(), XOR_UNIT_ID, "XOR", mvXorTargetWinRead);
#endif
#if defined(MV_INCLUDE_SATA)
	mvUnitAddrDecShow(mvCtrlSataMaxPortGet(), SATA_UNIT_ID, "Sata", mvSataWinRead);
#endif
}

/*******************************************************************************
* ctrlSizeToReg - Extract size value for register assignment.
*
* DESCRIPTION:
*       Address decode size parameter must be programed from LSB to MSB as
*       sequence of 1's followed by sequence of 0's. The number of 1's
*       specifies the size of the window in 64 KB granularity (e.g. a
*       value of 0x00ff specifies 256x64k = 16 MB).
*       This function extract the size value from the size parameter according
*       to given aligment paramter. For example for size 0x1000000 (16MB) and
*       aligment 0x10000 (64KB) the function will return 0x00FF.
*
* INPUT:
*       size - Size.
*       alignment - Size alignment. Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size register value correspond to size parameter.
*       If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32 ctrlSizeToReg(MV_U32 size, MV_U32 alignment)
{
	MV_U32 retVal;

	/* Check size parameter alignment               */
	if ((0 == size) || (MV_IS_NOT_ALIGN(size, alignment))) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size is zero or not aligned.\n"));
		return -1;
	}

	/* Take out the "alignment" portion out of the size parameter */
	alignment--;		/* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */
	/* and size is 0x1000000 (16MB) for example     */
	while (alignment & 1) {	/* Check that alignmet LSB is set       */
		size = (size >> 1);	/* If LSB is set, move 'size' one bit to right      */
		alignment = (alignment >> 1);
	}

	/* If after the alignment first '0' was met we still have '1' in                */
	/* it then aligment is invalid (not power of 2)                                 */
	if (alignment) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", (MV_U32) alignment));
		return -1;
	}

	/* Now the size is shifted right according to aligment: 0x0100                  */
	size--;			/* Now the size is a sequance of '1': 0x00ff                    */
	retVal = size;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's         */
	while (size & 1)	/* Check that LSB is set    */
		size = (size >> 1);	/* If LSB is set, move one bit to the right         */

	if (size) {		/* Sequance of 1's is over. Check that we have no other 1's         */
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size parameter 0x%x invalid.\n", size));
		return -1;
	}
	return retVal;
}

/*******************************************************************************
* ctrlRegToSize - Extract size value from register value.
*
* DESCRIPTION:
*       This function extract a size value from the register size parameter
*       according to given aligment paramter. For example for register size
*       value 0xff and aligment 0x10000 the function will return 0x01000000.
*
* INPUT:
*       regSize   - Size as in register format.	See ctrlSizeToReg.
*       alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size.
*       If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32 ctrlRegToSize(MV_U32 regSize, MV_U32 alignment)
{
	MV_U32 temp;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's         */
	temp = regSize;		/* Now the size is a sequance of '1': 0x00ff            */

	while (temp & 1)	/* Check that LSB is set                                    */
		temp = (temp >> 1);	/* If LSB is set, move one bit to the right         */

	if (temp) {		/* Sequance of 1's is over. Check that we have no other 1's         */
		DB(mvOsPrintf("ctrlRegToSize: ERR. Size parameter 0x%x invalid.\n", regSize));
		return -1;
	}

	/* Check that aligment is a power of two                                        */
	temp = alignment - 1;	/* Now the alignmet is a sequance of '1' (0xffff)          */

	while (temp & 1)	/* Check that alignmet LSB is set                           */
		temp = (temp >> 1);	/* If LSB is set, move 'size' one bit to right      */

	/* If after the 'temp' first '0' was met we still have '1' in 'temp'            */
	/* then 'temp' is invalid (not power of 2)                                      */
	if (temp) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", alignment));
		return -1;
	}

	regSize++;		/* Now the size is 0x0100                                       */

	/* Add in the "alignment" portion to the register size parameter                */
	alignment--;		/* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */

	while (alignment & 1) {	/* Check that alignmet LSB is set                       */
		regSize = (regSize << 1);	/* LSB is set, move 'size' one bit left     */
		alignment = (alignment >> 1);
	}

	return regSize;
}

/*******************************************************************************
* ctrlSizeRegRoundUp - Round up given size
*
* DESCRIPTION:
*       This function round up a given size to a size that fits the
*       restrictions of size format given an aligment parameter.
*		to given aligment paramter. For example for size parameter 0xa1000 and
*		aligment 0x1000 the function will return 0xFF000.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size value correspond to size in register.
*******************************************************************************/
MV_U32 ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment)
{
	MV_U32 msbBit = 0;
	MV_U32 retSize;

	/* Check if size parameter is already comply with restriction   */
	if (!(-1 == ctrlSizeToReg(size, alignment)))
		return size;

	while (size) {
		size = (size >> 1);
		msbBit++;
	}

	retSize = (1 << msbBit);

	if (retSize < alignment)
		return alignment;
	else
		return retSize;
}

/*******************************************************************************
* mvCtrlIsBootFromNOR
*
* DESCRIPTION:
*       Check if device is configured to boot from NOR flash according to the
*	SAR registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device boot from SPI.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromNOR(MV_VOID)
{
	MV_U32 satr;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_BOOT_MODE_MASK;

	if (satr == MSAR_BOOT_NOR)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
* mvCtrlIsBootFromSPI
*
* DESCRIPTION:
*       Check if device is configured to boot from SPI flash according to the
*	SAR registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device boot from SPI.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromSPI(MV_VOID)
{
	MV_U32 satr;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_BOOT_MODE_MASK;

	if (satr == MSAR_BOOT_SPI)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
* mvCtrlIsBootFromNAND
*
* DESCRIPTION:
*       Check if device is confiogured to boot from NAND flash according to the SAR
*	registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device boot from NAND.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromNAND(MV_VOID)
{
	MV_U32 satr;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_BOOT_MODE_MASK;

	if ((satr == MSAR_BOOT_DOVE_NAND) || (satr == MSAR_BOOT_LEGACY_NAND))
		return MV_TRUE;
	else
		return MV_FALSE;
}

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
/*******************************************************************************
* mvCtrlPwrClckSet - Set Power State for specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	/* Clock gating is not supported on FPGA */
	if (mvCtrlModelGet() == MV_FPGA_DEV_ID)
		return;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_CESASTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_CESASTOPCLOCK_MASK);

		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);

		break;
#endif
	case TDM_32CH_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);

		break;
	default:
		break;
	}
}

/*******************************************************************************
* mvCtrlPwrClckGet - Get Power State of specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(POWER_MNG_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	/* Clock gating is not supported on FPGA */
	if (mvCtrlModelGet() == MV_FPGA_DEV_ID)
		return MV_TRUE;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPCLOCK_MASK(index)) == PMC_PEXSTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & PMC_GESTOPCLOCK_MASK(index)) == PMC_GESTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATASTOPCLOCK_MASK(index)) == PMC_SATASTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if ((reg & PMC_CESASTOPCLOCK_MASK) == PMC_CESASTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USBSTOPCLOCK_MASK(index)) == PMC_USBSTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & PMC_SDIOSTOPCLOCK_MASK) == PMC_SDIOSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_32CH_UNIT_ID:
		if ((reg & PMC_TDMSTOPCLOCK_MASK) == PMC_TDMSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
	default:
		state = MV_TRUE;
		break;
	}

	return state;
}

/*******************************************************************************
* mvCtrlPwrMemSet - Set Power State for memory on specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrMemSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PEX), PMC_PEXSTOPMEM_STOP(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PEX), PMC_PEXSTOPMEM_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_GE), PMC_GESTOPMEM_STOP(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_GE), PMC_GESTOPMEM_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_SATA), PMC_SATASTOPMEM_STOP(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_SATA), PMC_SATASTOPMEM_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_CESA), PMC_CESASTOPMEM_STOP);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_CESA), PMC_CESASTOPMEM_MASK);

		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_USB), PMC_USBSTOPMEM_STOP(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_USB), PMC_USBSTOPMEM_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_XOR), PMC_XORSTOPMEM_STOP(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_XOR), PMC_XORSTOPMEM_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_BM)
	case BM_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_BM), PMC_BMSTOPMEM_STOP);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_BM), PMC_BMSTOPMEM_MASK);

		break;
#endif
#if defined(MV_INCLUDE_PNC)
	case PNC_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PNC), PMC_PNCSTOPMEM_STOP);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PNC), PMC_PNCSTOPMEM_MASK);

		break;
#endif
	default:
		break;
	}
}

/*******************************************************************************
* mvCtrlPwrMemGet - Get Power State of memory on specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL mvCtrlPwrMemGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg;
	MV_BOOL state = MV_TRUE;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PEX));
		if ((reg & PMC_PEXSTOPMEM_MASK(index)) == PMC_PEXSTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_GE));
		if ((reg & PMC_GESTOPMEM_MASK(index)) == PMC_GESTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_SATA));
		if ((reg & PMC_SATASTOPMEM_MASK(index)) == PMC_SATASTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_CESA));
		if ((reg & PMC_CESASTOPMEM_MASK) == PMC_CESASTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_USB));
		if ((reg & PMC_USBSTOPMEM_MASK(index)) == PMC_USBSTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_XOR));
		if ((reg & PMC_XORSTOPMEM_MASK(index)) == PMC_XORSTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_BM)
	case BM_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_BM));
		if ((reg & PMC_BMSTOPMEM_MASK) == PMC_BMSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PNC)
	case PNC_UNIT_ID:
		reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG(PMC_MCR_NUM_PNC));
		if ((reg & PMC_PNCSTOPMEM_MASK) == PMC_PNCSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
	default:
		state = MV_TRUE;
		break;
	}

	return state;
}
#else
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	return;
}

MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	return MV_TRUE;
}
#endif /* #if defined(MV_INCLUDE_CLK_PWR_CNTRL) */

/*******************************************************************************
* mvCtrlSerdesMaxLinesGet - Get Marvell controller number of SERDES lines.
*
* DESCRIPTION:
*       This function returns Marvell controller number of SERDES lines.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX units. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlSerdesMaxLinesGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_78130_DEV_ID:
	case MV_6710_DEV_ID:
	case MV_78230_DEV_ID:
		return 7;
	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
		return 12;
		break;
	case MV_78460_DEV_ID:
	case MV_78000_DEV_ID:
		return 16;
	default:
		return 0;
	}
}

MV_U32 mvCtrlDDRBudWidth(MV_VOID)
{
	MV_U32 reg;
	reg = MV_REG_READ(0x1400);

	return (reg & 0x8000) ? 64 : 32;
}
MV_BOOL mvCtrlDDRThruXbar(MV_VOID)
{
	MV_U32 reg;
	reg = MV_REG_READ(0x20184);

	return (reg & 0x1) ? MV_FALSE : MV_TRUE;
}

MV_BOOL mvCtrlDDRECC(MV_VOID)
{
	MV_U32 reg;
	reg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);

	return (reg & (0x1 << REG_SDRAM_CONFIG_ECC_OFFS)) ? MV_TRUE : MV_FALSE;
}

static const MV_U8 serdesCfg[][8] = SERDES_CFG;

/*******************************************************************************
* mvCtrlSerdesPhyConfig
*
* DESCRIPTION:
*	Configure Serdes MUX and init PHYs connected to SERDES lines.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Status
*
*******************************************************************************/
MV_STATUS mvCtrlSerdesPhyConfig(MV_VOID)
{
	MV_U32		serdesLineCfg;
	MV_U8		serdesLineNum;
	MV_U32		regAddr[16][11], regVal[16][11]; /* addr/value for each line @ every setup step */
	MV_U8		pexUnit, pexLineNum;
	MV_U8		step;
	MV_U8		maxSerdesLines = mvCtrlSerdesMaxLinesGet();
	MV_SERDES_CFG	*pSerdesInfo = mvBoardSerdesCfgGet();
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
	MV_STATUS	status = MV_OK;
	MV_U32		tmp;

/* this is a mapping of the final power management clock gating control register value @ 0x18220.*/
	MV_U32	powermngmntctrlregmap = 0x0;
	MV_U32	ethport = 0;
	MV_U32	tmpcounter = 0;

	/* Check if no SERDESs available - FPGA */
	if (maxSerdesLines == 0)
		return MV_OK;

	if (pSerdesInfo == NULL) {
		DB(mvOsPrintf("%s: Error reading SERDES configuration!\n", __func__));
		return MV_ERROR;
	}

	memset(regAddr, 0, sizeof(regAddr));
	memset(regVal,  0, sizeof(regVal));
	memset(boardPexInfo, 0, sizeof(MV_BOARD_PEX_INFO));

	/* Prepare PHY parameters for each step according to  MUX selection */
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/
		MV_U32	*pRegVal = regVal[serdesLineNum];
		MV_U32	*pRegAddr = regAddr[serdesLineNum];
		MV_U8	sgmiiPort = 0;

		if (serdesLineNum < 8)
			serdesLineCfg = (pSerdesInfo->serdesLine0_7 >> (serdesLineNum << 2)) & 0xF;
		else
			serdesLineCfg = (pSerdesInfo->serdesLine8_15 >> ((serdesLineNum - 8) << 2)) & 0xF;

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum >> 2;
			pexLineNum = serdesLineNum % 4;

			/* Map the PCI-E interfaces according to their HW mapping
			   Map PCI-E virtual intrefaces in an array where we have information about
			   every interface (e.g. HW mapping, x1,x4,x8 ( disabled) .. )
			*/
			switch (pexUnit) {
			case 0:
				boardPexInfo->pexUnitCfg[pexUnit].pexCfg = pSerdesInfo->pex0Mod;
				break;
			case 1:
				boardPexInfo->pexUnitCfg[pexUnit].pexCfg = pSerdesInfo->pex1Mod;
				break;
			case 2:
				if (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X4)
					boardPexInfo->pexUnitCfg[pexUnit].pexCfg = PEX_BUS_MODE_X4;
				else
					boardPexInfo->pexUnitCfg[pexUnit].pexCfg = PEX_BUS_DISABLED;
				break;
			case 3:
				boardPexInfo->pexUnitCfg[pexUnit].pexCfg = pSerdesInfo->pex3Mod;
				break;
			}
			if ((pexUnit < 2) && (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X1)) {

				boardPexInfo->pexMapping[boardPexInfo->boardPexIfNum] = pexUnit * 4 + pexLineNum;
				boardPexInfo->boardPexIfNum++;
				boardPexInfo->pexUnitCfg[pexUnit].pexLaneStat[pexLineNum] = 0x1;
				powermngmntctrlregmap = powermngmntctrlregmap | (0x1<<(serdesLineNum+5));
			} else if ((pexUnit < 4) &&
				(boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X4) &&
				(pexLineNum == 0)) {

				switch (pexUnit) {
				case 0:
				case 1:
					boardPexInfo->pexMapping[boardPexInfo->boardPexIfNum] = pexUnit*4 + pexLineNum;
					powermngmntctrlregmap = powermngmntctrlregmap | (0xF<<(serdesLineNum+5));
					break;
				case 2:
				case 3:
					boardPexInfo->pexMapping[boardPexInfo->boardPexIfNum] = PEX2_0x4 + (pexUnit - 2);
					powermngmntctrlregmap = powermngmntctrlregmap | (0x1 << (pexUnit+24));
					break;
				}

				boardPexInfo->boardPexIfNum++;

			} else if ((pexUnit == 3) &&
				(pSerdesInfo->pex3Mod == PEX_BUS_MODE_X8) &&
				(pexLineNum == 0)) {

				boardPexInfo->pexMapping[boardPexInfo->boardPexIfNum] = PEX3_0x4;
				boardPexInfo->boardPexIfNum++;
			}

			/* Needed for PEX_PHY_ACCESS_REG macro */
			if ((serdesLineNum > 7) && (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X8))
				pexUnit = 3; /* lines 8 - 15 are belong to PEX3 in x8 mode */

			/* regVal bits:
			bit[31] - 0:write, 1:read
			bit[23:16] - PHY REG offset
			bit[29:24] - PEX line
			bit[15:0] - value to be set in PHY REG
			*/
			pRegAddr[0] = PEX_PHY_ACCESS_REG(pexUnit);
			pRegAddr[1] = 0; /*PEX_PHY_ACCESS_REG(pexUnit); */
			pRegAddr[2] = PEX_PHY_ACCESS_REG(pexUnit);
			pRegAddr[3] = 0;
			pRegAddr[4] = 0;
			pRegAddr[5] = PEX_PHY_ACCESS_REG(pexUnit);
			pRegAddr[6] = 0;
			pRegAddr[7] = 0;
			pRegAddr[8] = 0;
			pRegAddr[9] = 0;

			/* PHY_MODE [7:5] - 3 (PCIe)
			   REF_FREF_SEL [4:0] - 0 (100MHz) */
			pRegVal[0]  = (0x01 << 16) | (pexLineNum << 24) | 0xFC60;

#if 0
			/* SEL_BITS [11:10]
				For 2.5G: 0 (10bit)
				For 5G:   1 (20bit) */
			pRegVal[1]  = (pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0 ? 0x0400 : 0x0;
			pRegVal[1] |= (0x23 << 16) | (pexLineNum << 24);
#endif
			/* SEL_BITS [11:10] - NA */
			pRegVal[1] = 0;

			/* REFCLK_SEL [10] - 0 */
			pRegVal[2]  = (0x46 << 16) | (pexLineNum << 24);

			/* PIN_PHY_GEN [12:5] - NA */
			pRegVal[3]  = 0;
			pRegVal[4]  = 0;

			/* Termination enable */
			pRegVal[5]  = (0x48 << 16) | (pexLineNum << 24) | 0x9080;
		} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {

			MV_U8	sataPort;

			if ((serdesLineNum == 4) || (serdesLineNum == 6)) {
				sataPort = 0;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_SATASTOPCLOCK_MASK(sataPort);
			} else if (serdesLineNum == 5) {
				sataPort = 1;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_SATASTOPCLOCK_MASK(sataPort);
			} else
				goto err_cfg;

			pRegAddr[0] = SATA_PWR_PLL_CTRL_REG(sataPort);
			pRegAddr[1] = SATA_DIG_LP_ENA_REG(sataPort);
			pRegAddr[2] = SATA_REF_CLK_SEL_REG(sataPort);
			pRegAddr[3] = SATA_LP_PHY_EXT_CTRL_REG(sataPort);
			pRegAddr[4] = SATA_LP_PHY_EXT_STAT_REG(sataPort);
			pRegAddr[5] = SATA_IMP_TX_SSC_CTRL_REG(sataPort);
			pRegAddr[6] = SATA_GEN_1_SET_0_REG(sataPort);
			pRegAddr[7] = SATA_GEN_1_SET_1_REG(sataPort);
			pRegAddr[8] = SATA_GEN_2_SET_0_REG(sataPort);
			pRegAddr[9] = SATA_GEN_2_SET_1_REG(sataPort);
			pRegAddr[10] = SATA_COMPHY_CTRL_REG(sataPort);
			pRegVal[0]  = 0xF801;
			pRegVal[1]  = 0x400;
			pRegVal[2]  = 0x400;
			pRegVal[3]  = (pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0 ? 0x227 : 0x7;
			pRegVal[4]  = 0x7;
			pRegVal[5]  = 0x6F14;
			pRegVal[6]  = 0xC928;
			pRegVal[7]  = 0x30F0;
			pRegVal[8]  = 0xAA31;
			pRegVal[9]  = 0x30F5;
			pRegVal[10] = 0x9080;

		} else {

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
				sgmiiPort = 0;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_GESTOPCLOCK_MASK(sgmiiPort);
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1]) {
				sgmiiPort = 1;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_GESTOPCLOCK_MASK(sgmiiPort);
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2]) {
				sgmiiPort = 2;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_GESTOPCLOCK_MASK(sgmiiPort);
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3]) {
				sgmiiPort = 3;
				powermngmntctrlregmap = powermngmntctrlregmap | PMC_GESTOPCLOCK_MASK(sgmiiPort);
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
				sgmiiPort = 0;
				powermngmntctrlregmap = powermngmntctrlregmap | \
										PMC_GESTOPCLOCK_MASK(0) | PMC_GESTOPCLOCK_MASK(1) | \
										PMC_GESTOPCLOCK_MASK(2) | PMC_GESTOPCLOCK_MASK(3);
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
				continue;

			pRegAddr[0] = SGMII_PWR_PLL_CTRL_REG(sgmiiPort);
			pRegAddr[1] = SGMII_DIG_LP_ENA_REG(sgmiiPort);
			pRegAddr[2] = SGMII_REF_CLK_SEL_REG(sgmiiPort);
			pRegAddr[3] = SGMII_SERDES_CFG_REG(sgmiiPort);
			pRegAddr[4] = SGMII_SERDES_STAT_REG(sgmiiPort);
			pRegAddr[5] = SGMII_COMPHY_CTRL_REG(sgmiiPort);
			pRegAddr[6] = 0;
			pRegAddr[7] = 0;
			pRegAddr[8] = 0;
			pRegAddr[9] = 0;
			pRegVal[0]  = 0xF881;
			pRegVal[1]  = 0;
			pRegVal[2]  = 0x400;
			pRegVal[3]  = (pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0 ? 0x1547 : 0xCC7;
			pRegVal[4]  = 0x7;
			pRegVal[5]  = 0x9080;

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
				pRegVal[1]  = 0x400;
				pRegVal[3]  = 0x667;
			}
		}

	} /* for each serdes lane*/

	/*
	 * If clock gatting was done in U-boot then we don't want
	 *to try to access  a HW block that is clock gated
	 */
	if ((MV_REG_READ(POWER_MNG_CTRL_REG) == 0xFFFFFFFF)) {
		/* Step 0 - configure SERDES MUXes */
		MV_REG_WRITE(SERDES_LINE_MUX_REG_0_7,  pSerdesInfo->serdesLine0_7);
		MV_REG_WRITE(SERDES_LINE_MUX_REG_8_15, pSerdesInfo->serdesLine8_15);
		DB(mvOsPrintf("\nStep[0].1 Addr[0x%08x] Value[0x%08x]\n", \
					  SERDES_LINE_MUX_REG_0_7, pSerdesInfo->serdesLine0_7));
		DB(mvOsPrintf("Step[0].2 Addr[0x%08x] Value[0x%08x]\n\n", \
					  SERDES_LINE_MUX_REG_8_15, pSerdesInfo->serdesLine8_15));

		/* Step 1 [PEX-Only] PEX-Main configuration (X4 or X1): */
		/* First disable all PEXs in SoC Control Reg */
		MV_REG_WRITE(SOC_CTRL_REG, 0x0);

		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_DISABLED)
				continue;
			/* SoC Control
					0x18204 [7,8]
						0x0 X4-Link.
						0x1 n*X1-Link’s */
			if (pexUnit < 2) {
				tmp = MV_REG_READ(SOC_CTRL_REG);
				if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X1)
					tmp |= SCR_PEX_4BY1_MASK(pexUnit);

				DB(mvOsPrintf("Step[1].1 Addr[0x%08x] pexUnit [%d] value [0x%x]\n", SOC_CTRL_REG, pexUnit, tmp));
				MV_REG_WRITE(SOC_CTRL_REG, tmp);
			}
		}

		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {

			if (serdesLineNum < 8)
				serdesLineCfg = (pSerdesInfo->serdesLine0_7 >> (serdesLineNum << 2)) & 0xF;
			else
				serdesLineCfg = (pSerdesInfo->serdesLine8_15 >> ((serdesLineNum - 8) << 2)) & 0xF;

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				/* PCI Express Control
					0xX1A00 [0]:
					0x0 X4-Link.
					0x1 X1-Link */
				tmp = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)));
				tmp &= ~(0x3F0);
				if (boardPexInfo->pexUnitCfg[serdesLineNum >> 2].pexCfg == PEX_BUS_MODE_X1)
					tmp |= (0x1 << 4);
				if (boardPexInfo->pexUnitCfg[serdesLineNum >> 2].pexCfg == PEX_BUS_MODE_X4)
					tmp |= (0x4 << 4);
				MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);
				DB(mvOsPrintf("Step[1].2 Addr[0x%08x] serdesLine [%d] value [0x%x]\n",
						  PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), serdesLineNum, tmp));
				/* if pex is X4, no need to pass thru the other 3X1 serdes lines */
				if (boardPexInfo->pexUnitCfg[serdesLineNum >> 2].pexCfg == PEX_BUS_MODE_X4)
					serdesLineNum += 3;
			}
		}
		DB(mvOsPrintf("\n"));

		/* Step 2 [PEX-X4 Only] To create PEX-Link that contain 4-lanes you need to config the
		 register SOC_Misc/General Purpose2 (Address= 182F8)*/
		tmp = MV_REG_READ(GEN_PURP_RES_2_REG);

		if (pSerdesInfo->pex0Mod == PEX_BUS_MODE_X4)
			DB(mvOsPrintf("\n"));

		/* Step 2 [PEX-X4 Only] To create PEX-Link that contain 4-lanes you need to config the
		 register SOC_Misc/General Purpose2 (Address= 182F8)*/
		tmp = MV_REG_READ(GEN_PURP_RES_2_REG);  tmp |= 0x0000000F;
/*	else
		tmp &= 0xFFFFFFF0;
*/
		if (pSerdesInfo->pex1Mod == PEX_BUS_MODE_X4)
			tmp |= 0x000000F0;
/*	else
		tmp &= 0xFFFFFF0F; */

		if (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X4)
			tmp |= 0x0000FF00;
/*	else
		tmp &= 0xFFFF00FF; */

		DB(mvOsPrintf("Step[2] Addr[0x%08x] Value[0x%08x]\n\n", GEN_PURP_RES_2_REG, tmp));

		MV_REG_WRITE(GEN_PURP_RES_2_REG, tmp);

		/* STEP 3 [PEX-Only] First phase of PEX-PIPE Configuration:*/
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (MV_REG_READ(PEX_STATUS_REG(pexUnit)) & PXSR_DL_DOWN) { /* If there is no link */
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_DISABLED)
				continue;
			DB(mvOsPrintf("Step[3].1 Addr[0x%08x] Value[0x%08x]\n", PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0xA5));
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X4)
				DB(mvOsPrintf("Step[3].2 Addr[0x%08x] Value[0x%08x]\n", \
							  PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x200));
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X1)
				DB(mvOsPrintf("Step[3].3 Addr[0x%08x] Value[0x%08x]\n", \
							  PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F));

			DB(mvOsPrintf("Step[3].4 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0xC8 << 16) | 0x05));
			DB(mvOsPrintf("Step[3].5 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0xD0 << 16) | 0x100));
			DB(mvOsPrintf("Step[3].6 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0xD1 << 16) | 0x3014));
			DB(mvOsPrintf("Step[3].7 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0xC5 << 16) | 0x11F));
			DB(mvOsPrintf("Step[3].8 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0x80 << 16) | 0x1000));
			DB(mvOsPrintf("Step[3].9 Addr[0x%08x] Value[0x%08x]\n\n",\
						  PEX_PHY_ACCESS_REG(pexUnit), (0x81 << 16) | 0x11));



			#ifdef MV88F78X60_Z1
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0xA5);
			#else
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x25);
			#endif
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X4)
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x200);
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_MODE_X1)
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC8 << 16) | 0x05);
			/* MV_REG_WRITE(PEX_PHY_ACCESS_REG(0), (0xC2 << 16) | 0x200); */ /* BC was disabled per HW team request */
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xD0 << 16) | 0x100);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xD1 << 16) | 0x3014);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC5 << 16) | 0x11F);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0x80 << 16) | 0x1000);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0x81 << 16) | 0x11);
			}
		}
		/* Steps 4, 5, 6, 7 - use prepared REG addresses and values */
		for (step = 0; step < 4; step++) {
			for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
				if (regAddr[serdesLineNum][step] != 0) {
					DB(mvOsPrintf("Step[%d] Lane[%d] Addr[0x%08x] Value[0x%08x]\n",	\
								  step + 4, serdesLineNum, \
								  regAddr[serdesLineNum][step], regVal[serdesLineNum][step]));
					MV_REG_WRITE(regAddr[serdesLineNum][step], regVal[serdesLineNum][step]);
				}
			}
			DB(mvOsPrintf("\n"));
		}

		/* Step 8 [PEX-Only] Last phase of PEX-PIPE Configuration */
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (MV_REG_READ(PEX_STATUS_REG(pexUnit)) & PXSR_DL_DOWN) { /* If there is no link */
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg == PEX_BUS_DISABLED)
				continue;
			DB(mvOsPrintf("Step[8].1 Addr[0x%08x] Value[0x%08x]\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0x2 << 16) | 0x40));
			DB(mvOsPrintf("Step[8].2 Addr[0x%08x] Value[0x%08x]\n\n", \
						  PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0xA4));

			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0x2 << 16) | 0x40);
			#ifdef MV88F78X60_Z1
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0xA4);
			#else
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
			#endif
			}
		}

		/*Step 9 - Init additional registers */
		for (step = 0; step < 6; step++) {
			for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
				if (regAddr[serdesLineNum][step+5] != 0) {
					DB(mvOsPrintf("Step[9].%d >>>>>>>> Lane[%d] Addr[0x%08x] Value[0x%08x]\n",
								  step+5, serdesLineNum, regAddr[serdesLineNum][step+5], regVal[serdesLineNum][step+5]));
					MV_REG_WRITE(regAddr[serdesLineNum][step+5], regVal[serdesLineNum][step+5]);
				}
			}
		}
		DB(mvOsPrintf("\n"));

		/* Wait 5ms before checking results */
		mvOsDelay(15);

		/* Step 10 - check that PLL RX/TX is ready and RX init done */
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			MV_U32  tmp;
			if (regAddr[serdesLineNum][4] != 0) { /* SATA, QSGMII, SGMII */
				tmp = MV_REG_READ(regAddr[serdesLineNum][4]);
				if ((tmp & 0x7) != regVal[serdesLineNum][4]) { /* check bits[2:0] */
					DB(mvOsPrintf("%s: PHY on SERDES lane %d is not ready!\n",
								  __func__, serdesLineNum));
					status = MV_ERROR;
				}
			} else if (regAddr[serdesLineNum][0] != 0) { /* PEX that is enabled and passed step1 */
				/* First step writes into Power and PLL control register.
				   It's possible to read it back now for checking the PLL lock status */
				MV_REG_WRITE(regAddr[serdesLineNum][0], (regVal[serdesLineNum][0] | BIT31) & 0xFFFF0000);
				tmp = MV_REG_READ(regAddr[serdesLineNum][0]);
				if ((tmp & BIT8) == 0) {
					DB(mvOsPrintf("%s: PEX PHY PLL on SERDES lane %d is not locked!\n",
								  __func__, serdesLineNum));
					status = MV_ERROR;
				}
			}
		}

	} /*if No clock gating */

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/*check if GE0 / GE1 are not enabled via MPPs and not Serdes - if yes you have to enable the clock*/
	for (ethport = 0; ethport < 2; ethport++)
		if (MV_TRUE ==  mvBoardIsGbEPortConnected(ethport))
			powermngmntctrlregmap = powermngmntctrlregmap | PMC_GESTOPCLOCK_MASK(ethport);

	/* Hard core enable DDR, USB, SDIO, LCD, XOR, IDMA, CESA cause we don't support this at this momemt*/
	powermngmntctrlregmap = powermngmntctrlregmap | (BIT0  | BIT13 | (0x1FF<<16) | BIT24 | BIT28 | BIT31);
	DB(mvOsPrintf("%s:Shutting down unused interfaces:\n", __func__));
	/*now report everything to the screen*/
	if (!(powermngmntctrlregmap & PMC_SATASTOPCLOCK_MASK(0))) {
		DB(mvOsPrintf("%s:       SATA0\n", __func__));
		mvCtrlPwrClckSet(SATA_UNIT_ID, 0, MV_FALSE);
	}
	if (!(powermngmntctrlregmap & PMC_SATASTOPCLOCK_MASK(1))) {
		DB(mvOsPrintf("%s:       SATA1\n", __func__));
		mvCtrlPwrClckSet(SATA_UNIT_ID, 1, MV_FALSE);
	}
	for (tmpcounter = 0; tmpcounter < 4; tmpcounter++) {
		if (!(powermngmntctrlregmap & (1 << (4 - tmpcounter)))) {
			/*mvOsOutput("       GBE%d\n", tmpcounter );*/
			DB(mvOsPrintf("%s:       GBE%d\n", __func__, tmpcounter));
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, tmpcounter, MV_FALSE);
		}
	}
	for (tmpcounter = 0; tmpcounter < 8; tmpcounter++) {
		if (!(powermngmntctrlregmap & (1 << (5 + tmpcounter)))) {
			DB(mvOsPrintf("%s:       PEX%d.%d\n", __func__, tmpcounter>>2, tmpcounter % 4));
			mvCtrlPwrClckSet(PEX_UNIT_ID, tmpcounter, MV_FALSE);
		}
	}
	if (!(powermngmntctrlregmap & BIT26)) {
		DB(mvOsPrintf("%s:       PEX2\n", __func__));
		mvCtrlPwrClckSet(PEX_UNIT_ID, 8, MV_FALSE);
	}
	if (!(powermngmntctrlregmap & BIT27)) {
		DB(mvOsPrintf("%s:       PEX3\n", __func__));
		mvCtrlPwrClckSet(PEX_UNIT_ID, 9, MV_FALSE);
	}

#ifndef MV88F78X60_Z1
/*this code is valid for all devices after Z1*/
	if(!(powermngmntctrlregmap & BIT25)) {
		DB(mvOsPrintf("%s:       TDM\n", __func__));
		mvCtrlPwrClckSet(TDM_32CH_UNIT_ID, 0, MV_FALSE);
	}
#endif
	/*apply clock gatting*/
	MV_REG_WRITE(POWER_MNG_CTRL_REG, MV_REG_READ(POWER_MNG_CTRL_REG) & powermngmntctrlregmap);

	/*the Sata driver doesn't support clock gating at this point so we enable the logic to the block*/
	MV_REG_WRITE(POWER_MNG_CTRL_REG, MV_REG_READ(POWER_MNG_CTRL_REG) | (BIT15 | BIT30));
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */

	return status;
err_cfg:
	DB(mvOsPrintf("%s: Wrong CFG (%#x) for SERDES line %d.\n",
		__func__, serdesLineCfg, serdesLineNum));
	return MV_ERROR;

}


