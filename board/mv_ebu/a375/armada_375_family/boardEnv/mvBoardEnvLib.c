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

extern MV_BOARD_INFO *marvellBoardInfoTbl[];
extern MV_BOARD_INFO *customerBoardInfoTbl[];
extern MV_BOARD_SATR_INFO boardSatrInfo[];
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_BOARD_CONFIG_INFO;

/* Locals */
static MV_DEV_CS_INFO *mvBoardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
/* Global variables should be removed from BSS (set to a non-zero value)
   for avoiding memory corruption during early access upon code relocation */
static MV_BOARD_INFO *board = (MV_BOARD_INFO *)-1;

/*******************************************************************************
* mvBoardPortTypeGet
*
* DESCRIPTION:
*       This routine returns port type
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None
*
* RETURN:
*       Mode of the port
*
*******************************************************************************/
MV_U32 mvBoardPortTypeGet(MV_U32 ethPortNum)
{
	if (mvBoardIsPortInSgmii(ethPortNum))
		return MV_PORT_TYPE_SGMII;
	else
		return MV_PORT_TYPE_RGMII;
}

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
	return boardId & (MARVELL_BOARD_ID_BASE - 1);
}

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
	MV_U32 nandDev;
	MV_U32 norDev;

	mvBoardSet(mvBoardIdGet());
	MV_U32 syncCtrl = 0;

	nandDev = mvBoardGetDevCSNum(0, BOARD_DEV_NAND_FLASH);
	if (nandDev != 0xFFFFFFFF) {
		/* Set NAND interface access parameters */
		MV_REG_WRITE(DEV_BANK_PARAM_REG(nandDev), board->nandFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(nandDev), board->nandFlashWriteParams);
		MV_REG_WRITE(DEV_NAND_CTRL_REG, board->nandFlashControl);
		/* Set Ready Polarity to Active High */
		syncCtrl |= SYNC_CTRL_READY_POL(nandDev);
	}

	norDev = mvBoardGetDevCSNum(0, BOARD_DEV_NOR_FLASH);
	if (norDev != 0xFFFFFFFF) {
		/* Set NOR interface access parameters */
		MV_REG_WRITE(DEV_BANK_PARAM_REG(norDev), board->norFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(norDev), board->norFlashWriteParams);
		/* Ignore Ready signal */
		syncCtrl |= SYNC_CTRL_READY_IGNORE(norDev);
	}

	if (nandDev != 0xFFFFFFFF || norDev != 0xFFFFFFFF) {
		/* Set TCLK Divide Value to 1:1 */
		syncCtrl |= 0x1;
		/* Finally - write the Bus Sync Control configuration */
		MV_REG_WRITE(DEV_BUS_SYNC_CTRL, syncCtrl);
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
	return mvBoardIdIndexGet(mvBoardIdGet()) >> 16;
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
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();
	if (ethPortNum == 1 && (ethComplex & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES ||
			     (ethComplex & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP)))
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
*       MV_TRUE - port in GMII.
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
* mvBoardQuadPhyAddr0Get - Get the PHY address
*
* DESCRIPTION:
*       This routine returns the PHY address of a given Ethernet port.
*       Required to initialize QUAD PHY through a specific PHY address
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing PHY address, -1 if the port number is wrong.
*
*******************************************************************************/
MV_32 mvBoardQuadPhyAddr0Get(MV_U32 ethPortNum)
{
	if (ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return MV_ERROR;
	}

	return board->pBoardMacInfo[ethPortNum].boardEthSmiAddr0;
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
	if (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode))
		return (MV_U32)(1000000 * freqMode.ddrFreq);
	else
		return MV_ERROR;
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
* mvBoardIoExpValGet - read a specified value of a bit from IO Expanders
*
* DESCRIPTION:
*       This function returns specified bit value from IO Expanders
*
* INPUT:
*	regNum		- IO.exp register number
*	expanderNum	- IO.exp number
*	offset		- requested bit offset
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U8  :return requested bit value , if TWSI read was succesfull, else 0xFF.
*
*******************************************************************************/
MV_U8 mvBoardIoExpValGet(MV_U8 regNum, MV_U8 expanderNum, MV_U8 offset)
{
	MV_U8 val, mask;

	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum, regNum, &val) != MV_OK) {
		mvOsPrintf("%s: Error: Read from IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum));
		return (MV_U8)MV_ERROR;
	}

	mask = (1 << offset);
	return (val & mask) >> offset;
}

/*******************************************************************************
* mvBoardIoExpValSet - write a specified bit value to IO Expanders
*
* DESCRIPTION:
*       This function writes specified bit value to IO Expanders
*
* INPUT:
*	regNum		- IO.exp register number
*	expanderNum	- IO.exp number
*	offset		- requested bit offset
*	value		- requested bit new value
* OUTPUT:
*       None.
*
* RETURN:
*       MV_STATUS: MV_OK if succeeded, MV_ERROR else.
*
*******************************************************************************/
MV_STATUS mvBoardIoExpValSet(MV_U8 regNum, MV_U8 expanderNum, MV_U8 offset, MV_U8 value)
{
	MV_U8 readVal, configVal;

	/* Read Value */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum,
					regNum, &readVal) != MV_OK) {
		mvOsPrintf("%s: Error: Read from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Read Configuration Value */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum,
					regNum + 6, &configVal) != MV_OK) {
		mvOsPrintf("%s: Error: Read Configuration from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Modify Configuration value to Enable write for requested bit */
	configVal &= ~(1 << offset);	/* clean bit of old value  */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum,
					regNum + 6, configVal) != MV_OK) {
		mvOsPrintf("%s: Error: Enable Write to IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum));
		return MV_ERROR;
	}

	/* Modify */
	readVal &= ~(1 << offset);	/* clean bit of old value  */
	readVal |= (value << offset);

	/* Write */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum,
					regNum + 2, readVal) != MV_OK) {
		mvOsPrintf("%s: Error: Write to IO Expander at 0x%x failed\n", __func__
			   , mvBoardTwsiAddrGet(BOARD_DEV_TWSI_IO_EXPANDER, expanderNum));
		return MV_ERROR;
	}

	return MV_OK;
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
* mvBoardDevStateUpdate -
*
* DESCRIPTION:
*	Update Board devices state (active/passive) according to boot source
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
MV_VOID mvBoardFlashDeviceUpdate(MV_VOID)
{
	MV_BOARD_BOOT_SRC	bootSrc = mvBoardBootDeviceGet();
	MV_U32				numOfDevices;
	MV_U8				devNum;
	MV_U32				devBus;

	/* Assume that the board sctructure sets SPI flash active as the default boot device */
	if (bootSrc == MSAR_0_BOOT_NAND_NEW) {
		/* Activate first NAND device */
		mvBoardSetDevState(0, BOARD_DEV_NAND_FLASH, MV_TRUE);

		/* Deactivate all SPI0 devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_SPI_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++) {
			devBus = mvBoardGetDevBusNum(devNum, BOARD_DEV_SPI_FLASH);
			if (devBus == 0)
				mvBoardSetDevState(devNum, BOARD_DEV_SPI_FLASH, MV_FALSE);
		}

	} else if (bootSrc == MSAR_0_BOOT_NOR_FLASH) {

		/* Activate first NOR device */
		mvBoardSetDevState(0, BOARD_DEV_NOR_FLASH, MV_TRUE);

		/* Deactivate all SPI devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_SPI_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++)
			mvBoardSetDevState(devNum, BOARD_DEV_SPI_FLASH, MV_FALSE);

		/* Deactivate all NAND devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_NAND_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++)
			mvBoardSetDevState(devNum, BOARD_DEV_NAND_FLASH, MV_FALSE);

	}
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
	MV_U32 ethComplex;

	ethComplex = mvBoardEthComplexConfigGet();

	if (mvBoardIsMac1Sgmii() == MV_TRUE) {
		/* disable GE-PHY#3, and enable SGMII */
		ethComplex &= ~MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3;
		ethComplex |= MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP;
	} else {
		/* disable SGMII, and enable GE-PHY#3 */
		ethComplex &= ~MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP;
		ethComplex |= MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3;
	}

	mvBoardEthComplexConfigSet(ethComplex);

	/* Update SMI phy address for MAC0/1 */
	if (ethComplex & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0)
		mvBoardPhyAddrSet(0, 0x0);
	else if (ethComplex & MV_ETHCOMP_GE_MAC0_2_RGMII0)
		mvBoardPhyAddrSet(0, 0x0);
	else
		mvBoardPhyAddrSet(0, -1); /* SMI address is specified only for a PHY, else inBand SGMII/Switch*/

	if (ethComplex & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3)
		mvBoardPhyAddrSet(1, 0x3);
	else if (ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1)
		mvBoardPhyAddrSet(1, 0x1);
	else
		mvBoardPhyAddrSet(1, -1); /* SMI address is specified only for a PHY, else inBand SGMII/Switch*/

	/* Update MPP group types and values according to board configuration */
	mvBoardMppIdUpdate();

	mvBoardFlashDeviceUpdate();
}

/*******************************************************************************
* mvBoardMppIdUpdate - Update MPP ID's according to modules auto-detection.
*
* DESCRIPTION:
*	Update MPP ID's according to boot source
*	Update board information for changed mpp values
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
	/* MPP Groups initialization : */
	/* Set Group 0-1 - Boot device (else if booting from SPI1: Set Groups 3-4) */
	mvBoardBootDeviceGroupSet();

	/* Groups 3,4: GE1 settings (internal GE-PHY / SGMII)
	 * Group  7  : I2C-1 needed for SGMII, to set sfpTXdisable via IO expander */
	if (mvBoardIsMac1Sgmii() == MV_TRUE) {
		/* disable GE-PHY#3, and enable SGMII */
		mvBoardMppTypeSet(3, GE1_SGMII_UNIT);
		mvBoardMppTypeSet(4, GE1_SGMII_UNIT);
		mvBoardMppTypeSet(7, GE0_LED_I2C1_UNIT);
	} else {
		/* disable SGMII, and enable GE-PHY#3 */
		mvBoardMppTypeSet(3, GE1_PHY3_UNIT);
		mvBoardMppTypeSet(4, GE1_PHY3_UNIT);
		mvBoardMppTypeSet(7, GE0_LED_UNIT);
	}
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
	MV_BOARD_BOOT_SRC bootSrc = mvBoardBootDeviceGet();

	switch (bootSrc) {
	case MSAR_0_BOOT_NAND_NEW:
		mvBoardMppTypeSet(0, NAND_BOOT_V2);
		mvBoardMppTypeSet(1, NAND_BOOT_V2);
		break;
	case MSAR_0_BOOT_SPI_FLASH:
		mvBoardMppTypeSet(0, SPI0_BOOT);
		mvBoardMppTypeSet(1, SPI0_BOOT);
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
	MV_SATR_BOOT_TABLE satrBootEntry;
	MV_BOARD_BOOT_SRC defaultBootSrc;

/* prepare default boot source, in case of boot from UART / S@R read ERROR */
#if defined(MV_SPI_BOOT)
	defaultBootSrc = MSAR_0_BOOT_SPI_FLASH;
	DB(mvOsPrintf("default boot source is SPI-0\n"));
#elif defined(MV_NAND_BOOT)
	defaultBootSrc = MSAR_0_BOOT_NAND_NEW;
	DB(mvOsPrintf("default boot source is NAND\n"));
#endif

	if (satrBootDeviceValue == MV_ERROR) {
		mvOsPrintf("%s: Error: failed to read boot source\n", __func__);
		mvOsPrintf("Using pre-compiled image type as boot source\n");
		return defaultBootSrc;
	}

	/* Get boot source entry from Satr boot table */
	satrBootEntry = satrTable[satrBootDeviceValue];

	/* If booting from UART, return pre-compiled boot source*/
	if (satrBootEntry.bootSrc == MSAR_0_BOOT_UART) {
		mvOsPrintf("\t** Booting from UART (restore DIP-switch to");
		mvOsPrintf("requested boot source before reset!) **\n");
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
* mvBoardMac1ConfigGet
*
* DESCRIPTION: Read MAC configuration for S@R
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*        MV_U32 describing MAC1 settings : 0 = Internal PHY , 1 = SGMII (via ETH SerDes)
*
*******************************************************************************/
MV_U32 mvBoardMac1ConfigGet(MV_VOID)
{
	MV_U32 mac1Config;

	mac1Config = mvCtrlSatRRead(MV_SATR_MAC1);
	if (mac1Config == MV_ERROR) {
		DB(mvOsPrintf("%s: Error: MV_SATR_MAC1 is not active for board (using default)\n", __func__));
		return 0;
	} else
		return mac1Config;
}

/*******************************************************************************
* mvBoardIsMac1Sgmii
*
* DESCRIPTION: return True if MAC is configured to be SGMII (S@R)
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*        MV_BOOL : MV_TRUE if MAC1 is configured at S@R to be SGMII
*
*******************************************************************************/
MV_BOOL mvBoardIsMac1Sgmii(MV_VOID)
{
	return (mvBoardMac1ConfigGet() == 1 ? MV_TRUE : MV_FALSE);
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
	MV_U32 laneConfig;
	/* Lane 1 */
	laneConfig = mvCtrlSysConfigGet(MV_CONFIG_LANE1);
	if (laneConfig == MV_ERROR)
		return MV_ERROR;
	else if (laneConfig == 0x1)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_1;
	/* Lane 2 */
	laneConfig = mvCtrlSysConfigGet(MV_CONFIG_LANE2);
	if (laneConfig == MV_ERROR)
		return MV_ERROR;
	else if (laneConfig == 0x0)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_2;
	/* Lane 3 */
	laneConfig = mvCtrlSysConfigGet(MV_CONFIG_LANE3);
	if (laneConfig == MV_ERROR)
		return MV_ERROR;
	else if (laneConfig == 0x1)
		return MV_ETHCOMP_GE_MAC0_2_COMPHY_3;

	mvOsPrintf("%s: Error: unexpected value for Serdes Lane board configuration\n", __func__);
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardIsInternalSwitchConnectedToPort
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
MV_STATUS mvBoardIsInternalSwitchConnectedToPort(MV_U32 ethPortNum)
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
	return board->switchforceLinkMask;
}

/*******************************************************************************
* mvBoardFreqModesNumGet
*
* DESCRIPTION:
*      Return the number of supported frequency modes for this board
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
	MV_U16 ctrlModel = mvCtrlModelGet();

	if (ctrlModel == MV_6720_DEV_ID)
		return FREQ_MODES_NUM_6720;

	mvOsPrintf("%s: Error: Illegal ctrl Model (%x)\n", __func__, ctrlModel);
	return MV_ERROR;
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
	MV_U32 i, ethConfig = mvBoardEthComplexConfigGet();

	mvOsOutput("Board configuration detected:\n");

	/* RGMII */
	if (ethConfig & MV_ETHCOMP_GE_MAC0_2_RGMII0)
		mvOsOutput("\tRGMII0 Module on MAC0\n");
	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_RGMII1)
		mvOsOutput("\tRGMII1 on MAC1\n");
	if (ethConfig & MV_ETHCOMP_SW_P4_2_RGMII0)
		mvOsOutput("\tRGMII0 Module on Switch port #4\n");

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
	if (ethConfig & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP)
		mvOsOutput("\tETH SERDES on MAC1 [SFP]\n");

	/* SERDES Lanes*/
	mvOsOutput("SERDES configuration:\n");
	for (i = 0; i < 4; i++) {
		switch (mvCtrlLaneSelectorGet(i)) {
		case PEX_UNIT_ID:
			mvOsOutput("\tLane #%d: PCIe%d\n", i, i);
			break;
		case USB3_UNIT_ID:
			mvOsOutput("\tLane #%d: USB3\n", i);
			break;
		case SATA_UNIT_ID:
			mvOsOutput("\tLane #%d: SATA%d\n", i, (i == 1 ? 1 : 0));
			break;
		case SGMII_UNIT_ID:
			mvOsOutput("\tLane #%d: SGMII\n", i);
			break;
		default:
			break;
		}
	}
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
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

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
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

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
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

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
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

	if (devEntry)
		return mvCpuIfTargetWinSizeGet(DEV_TO_TARGET(devEntry->deviceCS));

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDevEntry - returns the entry pointer of a device on the board
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
static MV_DEV_CS_INFO *mvBoardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
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
* mvBoardGetDevCSNum
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
MV_U32 mvBoardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

	if (devEntry)
		return devEntry->deviceCS;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDevBusNum
*
* DESCRIPTION:
*	Return the device's bus number.
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
*	dev bus number else the function returns 0xFFFFFFFF
*
*******************************************************************************/
MV_U32 mvBoardGetDevBusNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

	if (devEntry)
		return devEntry->busNum;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDevState
*
* DESCRIPTION:
*	Return the device's activity state.
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
*	dev activity state else the function returns 0xFFFFFFFF
*
*******************************************************************************/
MV_BOOL mvBoardGetDevState(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

	if (devEntry)
		return devEntry->active;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardSetDevState
*
* DESCRIPTION:
*	Sets the device's activity state.
*
* INPUT:
*	devIndex - The device sequential number on the board
*   devType - The device type ( Flash,RTC , etc .. )
*   newState - requested deevice state
*
* OUTPUT:
*	None.
*
* RETURN:
*	If the device is found on the board the then the functions returns
*	MV_OK else MV_ERROR
*
*******************************************************************************/
MV_STATUS mvBoardSetDevState(MV_32 devNum, MV_BOARD_DEV_CLASS devClass, MV_BOOL newState)
{
	MV_DEV_CS_INFO *devEntry = mvBoardGetDevEntry(devNum, devClass);

	if (devEntry) {
		devEntry->active = newState;
		return MV_OK;
	} else
		return MV_ERROR;
}

/*******************************************************************************
* mvBoardSgmiiSfp1TxSet - enable/disable SGMII_SFP1_TX_DISABLE status
*
* DESCRIPTION:
*       This function enables/disables the field status.
*
* INPUT:
*       enable - Boolean to indicate requested status
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
********************************************************************************/
MV_STATUS mvBoardSgmiiSfp1TxSet(MV_BOOL enable)
{
	return mvBoardIoExpValSet(0, 0, 1 , (enable ? 0x0 : 0x1));
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
	int i, start, end;
	MV_U32 boardId = mvBoardIdIndexGet(mvBoardIdGet());

	if (read == MV_TRUE) {	/* if read request, check read SATR fields */
		start = 0;
		end = MV_SATR_READ_MAX_OPTION;
	} else {		/* if write request, check write SATR fields */
		start = MV_SATR_READ_MAX_OPTION;
		end = MV_SATR_WRITE_MAX_OPTION;
	}

	/* verify existence of requested SATR type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = start; i < end ; i++)
		if (boardSatrInfo[i].satrId == satrClass) {
			*satrInfo = boardSatrInfo[i];
			/* if read sequence, or an authorized write sequence -> return OK */
			if (read == MV_TRUE || boardSatrInfo[i].isActiveForBoard[boardId])
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
MV_BOOL mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdIndexGet(mvBoardIdGet());

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
	return MV_FALSE;
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
* mvBoardSet - Set Board model
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
static MV_U32 gBoardId = -1;
MV_VOID mvBoardSet(MV_U32 boardId)
{
	/* board ID's >0x10 are for Marvell Boards */
	if (boardId >= MARVELL_BOARD_ID_BASE && boardId < MV_MAX_MARVELL_BOARD_ID) { /* Marvell Board */
		board = marvellBoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	} else if (boardId >= CUTOMER_BOARD_ID_BASE && boardId < MV_MAX_CUSTOMER_BOARD_ID) { /* Customer Board */
		board = customerBoardInfoTbl[mvBoardIdIndexGet(boardId)];
		gBoardId = boardId;
	} else
		mvOsPrintf("%s: Error: wrong boardId (%d)\n", __func__, boardId);
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

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	#ifdef CONFIG_CUSTOMER_BOARD_0
		gBoardId = ARMADA_375_CUSTOMER_BOARD_ID0;
	#elif CONFIG_CUSTOMER_BOARD_1
		gBoardId = ARMADA_375_CUSTOMER_BOARD_ID1;
	#endif
#else

/*
 * Disabled S@R board ID read, and use static DB6720 init (there is only 1 A375 Marvell board)
 * Motivation: Dedicated MPP's for Board ID conflicts with i2c bus #1 pull up pin
 */
#if 0
	MV_U32 readValue;

	readValue = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	readValue = ((readValue & (0xF0)) >> 4);

	/* A375 DB board ID is 0xc - align it to be 1st board */
	if (readValue == DB_6720_HW_ID)
		readValue = 0x0;

	if (readValue < MV_MARVELL_BOARD_NUM && readValue >= 0) {
		gBoardId = MARVELL_BOARD_ID_BASE + readValue;
	} else {
		mvOsPrintf("%s: Error: read wrong board (%d)\n", __func__, readValue);
		return MV_INVALID_BOARD_ID;
	}
#endif /* if 0 */

	gBoardId = DB_6720_ID;
#endif

	return gBoardId;
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
	MV_U8 data, chanNum = 0;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;

	DB(mvOsPrintf("Board: TWSI Read device\n"));
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);

	/* for A375, IO expander is on i2c bus #1 */
	if (twsiClass == BOARD_DEV_TWSI_IO_EXPANDER)
		chanNum = 1;

	mvTwsiInit(chanNum, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;

	if (MV_OK != mvTwsiRead(chanNum, &twsiSlave, &data, 1)) {
		mvOsPrintf("%s: Twsi Read fail\n", __func__);
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded\n"));

	*pData = data;
	return MV_OK;
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
	MV_U8 chanNum = 0;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);
	twsiSlave.validOffset = MV_TRUE;

	/* for A375, IO expander is on i2c bus #1 */
	if (twsiClass == BOARD_DEV_TWSI_IO_EXPANDER)
		chanNum = 1;

	mvTwsiInit(chanNum, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	DB(mvOsPrintf("%s: TWSI Write addr %x, type %x, data %x\n", __func__,
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	if (MV_OK != mvTwsiWrite(chanNum, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("%s: Write S@R fail\n", __func__));
		return MV_ERROR;
	}
	DB(mvOsPrintf("%s: Write S@R succeded\n", __func__));

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
	MV_U32 c = board->pBoardModTypeValue->ethSataComplexOpt;
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
* mvBoardIsEthConnected - detect if a certain Ethernet port is active
*
* DESCRIPTION:
*	This routine returns true if a certain Ethernet port is active
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
	MV_BOOL isActive = MV_FALSE;

	if (ethNum == 0 && ((c & MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0) ||
			(c & MV_ETHCOMP_GE_MAC0_2_RGMII0)))
			isActive = MV_TRUE;

	if (ethNum == 1 && ((c & MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3) ||
			    (c & MV_ETHCOMP_GE_MAC1_2_RGMII1) ||
			    (c & MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES_SFP)))
			isActive = MV_TRUE;

	return isActive;
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
	/* for A375, all connected ports are Active and usabe */
	return mvBoardIsEthConnected(ethNum);
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
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();
	if (switchPortNum >= BOARD_ETH_SWITCH_PORT_NUM) {
		mvOsPrintf("%s: Error: wrong switch port number (%d)\n", __func__, switchPortNum);
		return -1;
	}

	if ((switchPortNum == 0) && (ethComplex & MV_ETHCOMP_SW_P0_2_GE_PHY_P0))
		return 0;
	else if ((switchPortNum == 1) && (ethComplex & MV_ETHCOMP_SW_P1_2_GE_PHY_P1))
		return 1;
	else if ((switchPortNum == 2) && (ethComplex & MV_ETHCOMP_SW_P2_2_GE_PHY_P2))
		return 2;
	else if ((switchPortNum == 3) && (ethComplex & MV_ETHCOMP_SW_P3_2_GE_PHY_P3))
		return 3;
	else if ((switchPortNum == 4) && (ethComplex & MV_ETHCOMP_SW_P4_2_RGMII0))
		return 4;

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
	return &board->boardPexInfo;
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
* mvBoardUartPortGet
*
* DESCRIPTION: Return the UART port
* INPUT:  None.
* OUTPUT: None.
* RETURN: None.
*******************************************************************************/
MV_U32 mvBoardUartPortGet(void)
{
	return whoAmI();
}

/*******************************************************************************
* mvBoardNandECCModeGet
*
* DESCRIPTION:
*	Obtain NAND ECC mode
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_NFC_ECC_MODE type
*
*******************************************************************************/
MV_NFC_ECC_MODE mvBoardNandECCModeGet()
{
#if defined(MV_NAND_4BIT_MODE)
	return MV_NFC_ECC_BCH_2K;
#elif defined(MV_NAND_8BIT_MODE)
	return MV_NFC_ECC_BCH_1K;
#elif defined(MV_NAND_12BIT_MODE)
	return MV_NFC_ECC_BCH_704B;
#elif defined(MV_NAND_16BIT_MODE)
	return MV_NFC_ECC_BCH_512B;
#else
	MV_U32 satrBootDeviceValue;
	MV_SATR_BOOT_TABLE satrTable[] = MV_SATR_TABLE_VAL;

	if (mvBoardBootDeviceGet() == MSAR_0_BOOT_NAND_NEW) {
		satrBootDeviceValue = mvCtrlSatRRead(MV_SATR_BOOT_DEVICE);
		switch (satrTable[satrBootDeviceValue].attr3) {
		case MSAR_0_NAND_ECC_4BIT:
			return MV_NFC_ECC_BCH_2K;
		case MSAR_0_NAND_ECC_8BIT:
			return MV_NFC_ECC_BCH_1K;
		case MSAR_0_NAND_ECC_12BIT:
			return MV_NFC_ECC_BCH_704B;
		case MSAR_0_NAND_ECC_16BIT:
			return MV_NFC_ECC_BCH_512B;
		default:
			break;
		}
	}
	return MV_NFC_ECC_DISABLE;
#endif
}

/*******************************************************************************
* mvBoardCompatibleNameGet
*
* DESCRIPTION: return string containing 'compatible' property value
*		needed for Device Tree auto-update parsing sequence
*
* INPUT:  None
* OUTPUT: None.
*
* RETURN: pointer to string
*
*******************************************************************************/
MV_STATUS mvBoardCompatibleNameGet(char *pNameBuff)
{
	/* i.e: "marvell,a375-db", "marvell,armada375"; */
	sprintf(pNameBuff, "marvell,a375-%s\", \"marvell,armada375", board->compatibleDTName);

	return MV_OK;
}

MV_NAND_IF_MODE mvBoardNandIfGet()
{
	MV_BOARD_BOOT_SRC boot_src = mvBoardBootDeviceGet();
	switch (boot_src) {
	case MSAR_0_BOOT_NAND_NEW:
		return NAND_IF_NFC;
	case MSAR_0_BOOT_NAND_SPI:
#ifdef MV_NAND_SPI
		return NAND_IF_SPI;
#else
			mvOsPrintf("%s: Error: NAND_IF_SPI isn't defined while " \
					"MSAR configured to MSAR_0_BOOT_NAND_SPI\n", __func__);
		return NAND_IF_NONE;
#endif
	default:
		return board->nandIfMode;
	}
}
