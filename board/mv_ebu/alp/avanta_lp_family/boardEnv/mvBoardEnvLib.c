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
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_BOARD_CONFIG_INFO;

/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
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
	MV_U32 boardId = mvBoardIdGet();

	mvBoardIdSet(boardId);

	/*  FPGA board doesn't use MPP neither GPIO */
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
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
	MV_U32 ethComplex;
#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return ethPortNum == 2;
#endif
	ethComplex = mvBoardEthComplexConfigGet();
	if ((ethPortNum == 0 && (ethComplex & MV_ETHCOMP_GE_MAC0_2_COMPHY_1 ||
		ethComplex & MV_ETHCOMP_GE_MAC0_2_COMPHY_2 || ethComplex & MV_ETHCOMP_GE_MAC0_2_COMPHY_3)) ||
		(ethPortNum == 1 && (ethComplex & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)))
		return MV_TRUE;
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
* mvBoardPhyAddrSet - Set the phy address
*
* DESCRIPTION:
*       This routine sets the Phy address of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*       smiAddr    - requested phy address
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardPhyAddrSet(MV_U32 ethPortNum, MV_U32 smiAddr)
{
	if (ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return;
	}

	board->pBoardMacInfo[ethPortNum].boardEthSmiAddr = smiAddr;
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
* mvBoardMacSpeedSet - Set the Mac speed
*
* DESCRIPTION:
*       This routine Sets the Mac speed of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*       macSpeed   - Requested MAC speed
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*
*******************************************************************************/
MV_VOID mvBoardMacSpeedSet(MV_U32 ethPortNum, MV_BOARD_MAC_SPEED macSpeed)
{
	if (ethPortNum >= board->numBoardMacInfo)
		mvOsPrintf("%s: Error: wrong eth port (%d)\n", __func__, ethPortNum);

	board->pBoardMacInfo[ethPortNum].boardMacSpeed = macSpeed;
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
	return (ethPortNum == 2);
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
	MV_U32 tclk;
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	/* FPGA is limited to 25Mhz */
	return MV_FPGA_CORE_CLK;
#else
	tclk = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)));
	tclk = ((tclk & 0x400000) >> 22);
	switch (tclk) {
	case 0:
		return MV_BOARD_TCLK_166MHZ;
	case 1:
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_200MHZ;
	}
#endif
}

/*******************************************************************************
* mvBoardL2ClkGet
*
* DESCRIPTION:
*	This routine extract the L2 Cache frequency/clock.
*	Note: this function is called at the very early stage
*	in Linux Kernel, hence, it has to read from SoC register, not
*	from pre-built database.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardL2ClkGet(MV_VOID)
{
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	return MV_FPGA_L2_CLK;
#else
	MV_U32 clkSelect;
	MV_FREQ_MODE freqMode;

	clkSelect = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	clkSelect = clkSelect & (0x1f << 17);
	clkSelect >>= 17;

	mvCtrlFreqModeGet(clkSelect, &freqMode);

	return 1000000 * freqMode.l2Freq;
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
		return (MV_U32)(1000000 * freqMode.ddrFreq);
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
	if (slicType >= MV_BOARD_SLIC_MAX_OPTION) {
		mvOsPrintf("%s: Error: Unsupported SLIC/TDM configuration selected (%x)\n",
				__func__, slicType);
		slicType = MV_BOARD_SLIC_DISABLED;
	}

	board->boardTdmInfoIndex = slicType;
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
	MV_U32 smiAddress, slicDev, ethComplex;
	MV_BOARD_MAC_SPEED macSpeed = BOARD_MAC_SPEED_AUTO; /*if Mac is not connected to switch, auto-negotiate speed*/

	/* Update Ethernet complex according to board configuration */
	mvBoardEthComplexInfoUpdate();

	/* Update SMI phy address for MAC0/1 */
	ethComplex = mvBoardEthComplexConfigGet();
	if (ethComplex & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)
		smiAddress = 0x0;
	else if (ethComplex & MV_ETHCOMP_GE_MAC0_2_RGMII0) {
		/* External PHY SMI address (extenal module) for Zx revision is
		** 0x8, for A0 the module is on board with address 0x5 */
		if (mvCtrlRevGet() <= MV_88F66X0_Z3_ID)
			smiAddress = 0x8;
		else
			smiAddress = 0x5;
	}
	else {				/* else MAC0 is connected to SW port 6 */
		smiAddress = -1;	/* no SMI address if connected to switch */
		if (ethComplex & MV_ETHCOMP_P2P_MAC0_2_SW_SPEED_2G)
			macSpeed = BOARD_MAC_SPEED_2000M;
		else
			macSpeed = BOARD_MAC_SPEED_1000M;
	}
	mvBoardPhyAddrSet(0, smiAddress);
	mvBoardMacSpeedSet(0, macSpeed);

	macSpeed = BOARD_MAC_SPEED_AUTO; /*if Mac is not connected to switch, auto-negotiate speed*/
	if (ethComplex & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)
		smiAddress = 0x3;
	/* MAC1 to RGMII, or MAC1 to SGMII: both configs use the same SMI address (0x1) */
	else if (ethComplex & (MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES))
		smiAddress = 0x1;
	else if (ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII0)
		smiAddress = 0x5;
	else {
		smiAddress = -1; /* no SMI address if connected to switch */
		macSpeed = BOARD_MAC_SPEED_1000M;
	}
	mvBoardPhyAddrSet(1, smiAddress);
	mvBoardMacSpeedSet(1, macSpeed);

	/* Update SLIC device configuration */
	slicDev = mvCtrlSysConfigGet(MV_CONFIG_SLIC_TDM_DEVICE);
	if (slicDev == SLIC_EXTERNAL_ID && (ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1))
		mvOsPrintf("%s: Error: board configuration conflict between MAC1 to RGMII-1, " \
				"and External TDM - using RGMII-1 (disabled External TDM)\n\n", __func__);

	mvBoardSlicUnitTypeSet(slicDev);

	/* Update MPP group types and values according to board configuration */
	mvBoardMppIdUpdate();

	/* specific initializations required only for DB-6660 */
	if (mvBoardIdGet() == DB_6660_ID) {
		/* Verify board config vs. SerDes actual setup (Common phy Selector) */
		mvBoardVerifySerdesCofig();

		/* Enable Super-Speed mode on Avanta DB boards (900Mah output) */
		mvBoardUsbSsEnSet(MV_TRUE);

		/* If needed, enable SFP0 TX for SGMII, for DB-6660 */
		if (ethComplex & MV_ETHCOMP_GE_MAC0_2_COMPHY_2)
			mvBoardSgmiiSfp0TxSet(MV_TRUE);

		/* Check conflicts between device bus module and NAND */
		mvBoardAudioModuleConfigCheck();

		/* Check DDR buswidth configuration */
		mvBoardDDRBusWidthCheck();
	}
}

/*******************************************************************************
* mvBoardVerifySerdesCofig - Verify board config vs. SerDes actual setup (Common phy Selector)
*
* DESCRIPTION:
	read board configuration for SerDes lanes 1-3 (SerDes 1 is pre-fixed to PCI-e)
	and compare it to SerDes common phy Selector (Actual SerDes config in use)
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
MV_VOID mvBoardVerifySerdesCofig(MV_VOID)
{
	MV_U32 i, laneConfig, laneSelector;
	MV_CONFIG_TYPE_ID configID = MV_CONFIG_LANE1;

	/* Lane 1 & Lane 3 use the same values for SerDes config and selector,
	 * Lane2 values  are reversed:
	 * | Value  | board Config | Selector |
	 * | 0x0    | SATA0        | SGMII-0  |
	 * | 0x1    | SGMII-0      | SATA0    | */
	for (i = 0; i < 4; i++) {
		if (i == 0) /* lane0 is hard-coded to PCIe0 - not selected by board configuration */
			laneConfig = 1;
		else /* lane 1-3 are selected at board configuration */
			laneConfig = mvCtrlSysConfigGet(configID++);
		/* using different Mask/Offset, since SATA1 option at lane1 was
		** not supported in Z1, Z2, Z3 */
		laneSelector = mvBoardLaneSelectorGet(i);
		if ((i != 2 && laneSelector != laneConfig) || /* lanes 1,3 use the same value */
			(i == 2 && laneSelector == laneConfig)) { /* lane 2 use opposite values */
			mvOsPrintf("Error: board configuration conflicts with SerDes configuration\n");
			mvOsPrintf("SerDes#%d: Board configuration= %x  SerDes Selector = %x\n" \
			, i, laneConfig, laneSelector);
		}
	}
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
	MV_U32 ethComplexOptions = mvBoardEthComplexConfigGet();
	MV_BOOL singleCpu, tdmLqUnit;

	/* MPP Groups initialization : */
	/* Set Group 0-1 - Boot device (else if booting from SPI1: Set Groups 3-4) */
	bootDev = mvBoardBootDeviceGroupSet();

	/* Group 2 - SLIC Tdm unit */
	slicDev = mvBoardSlicUnitTypeGet();
	mvBoardMppTypeSet(2, slicDev);

	/* Groups 3-4  - (only if not Booting from SPI1)*/
	if (bootDev != MSAR_0_BOOT_SPI1_FLASH) {
		tdmLqUnit = (slicDev == SLIC_LANTIQ_ID);
		if (ethComplexOptions & (MV_ETHCOMP_GE_MAC1_2_RGMII1 | MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)) {
			mvBoardMppTypeSet(3, GE1_RGMII1_UNIT);
			mvBoardMppTypeSet(4, (tdmLqUnit ? GE1_RGMII1_CPU_SMI_CTRL_TDM_LQ_UNIT : \
						GE1_RGMII1_CPU_SMI_CTRL_REF_CLK_OUT));
		} else { /* if RGMII-1 isn't used, set SPI1 MPP's */
			mvBoardMppTypeSet(3, SDIO_SPI1_UNIT);
			mvBoardMppTypeSet(4, (tdmLqUnit ? SPI1_CPU_SMI_CTRL_TDM_LQ_UNIT : \
						SPI1_CPU_SMI_CTRL_REF_CLK_OUT));
		}
	}

	/* Groups 5-6-7 Set GE0, GE1_RGMII0, or Switch port 4 */
	singleCpu = (mvCtrlGetCpuNum() == 0); /* if using Dual CPU ,set UART1 */
	if (ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_RGMII0) {
		mvBoardMppTypeSet(5, GE0_UNIT_PON_TX_FAULT);
		mvBoardMppTypeSet(6, GE0_UNIT);
		mvBoardMppTypeSet(7, (singleCpu ? GE0_UNIT_LED_MATRIX : GE0_UNIT_UA1_PTP));
	} else if (ethComplexOptions & MV_ETHCOMP_SW_P4_2_RGMII0) {
		mvBoardMppTypeSet(5, SWITCH_P4_PON_TX_FAULT);
		mvBoardMppTypeSet(6, SWITCH_P4);
		mvBoardMppTypeSet(7, (singleCpu ? SWITCH_P4_LED_MATRIX : SWITCH_P4_UA1_PTP));
	} else if (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_RGMII0) {
		mvBoardMppTypeSet(5, GE1_RGMII0_UNIT_PON_TX_FAULT);
		mvBoardMppTypeSet(6, GE1_RGMII0_UNIT);
		mvBoardMppTypeSet(7, (singleCpu ? GE1_RGMII0_UNIT_LED_MATRIX : GE1_RGMII0_UNIT_UA1_PTP));
	}
}

/*******************************************************************************
 * mvBoardEthComplexInfoUpdate
 *
 * DESCRIPTION:
 *	Verify and Update etherntComplex configuration,
 *	according to modules detection (S@R & board configuration)
 *
 * INPUT:
 *	updateSetup:
 *	MV_TRUE  : update configuration in board structure (occurs once on startup)
 *	MV_FALSE : only verify if configuration is valid, when altered config at runtime
 *
 * OUTPUT:
 *	None.
 *
 * RETURN:
 *	MV_OK - on success,
 *	MV_ERROR - On failure.
 *
*******************************************************************************/
MV_STATUS mvBoardEthComplexInfoUpdate()
{
	MV_U32 ethComplexOptions = 0x0;

	ethComplexOptions = mvBoardEthComplexMacConfigCheck();
	if (ethComplexOptions == MV_ERROR)
		return MV_ERROR;

	/* read if using 2G speed for MAC0 to Switch*/
	if (mvCtrlSysConfigGet(MV_CONFIG_MAC0_SW_SPEED) == 0x0)
		ethComplexOptions |= MV_ETHCOMP_P2P_MAC0_2_SW_SPEED_2G;

	if (mvCtrlSysConfigGet(MV_CONFIG_SGMII0_CAPACITY) == 0x1)
		ethComplexOptions |= MV_ETHCOMP_GE_MAC0_2_COMPHY_SPEED_2G;

	/* if MAC1 is NOT connected to PON SerDes --> connect PON MAC to to PON SerDes */
	if ((ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES) == MV_FALSE)
		ethComplexOptions |= MV_ETHCOMP_P2P_MAC_2_PON_ETH_SERDES;

	/* Switch Ports*/
	if ((ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_SW_P6) ||
	    (ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_SW_P4)) {
		/* if MAC0 is NOT connected to GE_PHY_P0 --> connect Switch port 0 to QUAD_PHY_P0 */
		if ((ethComplexOptions & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) == MV_FALSE)
			ethComplexOptions |= MV_ETHCOMP_SW_P0_2_GE_PHY_P0;

		/* if MAC1 is BOT connected to GE_PHY_P3 --> connect Switch port 3 to QUAD_PHY_P3 */
		if ((ethComplexOptions & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) == MV_FALSE)
			ethComplexOptions |= MV_ETHCOMP_SW_P3_2_GE_PHY_P3;

		/* connect Switch ports 2/3 to QUAD_PHY_P2/3 */
		ethComplexOptions |= (MV_ETHCOMP_SW_P1_2_GE_PHY_P1 | MV_ETHCOMP_SW_P2_2_GE_PHY_P2);
	}

	mvBoardEthComplexConfigSet(ethComplexOptions);
	return MV_OK;
}

/*******************************************************************************
 * mvBoardEthComplexMacConfigCheck
 *
 * DESCRIPTION:
 *	Verify and return etherntComplex MAC0 configuration,
 *	according to modules detection (S@R & board configuration)
 *
 * INPUT:
 *	None.
 * OUTPUT:
 *	None.
 *
 * RETURN:
 *	MV_ETH_COMPLEX_TOPOLOGY which defines MAC0 and MAC1 board configuration
 *
*******************************************************************************/
MV_ETH_COMPLEX_TOPOLOGY mvBoardEthComplexMacConfigCheck()
{
	MV_ETH_COMPLEX_TOPOLOGY mac0Config, mac1Config;

	/* Ethernet Complex initialization : */
	/* MAC0/1 */
	mac0Config = mvBoardMac0ConfigGet();
	mac1Config = mvBoardMac1ConfigGet();

	if (mac0Config == MV_ERROR || mac1Config == MV_ERROR) {
		mvOsPrintf("Warning: Will use default configuration.\n\n");
		return MV_ERROR;
	}

	return mac0Config | mac1Config;
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
		/* read SPDIF_AUDIO/I2S_AUDIO board configuration for DB-6660 board */
		if ((mvBoardIdGet() == DB_6660_ID) &&
			((mvCtrlSysConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x2) ||	/* 0x2=I2S_AUDIO   */
			 (mvCtrlSysConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x3)))	/* 0x3=SPDIF_AUDIO */
			groupType = SPI0_BOOT_SPDIF_I2S_AUDIO;
		else
			groupType = SPI0_BOOT;

		mvBoardMppTypeSet(0, groupType);
		mvBoardMppTypeSet(1, groupType);
		break;
	case MSAR_0_BOOT_SPI1_FLASH:    /* MSAR_0_SPI1 - update Groups 3-4 */
		mvBoardMppTypeSet(3, SDIO_SPI1_UNIT);
		if ( mvBoardSlicUnitTypeGet() == SLIC_LANTIQ_ID)
			mvBoardMppTypeSet(4, SPI1_CPU_SMI_CTRL_TDM_LQ_UNIT);
		else    /*REF_CLK_OUT*/
			mvBoardMppTypeSet(4, SPI1_CPU_SMI_CTRL_REF_CLK_OUT);
		mvBoardMppTypeSet(0, SPI0_BOOT_SPDIF_I2S_AUDIO);
		mvBoardMppTypeSet(1, SPI0_BOOT_SPDIF_I2S_AUDIO);
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
	MV_U32 satrBootDeviceValue;
	MV_SATR_BOOT_TABLE satrTable[] = MV_SATR_TABLE_VAL;
	MV_SATR_BOOT_TABLE satrBootEntry;
	MV_BOARD_BOOT_SRC defaultBootSrc;

	/* prepare default boot source, in case of:
	 * 1. S@R read ERROR
	 * 2. Boot from UART is selected as boot source at S@R
	 *    Pre-compiled image type (SPI/NAND) is selected as boot source
	 */
#if defined(MV_SPI_BOOT)
	defaultBootSrc = MSAR_0_BOOT_SPI_FLASH;
	DB(mvOsPrintf("default boot source is SPI-0\n"));
#elif defined(MV_NAND_BOOT)
	defaultBootSrc = MSAR_0_BOOT_NAND_NEW;
	DB(mvOsPrintf("default boot source is NAND\n"));
#endif

	if (mvCtrlSatRRead(MV_SATR_BOOT_DEVICE, &satrBootDeviceValue) != MV_OK) {
		mvOsPrintf("%s: Error: failed to read boot source\n", __func__);
		mvOsPrintf("Using pre-compiled image type as boot source\n");
		return defaultBootSrc;
	}

	/* Get boot source entry from Satr boot table */
	satrBootEntry = satrTable[satrBootDeviceValue];

	/* If booting from UART, return pre-compiled boot source*/
	if (satrBootEntry.bootSrc == MSAR_0_BOOT_UART) {
		mvOsPrintf("\t** Booting from UART (restore DIP-switch to");
		mvOsPrintf(" requested boot source before reset!) **\n");
		return defaultBootSrc;
	}

	/* If not booting from SPI, return boot source*/
	if (satrBootEntry.bootSrc != MSAR_0_BOOT_SPI_FLASH)
		return satrBootEntry.bootSrc;

	/* if booting from SPI ,verify which CS (0/1) */
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
	MV_ETH_COMPLEX_TOPOLOGY sgmiiLane;
	MV_STATUS isSgmiiLaneEnabled = MV_FALSE;
	MV_U32 mac0Config = mvCtrlSysConfigGet(MV_CONFIG_MAC0);

	if (mac0Config == MV_ERROR) {
		mvOsPrintf("%s: Error: failed reading MAC0 board configuration\n", __func__);
		return MV_ERROR;
	}

	/* Serdes lane board configuration is only for DB-6660  */
	if (mvBoardIdGet() == DB_6660_ID)
		isSgmiiLaneEnabled = mvBoardLaneSGMIIGet(&sgmiiLane);
	else if (mac0Config == MAC0_2_SGMII) {
		mvOsPrintf("%s: Warning: only DB-6660 board supports MAC0 to SGMII", __func__);
		return MV_ERROR;
	}

	/* if a Serdes lane is configured to SGMII, and conflicts MAC0 settings,
	 * then the selected Serdes lane will be used (overriding MAC0 settings) */

	if (isSgmiiLaneEnabled == MV_TRUE && mac0Config != MAC0_2_SGMII) {
		mvOsPrintf("Warning: a Serdes lane is set to SGMII, but MAC0 is not set to");
		mvOsPrintf(" SGMII - Overriding MAC0 setting to be SGMII.\n\n");

		return sgmiiLane;
	}

	switch (mac0Config) {
	case MAC0_2_SW_P6:
		return MV_ETHCOMP_GE_MAC0_2_SW_P6;
	case MAC0_2_GE_PHY_P0:
		return MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0;
	case MAC0_2_RGMII:
		return MV_ETHCOMP_GE_MAC0_2_RGMII0;
	case MAC0_2_SGMII:
		if (isSgmiiLaneEnabled == MV_TRUE)
			return sgmiiLane;
	}

	/* if MAC0 set to SGMII, but no Serdes lane selected, --> ERROR (use defaults) */
	mvOsPrintf("Warning: MAC0 is set to SGMII but no Serdes lane is set to SGMII\n\n");
	return MV_ERROR;

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
	case 0x3:
		return MV_ETHCOMP_GE_MAC1_2_RGMII0;
		break;
	default:
		mvOsPrintf("%s: Error: Configuration conflict for MAC1 connection.\n", __func__);
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
*      MV_TRUE if SGMII settings are valid and Enabled only for a single lane
*
*******************************************************************************/
MV_BOOL mvBoardLaneSGMIIGet(MV_ETH_COMPLEX_TOPOLOGY *sgmiiConfig)
{
	MV_BOOL isSgmiiLaneEnabled = MV_FALSE;
	MV_U8 i;
	MV_ETH_COMPLEX_TOPOLOGY laneConfig = 0;
	MV_ETH_COMPLEX_TOPOLOGY laneOptions[] = {MV_ETHCOMP_GE_MAC0_2_COMPHY_1, \
						MV_ETHCOMP_GE_MAC0_2_COMPHY_2, \
						MV_ETHCOMP_GE_MAC0_2_COMPHY_3 };
	MV_CONFIG_TYPE_ID configID[] = {MV_CONFIG_LANE1, MV_CONFIG_LANE2, MV_CONFIG_LANE3};


	if (sgmiiConfig == NULL) {
		mvOsPrintf("%s: Error: NULL pointer parameter\n", __func__);
		return MV_FALSE;
	}

	for (i = 0; i < 3; i++) {
		/* if not set to SGMII, check next lane*/
		if (mvCtrlSysConfigGet(configID[i]) != 0x1)
			continue;

		/* if no Lane already set to SGMII */
		if (isSgmiiLaneEnabled == MV_FALSE) {
			laneConfig = laneOptions[i];
			isSgmiiLaneEnabled = MV_TRUE;
		} else { /* SGMII was already set in the previous lanes */
			mvOsPrintf("%s: Error: Only one Serdes lanes can be configured to SGMII\n\n", __func__);
			return MV_FALSE;
		}
	}

	if (isSgmiiLaneEnabled != MV_TRUE)
		return MV_FALSE;

	*sgmiiConfig = laneConfig;

	return MV_TRUE;
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
MV_STATUS mvBoardIsInternalSwitchConnected(void)
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();

	if ((ethComplex & MV_ETHCOMP_GE_MAC0_2_SW_P6) ||
	    (ethComplex & MV_ETHCOMP_GE_MAC1_2_SW_P4))
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
*	switch port connected to the ethPort
*
*******************************************************************************/
MV_32 mvBoardSwitchConnectedPortGet(MV_U32 ethPort)
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();

	if (ethPort >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: Illegal port number(%u)\n", __func__, ethPort);
		return MV_FALSE;
	}

	if ((ethPort == 0) && (ethComplex & MV_ETHCOMP_GE_MAC0_2_SW_P6))
		return 6;
	else if ((ethPort == 1) && (ethComplex & MV_ETHCOMP_GE_MAC1_2_SW_P4))
		return 4;
	else
		return -1;

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
MV_U32 mvBoardSwitchPortsMaskGet(MV_U32 switchIdx)
{
	MV_U32 mask = 0, c = mvBoardEthComplexConfigGet();

	if (c & MV_ETHCOMP_SW_P0_2_GE_PHY_P0)
		mask |= BIT0;
	if (c & MV_ETHCOMP_SW_P1_2_GE_PHY_P1)
		mask |= BIT1;
	if (c & MV_ETHCOMP_SW_P2_2_GE_PHY_P2)
		mask |= BIT2;
	if (c & MV_ETHCOMP_SW_P3_2_GE_PHY_P3)
		mask |= BIT3;
	if ((c & MV_ETHCOMP_SW_P4_2_RGMII0) || (c & MV_ETHCOMP_GE_MAC1_2_SW_P4))
		mask |= BIT4;
	if (c & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		mask |= BIT6;

	return mask;
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
	MV_U32 mask = 0, c = mvBoardEthComplexConfigGet();

	if (c & MV_ETHCOMP_GE_MAC1_2_SW_P4)
		mask |= BIT4;
	if (c & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		mask |= BIT6;

	/* If switch port 4 is connected to RGMII-0, the PHY SMI is controlled by CPU MAC,
	 * To avoid address conflict between internal PHY (0x1), SMI is not handled by switch.
	 * Auto-negotiation can not be applied with this configuration - so we use force link */
	if (c & MV_ETHCOMP_SW_P4_2_RGMII0)
		mask |= BIT4;
	return mask;

}

/*******************************************************************************
* mvBoardFreqModesNumGet
*
* DESCRIPTION: Return the number of supported frequency modes for this SoC
*
*
* INPUT:
*      None.
*
* OUTPUT:
*      None.
*
* RETURN:
*      Number of supported frequency modes
*
*******************************************************************************/
MV_U32 mvBoardFreqModesNumGet()
{
	MV_U32 freqNum;

	switch (mvCtrlModelGet()) {
	case MV_6610_DEV_ID:
	case MV_6610F_DEV_ID:
		freqNum = FREQ_MODES_NUM_6610;
		break;
	case MV_6650_DEV_ID:
	case MV_6650F_DEV_ID:
	case MV_6658_DEV_ID:
		freqNum = FREQ_MODES_NUM_6650;
		break;
	case MV_6660_DEV_ID:
	case MV_6665_DEV_ID:
		if (mvCtrlRevGet() <= MV_88F66X0_Z3_ID)
			freqNum = FREQ_MODES_NUM_6660_Z_REV;
		else
			freqNum = FREQ_MODES_NUM_6660;
		break;
	default:
		mvOsPrintf("%s: Error: failed to read ctrlModel (SoC ID)\n", __func__);
		return MV_ERROR;
	}

	return freqNum;
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
* mvBoardTdmSpiIdGet
*
* DESCRIPTION:
*	Return SPI port ID per board.
*
* INPUT:
*	None
*
* OUTPUT:
*       None.
*
* RETURN:
*	SPI port ID.
*
*******************************************************************************/
MV_U8 mvBoardTdmSpiIdGet(MV_VOID)
{
	MV_16 index;

	index = board->boardTdmInfoIndex;
	if (index == -1)
		return 0;

	return board->pBoardTdmSpiInfo[index].spiId;
}

/*******************************************************************************
* mvBoardAudioModuleConfigCheck
*
* DESCRIPTION:
*	Check if used audio modules when booting from NAND
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
MV_VOID mvBoardAudioModuleConfigCheck(MV_VOID)
{
	if ((mvBoardBootDeviceGet() == MSAR_0_BOOT_NAND_NEW) &&
		((mvCtrlSysConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x2) ||		/* 0x2=I2S_AUDIO   */
		(mvCtrlSysConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x3)))		/* 0x3=SPDIF_AUDIO */
			mvOsPrintf("Error: Audio modules not supported when booting from NAND\n");
}

/*******************************************************************************
* mvBoardDDRBusWidthCheck
*
* DESCRIPTION:
*	Check if DDR buswidth is different from the board configuration
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
MV_VOID mvBoardDDRBusWidthCheck(MV_VOID)
{
	MV_U32 configVar, ddrVar;

	/* DDR buswidth field control relevant for MV88F6660 SoC */
	if (mvBoardIdGet() != DB_6660_ID)
		return;

	configVar = (mvCtrlSysConfigGet(MV_CONFIG_DDR_BUSWIDTH) == 0x0) ? 32 : 16;
	ddrVar = mvCtrlDDRBudWidth();

	if (configVar != ddrVar)
		mvOsPrintf("Error: Mismatch between DDR buswidth - %d, and board configuration DDR buswidth - %d\n",\
				ddrVar, configVar);
}


/*******************************************************************************
* mvBoardConfigurationPrint
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
MV_VOID mvBoardConfigurationPrint(MV_VOID)
{
	char *lane1[] = {"PCIe1", "SGMII-0", "SATA-1", "Unconnected" };
	char *tdmSlic[] = {"None", "SSI", "ISI", "ZSI", "TDM"};
	MV_U32 slicDevice, ethConfig = mvBoardEthComplexConfigGet();
	MV_U16 modelID = mvCtrlModelGet();

	mvOsOutput("\nBoard configuration:\n");

	mvOsOutput("\tEEPROM/Dip Switch: %s\n", mvBoardIsEepromEnabled() ? "EEPROM" : "DIP-Switch");

	/* Mac configuration */
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_COMPHY_1)
		mvOsOutput("\tSGMII0 on MAC0 [Lane1]\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_COMPHY_2)
		mvOsOutput("\tSGMII0 on MAC0 [Lane2]\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_COMPHY_3)
		mvOsOutput("\tSGMII0 on MAC0 [Lane3]\n");


	/* Switch configuration */
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		mvOsOutput("\tEthernet Switch port 6 on MAC0 [CPU Port], %s Speed\n"
				, (ethConfig & MV_ETHCOMP_P2P_MAC0_2_SW_SPEED_2G) ? "2G" : "1G");
	else if ((ethConfig & MV_ETHCOMP_GE_MAC1_2_SW_P4) &&
		!(ethConfig & MV_ETHCOMP_GE_MAC0_2_SW_P6))
		mvOsOutput("\tEthernet Switch port 4 on MAC1 [CPU Port], 1G Speed\n");
	if ((ethConfig & MV_ETHCOMP_GE_MAC0_2_SW_P6) &&
		(ethConfig & MV_ETHCOMP_GE_MAC1_2_SW_P4)) {
		mvOsOutput("\tEthernet Switch port 4 on MAC1, 1G Speed\n");
	}

	/* RGMII */
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_RGMII0)
		mvOsOutput("\tRGMII0 Module on MAC0\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_RGMII1)
		mvOsOutput("\tRGMII1 Module on MAC1\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_RGMII0)
		mvOsOutput("\tRGMII0 Module on MAC1\n");
	if (ethConfig & MV_ETHCOMP_SW_P4_2_RGMII0_EXT_PHY)
		mvOsOutput("\tExternal PHY-RGMII0 Module on Switch port #4, 1G speed\n");
	else if (ethConfig & MV_ETHCOMP_SW_P4_2_RGMII0)
		mvOsOutput("\tRGMII0 Module on Switch port #4, 1G speed\n");

	/* Internal GE Quad Phy */
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)
			mvOsOutput("\tGE-PHY-0 on MAC0\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)
			mvOsOutput("\tGE-PHY-3 on MAC1\n");
	if ((ethConfig & MV_ETHCOMP_SW_P0_2_GE_PHY_P0) && (ethConfig & MV_ETHCOMP_SW_P1_2_GE_PHY_P1)
		&& (ethConfig & MV_ETHCOMP_SW_P2_2_GE_PHY_P2) && (ethConfig & MV_ETHCOMP_SW_P3_2_GE_PHY_P3))
			mvOsOutput("\t4xGE-PHY Module on 4 Switch ports\n");
	else {
		if (ethConfig & MV_ETHCOMP_SW_P0_2_GE_PHY_P0)
			mvOsOutput("\tGE-PHY-0 Module on Switch port #0\n");
		if (ethConfig & MV_ETHCOMP_SW_P1_2_GE_PHY_P1)
			mvOsOutput("\tGE-PHY-1 Module on Switch port #1\n");
		if (ethConfig & MV_ETHCOMP_SW_P2_2_GE_PHY_P2)
			mvOsOutput("\tGE-PHY-2 Module on Switch port #2\n");
		if (ethConfig & MV_ETHCOMP_SW_P3_2_GE_PHY_P3)
			mvOsOutput("\tGE-PHY-3 Module on Switch port #3\n");
	}

	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)
		mvOsOutput("       PON ETH SERDES on MAC1\n");
	if (ethConfig & MV_ETHCOMP_P2P_MAC_2_PON_ETH_SERDES)
		mvOsOutput("       ETH SERDES on P2P MAC\n");

	/* TDM / Slic configuration */
	slicDevice = mvBoardSlicUnitTypeGet();
	if (slicDevice < MV_BOARD_SLIC_MAX_OPTION) /* 4 supported configurations */
		mvOsOutput("\tTDM/SLIC: %s\n", tdmSlic[slicDevice]);
	else
		mvOsOutput("\tTDM/SLIC: Unsupported configuration\n");

	/* SERDES Lanes*/
	mvOsOutput("\nSERDES configuration:\n");
	mvOsOutput("\tLane #0: PCIe0\n");	/* Lane 0 is always PCIe0 */

	/* Dynamic config for SerDes lanes is relevant only to MV88F6660/65/58 */
	if (modelID != MV_6660_DEV_ID && modelID != MV_6665_DEV_ID && modelID != MV_6658_DEV_ID)
		return;
	/* Read Common Phy selectors to determine SerDes configuration */
	mvOsOutput("\tLane #1: %s\n", lane1[mvBoardLaneSelectorGet(1)]);
	/* SERDES lanes #2,#3 are relevant only to MV88F6660/65 SoC */
	if (modelID != MV_6660_DEV_ID && modelID != MV_6665_DEV_ID)
		return;
	mvOsOutput("\tLane #2: %s\n", (mvBoardLaneSelectorGet(2) ? "SATA-0" : "SGMII-0"));
	mvOsOutput("\tLane #3: %s\n", (mvBoardLaneSelectorGet(3) ? "SGMII-0" : "USB3"));
}

/*******************************************************************************
* mvBoardLaneSelectorGet
*
*  DESCRIPTION:
*       Get Lane Selector
*
* INPUT:
*       Lane number
*
* OUTPUT:
*
* RETURN:
*	Lane selector
*
*******************************************************************************/
MV_U32 mvBoardLaneSelectorGet(MV_U32 laneNum)
{
	MV_U32 revID = mvCtrlRevGet();
	MV_U32 laneSelector, selector = MV_REG_READ(MV_COMMON_PHY_REGS_OFFSET);
	if (revID <= MV_88F66X0_Z3_ID)
		laneSelector = (selector & SERDES_LANE_MASK_Z_REV(laneNum)) >> SERDES_LANE_OFFS_Z_REV(laneNum);
	else
		laneSelector = (selector & SERDES_LANE_MASK(laneNum)) >> SERDES_LANE_OFFS(laneNum);
	return laneSelector;
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

	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum, ioInfo->regNum, &val) != MV_OK) {
		mvOsPrintf("%s: Error: Read from IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum));
		return (MV_U8)MV_ERROR;
	}

	mask = (1 << ioInfo->offset);
	return (val & mask) >> ioInfo->offset;
}

/*******************************************************************************
* mvBoardIoExpValSet - write a specified value to IO Expanders
*
* DESCRIPTION:
*       This function writes specified value to IO Expanders
*
* INPUT:
*       ioInfo  - relevant IO Expander information
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U8  :return requested value , if TWSI read was succesfull, else 0xFF.
*
*******************************************************************************/
MV_STATUS mvBoardIoExpValSet(MV_BOARD_IO_EXPANDER_TYPE_INFO *ioInfo, MV_U8 value)
{
	MV_U8 readVal, configVal;

	if (ioInfo == NULL) {
		mvOsPrintf("%s: Error: Write to IO Expander failed (invalid Expander info)\n", __func__);
		return MV_ERROR;
	}
	/* Read Value */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum,
					ioInfo->regNum, &readVal) != MV_OK) {
		mvOsPrintf("%s: Error: Read from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Read Configuration Value */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum,
					ioInfo->regNum + 6, &configVal) != MV_OK) {
		mvOsPrintf("%s: Error: Read Configuration from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Modify Configuration value to Enable write for requested bit */
	configVal &= ~(1 << ioInfo->offset);	/* clean bit of old value  */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum,
					ioInfo->regNum + 6, configVal) != MV_OK) {
		mvOsPrintf("%s: Error: Enable Write to IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum));
		return MV_ERROR;
	}

	/* Modify */
	readVal &= ~(1 << ioInfo->offset);	/* clean bit of old value  */
	readVal |= (value << ioInfo->offset);

	/* Write */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum,
					ioInfo->regNum + 2, readVal) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, ioInfo->expanderNum));
		return MV_ERROR;
	}

	return MV_OK;
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
* mvBoardSatrInfoConfig
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
MV_STATUS mvBoardSatrInfoConfig(MV_SATR_TYPE_ID satrClass, MV_BOARD_SATR_INFO *satrInfo, MV_BOOL read)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	/* verify existence of requested SATR type, and pull its data,
	 * if write sequence, check if field is writeable for running board */
	for (i = 0; i < MV_SATR_WRITE_MAX_OPTION ; i++)
		if (boardSatrInfo[i].satrId == satrClass) {

			/* if read sequence, or an authorized write sequence -> return true */
			if (read == MV_TRUE || boardSatrInfo[i].isWriteable[boardId]) {
				*satrInfo = boardSatrInfo[i];
				return MV_OK;
			}
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
MV_BOOL mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	/* verify existence of requested config type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION ; i++)
		if (boardConfigTypesInfo[i].configId == configClass) {
			*configInfo = boardConfigTypesInfo[i];
			if (boardConfigTypesInfo[i].isActiveForBoard[boardId])
				return MV_TRUE;
			else
				return MV_FALSE;
		}
	mvOsPrintf("%s: Error: requested MV_CONFIG_TYPE_ID was not found (%d)\n", __func__, configClass);
	return MV_FALSE;
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
MV_STATUS mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_TYPE_ID ioClass,
		MV_BOARD_IO_EXPANDER_TYPE_INFO *ioInfo)
{
	MV_U32 i;

	MV_BOARD_IO_EXPANDER_TYPE_INFO ioe_db6660[] = MV_BOARD_IO_EXP_DB6660_INFO;
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioe_db6650[] = MV_BOARD_IO_EXP_DB6650_INFO;
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioe_rd6660[] = MV_BOARD_IO_EXP_RD6660_INFO;
	MV_BOARD_IO_EXPANDER_TYPE_INFO *ioe;
	MV_U8 n = 0;

	switch (mvBoardIdGet()) {
	case DB_6650_ID:
		ioe = ioe_db6650;
		n = ARRSZ(ioe_db6650);
		break;
	case DB_6660_ID:
		ioe = ioe_db6660;
		n = ARRSZ(ioe_db6660);
		break;
	case RD_6660_ID:
		ioe = ioe_rd6660;
		n = ARRSZ(ioe_rd6660);
		break;
	default:
		mvOsPrintf("%s: Error: IO Expander doesn't exists on board\n", __func__);
		return MV_ERROR;
	}

	/* verify existance of requested config type, pull its data */
	for (i = 0; i < n ; i++)
		if (ioe[i].ioFieldid == ioClass) {

			*ioInfo = ioe[i];
			return MV_OK;
		}

	mvOsPrintf("%s: Error: requested IO expander id was not found (%d)\n",
			__func__, ioClass);
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardExtPhyBufferSelect - enable/disable buffer status
*
* DESCRIPTION:
*	This function enables/disables the buffer status.
*
* INPUT:
*	enable - Boolean to indicate requested status
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_STATUS mvBoardExtPhyBufferSelect(MV_BOOL enable)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;
	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_EXT_PHY_SMI_EN, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO expander failed (External Phy SMI Buffer select)\n", __func__);
		return MV_ERROR;
	}

	return mvBoardIoExpValSet(&ioInfo, (enable ? 0x0 : 0x1));
}

/*******************************************************************************
* mvBoardSgmiiSfp0TxSet - enable/disable SGMII_SFP0_TX_DISABLE status
*
* DESCRIPTION:
*	This function enables/disables the field status.
*
* INPUT:
*	enable - Boolean to indicate requested status
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_STATUS mvBoardSgmiiSfp0TxSet(MV_BOOL enable)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_SFP0_TX_DIS, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO expander failed (SFP0_TX_DIS)\n", __func__);
		return MV_ERROR;
	}

	return mvBoardIoExpValSet(&ioInfo, (enable ? 0x0 : 0x1));
}

/*******************************************************************************
* mvBoardHDDSelecteExternal - Select External / Internal HDD
*
* DESCRIPTION:
*	This function External / Internal HDD
*	HDD_SELECT = 1 --> Routes SATA signals to External connector
*	HDD_SELECT = 0 --> Routes SATA signals to Internal connector
*
* INPUT:
*	enableInternal - Boolean to indicate requested status
*	MV_TRUE --> set Internal HDD
*	MV_FALSE --> set External HDD
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_STATUS mvBoardHddExtSet(MV_BOOL enableExternal)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_HDD_SELECT, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO expander failed (USB_SS_EN)\n", __func__);
		return MV_ERROR;
	}

	return mvBoardIoExpValSet(&ioInfo, (enableExternal ? 0x1 : 0x0));
}

/*******************************************************************************
* mvBoardHDDPowerSet - enable HDD Power status
*
* DESCRIPTION:
*	This function enables/disables  HDD Power status.
*	HDD_SELECT = 1 --> Enables SATA power
*
* INPUT:
*	enable - Boolean to indicate requested status
*	MV_TRUE -->  Enables SATA power
*	MV_FALSE --> Disables SATA power
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_STATUS mvBoardHDDPowerSet(MV_BOOL enable)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_HDD_PWR_EN, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO expander failed (USB_SS_EN)\n", __func__);
		return MV_ERROR;
	}

	return mvBoardIoExpValSet(&ioInfo, (enable ? 0x1 : 0x1));
}

/*******************************************************************************
* mvBoardUsbSsEnSet - enable/disable USB_SS_EN status
*
* DESCRIPTION:
*	This function enables/disables USB_SS_EN status.
*	USB_SS_EN = 1 --> Enable USB 3.0 900mA current limit
*
* INPUT:
*	enable - Boolean to indicate requested status
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_STATUS mvBoardUsbSsEnSet(MV_BOOL enable)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_USB_SUPER_SPEED, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO expander failed (USB_SS_EN)\n", __func__);
		return MV_ERROR;
	}

	return mvBoardIoExpValSet(&ioInfo, (enable ? 0x1 : 0x0));
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

	board = boardInfoTbl[boardId];
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
MV_STATUS mvBoardTwsiGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	DB(mvOsPrintf("Board: TWSI Read device\n"));
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);

	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
	twsiSlave.offset = regNum;

	if (twsiClass == BOARD_DEV_TWSI_EEPROM)
		twsiSlave.moreThen256 = MV_TRUE;
	else
		twsiSlave.moreThen256 = MV_FALSE;

	if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
		mvOsPrintf("%s: Twsi Read fail\n", __func__);
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded\n"));

	*pData = data;
	return MV_OK;
}

/*******************************************************************************
* mvBoardTwsiSet
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
	DB(mvOsPrintf("%s: TWSI Write addr %x, type %x, data %x\n", __func__,
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;

	/* need to use 2 address bytes when accessing EEPROM */
	if (twsiClass == BOARD_DEV_TWSI_EEPROM)
		twsiSlave.moreThen256 = MV_TRUE; /* use  2 address bytes */
	else
		twsiSlave.moreThen256 = MV_FALSE; /* use  1 address byte */


	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("%s: Write S@R fail\n", __func__));
		return MV_ERROR;
	}
	DB(mvOsPrintf("%s: Write S@R succeded\n", __func__));

	return MV_OK;
}


/*******************************************************************************
* mvBoardEepromWrite - Write a new configuration value for a specific field
*
* DESCRIPTION:
*	Verify if the EEPROM have been initialized (if not, initialize it):
*	EEPROM expected mapping:
*	[0x0] - configuration 1st byte
*	[0x1] - configuration 2nd byte
*	[0x2] - configuration 3rd byte
*	[0x4-0x7] - 32bit pattern to detect if EEPROM is initialized
* INPUT:
*       None
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
MV_STATUS mvBoardEepromWrite(MV_CONFIG_TYPE_ID configType, MV_U8 value)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_U8 readValue, regNum;

	if (mvBoardConfigTypeGet(configType, &configInfo) != MV_TRUE) {
		mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
		return MV_ERROR;
	}

	/* reg num is according to DIP-switch mapping (each Expander conatins 2 registers) */
	regNum = configInfo.expanderNum * 2 + configInfo.regNum;

	/* Read */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_EEPROM, 0, regNum , &readValue) != MV_OK) {
		mvOsPrintf("%s: Error: Read configuration from EEPROM failed\n", __func__);
		return MV_ERROR;
	}

	/* Modify */
	readValue &= ~configInfo.mask;
	readValue |= (value << configInfo.offset);

	/* Write */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_EEPROM, 0, regNum, readValue) != MV_OK) {
			mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
			return MV_ERROR;
	}

	/* Update local array information */
	mvCtrlSysConfigSet(configInfo.configId, value);

	/* run conflict verification sequence on MAC and SerDes configuration */
	mvBoardEthComplexMacConfigCheck();

	/* Check conflicts between device bus module and NAND */
	mvBoardAudioModuleConfigCheck();

	return MV_OK;
}

/*******************************************************************************
* mvBoardTwsiProbe - Probe the given I2C chip address
*
* DESCRIPTION:
*
* INPUT:
*       chip - i2c chip address to probe
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
MV_STATUS mvBoardTwsiProbe(MV_U32 chip)
{
	MV_TWSI_ADDR eepromAddress, slave;
	MV_U32 status = 0;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;

	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	status = mvTwsiStartBitSet(0);

	if (status) {
		mvOsPrintf("%s: Transaction start failed: 0x%02x\n", __func__, status);
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}

	eepromAddress.type = ADDR7_BIT;
	eepromAddress.address = chip;

	status = mvTwsiAddrSet(0, &eepromAddress, MV_TWSI_WRITE); /* send the slave address */
	if (status) {
		mvOsPrintf("%s: Failed to set slave address: 0x%02x\n", __func__, status);
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}
	DB(mvOsPrintf("address %#x returned %#x\n", chip,
				MV_REG_READ(TWSI_STATUS_BAUDE_RATE_REG(i2c_current_bus))));

	/* issue a stop bit */
	mvTwsiStopBitSet(0);

	DB(mvOsPrintf("%s: successful I2C probe\n", __func__));
	return MV_TRUE; /* successful completion */
}

/*******************************************************************************
* mvBoardEepromInit - Verify if the EEPROM have been initialized
*
* DESCRIPTION:
*	Verify if the EEPROM have been initialized (if not, initialize it):
*	EEPROM expected mapping:
*	[0x0] - configuration 1st byte
*	[0x1] - configuration 2nd byte
*	[0x2] - configuration 3rd byte
*	[0x4-0x7] - 32bit pattern
* INPUT:
*       None
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
MV_STATUS mvBoardEepromInit()
{
	MV_U32 readValue, i, pattern = 0;
	MV_U8 patternByte;

	if (mvBoardIsEepromEnabled() != MV_TRUE) {
		DB(printf("%s: EEPROM doesn't exists on board\n" , __func__));
		return MV_ERROR;
	}

	/* verify EEPROM: read 4 bytes at address 0x4 (read magic pattern) */
	for (i = 0; i < 4; i++) {
		if (mvBoardTwsiGet(BOARD_DEV_TWSI_EEPROM, 0, 0x4 + i, (MV_U8 *)&readValue) != MV_OK) {
			mvOsPrintf("%s: Error: Read pattern from EEPROM failed\n", __func__);
			return MV_ERROR;
		}
		/* shift byte to correct location in 32bit pattern */
		pattern |= (readValue & 0x000000FF) << (32 - 8*(i+1));
	}

	/* If EEPROM is initialized with magic pattern, continue and exit*/
	if (pattern == EEPROM_VERIFICATION_PATTERN)
		return MV_OK;

	/* Else write default configuration and set magic pattern */
	for (i = 0x0; i < 4; i++) {
		/* write default board configuration (default value for all fields is 0x0) */
		if (mvBoardTwsiSet(BOARD_DEV_TWSI_EEPROM, 0, i, 0x0) != MV_OK) {
			mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
			return MV_ERROR;
		}

		/* shift bytes to correct location from 32bit pattern to 1 byte chunks*/
		patternByte = ((EEPROM_VERIFICATION_PATTERN) >> (32 - 8*(i+1))) & 0x000000FF;
		if (mvBoardTwsiSet(BOARD_DEV_TWSI_EEPROM, 0, 0x4+i, patternByte) != MV_OK) {
			mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
			return MV_ERROR;
		}
	}

	mvOsPrintf("\n%s: Initialized EEPROM with default board ", __func__);
	mvOsPrintf("configuration (1st use of EEPROM)\n\n");
	return MV_OK;
}

/*******************************************************************************
* mvBoardIsEepromEnabled - read EEPROM and verify if EEPROM exists
*
* DESCRIPTION:
*       This function returns MV_TRUE if board configuration EEPROM exists on board.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM exists, else return MV_FALSE.
*
*******************************************************************************/
MV_BOOL mvBoardIsEepromEnabled()
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO ioInfo;
	MV_U8 value, addr = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_EEPROM, 0);

	if (addr == 0xFF)
		return MV_FALSE;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_JUMPER2_EEPROM_ENABLED, &ioInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Read from IO expander failed (EEPROM enabled jumper)\n", __func__);
		return MV_FALSE;
	}

	value = mvBoardIoExpValGet(&ioInfo);
	if (value == 0x1) { /* Jumper is OUT: EEPROM disabled */
		DB(mvOsPrintf("%s: EEPROM Jumper is disabled\n", __func__));
		return MV_OK;
	}
	/* else Jumper is IN: EEPROM enabled */
	DB(mvOsPrintf("%s: EEPROM Jumper is enabled\n", __func__));

	DB(mvOsPrintf("%s probing for i2c chip 0x%x\n", __func__, addr));
	if (mvBoardTwsiProbe((MV_U32)addr) == MV_TRUE)
		return MV_TRUE;  /* EEPROM enabled */
	else
		return MV_FALSE; /* EEPROM disabled */
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
	return BOARD_ETH_SWITCH_SMI_SCAN_MODE;
}

/*******************************************************************************
* mvBoardSwitchCpuPortGet - Get the the Ethernet Switch CPU port
*
* DESCRIPTION:
*	This routine returns the Switch CPU port if connected, -1 else.
*
* INPUT:
*	switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*	None.
*
* RETURN:
*	the Switch CPU port, -1 if the switch is not connected.
*
*******************************************************************************/
MV_U32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx)
{
	MV_U32 c = mvBoardEthComplexConfigGet();
	MV_U32 cpuPort = -1;

	if (c & MV_ETHCOMP_GE_MAC0_2_SW_P6)
		cpuPort = 6;
	else if (c & MV_ETHCOMP_GE_MAC1_2_SW_P4)
		cpuPort = 4;
	else
		mvOsPrintf("%s: Error: No CPU port.\n", __func__);

	return cpuPort;
}

/*******************************************************************************
* mvBoardIsEthConnected - detect if a certain Ethernet port is connected
*
* DESCRIPTION:
*	This routine returns true if a certain Ethernet port is connected
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
	MV_U32 c = mvBoardEthComplexConfigGet();
	MV_BOOL isConnected = MV_FALSE;

	if (ethNum >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: Illegal port number(%u)\n", __func__, ethNum);
		return MV_FALSE;
	}

	/* Determine if port is connected:
	 * If set as active in Ethernet complex board configuration */
	if (ethNum == 0 && ((c & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) ||
			(c & MV_ETHCOMP_GE_MAC0_2_RGMII0) ||
			(c & MV_ETHCOMP_GE_MAC0_2_COMPHY_1) ||
			(c & MV_ETHCOMP_GE_MAC0_2_COMPHY_2) ||
			(c & MV_ETHCOMP_GE_MAC0_2_COMPHY_3) ||
			(c & MV_ETHCOMP_GE_MAC0_2_SW_P6)))
			isConnected = MV_TRUE;

	if (ethNum == 1 && ((c & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) ||
			(c & MV_ETHCOMP_GE_MAC1_2_RGMII1) ||
			(c & MV_ETHCOMP_GE_MAC1_2_RGMII0) ||
			(c & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES) ||
			(c & MV_ETHCOMP_GE_MAC1_2_SW_P4)))
			isConnected = MV_TRUE;

	if (ethNum == 2 && mvBoardIsPortLoopback(ethNum))
			isConnected = MV_TRUE;

	return isConnected;
}

/*******************************************************************************
* mvBoardIsEthActive - detect if a certain Ethernet port is Active and usable
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
	MV_U32 c = mvBoardEthComplexConfigGet();
	MV_BOOL isActive = MV_FALSE;

	if (ethNum >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: Illegal port number(%u)\n", __func__, ethNum);
		return MV_FALSE;
	}

	/*
	 * Determine if port is active - both connected and usable:
	 * 1. connected : if MAC is set as connected in Ethernet complex board configuration
	 * 2. usable    : - MAC always usable when connected to RGMII, COMPHY, or GE-PHY
	 *                - if connected to switch ,a MAC is usable only as the CPU Port
	 *                  (if another MAC is connected to switch, it's used for Loopback)
	 */
	if (ethNum == 0 && ((c & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) ||
			(c & MV_ETHCOMP_GE_MAC0_2_RGMII0) ||
			(c & MV_ETHCOMP_GE_MAC0_2_COMPHY_2) ||
			((c & MV_ETHCOMP_GE_MAC0_2_SW_P6) && mvBoardMacCpuPortGet() == 0)))
			isActive = MV_TRUE;

	if (ethNum == 1 && ((c & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) ||
			(c & MV_ETHCOMP_GE_MAC1_2_RGMII1) ||
			(c & MV_ETHCOMP_GE_MAC1_2_RGMII0) ||
			(c & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES) ||
			((c & MV_ETHCOMP_GE_MAC1_2_SW_P4) && mvBoardMacCpuPortGet() == 1)))
			isActive = MV_TRUE;

	return isActive;
}
/*******************************************************************************
* mvBoardMacCpuPortGet - returns the MAC CPU port connected to switch
*
* DESCRIPTION:
*	This routine returns true returns the MAC CPU port connected to switch
*
* INPUT:
*	None
*
* OUTPUT:
*	None
*
* RETURN:
*	the MAC CPU port number connected to switch
*
*******************************************************************************/
MV_U32 mvBoardMacCpuPortGet(MV_VOID)
{
	MV_U32 c = mvBoardEthComplexConfigGet();
	MV_U32 macCpuPort = -1;

	if (c & MV_ETHCOMP_GE_MAC0_2_SW_P6)		/* MAC0 is the default CPU port */
		macCpuPort = 0;
	/* only If MAC0 isn't connected to switch, then MAC1 is the CPU port
	 * If both MAC0 and MAC1 connected to switch, MAC1 is used for Loopback */
	else if (c & MV_ETHCOMP_GE_MAC1_2_SW_P4)
		macCpuPort = 1;
	else
		DB(mvOsPrintf("%s: Error: No MAC CPU port.\n", __func__));

	return macCpuPort;
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

/*******************************************************************************
* mvBoardConfigurationConflicts
*
* DESCRIPTION:
*       Check if there is any conflicts with the board configurations
*
* INPUT:
*	field = Field name of configuration
*	writeVal = option number
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_OK: if there is no conflicts
*	MV_ERROR: conflict in one configuration
*
*******************************************************************************/
MV_STATUS mvBoardConfigVerify(MV_CONFIG_TYPE_ID field, MV_U8 writeVal)
{
	MV_U32 c = mvBoardEthComplexConfigGet();
	/* Config Lane1: 0x2 = SATA1, 0x3 = Unconnected are supported only for A0
	** config MAC1 to RGMII0 are supported only for A0 */
	if (((field == MV_CONFIG_LANE1 && (writeVal == 0x2 || writeVal == 0x3)) ||
			(field == MV_CONFIG_MAC1 && writeVal == 0x3)) &&
			(mvCtrlRevGet() <= MV_88F66X0_Z3_ID)) {
		mvOsPrintf("Error: this option is not supported in Z stepping revision\n");
		return MV_ERROR;
	}
	if (field == MV_CONFIG_MAC1 && (c & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES)) {
		mvOsPrintf("Warning: MAC1 is connected to PON Serdes\n");
		return MV_ERROR;
	}
	/* 0x3 = RGMII0, check if MAC0 is connected to RGMII0 */
	if (field == MV_CONFIG_MAC1 && writeVal == 0x3 && (c & MV_ETHCOMP_GE_MAC0_2_RGMII0)) {
		mvOsPrintf("Warning: MAC0 is connected to RGMII0 Module\n");
		return MV_ERROR;
	}
	/* 0x2 = RGMII0, check if MAC1 is connected to RGMII0 */
	if (field == MV_CONFIG_MAC0 && writeVal == 0x2 && (c & MV_ETHCOMP_GE_MAC1_2_RGMII0)) {
		mvOsPrintf("Warning: MAC1 is connected to RGMII0 Module\n");
		return MV_ERROR;
	}

	return MV_OK;

}
