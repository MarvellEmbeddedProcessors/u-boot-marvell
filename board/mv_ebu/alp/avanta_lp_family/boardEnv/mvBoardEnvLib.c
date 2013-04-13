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
#include "pci/mvPci.h"
#include "device/mvDevice.h"

#if defined(CONFIG_MV_ETH_NETA)
#include "neta/gbe/mvEthRegs.h"
#elif defined(CONFIG_MV_ETH_PP2)
#include "pp2/gbe/mvPp2GbeRegs.h"
#endif

#include "gpp/mvGppRegs.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x)   x
#else
#define DB(x)
#endif

extern MV_BOARD_INFO *boardInfoTbl[];
extern MV_BOARD_SATR_INFO boardSatrInfo[];
extern MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[];

/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
static MV_U32 gBoardId = MV_INVALID_BOARD_ID;
static MV_BOARD_INFO *board = NULL;


/*******************************************************************************
* mvBoardEnvInit
*
* DESCRIPTION:
*	In this function the board environment take care of device bank
*	initialization.
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
	/*
	 * FPGA board doesn't use MPP neither GPIO.
	 */
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA_FPGA)
	MV_U32 nandDev;
	MV_U32 norDev;

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
	MV_REG_WRITE(GPP_DATA_OUT_REG(2), board->gppOutValHigh);

	/* set GPP polarity */
	mvGppPolaritySet(0, 0xFFFFFFFF, board->gppPolarityValLow);
	mvGppPolaritySet(1, 0xFFFFFFFF, board->gppPolarityValMid);
	mvGppPolaritySet(2, 0xFFFFFFFF, board->gppPolarityValHigh);

	/* Set GPP Out Enable */
	mvGppTypeSet(0, 0xFFFFFFFF, board->gppOutEnValLow);
	mvGppTypeSet(1, 0xFFFFFFFF, board->gppOutEnValMid);
	mvGppTypeSet(2, 0xFFFFFFFF, board->gppOutEnValHigh);
#endif
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
	return mvBoardIdGet() >> 16;
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
	return mvBoardIdGet() & 0xFFFF;
}

/*******************************************************************************
* mvBoardNameGet - Get Board name
*
* DESCRIPTION:
*       This function returns a string describing the board model and revision.
*       String is extracted from board I2C EEPROM.
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
MV_STATUS mvBoardNameGet(char *pNameBuff, MV_U32 size)
{
	mvOsSNPrintf(pNameBuff, size, "%s", board->boardName);
	return MV_OK;
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
#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return ethPortNum == 2;
#endif

	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInGmii
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
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum)
{
	return MV_FALSE;
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

#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return 8;
#endif

	if (ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return MV_ERROR;
	}

	return board->pBoardMacInfo[ethPortNum].boardEthSmiAddr;
}

/*******************************************************************************
* mvBoardSpecInitGet -
*
* DESCRIPTION:
*       This routine returns the board specific initializtion information.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       A pointer to the array holding the board specific initializations.
*
*******************************************************************************/
MV_BOARD_SPEC_INIT *mvBoardSpecInitGet(MV_VOID)
{
	return board->pBoardSpecInit;
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
	if (ethPortNum >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: wrong eth port (%d)\n", __func__, ethPortNum);
		return BOARD_MAC_SPEED_100M;
	}

#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return (ethPortNum == 2) ? BOARD_MAC_SPEED_1000M : BOARD_MAC_SPEED_100M;
#endif

	return board->pBoardMacInfo[ethPortNum].boardMacSpeed;
}

/*******************************************************************************
* mvBoardIsPortLoopback -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for loopback port number or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port is loopback.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortLoopback(MV_U32 ethPortNum)
{
#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return (ethPortNum == 2);
#endif

	return MV_FALSE;
}

/*******************************************************************************
* mvBoardTclkGet
*
* DESCRIPTION:
*       This routine extract the controller core clock, aka, TCLK clock.
*
* INPUT:
* 	None.
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
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	/* FPGA is limited to 25Mhz */
	return MV_FPGA_CORE_CLK;
#else
	switch (mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT)) {
	case 0:
		return _166MHz;
	case 1:
		return _200MHz;
	default:
		return _200MHz;
	}
#endif
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
	MV_FREQ_MODE freqMode;
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	/* FPGA is limited to 25Mhz */
	return MV_FPGA_SYS_CLK;
#else
	if (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode))
		return (MV_U32)(1000000 * simple_strtoul(freqMode.ddrFreq, NULL, 16));
	else
		return MV_ERROR;
#endif
}

/*******************************************************************************
* mvBoardDebugLedNumGet - Get number of debug Leds
*
* DESCRIPTION:
* INPUT:
*       boardId
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId)
{
	return board->activeLedsNumber;
}

/*******************************************************************************
* mvBoardDebugLed - Set the board debug Leds
*
* DESCRIPTION: turn on/off status leds.
*              Note: assume MPP leds are part of group 0 only.
*
* INPUT:
*       hexNum - Number to be displied in hex by Leds.
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
	/* empty */
}

/*******************************************************************************
* mvBoarGpioPinNumGet
*
* DESCRIPTION:
*
* INPUT:
*	gppClass - MV_BOARD_GPP_CLASS enum.
*
* OUTPUT:
*	None.
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
				return (MV_U32)board->pBoardGppInfo[i].gppPinNum;
			else
				indexFound++;
		}
	}

	return MV_ERROR;
}

/*******************************************************************************
* mvBoardReset
*
* DESCRIPTION:
*	Reset the board
* INPUT:
*	None.
*
* OUTPUT:
*	None.
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
	else
	{
		/* No gpp reset pin was found, try to reset using system reset out */
		MV_REG_BIT_SET( CPU_RSTOUTN_MASK_REG , BIT0);
		MV_REG_BIT_SET( CPU_SYS_SOFT_RST_REG , BIT0);
	}
}

/*******************************************************************************
* mvBoardResetGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
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
* mvBoardSDIOGpioPinGet
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
	if (type != BOARD_GPP_SDIO_POWER &&
	    type != BOARD_GPP_SDIO_DETECT &&
	    type != BOARD_GPP_SDIO_WP)
		return MV_FAIL;

	return mvBoarGpioPinNumGet(type, 0);
}

/*******************************************************************************
* mvBoardUSBVbusGpioPinGet - return Vbus input GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusGpioPinGet(MV_32 devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS, devId);
}

/*******************************************************************************
* mvBoardUSBVbusEnGpioPinGet - return Vbus Enable output GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusEnGpioPinGet(MV_32 devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS_EN, devId);
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
	case 0:
		return board->intsGppMaskLow;
		break;
	case 1:
		return board->intsGppMaskMid;
		break;
	case 2:
		return board->intsGppMaskHigh;
		break;
	default:
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardSlicMppModeGet - Get board MPP Group type for SLIC unit (pre-defined)
*
* DESCRIPTION:
*	if not using auto detection mudules according to board configuration settings,
*	use pre-defined SLIC type from board information
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing MPP control register value.
*
*******************************************************************************/
MV_U32 mvBoardSlicUnitTypeGet(MV_VOID)
{
	return board->pBoardModTypeValue->boardMppSlic;
}

/*******************************************************************************
* mvBoardSlicUnitTypeSet - Get board MPP Group type for SLIC unit (pre-defined)
*
* DESCRIPTION:
*	if not using auto detection mudules according to board configuration settings,
*	use pre-defined SLIC type from board information
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing MPP control register value.
*
*******************************************************************************/
MV_VOID mvBoardSlicUnitTypeSet(MV_U32 slicType)
{
	board->pBoardModTypeValue->boardMppSlic = slicType;
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
	return board->pBoardMppConfigValue->mppGroup[mppGroupNum];
}

/*******************************************************************************
* mvBoardMppSet - Set board dependent MPP register value
*
* DESCRIPTION:
*	This function updates board dependend MPP register value.
*
* INPUT:
*       mppGroupNum - MPP group number.
*	mppValue - new MPP value to be written
*
* OUTPUT:
*       None.
*
* RETURN:
*       -None
*
*******************************************************************************/
MV_VOID mvBoardMppSet(MV_U32 mppGroupNum, MV_U32 mppValue)
{
	board->pBoardMppConfigValue->mppGroup[mppGroupNum] = mppValue;
}

/*******************************************************************************
* mvBoardMppTypeSet - Set board dependent MPP Group Type value
*
* DESCRIPTION:
*	This function updates board dependend MPP Group Type value.
*
* INPUT:
*       mppGroupNum - MPP group number.
*	groupType - new MPP Group type. derrive MPP Value using groupType
*
* OUTPUT:
*       None.
*
* RETURN:
*       -None
*
*******************************************************************************/
MV_VOID mvBoardMppTypeSet(MV_U32 mppGroupNum, MV_U32 groupType)
{
	MV_U32 mppVal;
	MV_U32 mppGroups[MV_BOARD_MAX_MPP_GROUPS][MV_BOARD_MPP_GROUPS_MAX_TYPES] = MPP_GROUP_TYPES;

	mppVal = mppGroups[mppGroupNum][groupType];
	mvBoardMppSet(mppGroupNum,mppVal);

	/* add Group types update here (if needed for later usage),
	 * and add mvBoardMppTypeGet to detect which type is in use currently */
}

/*******************************************************************************
* mvBoardInfoUpdate - Update Board information structures according to auto-detection.
*
* DESCRIPTION:
*	Update board information according to detection using TWSI bus.
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
MV_VOID mvBoardInfoUpdate(MV_VOID)
{
	MV_U32 slicDev;

	mvBoardEthComplexInfoUpdate();
	mvBoardMppIdUpdate();
	mvBoardSwitchInfoUpdate();
	slicDev = mvCtrlSysConfigGet(MV_CONFIG_SLIC_TDM_DEVICE);
	mvBoardSlicUnitTypeSet(slicDev);
}

/*******************************************************************************
* mvBoardMppIdUpdate - Update MPP ID's according to modules auto-detection.
*
* DESCRIPTION:
*	Update MPP ID's according to on-board modules as detected using TWSI bus.
*	Update board information for changed mpp values
	Must run AFTER mvBoardEthComplexInfoUpdate
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
MV_VOID mvBoardMppIdUpdate(MV_VOID)
{
	MV_BOARD_BOOT_SRC bootDev;
	MV_SLIC_UNIT_TYPE slicDev;
	MV_U32	ethComplexOptions = mvBoardEthComplexConfigGet();

	/* MPP Groups initialization : */
	/* Group 0-1 - Boot device (or if SPI1 boot : Groups 3-4) */
	bootDev = mvBoardBootDeviceGroupSet();

	/* Group 2 - SLIC Tdm unit */
	slicDev = mvBoardSlicUnitTypeGet();
	mvBoardMppTypeSet(2, slicDev);

	/* Groups 3-4  - (only if not Booting from SPI1)*/
	if (bootDev != MSAR_0_BOOT_SPI1_FLASH) {
		if ( (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_RGMII1) == MV_TRUE) /* 00 - MAC1 connected to RGMII-1 */
			mvBoardMppTypeSet(3, GE1_UNIT);
		else
			mvBoardMppTypeSet(3, SDIO_UNIT);

		if (slicDev == SLIC_LANTIQ_ID)
			mvBoardMppTypeSet(4, GE1_CPU_SMI_CTRL_TDM_LQ_UNIT);
		else    /*REF_CLK_OUT*/
			mvBoardMppTypeSet(4, GE1_CPU_SMI_CTRL_REF_CLK_OUT);
	}

	/* Groups 5-6-7  - */
	if ( (ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_RGMII0) == MV_TRUE) { /* 10 - MAC0 connected to RGMII-0  */
		/* omriii: what scenario leads to enable PON_CLK_OUT insted of PON_TX_FAULT?? */
		mvBoardMppTypeSet(5, GE0_UNIT_PON_TX_FAULT);
		mvBoardMppTypeSet(6, GE0_UNIT);
		mvBoardMppTypeSet(7, GE0_UNIT_LED_MATRIX);      /* omriii :when to use GE0_UNIT_UA1_PTP */
	}else if ( (ethComplexOptions & MV_ETHCOMP_SW_P4_2_RGMII0) == MV_TRUE) {
		mvBoardMppTypeSet(5, SWITCH_P4_PON_TX_FAULT);
		mvBoardMppTypeSet(6, SWITCH_P4);
		mvBoardMppTypeSet(7, SWITCH_P4_LED_MATRIX); /* omriii : when to use SWITCH_P4_UA1_PTP */
	}
}

/*******************************************************************************
* mvBoardEthComplexInfoUpdate
*
* DESCRIPTION:
*	Update etherntComplex configuration,
*	according to modules detection (S@R & board configuration)
*
** INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - On failure.
*
*******************************************************************************/
MV_STATUS mvBoardEthComplexInfoUpdate(MV_VOID)
{
	MV_U32 ethComplexOptions = 0x0;
	MV_ETH_COMPLEX_TOPOLOGY mac0con, mac1con;

	/* Ethernet Complex initialization : */
	if ( (mac0con = mvBoardMac0ConfigGet()) != MV_ERROR)
		ethComplexOptions |= mac0con;
	else {
		mvOsPrintf("%s: Error: Ethernet Complex init failed (MAC0). Using default configuration. \n", __func__);
		return MV_ERROR;
	}

	if ( (mac1con = mvBoardMac1ConfigGet()) != MV_ERROR)
		ethComplexOptions |= mac1con;
	else {
		mvOsPrintf("%s: Error: Ethernet Complex init failed (MAC1). Using default configuration. \n", __func__);
		return MV_ERROR;
	}

	/* if MAC0 is NOT connected to QUAD_PHY_P0 --> connect Switch port 0 to QUAD_PHY_P0 */
	if ( (ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) == MV_FALSE )
		ethComplexOptions |= MV_ETHCOMP_SW_P0_2_GE_PHY_P0;

	/* if MAC1 is NOT connected to QUAD_PHY_P3 --> connect Switch port 3 to QUAD_PHY_P3 */
	if ( (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) == MV_FALSE )
		ethComplexOptions |= MV_ETHCOMP_SW_P3_2_GE_PHY_P3;

	/* if MAC1 is NOT connected to PON SerDes --> connect PON MAC to to PON SerDes */
	if ( (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES) == MV_FALSE )
		ethComplexOptions |= MV_ETHCOMP_P2P_MAC_2_PON_ETH_SERDES;

	mvBoardEthComplexConfigSet(ethComplexOptions);
	return MV_OK;
}
/*******************************************************************************
* mvBoardSwitchInfoUpdate
*
* DESCRIPTION:
*	Update board switch information structure,
*	according to modules detection (S@R & board configuration)
*
** INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - On failure.
*
*******************************************************************************/
MV_STATUS mvBoardSwitchInfoUpdate(MV_VOID)
{
	MV_U32 i, ethComplexOptions = mvBoardEthComplexConfigGet();

	/* Update board switch information according to Ethernet Complex init */
	if ((!mvBoardIsInternalSwitchConnected(0)) && (!mvBoardIsInternalSwitchConnected(1))) {
		board->switchInfoNum = 0;
		return MV_OK;
	}

	/* Update the cpuPort & connectedPort fields */
	board->pSwitchInfo[0].cpuPort = -1;
	for (i = 0; i < MV_ETH_MAX_PORTS; i++)
		board->pSwitchInfo[0].connectedPort[i] = -1;

	/* Check if Switch port 6 is connected to MAC0 */
	if (ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_SW_P6) {
		board->pSwitchInfo[0].cpuPort = 6;	/* if connected, Switch port 6 is the default cpu port */
		board->pSwitchInfo[0].connectedPort[0] = 6;
	}

	/* Check if Switch port 4 is connected to MAC1 */
	if (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_SW_P4) {
		board->pSwitchInfo[0].connectedPort[1] = 4;
		if (board->pSwitchInfo[0].cpuPort != -1)
			board->pSwitchInfo[0].cpuPort = 4;	/*  If Switch port 6 is not connected, set Sw port 4 to cpu port */
	}

	/* Set all switch ports to -1 and clean connected ports mask*/
	board->pSwitchInfo[0].connectedPortMask = 0;
	for (i=0 ; i < BOARD_ETH_SWITCH_PORT_NUM; i++)
		board->pSwitchInfo[0].switchPort[i] = -1;

	/* Set Switch port 0 */
	if (ethComplexOptions & MV_ETHCOMP_SW_P0_2_GE_PHY_P0) {
		board->pSwitchInfo[0].switchPort[0] = 0;
		board->pSwitchInfo[0].connectedPortMask |= BIT0;
	}
	/* Set Switch port 1-2 */
	board->pSwitchInfo[0].switchPort[1] = 1;
	board->pSwitchInfo[0].switchPort[2] = 2;
	board->pSwitchInfo[0].connectedPortMask |= ( BIT1 | BIT2 );
	/* Switch port 3 */
	if (ethComplexOptions & MV_ETHCOMP_SW_P3_2_GE_PHY_P3) {
		board->pSwitchInfo[0].switchPort[3] = 0;
		board->pSwitchInfo[0].connectedPortMask |= BIT3;
	}
	/* Set Switch port 4 */
	if (ethComplexOptions & MV_ETHCOMP_SW_P4_2_RGMII0) {
		board->pSwitchInfo[0].switchPort[4] = 4;
		board->pSwitchInfo[0].connectedPortMask |= BIT4;
	}
	/* Set Switch port 6 */
	if (ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		board->pSwitchInfo[0].connectedPortMask |= BIT6;

	return MV_OK;
}

/*******************************************************************************
* mvBoardBootDeviceGroupSet - test board Boot configuration and set MPP groups
*
* DESCRIPTION:
*   read board BOOT configuration and set MPP groups accordingly
*	-  Sets groups 0-1 for NAND or SPI0 Boot
*	   Or   groups 3-4 for SPI1 Boot
*	- return Selected boot device
*
* INPUT:
*
* OUTPUT:  None.
*
* RETURN:
*       the selected MV_BOARD_BOOT_SRC
*
*******************************************************************************/
MV_BOARD_BOOT_SRC mvBoardBootDeviceGroupSet()
{
	MV_U32 groupType;
	MV_BOARD_BOOT_SRC bootSrc = mvBoardBootDeviceGet();

	switch (bootSrc) {
	case MSAR_0_BOOT_NAND_NEW:
		mvBoardMppTypeSet(0, NAND_BOOT_V2);
		mvBoardMppTypeSet(1, NAND_BOOT_V2);
		break;
	case MSAR_0_BOOT_SPI_FLASH:
		groupType = ((mvCtrlSysConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x2) ? SPI0_BOOT_SPDIF_AUDIO : SPI0_BOOT);
		mvBoardMppTypeSet(0, groupType);
		mvBoardMppTypeSet(1, groupType);
	case MSAR_0_BOOT_SPI1_FLASH:    /* MSAR_0_SPI1 - update Groups 3-4 */
		mvBoardMppTypeSet(3, SDIO_SPI1_UNIT);
		if ( mvBoardSlicUnitTypeGet() == SLIC_LANTIQ_ID)
			mvBoardMppTypeSet(4, SPI1_CPU_SMI_CTRL_TDM_LQ_UNIT);
		else    /*REF_CLK_OUT*/
			mvBoardMppTypeSet(4, SPI1_CPU_SMI_CTRL_REF_CLK_OUT);
		mvBoardMppTypeSet(0, SPI0_BOOT_SPDIF_AUDIO);
		mvBoardMppTypeSet(1, SPI0_BOOT_SPDIF_AUDIO);
		break;
	default:
		return MV_ERROR;
	}
	return bootSrc;
}

/*******************************************************************************
* mvBoardBootDeviceGet -   Get the Selected S@R boot device
*
* DESCRIPTION:
*   read board BOOT configuration from S@R and return Boot device accordingly
*
* INPUT:
*
* OUTPUT:  None.
*
* RETURN:
*       the selected MV_BOARD_BOOT_SRC
*
*******************************************************************************/
MV_BOARD_BOOT_SRC mvBoardBootDeviceGet()
{
	MV_U32 satrBootDeviceValue = mvCtrlSatRRead(MV_SATR_BOOT_DEVICE);
	MV_SATR_BOOT_TABLE satrTable[] = MV_SATR_TABLE_VAL;
	MV_SATR_BOOT_TABLE satrBootEntry = satrTable[satrBootDeviceValue];

	if (satrBootEntry.bootSrc != MSAR_0_BOOT_SPI_FLASH)
		return satrBootEntry.bootSrc;

	/* if boot source is SPI ,verify which CS (0/1) */
	if (mvBoardBootAttrGet(satrBootDeviceValue, 1) == MSAR_0_SPI0)
		return MSAR_0_BOOT_SPI_FLASH;
	else
		return MSAR_0_BOOT_SPI1_FLASH;
}

/*******************************************************************************
* mvBoardBootAttrGet -  Get the selected S@R Boot device attributes[1/2/3]
*
* DESCRIPTION:
*   read board BOOT configuration and return attributes accordingly
*
* INPUT:  satrBootDevice - BOOT_DEVICE value from S@R.*
*         attrNum - attribute number [1/2/3]
* OUTPUT:  None.
*
* RETURN:
*       the selected attribute value
*
*******************************************************************************/
MV_U32 mvBoardBootAttrGet(MV_U32 satrBootDeviceValue, MV_U8 attrNum)
{
	MV_SATR_BOOT_TABLE satrTable[] = MV_SATR_TABLE_VAL;
	MV_SATR_BOOT_TABLE satrBootEntry = satrTable[satrBootDeviceValue];

	switch (attrNum) {
	case 1:
		return satrBootEntry.attr1;
		break;
	case 2:
		return satrBootEntry.attr2;
		break;
	case 3:
		return satrBootEntry.attr3;
		break;
	default:
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardMac0ConfigGet - test board configuration and return the correct MAC0 config
*
* DESCRIPTION:
*	test board configuration regarding MAC0
*	if configured to SGMII-0 , will check which lane is configured to SGMII,
*	and return its MV_ETH_COMPLEX_TOPOLOGY define
*	else return error
*
* INPUT:  None.
*
* OUTPUT:  None.
*
* RETURN:
*       if configured correct, the MV_ETH_COMPLEX_TOPOLOGY define, else MV_ERROR
*
*******************************************************************************/
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac0ConfigGet()
{
	switch (mvCtrlSysConfigGet(MV_CONFIG_MAC0)) {
	case 0x0:
		return MV_ETHCOMP_GE_MAC0_2_SW_P6;
		break;
	case 0x1:
		return MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0;
		break;
	case 0x2:
		return MV_ETHCOMP_GE_MAC0_2_RGMII0;
		break;
	case 0x3:
		return mvBoardLaneSGMIIGet();
		break;
	default:
		mvOsPrintf("%s: Error: unexpected value from mvCtrlConfigGet \n", __func__);
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardMac1ConfigGet - test board configuration and return the correct MAC1 config
*
* DESCRIPTION:
*	test board configuration regarding PON_SERDES
*	if MAC0 is configured to PON SerDes Connection return its MV_ETH_COMPLEX_TOPOLOGY define
*	else test MV_CONFIG_MAC1 configuration
*
* INPUT:  None.
*
* OUTPUT:  None.
*
* RETURN:
*       if configured correct, the MV_ETH_COMPLEX_TOPOLOGY define, else MV_ERROR
*
*******************************************************************************/
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac1ConfigGet()
{
	if (mvCtrlSysConfigGet(MV_CONFIG_PON_SERDES) == 0x1)
		return MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES;
	/* else Scan MAC1 config to decide its connection */
	switch (mvCtrlSysConfigGet(MV_CONFIG_MAC1)) {
	case 0x0:
		return MV_ETHCOMP_GE_MAC1_2_RGMII1;
		break;
	case 0x1:
		return MV_ETHCOMP_GE_MAC1_2_SW_P4;
		break;
	case 0x2:
		return MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3;
		break;
	default:
		mvOsPrintf("%s: Error: unexpected value from mvCtrlConfigGet \n", __func__);
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardIsLaneSGMII - check if a board lane is configured to SGMII-0
*
* DESCRIPTION:
*	test board configuration regarding lanes-1/2/3
*	if one of them is configured to SGMII-0 , will return its MV_ETH_COMPLEX_TOPOLOGY define
*	else return error
*
* INPUT:  None.
*
* OUTPUT:  None.
*
* RETURN:
*       value =MV_ETH_COMPLEX_GE_MAC0_COMPHY_1/2/3 if lanes 1/2/3 are SGMII-0 (adaptively)
*       or -1 if none of them is SGMII-0
*
*******************************************************************************/
MV_ETH_COMPLEX_TOPOLOGY mvBoardLaneSGMIIGet()
{
	if (mvCtrlSysConfigGet(MV_CONFIG_LANE1) == 0x1)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_1;
	else if (mvCtrlSysConfigGet(MV_CONFIG_LANE2) == 0x0)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_2;
	else if (mvCtrlSysConfigGet(MV_CONFIG_LANE3) == 0x1)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_3;

	mvOsPrintf("%s: Error: unexpected value from mvCtrlConfigGet \n", __func__);
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardIsInternalSwitchConnected
*
* DESCRIPTION:
*       This routine returns port's connection status
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 - if ethPortNum is connected to switch, 0 otherwise
*
*******************************************************************************/
MV_STATUS mvBoardIsInternalSwitchConnected(MV_U32 ethPortNum)
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();

	if (ethPortNum >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: Illegal port number(%u)\n", __func__, ethPortNum);
		return MV_FALSE;
	}

	/* Check if internal switch is connected */
	if ((ethPortNum == 0) && (ethComplex & MV_ETHCOMP_GE_MAC0_2_SW_P6))
		return MV_TRUE;
	else if ((ethPortNum == 1) && (ethComplex & MV_ETHCOMP_GE_MAC1_2_SW_P4))
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
* mvBoardSwitchConnectedPortGet -
*
* DESCRIPTION:
*       This routine returns the switch port connected to the ethPort
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_32 mvBoardSwitchConnectedPortGet(MV_U32 ethPort)
{

	if (board->switchInfoNum == 0)
		return -1;

	return board->pSwitchInfo[0].connectedPort[ethPort];
}


/*******************************************************************************
* mvBoardSwitchPortsMaskGet -
*
* DESCRIPTION:
*       This routine returns a mask describing all the connected switch ports
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_8 mvBoardSwitchPortsMaskGet(MV_U32 switchIdx)
{
	if ((board->switchInfoNum == 0) || (switchIdx >= board->switchInfoNum))
	return -1;

	return board->pSwitchInfo[switchIdx].connectedPortMask;
}

/*******************************************************************************
* mvBoardInternalQuadPhyAddrGet - Get QUAD phy SMI address.
*
* DESCRIPTION:
*       This routine returns the external QUAD phy address.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The QUAD phy start address or -1 if error.
*
*******************************************************************************/
MV_32 mvBoardInternalQuadPhyAddrGet(MV_U32 switchIdx)
{

	if ((board->switchInfoNum == 0) || (switchIdx >= board->switchInfoNum))
		return -1;

	return board->pSwitchInfo[switchIdx].internalQuadPhyAddr;

}

/*******************************************************************************
* mvBoardSwitchPortForceLinkGet
*
* DESCRIPTION:
*       Return the switch ports force link bitmask.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       the ports bitmask, -1 if the switch is not connected.
*
*******************************************************************************/
MV_U32 mvBoardSwitchPortForceLinkGet(MV_U32 switchIdx)
{

	if ( board->switchInfoNum == 0 || switchIdx >= board->switchInfoNum )
		return (MV_U32)MV_ERROR;

	return board->pSwitchInfo[switchIdx].forceLinkMask;
}

/*******************************************************************************
* mvBoardConfigWrite - write MPP's config and Board general environment configuration
*
* DESCRIPTION:
*       This function writes the environment information that was initialized
*       by mvBoardConfigInit, such as MPP settings, Mux configuration,
*       and Board specific initializations.
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
MV_VOID mvBoardConfigWrite(void)
{
	MV_U32 mppGroup, i, reg;
	MV_BOARD_SPEC_INIT *boardSpec;

	for (mppGroup = 0; mppGroup < MV_MPP_MAX_GROUP; mppGroup++) {
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mvBoardMppGet(mppGroup));
	}

	boardSpec = mvBoardSpecInitGet();
	if (boardSpec != NULL) {
		i = 0;
		while (boardSpec[i].reg != TBL_TERM) {
			reg = MV_REG_READ(boardSpec[i].reg);
			reg &= ~boardSpec[i].mask;
			reg |= (boardSpec[i].val & boardSpec[i].mask);
			MV_REG_WRITE(boardSpec[i].reg, reg);
			i++;
		}
	}
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
* mvBoardTdmSpiModeGet - return SLIC/DAA connection
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_32 mvBoardTdmSpiModeGet(MV_VOID)
{
	return 0;
}

/*******************************************************************************
* mvBoardTdmDevicesCountGet
*
* DESCRIPTION:
*	Return the number of TDM devices on board.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	Number of devices.
*
*******************************************************************************/
MV_U8 mvBoardTdmDevicesCountGet(void)
{
	MV_16 index = board->boardTdmInfoIndex;

	if (index == -1)
		return 0;

	return board->numBoardTdmInfo[index];
}

/*******************************************************************************
* mvBoardTdmSpiCsGet
*
* DESCRIPTION:
*	Return the SPI Chip-select number for a given device.
*
* INPUT:
*	devId	- The Slic device ID to get the SPI CS for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The SPI CS if found, -1 otherwise.
*
*******************************************************************************/
MV_U8 mvBoardTdmSpiCsGet(MV_U8 devId)
{
	MV_16 index;

	index = board->boardTdmInfoIndex;
	if (index == -1)
		return 0;

	if (devId >= board->numBoardTdmInfo[index])
		return -1;

	return board->pBoardTdmInt2CsInfo[index][devId].spiCs;
}

/*******************************************************************************
* mvBoardMppModuleTypePrint
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
	mvOsOutput("Modules Detected:\n");

	/* TDM */
	if (mvBoardTdmDevicesCountGet() > 0)
		mvOsOutput("       TDM module.\n");

	/* LCD DVI Module */
	if (mvBoardIsLcdDviModuleConnected())
		mvOsOutput("       LCD DVI module.\n");

	/* Switch Module */
	if (mvBoardIsSwitchModuleConnected())
		mvOsOutput("       Switch module.\n");

	/* GMII Module */
	if (mvBoardIsGMIIModuleConnected())
		mvOsOutput("       GMII module.\n");
}

MV_VOID mvBoardOtherModuleTypePrint(MV_VOID)
{
	/* Pex Module */
	if (mvBoardIsPexModuleConnected())
		mvOsOutput("       PEX module.\n");
	/* SETM Module */
	if (mvBoardIsSetmModuleConnected())
		mvOsOutput("       SETM module.\n");
	/* LVDS Module */
	if (mvBoardIsLvdsModuleConnected())
		mvOsOutput("       LVDS module.\n");
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
	return MV_FALSE;
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

	for (devNum = START_DEV_CS; devNum < board->numBoardDeviceIf; devNum++)
		if (board->pDevCsInfo[devNum].devClass == devClass)
			foundIndex++;

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
	MV_DEV_CS_INFO *devEntry = boardGetDevEntry(devNum, devClass);

	if (devEntry)
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
	MV_DEV_CS_INFO *devEntry = boardGetDevEntry(devNum, devClass);

	if (devEntry)
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
	MV_DEV_CS_INFO *devEntry = boardGetDevEntry(devNum, devClass);

	if (devEntry)
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
	MV_DEV_CS_INFO *devEntry = boardGetDevEntry(devNum, devClass);

	if (devEntry)
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
	MV_DEV_CS_INFO *devEntry = boardGetDevEntry(devNum, devClass);

	if (devEntry)
		return devEntry->deviceCS;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardIoExpValGet - read a specified value from IO Expanders
*
* DESCRIPTION:
*       This function returns specified value from IO Expanders
*
* INPUT:
*       ioClass - Field identifier
*       ioInfo  - relevant IO Expander information
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U8  :return requested value , if TWSI read was succesfull, else 0xFF.
*
*******************************************************************************/
MV_U8 mvBoardIoExpValGet(MV_BOARD_IO_EXPANDER_TYPE_INFO *ioInfo)
{
	MV_U8 val, mask;

	if (ioInfo==NULL)
		return (MV_U8)MV_ERROR;

	val = mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum, ioInfo->regNum);
	if ((MV_U8)MV_ERROR != val) {
		mask = (1 << ioInfo->offset);
		return ( (val & mask) >> ioInfo->offset);
	}
	DB(mvOsPrintf("%s: Error: Read from IO Expander failed\n", __func__));
	return (MV_U8)MV_ERROR;
}

/*******************************************************************************
* mvBoardTwsiAddrTypeGet
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
	DB(mvOsPrintf("%s: Error: read TWSI address type failed\n", __func__));
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardTwsiAddrGet
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

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if ((board->pBoardTwsiDev[i].devClass == twsiClass) \
				&& (board->pBoardTwsiDev[i].devClassId == index)){
			return board->pBoardTwsiDev[i].twsiDevAddr;
		}
	}

	return 0xFF;
}

/*******************************************************************************
* mvBoardEthComplexConfigGet - Return ethernet complex board configuration.
*
* DESCRIPTION:
*	Returns the ethernet / Sata complex configuration from the board spec
*	structure.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing the ethernet complex config.
*
*******************************************************************************/
MV_U32 mvBoardEthComplexConfigGet(MV_VOID)
{
	return board->pBoardModTypeValue->ethSataComplexOpt;
}

/*******************************************************************************
* mvBoardEthComplexConfigSet - Set ethernet complex board configuration.
*
* DESCRIPTION:
*	Sets the ethernet / Sata complex configuration in the board spec
*	structure.
*
* INPUT:
*       ethConfig - 32bit value describing the ethernet complex config.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardEthComplexConfigSet(MV_U32 ethConfig)
{
	/* Set ethernet complex configuration. */
	board->pBoardModTypeValue->ethSataComplexOpt = ethConfig;
	return;
}

/*******************************************************************************
* mvBoardSatrInfoGet
*
* DESCRIPTION:
*	Return the SAR fields information for a given SAR class.
*
* INPUT:
*	satrClass - The SATR field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_BOARD_SATR_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_STATUS mvBoardSatrInfoGet(MV_SATR_TYPE_ID satrClass, MV_BOARD_SATR_INFO *satrInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	/* verify existence of requested SATR type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_SATR_READ_MAX_OPTION ; i++)
		if (boardSatrInfo[i].satrId == satrClass) {
			satrInfo = &boardSatrInfo[i];
			if (boardSatrInfo[i].isActiveForBoard[boardId])
				return MV_OK;
			else
				return MV_ERROR;
		}
	DB(mvOsPrintf("%s: Error: requested MV_SATR_TYPE_ID was not found (%d)\n", __func__,satrClass));
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardConfigTypeGet
*
* DESCRIPTION:
*	Return the Config type fields information for a given Config type class.
*
* INPUT:
*	configClass - The Config type field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_BOARD_CONFIG_TYPE_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_STATUS mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	/* verify existence of requested config type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION ; i++)
		if (boardConfigTypesInfo[i].configId == configClass) {
			configInfo = &boardConfigTypesInfo[i];
			if (boardConfigTypesInfo[i].isActiveForBoard[boardId])
				return MV_OK;
			else
				return MV_ERROR;
		}
	DB(mvOsPrintf("%s: Error: requested MV_CONFIG_TYPE_ID was not found (%d) \n", __func__, configClass));
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardIoExpanderTypeGet
*
* DESCRIPTION:
*	Return the Config type fields information for a given Config type class.
*
* INPUT:
*	configClass - The Config type field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_BOARD_CONFIG_TYPE_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_STATUS mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_TYPE_ID ioClass, MV_BOARD_IO_EXPANDER_TYPE_INFO *ioInfo)
{
	int i;

	/* verify existance of requested config type, pull its data */
	for (i = 0; i < board->numBoardIoExpanderInfo ; i++)
		if (board->pBoardIoExpanderInfo[i].ioFieldid == ioClass) {
			ioInfo = &board->pBoardIoExpanderInfo[i];
			return MV_OK;
		}
	DB(mvOsPrintf("%s: Error: requested MV_IO_EXPANDER_TYPE_ID was not found\n", __func__));
	return MV_ERROR;
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
			return devWidth / 8;
	}

	DB(mvOsPrintf("%s: Error: NAND device was not found\n", __func__));
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardIdSet - Set Board model
*
* DESCRIPTION:
*       This function sets the board ID.
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
*       void
*
*******************************************************************************/
MV_VOID mvBoardIdSet(MV_U32 boardId)
{
	if (boardId >= MV_MAX_BOARD_ID)
		mvOsPrintf("%s: Error: wrong boardId (%d)\n", __func__, boardId);

	board = boardInfoTbl[gBoardId];
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
	MV_U32 boardId, value;

#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	boardId = MV_BOARD_ID_AVANTA_LP_FPGA;
#else
	value = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	boardId = ((value & (0xF0)) >> 4);
#endif
	if (boardId >= MV_MAX_BOARD_ID) {
		mvOsPrintf("%s: Error: read wrong board (%d)\n", __func__, boardId);
		return MV_INVALID_BOARD_ID;
	}
	return boardId;
}

/*******************************************************************************
* mvBoardTwsiGet -
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
MV_U8 mvBoardTwsiGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
	DB(mvOsPrintf("Board: Read S@R device read\n"));
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);

	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;

	if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
		DB(mvOsPrintf("Board: Read S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded\n"));

	return data;
}

/*******************************************************************************
* mvBoardTwsiSatRSet
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
*	None.
*
* RETURN:
*	reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiSet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 regVal)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);
	twsiSlave.validOffset = MV_TRUE;
	DB(mvOsPrintf("Board: Write S@R device addr %x, type %x, data %x\n",
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("%s: Write S@R fail\n" __func__));
		return MV_ERROR;
	}
	DB(mvOsPrintf("%s: Write S@R succeded\n" __func__));

	return MV_OK;
}

/*******************************************************************************
 * SatR Configuration functions
 */

MV_U8 mvBoardCpuCoresNumGet(MV_VOID)
{
	return 1;
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
* mvBoardIsPexModuleConnected
*
* DESCRIPTION:
*	Check if PEX module is connected to the board.
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
MV_BOOL mvBoardIsPexModuleConnected(void)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsSetmModuleConnected
*
* DESCRIPTION:
*	Check if SETM module is connected to the board.
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
MV_BOOL mvBoardIsSetmModuleConnected(void)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsSwitchModuleConnected
*
* DESCRIPTION:
*	Check if PEX module is connected to the board.
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
* mvBoardIsLvdsModuleConnected
*
* DESCRIPTION:
*	Check if LVDS module is connected to the board.
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
MV_BOOL mvBoardIsLvdsModuleConnected(void)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsLcdDviModuleConnected
*
* DESCRIPTION:
*	Check if LVDS module is connected to the board.
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
MV_BOOL mvBoardIsLcdDviModuleConnected(void)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsGMIIModuleConnected
*
* DESCRIPTION:
*	Check if GMII module is connected to the board.
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
MV_BOOL mvBoardIsGMIIModuleConnected(void)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardTwsiMuxChannelSet
*
* DESCRIPTION:
*	Set the channel number of the on-board TWSI mux.
*
* INPUT:
*	chNum	- The channel number to set.
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardTwsiMuxChannelSet(MV_U8 muxChNum)
{
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardTwsiReadByteThruMux
*
* DESCRIPTION:
*	Read a single byte from a TWSI device through the TWSI Mux.
*
* INPUT:
*	muxChNum	- The Twsi Mux channel number to read through.
*	chNum		- The TWSI channel number.
*	pTwsiSlave	- The TWSI slave address.
*	data		- Buffer to read into (1 byte).
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardTwsiReadByteThruMux(MV_U8 muxChNum, MV_U8 chNum,
				     MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *data)
{
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardSmiScanModeGet - Get Switch SMI scan mode
*
* DESCRIPTION:
*       This routine returns Switch SMI scan mode.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 for SMI_MANUAL_MODE, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx)
{
	if ((board->switchInfoNum == 0) || (switchIdx >= board->switchInfoNum))
		return -1;

	return BOARD_ETH_SWITCH_SMI_SCAN_MODE;
}

/*******************************************************************************
* mvBoardSwitchCpuPortGet - Get the the Ethernet Switch CPU port
*
* DESCRIPTION:
*       This routine returns the Switch CPU port if connected , -1 else.
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
* mvBoardSwitchIrqGet - Get the IRQ number for the link status indication
*
* DESCRIPTION:
*       This routine returns the IRQ number for the link status indication.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*	the number of the IRQ for the link status indication, -1 if the port
*	number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSwitchIrqGet(MV_VOID)
{

	if (board->switchInfoNum == 0)
		return -1;

	return board->pSwitchInfo[0].switchIrq;
}

/*******************************************************************************
* mvBoardIsQsgmiiModuleConnected
*
* DESCRIPTION:
*       This routine returns whether the QSGMII module is connected or not.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if QSGMII module is connected, MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardIsQsgmiiModuleConnected(MV_VOID)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardGePhySwitchPortGet
*
* DESCRIPTION:
*       This routine returns whether the internal GE PHY is connected to
*	Switch Port 0, Switch port 5 or not connected to any Switch port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       0 if the internal GE PHY is connected to Switch Port 0,
*	5 if the internal GE PHY is connected to Switch Port 5,
*	-1 otherwise.
*
*******************************************************************************/
MV_32 mvBoardGePhySwitchPortGet(MV_VOID)
{
	return -1;
}

/*******************************************************************************
* mvBoardRgmiiASwitchPortGet
*
* DESCRIPTION:
*       This routine returns whether RGMII-A is connected to
*	Switch Port 5, Switch port 6 or not connected to any Switch port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       5 if the internal GE PHY is connected to Switch Port 5,
*	6 if the internal GE PHY is connected to Switch Port 6,
*	-1 otherwise.
*
*******************************************************************************/
MV_32 mvBoardRgmiiASwitchPortGet(MV_VOID)
{
	return -1;
}

/*******************************************************************************
* mvBoardSwitchPortMap
*
* DESCRIPTION:
*	Map front panel connector number to switch port number.
*
* INPUT:
*	switchIdx - The switch index.
*	switchPortNum - The switch port number to get the mapping for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The switch port mapping.
*	OR -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSwitchPortMap(MV_U32 switchIdx, MV_U32 switchPortNum)
{
	int i;
	if (board->switchInfoNum == 0 || switchIdx >= board->switchInfoNum) {
		mvOsPrintf("%s: Error: wrong switch index (%d)\n", __func__, switchIdx);
		return -1;
	}

	for (i = 0; i < BOARD_ETH_SWITCH_PORT_NUM; i++) {
		if (board->pSwitchInfo[switchIdx].switchPort[i] == switchPortNum)
			return i;
	}

	mvOsPrintf("%s: Error: switch port map not found\n", __func__);
	return -1;
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
	return NULL;
}

/*******************************************************************************
* mvBoardConfigAutoDetectEnabled
*
* DESCRIPTION:
*	Indicate if the board supports auto configuration and detection of
*	modules. This is usually enabled for DB boards only.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_TRUE if auto-config/detection is enabled.
*	MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardConfigAutoDetectEnabled()
{
	return board->configAutoDetect;
}
