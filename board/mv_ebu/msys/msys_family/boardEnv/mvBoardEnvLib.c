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

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "cpu/mvCpu.h"
#include "cntmr/mvCntmr.h"
#include "gpp/mvGpp.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPex.h"
#include "device/mvDevice.h"
#include "neta/gbe/mvEthRegs.h"

#if defined(CONFIG_MV_ETH_NETA)
#include "neta/gbe/mvEthRegs.h"
#endif

#include "gpp/mvGppRegs.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x)	x
#define DB1(x)	x
#else
#define DB(x)
#define DB1(x)
#endif

extern MV_BOARD_INFO *marvellBC2BoardInfoTbl[];
extern MV_BOARD_INFO *customerBC2BoardInfoTbl[];
extern MV_BOARD_INFO *marvellAC3BoardInfoTbl[];
extern MV_BOARD_INFO *customerAC3BoardInfoTbl[];
static MV_BOARD_INFO *board;

/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);

/*******************************************************************************
* mvBoardIdIndexGet
*
* DESCRIPTION:
*	returns an index for board arrays with direct memory access, according to board id
*
* INPUT:
*       boardId.
*
* OUTPUT:
*       direct access index for board arrays
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 as base for Board ID: mask MSB to receive index for board ID*/
	return boardId & (BOARD_ID_INDEX_MASK - 1);
}

/*******************************************************************************
* mvBoardEnvInit - Init board
*
* DESCRIPTION:
*		In this function the board environment take care of device bank
*		initialization.
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
MV_VOID mvBoardEnvInit(MV_VOID)
{
	MV_U32 nandDev;
	MV_U32 norDev;

	mvBoardSet(mvBoardIdGet());

	nandDev = boardGetDevCSNum(0, BOARD_DEV_NAND_FLASH);
	if (nandDev != 0xFFFFFFFF) {
		/* Set NAND interface access parameters */
		nandDev = BOOT_CS;
		MV_REG_WRITE(DEV_BANK_PARAM_REG(nandDev), board->nandFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(nandDev), board->nandFlashWriteParams);
		MV_REG_WRITE(DEV_NAND_CTRL_REG, board->nandFlashControl);
	}

	norDev = boardGetDevCSNum(0, BOARD_DEV_NOR_FLASH);
	if (norDev != 0xFFFFFFFF) {
		/* Set NOR interface access parameters */
		MV_REG_WRITE(DEV_BANK_PARAM_REG(norDev), board->norFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(norDev), board->norFlashWriteParams);
		MV_REG_WRITE(DEV_BUS_SYNC_CTRL, 0x11);
	}

	/* Set GPP Out value */
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), board->gppOutValLow);
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), board->gppOutValMid);

	/* set GPP polarity */
	mvGppPolaritySet(0, 0xFFFFFFFF, board->gppPolarityValLow);
	mvGppPolaritySet(1, 0xFFFFFFFF, board->gppPolarityValMid);

	/* Set GPP Out Enable */
	mvGppTypeSet(0, 0xFFFFFFFF, board->gppOutEnValLow);
	mvGppTypeSet(1, 0xFFFFFFFF, board->gppOutEnValMid);

}

/*******************************************************************************
* mvBoardModelGet - Get Board model
*
* DESCRIPTION:
*       This function returns 16bit describing board model.
*       Board model is constructed of one byte major and minor numbers in the
*       following manner:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardModelGet(MV_VOID)
{
	MV_U8 modelId;
	if (MV_ERROR == mvBoardTwsiRead(BOARD_DEV_TWSI_PLD, 0, 0, &modelId)) {
		mvOsWarning();
		return INVALID_BOARD_ID;
	}

	return (MV_U16)modelId;
}
/*******************************************************************************
* mbBoardRevlGet - Get Board revision
*
* DESCRIPTION:
*       This function returns a 32bit describing the board revision.
*       Board revision is constructed of 4bytes. 2bytes describes major number
*       and the other 2bytes describes minor munber.
*       For example for board revision 3.4 the function will return
*       0x00030004.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardRevGet(MV_VOID)
{
	return mvBoardIdIndexGet(mvBoardIdGet()) & 0xFFFF;
}
/*******************************************************************************
* mvBoardNameGet - Get Board name
*
* DESCRIPTION:
*       This function returns a string describing the board model and revision.
*       String is extracted from board I2C EEPROMboard.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board name string. Minimum size 32 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvBoardNameGet(char *pNameBuff)
{
	mvOsSPrintf(pNameBuff, "%s", board->boardName);
	return MV_OK;
}

/*******************************************************************************
* mvBoardIsEthConnected - detect if a certain Ethernet port is Connected
*
* DESCRIPTION:
*	This routine returns true if a certain Ethernet port is Connected
*
* INPUT:
*	ethNum - index of the ethernet port requested
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_TRUE if the requested ethernet port is connected.
*
*******************************************************************************/
MV_BOOL mvBoardIsEthConnected(MV_U32 ethNum)
{
	if (ethNum <= mvCtrlEthMaxPortGet())
		return MV_TRUE;

	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsEthActive - this routine indicate which ports can be used by U-Boot
*
* DESCRIPTION:
*	This routine returns true if a certain Ethernet port is
*	Active and usable as a regular eth interface
*
* INPUT:
*	ethNum - index of the ethernet port requested
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_TRUE if the requested ethernet port is Active and usable.
*
*******************************************************************************/
MV_BOOL mvBoardIsEthActive(MV_U32 ethNum)
{
	/* for Msys, all connected ports are Active and usabe */
	return mvBoardIsEthConnected(ethNum);
}

/*******************************************************************************
* mvBoardIsPortInSgmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in SGMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in SGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum)
{
	return MV_TRUE;
}
/*******************************************************************************
* mvBoardIsPortInGmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in GMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in SGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInMii
*
* DESCRIPTION:
*	This routine returns MV_TRUE for port number works in MII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in MII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInMii(MV_U32 ethPortNum)
{
	return MV_FALSE;
}
/*******************************************************************************
* mvBoardSwitchCpuPortGet - Get the the Ethernet Switch CPU port
*
* DESCRIPTION:
*       This routine returns the Switch CPU port.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       the Switch CPU port, -1 if the switch is not connected.
*
*******************************************************************************/
MV_32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx)
{
	if ((board->switchInfoNum == 0) || (switchIdx >= board->switchInfoNum))
		return -1;

	return board->pSwitchInfo[switchIdx].cpuPort;
}

/*******************************************************************************
* mvBoardPhyAddrGet - Get the phy address
*
* DESCRIPTION:
*       This routine returns the Phy address of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing Phy address, -1 if the port number is wrong.
*
*******************************************************************************/
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum)
{
	return board->pBoardMacInfo[ethPortNum].boardEthSmiAddr;
}

MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum)
{
	return !mvBoardIsPortInGmii(ethPortNum) && !mvBoardIsPortInSgmii(ethPortNum);
}

/*******************************************************************************
* mvBoardMacSpeedGet - Get the Mac speed
*
* DESCRIPTION:
*       This routine returns the Mac speed if pre define of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOARD_MAC_SPEED, -1 if the port number is wrong.
*
*******************************************************************************/
MV_BOARD_MAC_SPEED mvBoardMacSpeedGet(MV_U32 ethPortNum)
{
	return board->pBoardMacInfo[ethPortNum].boardMacSpeed;
}

/*******************************************************************************
* mvBoardTclkGet - Get the board Tclk (Controller clock)
*
* DESCRIPTION:
*       This routine extract the controller core clock.
*       This function uses the controller counters to make identification.
*		Note: In order to avoid interference, make sure task context switch
*		and interrupts will not occure during this function operation
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	return 200000000; /* constant Tclock @ 200MHz (not Sampled@Reset)  */
}

/*******************************************************************************
* mvBoardSysClkGet - Get the board SysClk (CPU bus clock , i.e. DDR clock)
*
* DESCRIPTION:
*       This routine extract the CPU bus clock.
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardSysClkGet(MV_VOID)
{
	MV_U32		idx;
	MV_U32		freq_tbl_bc2[] = MV_CORE_CLK_TBL_BC2;
	MV_U32		freq_tbl_ac3[] = MV_CORE_CLK_TBL_AC3;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	idx = MSAR_CORE_CLK(MV_DFX_REG_READ(DFX_DEVICE_SAR_REG(0)), MV_DFX_REG_READ(DFX_DEVICE_SAR_REG(1)));

	if (idx >= 7)
		return 0xFFFFFFFF;

	if (family == MV_BOBCAT2_DEV_ID)
		return freq_tbl_bc2[idx] * 1000000;
	else if (family == MV_ALLEYCAT3_DEV_ID)
		return freq_tbl_ac3[idx] * 1000000;
	else
		return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoarGpioPinGet - mvBoarGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		gppClass - MV_BOARD_GPP_CLASS enum.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS gppClass, MV_U32 index)
{
	MV_U32 i, indexFound = 0;

	for (i = 0; i < board->numBoardGppInfo; i++) {
		if (board->pBoardGppInfo[i].devClass == gppClass) {
			if (indexFound == index)
				return (MV_U32) board->pBoardGppInfo[i].gppPinNum;
			else
				indexFound++;
		}
	}
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardReset - mvBoardReset
*
* DESCRIPTION:
*			Reset the board
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       None
*
*******************************************************************************/
MV_VOID mvBoardReset(MV_VOID)
{
	MV_32 resetPin;

	/* Get gpp reset pin if define */
	resetPin = mvBoardResetGpioPinGet();
	if (resetPin != MV_ERROR)
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG((int)(resetPin/32)), (1 << (resetPin % 32)));
	else {
		/* No gpp reset pin was found, try to reset using system reset out */
		MV_REG_BIT_SET( CPU_RSTOUTN_MASK_REG , BIT0);
		MV_REG_BIT_SET( CPU_SYS_SOFT_RST_REG , BIT0);
	}
}

/*******************************************************************************
* mvBoardResetGpioPinGet - mvBoardResetGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardResetGpioPinGet(MV_VOID)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_RESET, 0);
}

/*******************************************************************************
* mvBoardSDIOGpioPinGet - mvBoardSDIOGpioPinGet
*
* DESCRIPTION:
*	used for hotswap detection
* INPUT:
*	type - Type of SDIO GPP to get.
*
* OUTPUT:
*	None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardSDIOGpioPinGet(MV_BOARD_GPP_CLASS type)
{
	if ((type != BOARD_GPP_SDIO_POWER) && (type != BOARD_GPP_SDIO_DETECT) && (type != BOARD_GPP_SDIO_WP))
		return MV_FAIL;

	return mvBoarGpioPinNumGet(type, 0);
}

/*******************************************************************************
* mvBoardGpioIntMaskGet - Get GPIO mask for interrupt pins
*
* DESCRIPTION:
*		This function returns a 32-bit mask of GPP pins that connected to
*		interrupt generating sources on board.
*		For example if UART channel A is hardwired to GPP pin 8 and
*		UART channel B is hardwired to GPP pin 4 the fuinction will return
*		the value 0x000000110
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*		See description. The function return -1 if board is not identified.
*
*******************************************************************************/
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp)
{
	switch (gppGrp) {
	case (0):
		return board->intsGppMaskLow;
		break;
	case (1):
		return board->intsGppMaskMid;
		break;
	case (2):
		return board->intsGppMaskHigh;
		break;
	default:
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardMppGet - Get board dependent MPP register value
*
* DESCRIPTION:
*	MPP settings are derived from board design.
*	MPP group consist of 8 MPPs. An MPP group represents MPP
*	control register.
*       This function retrieves board dependend MPP register value.
*
* INPUT:
*       mppGroupNum - MPP group number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing MPP control register value.
*
*******************************************************************************/
MV_32 mvBoardMppGet(MV_U32 mppGroupNum)
{
	MV_U32 mppMod;

	mppMod = board->pBoardModTypeValue->boardMppMod;
	if (mppMod >= board->numBoardMppConfigValue)
		mppMod = 0; /* default */

	return board->pBoardMppConfigValue[mppMod].mppGroup[mppGroupNum];
}

/*******************************************************************************
* mvBoardGppConfigGet
*
* DESCRIPTION:
*	Get board configuration according to the input configuration GPP's.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The value of the board configuration GPP's.
*
*******************************************************************************/
MV_U32 mvBoardGppConfigGet(void)
{
	MV_U32 gpp, i, result = 0;

	for (i = 0; i < board->numBoardGppInfo; i++) {
		if (board->pBoardGppInfo[i].devClass == BOARD_GPP_CONF) {
			gpp = board->pBoardGppInfo[i].gppPinNum;
			result <<= 1;
			result |= (mvGppValueGet(gpp >> 5, 1 << (gpp & 0x1F)) >> (gpp & 0x1F));
		}
	}
	return result;

}

/*******************************************************************************
* mvBoardModuleTypePrint
*
* DESCRIPTION:
*	Print on-board detected modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardMppModuleTypePrint(MV_VOID)
{
	return;
}
MV_VOID mvBoardOtherModuleTypePrint(MV_VOID)
{
	return;
}

/*******************************************************************************
* mvBoardIsGbEPortConnected
*
* DESCRIPTION:
*	Checks if a given GbE port is actually connected to the GE-PHY, internal Switch or any RGMII module.
*
* INPUT:
*	port - GbE port number (0 or 1).
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_TRUE if port is connected, MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardIsGbEPortConnected(MV_U32 ethPortNum)
{
	switch (ethPortNum) {
	case 0:
	case 1:
		break;
	case 2:
	case 3:
	default:
		return MV_FALSE;
	}

	return MV_TRUE;
}

/* Board devices API managments */

/*******************************************************************************
* mvBoardGetDeviceNumber - Get number of device of some type on the board
*
* DESCRIPTION:
*
* INPUT:
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		number of those devices else the function returns 0
*
*
*******************************************************************************/
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass)
{
	MV_U32 foundIndex = 0, devNum;

	for (devNum = START_DEV_CS; devNum < board->numBoardDeviceIf; devNum++) {
		if (board->pDevCsInfo[devNum].devClass == devClass)
			foundIndex++;
	}

	return foundIndex;
}

/*******************************************************************************
* mvBoardGetDeviceBaseAddr - Get base address of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*	Base address else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return mvCpuIfTargetWinBaseLowGet(DEV_TO_TARGET(devEntry->deviceCS));

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDeviceBusWidth - Get Bus width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		Bus width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->busWidth;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDeviceWidth - Get dev width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		dev width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->devWidth;

	return MV_ERROR;
}

/*******************************************************************************
* mvBoardGetDeviceWinSize - Get the window size of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		window size else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return mvCpuIfTargetWinSizeGet(DEV_TO_TARGET(devEntry->deviceCS));

	return 0xFFFFFFFF;
}

/*******************************************************************************
* boardGetDevEntry - returns the entry pointer of a device on the board
*
* DESCRIPTION:
*
* INPUT:
*	devIndex - The device sequential number on the board
*	devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*	None.
*
* RETURN:
*	If the device is found on the board the then the functions returns the
*	dev number else the function returns 0x0
*
*******************************************************************************/
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_U32 foundIndex = 0, devIndex;

	for (devIndex = START_DEV_CS; devIndex < board->numBoardDeviceIf; devIndex++) {
		if (board->pDevCsInfo[devIndex].devClass == devClass) {
			if (foundIndex == devNum)
				return &(board->pDevCsInfo[devIndex]);
			foundIndex++;
		}
	}

	/* device not found */
	return NULL;
}

/*******************************************************************************
* boardGetDevCSNum
*
* DESCRIPTION:
*	Return the device's chip-select number.
*
* INPUT:
*	devIndex - The device sequential number on the board
*	devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*	None.
*
* RETURN:
*	If the device is found on the board the then the functions returns the
*	dev number else the function returns 0x0
*
*******************************************************************************/
MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->deviceCS;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardTwsiAddrTypeGet -
*
* DESCRIPTION:
*	Return the TWSI address type for a given twsi device class.
*
* INPUT:
*	twsiClass - The TWSI device to return the address type for.
*	index	  - The TWSI device index (Pass 0 in case of a single
*		    device)
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TWSI address type.
*
*******************************************************************************/
MV_U8 mvBoardTwsiAddrTypeGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if (board->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return board->pBoardTwsiDev[i].twsiDevAddrType;
			else
				indexFound++;
		}
	}
	return (MV_ERROR);
}

/*******************************************************************************
* mvBoardTwsiAddrGet -
*
* DESCRIPTION:
*	Return the TWSI address for a given twsi device class.
*
* INPUT:
*	twsiClass - The TWSI device to return the address type for.
*	index	  - The TWSI device index (Pass 0 in case of a single
*		    device)
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TWSI address.
*
*******************************************************************************/
MV_U8 mvBoardTwsiAddrGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if (board->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return board->pBoardTwsiDev[i].twsiDevAddr;
			else
				indexFound++;
		}
	}
	return (0xFF);
}

/*******************************************************************************
* mvBoardNandWidthGet -
*
* DESCRIPTION: Get the width of the first NAND device in bytes
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: 1, 2, 4 or MV_ERROR
*
*
*******************************************************************************/
MV_32 mvBoardNandWidthGet(void)
{
	MV_U32 devNum;
	MV_U32 devWidth;

	for (devNum = START_DEV_CS; devNum < board->numBoardDeviceIf; devNum++) {
		devWidth = mvBoardGetDeviceWidth(devNum, BOARD_DEV_NAND_FLASH);
		if (devWidth != MV_ERROR)
			return (devWidth / 8);
	}

	/* NAND wasn't found */
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardSet - Set Board model
*
* DESCRIPTION:
*	Sets the global board structures and global board ID, according to boardId value
*	1. Detect correct MSYS family(Bobcat2/Alleycat3)
*	2. Detect Marvell/Customer board
*
* INPUT:
*	boardId :
*	- U-Boot : set boardID via mvBoardIdGet() - according to pre-compilation flag.
*	- Kernel : set boardID via tags received from U-Boot .
*
* OUTPUT:
*	None.
*
* RETURN:
*	void
*
*******************************************************************************/
static MV_U32 gBoardId = -1;
MV_VOID mvBoardSet(MV_U32 boardId)
{
	/* Marvell Bobcat2 Boards */
	if (boardId >= BC2_MARVELL_BOARD_ID_BASE && boardId < BC2_MARVELL_MAX_BOARD_ID) { /* Marvell Board */
		board = marvellBC2BoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	/* Marvell AlleyCat3 Boards */
	} else if (boardId >= AC3_MARVELL_BOARD_ID_BASE && boardId < AC3_MARVELL_MAX_BOARD_ID) { /* Marvell Board */
		board = marvellAC3BoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	/* Customer Bobcat2 Boards */
	} else if (boardId >= BC2_CUSTOMER_BOARD_ID_BASE && boardId < BC2_CUSTOMER_MAX_BOARD_ID) { /* Customer Board */
		board = customerBC2BoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	/* Customer AlleyCat3 Boards */
	} else if (boardId >= AC3_CUSTOMER_BOARD_ID_BASE && boardId < AC3_CUSTOMER_MAX_BOARD_ID) { /* Customer Board */
		board = customerAC3BoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	} else {
		mvOsPrintf("%s: Error: wrong board Id (%d)\n", __func__, boardId);
#ifdef CONFIG_ALLEYCAT3
		gBoardId = AC3_CUSTOMER_BOARD_ID0;
		board = customerAC3BoardInfoTbl[gBoardId];
#else
		gBoardId = BC2_CUSTOMER_BOARD_ID0;
		board = customerBC2BoardInfoTbl[gBoardId];
#endif
		mvOsPrintf("Applying default Customer board ID (%d: %s)\n", gBoardId, board->boardName);
	}
}

/*******************************************************************************
* mvBoardIdGet - Get Board model
*
* DESCRIPTION:
*       This function returns board ID.
*       Board ID is 32bit word constructed of board model (16bit) and
*       board revision (16bit) in the following way: 0xMMMMRRRR.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit board ID number, '-1' if board is undefined.
*
*******************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
	if (gBoardId != -1)
		return gBoardId;

#if defined CONFIG_ALLEYCAT3

	#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = AC3_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = AC3_CUSTOMER_BOARD_ID1;
		#endif
	#else	/* !CONFIG_CUSTOMER_BOARD_SUPPORT */

	/* For Marvell Boards: Temporarily set generic board struct pointer to
	   use S@R TWSI address, and read board ID */
	board = marvellAC3BoardInfoTbl[mvBoardIdIndexGet(DB_AC3_ID)];
	gBoardId = DB_AC3_ID; /* Terporary for usage by mvCtrlDevFamilyIdGet */
	MV_U8 readValue;

	if (mvBoardSarBoardIdGet(&readValue) != MV_OK) {
		mvOsPrintf("%s: Error obtaining Board ID\n", __func__);
		mvOsPrintf("%s: Set default board ID to DB-DXAC3-MM\n", __func__);
		readValue = DB_AC3_ID;
	}

	if (readValue < AC3_MARVELL_BOARD_NUM)
		gBoardId = AC3_MARVELL_BOARD_ID_BASE + readValue;
	else {
		mvOsPrintf("%s: Error: read wrong Board ID (%d)\n", __func__, readValue);
		return INVALID_BOARD_ID;
	}

	#endif	/* CONFIG_CUSTOMER_BOARD_SUPPORT */

#else /* CONFIG_BOBCAT2 */

	#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = BC2_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = BC2_CUSTOMER_BOARD_ID1;
		#endif
	#else	/* !CONFIG_CUSTOMER_BOARD_SUPPORT */
		#if defined(DB_BOBCAT2)
			gBoardId = DB_DX_BC2_ID;
		#elif defined(RD_BOBCAT2)
			gBoardId = RD_DX_BC2_ID;
		#elif defined(RD_MTL_BOBCAT2)
			gBoardId = RD_MTL_BC2;
		#else
			mvOsPrintf("%s: Board ID must be defined!\n", __func__);
			while (1)
				continue;
		#endif
	#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

#endif

	return gBoardId;
}
/*******************************************************************************
* mvBoardTwsiRead -
*
* DESCRIPTION:
*
* INPUT:
*       TWSI class id
*	device num - one of three devices
*	reg num - 0 or 1
*	pointer for output data
* OUTPUT:
*		reg value.
*
* RETURN:
*		status OK or Error
*
*******************************************************************************/
MV_STATUS mvBoardTwsiRead(MV_BOARD_TWSI_CLASS class1, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* Read MPP module ID */
	DB(mvOsPrintf("Board: Read S@R device read\n"));
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(class1, devNum);
	if (0xFF == twsiSlave.slaveAddr.address)
		return MV_ERROR;
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(class1, devNum);

	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	twsiSlave.validOffset = MV_TRUE;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);


	if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, 1)) {
		DB(mvOsPrintf("Board: Read S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read TWSI succeded,(0x%x)\n", *pData));

	return MV_OK;
}
/*******************************************************************************
* mvBoardTwsiWrite -
*
* DESCRIPTION:
*
* INPUT:
*       TWSI class id
*	device num - one of three devices
*	reg num - 0 or 1
*	input reg data
*
* OUTPUT:
*		None.
*
* RETURN:
*		status OK or Error
*******************************************************************************/
MV_STATUS mvBoardTwsiWrite(MV_BOARD_TWSI_CLASS class1, MV_U8 devNum, MV_U8 regNum, MV_U8 regVal)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(class1, devNum);
	if (0xFF == twsiSlave.slaveAddr.address)
		return MV_ERROR;
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(class1, devNum);
	twsiSlave.validOffset = MV_TRUE;
	DB(mvOsPrintf("Board: Write S@R device addr %x, type %x, data %x\n",
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write S@R succeded\n"));

	return MV_OK;
}

/*******************************************************************************
* mvBoardTwsiSatRGet -
*
* DESCRIPTION:
*
* INPUT:
*	device num - one of three devices
*	reg num - 0 or 1
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum, MV_U8 *pData)
{
	return mvBoardTwsiRead(BOARD_DEV_TWSI_SATR, devNum, regNum, pData);
}

/*******************************************************************************
* mvBoardTwsiSatRSet -
*
* DESCRIPTION:
*
* INPUT:
*	devNum - one of three devices
*	regNum - 0 or 1
*	regVal - value
*
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal)
{
	return mvBoardTwsiWrite(BOARD_DEV_TWSI_SATR, devNum, regNum, regVal);
}

/*******************************************************************************
* SatR Configuration functions
*******************************************************************************/
MV_STATUS mvBoardCoreFreqGet(MV_U8 *value)
{
	MV_U8		sar0;
	MV_STATUS	rc1;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);
	MV_U32		fieldOffs = (family == MV_BOBCAT2_DEV_ID) ? 0 : 2;

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	rc1 = mvBoardTwsiSatRGet(1, 0, &sar0);
	if (MV_ERROR == rc1)
		return MV_ERROR;

	*value = (sar0 & (0x7 << fieldOffs)) >> fieldOffs;

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardCoreFreqSet(MV_U8 freqVal)
{
	MV_U8		sar0;
	MV_STATUS	rc1;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);
	MV_U32		fieldOffs = (family == MV_BOBCAT2_DEV_ID) ? 0 : 2;

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	rc1 = mvBoardTwsiSatRGet(1, 0, &sar0);
	if (MV_ERROR == rc1)
		return MV_ERROR;

	sar0 &= ~(0x7 << fieldOffs);
	sar0 |= (freqVal & 0x7) << fieldOffs;

	if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar0)) {
		DB(mvOsPrintf("Board: Write core Freq S@R fail\n"));
		return MV_ERROR;
	}


	DB(mvOsPrintf("Board: Write core FreqOpt S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvBoardCpuFreqGet(MV_U8 *value)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family == MV_BOBCAT2_DEV_ID) {
		MV_U8		sar2;

		/* BC2 */
		if ((MV_ERROR == mvBoardTwsiSatRGet(1, 0, &sar)) ||
			(MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar2)))
			return MV_ERROR;

		*value = ((((sar2 & 0x1)) << 3) | ((sar & 0x18) >> 3));

	} else if (family == MV_ALLEYCAT3_DEV_ID) {

		/* AC3 */
		if (MV_ERROR == mvBoardTwsiSatRGet(3, 0, &sar))
			return MV_ERROR;

		*value = sar & 0x7;

	} else {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardCpuFreqSet(MV_U8 freqVal)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family == MV_BOBCAT2_DEV_ID) {
		MV_U8		sar2;
		/* BC2 */
		if ((MV_ERROR == mvBoardTwsiSatRGet(1, 0, &sar)) ||
			(MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar2)))
			return MV_ERROR;

		sar  &= ~0x18;
		sar2 &= ~0x1;
		sar  |= ((freqVal & 0x03) << 3);
		sar2 |= ((freqVal & 0x04) >> 2);

		if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
			DB(mvOsPrintf("Board: Write CpuFreq(1) S@R fail\n"));
			return MV_ERROR;
		}
		if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar2)) {
			DB(mvOsPrintf("Board: Write CpuFreq(2) S@R fail\n"));
			return MV_ERROR;
		}

	} else if (family == MV_ALLEYCAT3_DEV_ID) {

		/* AC3 */
		if (MV_ERROR == mvBoardTwsiSatRGet(3, 0, &sar))
			return MV_ERROR;

		sar  &= ~(0x7);
		sar  |= (freqVal & 0x7);

		if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar)) {
			DB(mvOsPrintf("Board: Write CpuFreq S@R fail\n"));
			return MV_ERROR;
		}

	} else {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write CpuFreq S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardTmFreqGet(MV_U8 *value)
{
	MV_U8		sar2;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_BOBCAT2_DEV_ID) {
		DB(mvOsPrintf("%s: AC3 controller family is not supported\n", __func__));
		return MV_ERROR; /* AC3 */
	}

	/* BC2 */
	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar2))
		return MV_ERROR;

	*value = ((sar2 & 0x0E) >> 1);

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardTmFreqSet(MV_U8 freqVal)
{
	MV_U8		sar2;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_BOBCAT2_DEV_ID) {
		DB(mvOsPrintf("%s: AC3 controller family is not supported\n", __func__));
		return MV_ERROR; /* AC3 */
	}

	/* BC2 */
	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar2))
		return MV_ERROR;

	sar2 &= ~0xE;
	sar2 |= ((freqVal & 0x07) << 1);

	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar2)) {
		DB(mvOsPrintf("Board: Write TM-Freq S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write TM-Freq S@R succeeded\n"));

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardBootDevGet(MV_U8 *value)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);
	MV_U8		twsiDevice = (family == MV_BOBCAT2_DEV_ID) ? 3 : 2;

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(twsiDevice, 0, &sar))
		return MV_ERROR;

	*value = (sar & 0x7);

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardBootDevSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);
	MV_U8		twsiDevice = (family == MV_BOBCAT2_DEV_ID) ? 3 : 2;

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(twsiDevice, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x7);
	sar |= (val & 0x7);

	if (MV_OK != mvBoardTwsiSatRSet(twsiDevice, 0, sar)) {
		DB(mvOsPrintf("Board: Write BootDev S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write BootDev S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvBoardDeviceIdGet(MV_U8 *value)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(0, 0, &sar))
		return MV_ERROR;

	*value = (sar & 0x1F);

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardDeviceIdSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if ((family != MV_ALLEYCAT3_DEV_ID) && (family != MV_BOBCAT2_DEV_ID)) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(0, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x1F);
	sar |= (val & 0x1F);

	if (MV_OK != mvBoardTwsiSatRSet(0, 0, sar)) {
		DB(mvOsPrintf("Board: Write device-id S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write deviceid S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardDeviceNumGet(MV_U8 *value)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(1, 0, &sar))
		return MV_ERROR;

	*value = (sar & 0x3);

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardDeviceNumSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(1, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x3);
	sar |= (val & 0x3);

	if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
		DB(mvOsPrintf("Board: Write device-id S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write deviceid S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardPcieModeGet(MV_U8 *val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar))
		return MV_ERROR;

	*val = (sar & (0x1 << 3)) >> 3;

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardPcieModeSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x1 << 3);
	sar |= (val & 0x1) << 3;

	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar)) {
		DB(mvOsPrintf("Board: Write pcimode S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write pcimode S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardPcieClockGet(MV_U8 *val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar))
		return MV_ERROR;

	*val = (sar & (0x1 << 4)) >> 4;

	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvBoardPcieClockSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(2, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x1 << 4);
	sar |= (val & 0x1) << 4;

	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar)) {
		DB(mvOsPrintf("Board: Write pciclock S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write pciclock S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardPllClockGet(MV_U8 *val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(3, 0, &sar))
		return MV_ERROR;

	*val = (sar & (0x1 << 3)) >> 3;

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardPllClockSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: BC2 controller family is not supported\n", __func__));
		return MV_ERROR; /* Not supported on BC2 */
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(3, 0, &sar))
		return MV_ERROR;

	sar &= ~(0x1 << 3);
	sar |= (val & 0x1) << 3;

	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar)) {
		DB(mvOsPrintf("Board: Write pllclock S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write pcllclock S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *value)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(1, 1, &sar))
		return MV_ERROR;

	*value = (sar & 0x7);

	return MV_OK;
}

/*******************************************************************************/
MV_STATUS mvBoardSarBoardIdSet(MV_U8 val)
{
	MV_U8		sar;
	MV_U16		family = mvCtrlDevFamilyIdGet(0);

	if (family != MV_ALLEYCAT3_DEV_ID) {
		DB(mvOsPrintf("%s: Controller family (0x%04x) is not supported\n", __func__, family));
		return MV_ERROR;
	}

	if (val >= AC3_MARVELL_BOARD_NUM) {
		mvOsPrintf("%s: Error: Unsupported board ID (%d)\n", __func__, val);
		return MV_ERROR;
	}

	if (MV_ERROR == mvBoardTwsiSatRGet(1, 1, &sar))
		return MV_ERROR;

	sar &= ~(0x7);
	sar |= (val & 0x7);

	if (MV_OK != mvBoardTwsiSatRSet(1, 1, sar)) {
		DB(mvOsPrintf("Board: Write boardid S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write boardid S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************
* End of SatR Configuration functions
*******************************************************************************/

/*******************************************************************************
* mvBoardMppModulesScan
*
* DESCRIPTION:
*	Scan for modules connected through MPP lines.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardMppModulesScan(void)
{
	return MV_OK;
}

/*******************************************************************************
* mvBoardOtherModulesScan
*
* DESCRIPTION:
*	Scan for modules connected through SERDES/LVDS/... lines.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardOtherModulesScan(void)
{
	return MV_OK;
}
/*******************************************************************************
* mvBoardIsSwitchModuleConnected
*
* DESCRIPTION:
*	Check if switch module is connected to the board.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_TRUE / MV_FALSE
*
*******************************************************************************/
MV_BOOL mvBoardIsSwitchModuleConnected(void)
{
	return MV_FALSE;
}
/*******************************************************************************
* mvBoardPexInfoGet - Get board PEX Info
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*******************************************************************************/
MV_BOARD_PEX_INFO *mvBoardPexInfoGet(void)
{
	return &board->boardPexInfo;
}

/*******************************************************************************
* mvBoardDebugLeg - Set the board debug Leds
*
* DESCRIPTION: turn on/off status leds.
* 	       Note: assume MPP leds are part of group 0 only.
*
* INPUT:
*       hexNum - Number to be displayed in hex by Leds.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardDebugLed(MV_U32 hexNum)
{
	MV_U32 val[MV_GPP_MAX_GROUP] = {0};
	MV_U32 mask[MV_GPP_MAX_GROUP] = {0};
	MV_U32 digitMask;
	MV_U32 i, pinNum, gppGroup;

	if (board->pLedGppPin == NULL)
		return;

	hexNum &= (1 << board->activeLedsNumber) - 1;

	for (i = 0, digitMask = 1; i < board->activeLedsNumber; i++, digitMask <<= 1) {
			pinNum = board->pLedGppPin[i];
			gppGroup = pinNum / 32;
			if (hexNum & digitMask)
				val[gppGroup]  |= (1 << (pinNum - gppGroup * 32));
			mask[gppGroup] |= (1 << (pinNum - gppGroup * 32));
	}

	for (gppGroup = 0; gppGroup < MV_GPP_MAX_GROUP; gppGroup++) {
		/* If at least one bit is set in the mask, update the whole GPP group */
		if (mask[gppGroup])
			mvGppValueSet(gppGroup, mask[gppGroup], board->ledsPolarity == 0 ?
					val[gppGroup] : ~val[gppGroup]);
	}

}
/*******************************************************************************
* mvBoardCpuCoresNumGet - Get number of CPU
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
*       8bit number of CPU - 0 for UP
*
*******************************************************************************/
MV_U8 mvBoardCpuCoresNumGet(MV_VOID)
{
	return 0;
}
