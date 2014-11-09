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
#include "cpu/mvCpu.h"

#include "pex/mvPex.h"
#include "pex/mvPexRegs.h"

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(CONFIG_MV_ETH_NETA)
#include "neta/gbe/mvNeta.h"
#endif /* MV_ETH_NETA */
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXor.h"
#endif

#include "ddr2_3/mvDramIfRegs.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

/* MSYS family linear id */
#define MV_MSYS_BC2_INDEX		0
#define MV_MSYS_AC3_INDEX		1
#define MV_MSYS_INDEX_MAX		2

MV_UNIT_ID mvCtrlSocUnitNums[MAX_UNITS_ID][MV_MSYS_INDEX_MAX] = {
/*			    BC2		AC3 */
/* DRAM_UNIT_ID         */ { 1,		1, },
/* PEX_UNIT_ID          */ { 1,		1, },
/* ETH_GIG_UNIT_ID      */ { 2,		2, },
/* XOR_UNIT_ID          */ { 1,		1, },
/* UART_UNIT_ID         */ { 2,		2, },
/* SPI_UNIT_ID          */ { 2,		1, },
/* SDIO_UNIT_ID         */ { 1,		1, },
/* I2C_UNIT_ID          */ { 2,		2, },
/* USB_UNIT_ID          */ { 0,		1, },
/* USB3_UNIT_ID         */ { 0,		0, },
};

static MV_U32 mvCtrlDevIdIndexGet(MV_U32 devId)
{
	MV_U32 index;

	switch (devId) {
	case MV_BOBCAT2_DEV_ID:
		index = MV_MSYS_BC2_INDEX;
		break;
	case MV_ALLEYCAT3_DEV_ID:
		index = MV_MSYS_AC3_INDEX;
		break;
	default:
		index = MV_MSYS_AC3_INDEX;
	}

	return index;
}

MV_U32 mvCtrlSocUnitInfoNumGet(MV_UNIT_ID unit)
{
	MV_U32 devIdIndex;

	if (unit >= MAX_UNITS_ID) {
		mvOsPrintf("%s: Error: Wrong unit type (%u)\n", __func__, unit);
		return 0;
	}

	devIdIndex = mvCtrlDevIdIndexGet(mvCtrlModelGet());
	return mvCtrlSocUnitNums[unit][devIdIndex];
}

MV_U32 mvCtrlGetCpuNum(MV_VOID)
{
	return 0;
}
MV_BOOL mvCtrlIsValidSatR(MV_VOID)
{
	return MV_TRUE;
}

/*******************************************************************************
* mvCtrlDevBusInit - Initialize Marvell device bus controller.
*
* DESCRIPTION:
*       This function initializes device bus controller for configured devices
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
MV_VOID mvCtrlDevBusInit(MV_VOID)
{
#if defined(MV_INCLUDE_NOR)
	MV_U32	numOfDevices;
	MV_U32	deviceId;
	MV_U32	busWidth;
	MV_U32	deviceCS;
	MV_U32	regVal;
	MV_U32	regAddr;

	/* Enable NOR Flash PUP */
	MV_REG_BIT_SET(PUP_EN_REG, BIT3);

	/* Configure NAND flush disabled */
	MV_REG_BIT_RESET(SOC_DEV_MUX_REG, BIT0);

	/* Set NfArbiterEn to device bus */
	MV_REG_BIT_SET(SOC_DEV_MUX_REG, BIT27);

	/* Configure NOR bus width */
	numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_NOR_FLASH);
	for (deviceId = 0; deviceId < numOfDevices; deviceId++) {
		deviceCS = boardGetDevCSNum(deviceId, BOARD_DEV_NOR_FLASH);
		if (deviceCS != 0xFFFFFFFF) {
			busWidth = mvBoardGetDeviceBusWidth(deviceId, BOARD_DEV_NOR_FLASH);

			if (deviceCS == DEV_BOOCS)
				regAddr = MV_DEV_BUS_REGS_OFFSET;
			else
				regAddr = MV_DEV_BUS_REGS_OFFSET + 8 + (deviceCS - DEVICE_CS0) * 8;

			regVal = MV_REG_READ(regAddr);
			regVal &= ~(BIT30|BIT31);
			regVal |= (busWidth >> 4); /* 8b = 0, 16b = 1, 32b = 2 */
			MV_REG_WRITE(regAddr, regVal);
		}
	}
#else
	/* Enable NAND Flash PUP (Pack-Unpack)
	 * HW machanism to accelerate transactions (controlled by SoC register) */
	MV_REG_BIT_SET(PUP_EN_REG, BIT4);

	/* Configure NAND flush enabled */
	MV_REG_BIT_SET(SOC_DEV_MUX_REG, BIT0);

	/* Set NfArbiterEn to NAND Flash (Disable arbitration between device and NAND) */
	MV_REG_BIT_RESET(SOC_DEV_MUX_REG, BIT27);
#endif
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

	mvCtrlDevBusInit();

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
	return mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);
}

/*******************************************************************************
* mvCtrlPexActiveUnitNumGet
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
*       Marvell controller number of PEX units.
*
*******************************************************************************/
MV_U32 mvCtrlPexActiveUnitNumGet(MV_VOID)
{
	return mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);
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
	return 0;
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
	return mvCtrlSocUnitInfoNumGet(ETH_GIG_UNIT_ID);
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
	return 2;
}

/*******************************************************************************
* mvCtrlUsbMaxGet - Get number of Marvell USB controllers
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
*       returns number of Marvell USB controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsbMaxGet(void)
{
	return mvCtrlSocUnitInfoNumGet(USB_UNIT_ID);
}

MV_U32 mvCtrlUsb3MaxGet(void)
{
	return 0; /* no USB3.0 */
}

MV_U32 mvCtrlUsb3HostMaxGet(void)
{
	return 0; /* no USB3.0 */
}

/*******************************************************************************
* mvCtrlUtmiPhySelectorSet - configures the shared MAC access between USB2/3
*
* DESCRIPTION:
*	MSYS doesn't utilize UTMI PHY selection
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID mvCtrlUtmiPhySelectorSet(MV_U32 usbUnitId)
{
	return;
}

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
	return mvCtrlSocUnitInfoNumGet(XOR_UNIT_ID);
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
	return mvCtrlSocUnitInfoNumGet(XOR_UNIT_ID);
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
	return mvCtrlSocUnitInfoNumGet(SDIO_UNIT_ID);
}
#endif

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
	MV_U32	ctrlId = MV_REG_READ(DEV_ID_REG);

	ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;

	switch (ctrlId) {
	case MV_BOBCAT2_DEV_ID:
	case MV_ALLEYCAT3_DEV_ID:
		return ctrlId;
	default:
		mvOsPrintf("%s: Error: Failed to obtain Controller Device ID\n", __func__);
		return mvCtrlDevFamilyIdGet(0);
	}
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
	MV_U32 value = MV_DFX_REG_READ(DEV_REV_ID_REG);
	return (value & (REVISON_ID_MASK)) >> REVISON_ID_OFFS;
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
	mvOsSPrintf(pNameBuff, "%s", SOC_NAME_PREFIX);
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
* mvCtrlRevNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the revision id.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain revision name string. Minimum size 30 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_VOID mvCtrlRevNameGet(char *pNameBuff)
{
	MV_U32 revId = mvCtrlRevGet();
	MV_U32 ctrlFamily = mvCtrlDevFamilyIdGet(0);

	if (ctrlFamily == MV_BOBCAT2_DEV_ID) {

		char *revArrayBC2[] = MV_BOBCAT2_ID_ARRAY;

		switch (revId) {
		case MV_BOBCAT2_A0_ID:
		case MV_BOBCAT2_B0_ID:
			mvOsSPrintf(pNameBuff, " Rev %s", revArrayBC2[revId]);
			return;
		}

	} else if (ctrlFamily == MV_ALLEYCAT3_DEV_ID) {

		char *revArrayAC3[] = MV_ALLEYCAT3_ID_ARRAY;

		switch (revId) {
		case MV_ALLEYCAT3_A0_ID:
		case MV_ALLEYCAT3_A1_ID:
			mvOsSPrintf(pNameBuff, " Rev %s", revArrayAC3[revId]);
			return;
		}

	} else
		mvOsPrintf("%s: Error: Wrong controller model %#x\n", __func__, ctrlFamily);

	mvOsPrintf("%s: Error: Failed to read Revision ID\n", __func__);
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
	mvCtrlNameGet(pNameBuff);
	mvCtrlRevNameGet(pNameBuff + strlen(pNameBuff));
	return MV_OK;
}

/*******************************************************************************
* mvCtrlDevFamilyIdGet - Get Device ID
*
* DESCRIPTION:
*       This function returns Device ID.
*
* INPUT:
*       ctrlModel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit board Device ID number, '-1' if Device ID is undefined.
*
*******************************************************************************/
MV_U32 mvCtrlDevFamilyIdGet(MV_U16 ctrlModel)
{
	MV_U32	boardId = mvBoardIdGet();

	if ((boardId >= BC2_CUSTOMER_BOARD_ID_BASE) && (boardId < BC2_MARVELL_MAX_BOARD_ID))
		return MV_BOBCAT2_DEV_ID;
	else if ((boardId >= AC3_CUSTOMER_BOARD_ID_BASE) && (boardId < AC3_MARVELL_MAX_BOARD_ID))
		return MV_ALLEYCAT3_DEV_ID;
	else {
		mvOsPrintf("%s: ERR. Invalid Board ID (%d) ,Using BC2 as default family\n", __func__, boardId);
		return MV_BOBCAT2_DEV_ID;
	}
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

#if defined(MV_INCLUDE_PEX)
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

	MV_U32 pexHWInf = 0;

	for (pexIf = 0; pexIf < MV_PEX_MAX_IF; pexIf++) {
		pexHWInf = pexIf;


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
#endif

#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_XOR)
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
					if (win.addrWin.size == 0)
						mvOsOutput("size %3dGB ", 4);
					else
						mvSizePrint(win.addrWin.size);
					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}
	}
	return;
}
#endif

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

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(CONFIG_MV_ETH_NETA)
	mvUnitAddrDecShow(mvCtrlEthMaxPortGet(), ETH_GIG_UNIT_ID, "ETH", mvNetaWinRead);
#endif
#endif

#if defined(MV_INCLUDE_XOR)
	mvUnitAddrDecShow(mvCtrlXorMaxChanGet(), XOR_UNIT_ID, "XOR", mvXorTargetWinRead);
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
	MV_U32 satr = MV_DFX_REG_READ(DFX_DEVICE_SAR_REG(0));

	if (mvCtrlDevFamilyIdGet(0) == MV_BOBCAT2_DEV_ID)
		satr = MSAR_BC2_BOOT_MODE(satr, 0);
	else
		satr = MSAR_AC3_BOOT_MODE(satr, 0);

	if (satr == SAR1_BOOT_FROM_NOR)
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
	MV_U32 satr = MV_DFX_REG_READ(DFX_DEVICE_SAR_REG(0));

	if (mvCtrlDevFamilyIdGet(0) == MV_BOBCAT2_DEV_ID)
		satr = MSAR_BC2_BOOT_MODE(satr, 0);
	else
		satr = MSAR_AC3_BOOT_MODE(satr, 0);

	if (satr == SAR1_BOOT_FROM_SPI)
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
	MV_U32 satr = MV_DFX_REG_READ(DFX_DEVICE_SAR_REG(0));

	if (mvCtrlDevFamilyIdGet(0) == MV_BOBCAT2_DEV_ID)
		satr = MSAR_BC2_BOOT_MODE(satr, 0);
	else
		satr = MSAR_AC3_BOOT_MODE(satr, 0);

	if (satr == SAR1_BOOT_FROM_NAND)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
 * mvCtrlIsDLBEnabled - Read DLB configuration
 *
 * DESCRIPTION: return True if DLB is enabled
 *
 * INPUT: None
 *
 * OUTPUT: None
 *
 * RETURN: MV_TRUE, if DLB is enabled
 ******************************************************************************/
MV_BOOL mvCtrlIsDLBEnabled(MV_VOID)
{
	MV_U32 reg;

	reg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);

	return (reg & 0x1) ? MV_TRUE : MV_FALSE;
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

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK);

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
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);

		break;
#endif
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
#if defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_SDIO)
	MV_U32 reg = MV_REG_READ(POWER_MNG_CTRL_REG);
#endif
	MV_BOOL state = MV_TRUE;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPCLOCK_MASK) == PMC_PEXSTOPCLOCK_STOP)
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
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & PMC_SDIOSTOPCLOCK_MASK) == PMC_SDIOSTOPCLOCK_STOP)
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

MV_U32 mvCtrlDDRBudWidth(MV_VOID)
{
	MV_U32 reg;
	reg = MV_REG_READ(REG_SDRAM_CONFIG_ADDR);

	return (reg & (1 << REG_SDRAM_CONFIG_DDR_BUS_OFFS)) ? 32 : 16;
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
	MV_U32		socCtrlReg;
	MV_STATUS	status = MV_OK;
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();

/* this is a mapping of the final power management clock gating control register value @ 0x18220.*/
	MV_U32	powermngmntctrlregmap = 0x0;
	MV_U32	tmpcounter = 0;

	/* Check if no SERDESs available - FPGA */

	memset(boardPexInfo, 0, sizeof(MV_BOARD_PEX_INFO));
	socCtrlReg = MV_REG_READ(SOC_CTRL_REG);

	if ((socCtrlReg & 1) == 0)
		boardPexInfo->pexUnitCfg[0].pexCfg = PEX_BUS_DISABLED;
	else {
		boardPexInfo->pexUnitCfg[0].pexCfg = PEX_BUS_MODE_X1;
		boardPexInfo->pexMapping[0] = 0;
		boardPexInfo->boardPexIfNum++;
		boardPexInfo->pexUnitCfg[0].pexLaneStat[0] = 0x1;
		powermngmntctrlregmap |= PMC_PEXSTOPCLOCK_EN;
	}

	powermngmntctrlregmap |= PMC_GESTOPCLOCK_EN(1);

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	powermngmntctrlregmap |= PMC_GESTOPCLOCK_EN(0); ; /* Enabling port GE0 always since we need SMI 0 to access other PHYs*/


	/* Hard core enable SDIO, XOR, device clock cause we don't support this at this momemt*/
	powermngmntctrlregmap |= BIT24 | BIT22 | BIT17 ;
	DB(mvOsPrintf("%s:Shutting down unused interfaces:\n", __func__));
	/*now report everything to the screen*/
	for (tmpcounter = 0; tmpcounter < 4; tmpcounter++) {
		if (!(powermngmntctrlregmap & (1 << (4 - tmpcounter)))) {
			/*mvOsOutput("       GBE%d\n", tmpcounter );*/
			DB(mvOsPrintf("%s:       GBE%d\n", __func__, tmpcounter));
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, tmpcounter, MV_FALSE);
		}
	}
	if (!(powermngmntctrlregmap & PMC_PEXSTOPCLOCK_EN)) {
		DB(mvOsPrintf("%s:       PEX%d.%d\n", __func__, tmpcounter>>2, tmpcounter % 4));
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
	}

/*this code is valid for all devices after Z1*/
	/*apply clock gatting*/
	MV_REG_WRITE(POWER_MNG_CTRL_REG, MV_REG_READ(POWER_MNG_CTRL_REG) & powermngmntctrlregmap);
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */


	return status;
}

/*******************************************************************************
* mvCtrlGetJuncTemp
*
* DESCRIPTION:
*       Read temperature, calibrate at first time the TSEN
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Tj value.
*******************************************************************************/
MV_U32 mvCtrlGetJuncTemp(MV_VOID)
{
#if 0
	MV_32 reg = 0;
	/* init the TSEN sensor once */
	if ((MV_REG_READ(TEMPERATURE_SENSOR_MSB_CTRL_REG) & TSEN_OTF_CALIB_MSK) == 0) {
		MV_REG_BIT_SET(TSEN_CONF_REG, TSEN_OTF_CALIB_MSK);

		reg = MV_REG_READ(TEMPERATURE_SENSOR_MSB_CTRL_REG);
		reg &= ~(TSMC_UNIT_CONTROL_MASK);
		reg |= (TSMC_UNIT_CONTROL_AVR << TSMC_UNIT_CONTROL_OFS)
		MV_REG_WRITE(TEMPERATURE_SENSOR_MSB_CTRL_REG, reg);


		/* Do not start calibration sequence */
		MV_REG_BIT_RESET(TSEN_CONF_REG, TSEN_CONF_START_CALIB_MASK);

		/* Initiate Soft Reset*/
		MV_REG_BIT_SET(TSEN_CONF_REG, TSEN_CONF_SOFT_RESET_MASK);
		mvOsDelay(1);

		/* Exit from Soft Reset*/
		MV_REG_BIT_RESET(TSEN_CONF_REG, TSEN_CONF_SOFT_RESET_MASK);
		mvOsDelay(10);
	}

	reg = MV_REG_READ(TSEN_STATUS_REG);
	reg = (reg & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;
#ifdef ERRATA_FE_982377
	{ /* Internal CPU Temperature Read Out Stability */
		int reg1, reg2, reg3, i;
		for (i = 0; i < 20; i++) {
			reg1 = MV_REG_READ(TSEN_STATUS_REG);
			reg1 = (reg1 & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;
			reg2 = MV_REG_READ(TSEN_STATUS_REG);
			reg2 = (reg2 & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;
			reg3 = MV_REG_READ(TSEN_STATUS_REG);
			reg3 = (reg3 & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;
			if ((reg1 == reg2) && (reg1 == reg3)) {
				reg = reg1;
				break;
			}
		}
	}
#endif
	return (3153000 - (10000 * reg)) / 13825;
#endif
	return -1;
}

/*******************************************************************************
* mvCtrlGetPexActive
*
* DESCRIPTION:
*	fill input boolean array of pex interface
*
* INPUT:
*       pPexActive - pointer to boolean array (size is max pex IF).
*	size	   - array size
*
* OUTPUT:
*        boolean array - true or false if pex exists
*
* RETURN:
*       None
*
*******************************************************************************/
void mvCtrlGetPexActive(MV_BOOL *pPexActive, int size)
{
	if (size == 0)
		return;
	pPexActive[0] = MV_TRUE;
}

/*******************************************************************************
* mvCtrlNandClkSet
*
* DESCRIPTION:
*       Set the division ratio of ECC Clock
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*******************************************************************************/
int mvCtrlNandClkSet(int nfc_clk_freq)
{
	int divider;
	MV_U32 nVal = MV_DFX_REG_READ(CORE_DIV_CLK_CTRL(2));
	MV_U32 pll_clk = mvCpuPllClkGet();

	/*
	 * Calculate nand divider for requested nfc_clk_freq. If integer divider
	 * cannot be achieved, it will be rounded-up, which will result in
	 * setting the closest lower frequency.
	 * ECC engine clock = (PLL frequency / divider)
	 * NFC clock = ECC clock / 2
	 */
	divider = DIV_ROUND_UP(pll_clk, (2 * nfc_clk_freq));
	DB(mvOsPrintf("%s: divider %d\n", __func__, divider));

	/* Set the division ratio of ECC Clock 0x000F8270[9:6] (ECC clock = CPU / dividor) */
	nVal &= ~(NAND_ECC_DIVCKL_RATIO_MASK);
	nVal |= (divider << NAND_ECC_DIVCKL_RATIO_OFFS);
	MV_DFX_REG_WRITE(CORE_DIV_CLK_CTRL(2), nVal);

	/* Set reload force of ECC clock 0x000F8268[27:21] to 0x40 (force the dividor only the NAND ECC clock) */
	nVal = MV_DFX_REG_READ(CORE_DIV_CLK_CTRL(0));
	nVal &= ~(CORE_DIVCLK_RELOAD_FORCE_MASK);
	nVal |= (CORE_DIVCLK_RELOAD_FORCE_VAL << CORE_DIVCLK_RELOAD_FORCE_OFFS);
	MV_DFX_REG_WRITE(CORE_DIV_CLK_CTRL(0), nVal);

	/* Set reload ratio bit 0x000F8270[10] to 1'b1 */
	MV_DFX_REG_BIT_SET(CORE_DIV_CLK_CTRL(2), CORE_DIVCLK_RELOAD_RATIO_MASK);
	mvOsDelay(1); /*  msec */
	/* Set reload ratio bit 0x000F8270[10] to 0'b1 */
	MV_DFX_REG_BIT_RESET(CORE_DIV_CLK_CTRL(2), CORE_DIVCLK_RELOAD_RATIO_MASK);

	/* Return calculated nand clock frequency */
	return pll_clk/(2 * divider);
}

/*******************************************************************************
* mvCtrlUsbMapGet
*
* DESCRIPTION:
*       Get the map of USB ports if exists
*
* INPUT:
*       The current usbActive.
*
* OUTPUT:
*       Mapped usbActive.
*
* RETURN:
*       None
*******************************************************************************/
MV_U32 mvCtrlUsbMapGet(MV_U32 usbUnitId, MV_U32 usbActive)
{
	/* MSYS has no usb mapping: only single */
	return usbActive;
}
