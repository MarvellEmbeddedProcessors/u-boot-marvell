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
MV_MODULE_TYPE_INFO boardModuleTypesInfo[] = MV_MODULE_INFO;
MV_BOARD_SATR_INFO boardSatrInfo[] = MV_SAR_INFO;
MV_BOARD_SATR_INFO boardSatrInfo2[] = MV_SAR_INFO2;
MV_SATR_BOOT_TABLE satrBootSrcTable[] = MV_SATR_BOOT_SRC_TABLE_VAL;



/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
/* Global variables should be removed from BSS (set to a non-zero value)
   for avoiding memory corruption during early access upon code relocation */
static MV_BOARD_INFO *board = (MV_BOARD_INFO *)-1;
static MV_VOID mvBoardModuleAutoDetect(MV_VOID);

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
	MV_U32 nandDev	= 0xFFFFFFFF;
	MV_U32 norDev	= 0xFFFFFFFF;
	MV_U32 syncCtrl	= 0;
	MV_BOARD_BOOT_SRC bootSrc;

	mvBoardSet(mvBoardIdGet());
	if (mvBoardConfigAutoDetectEnabled())
		mvBoardModuleAutoDetect();
	bootSrc = mvBoardBootDeviceGroupSet();
	if (MSAR_0_BOOT_NAND_NEW == bootSrc) {	/* init NAND only if boot from NAND */
		nandDev = boardGetDevCSNum(0, BOARD_DEV_NAND_FLASH);
		if (nandDev != 0xFFFFFFFF) {
			/* Set NAND interface access parameters */
			MV_REG_WRITE(DEV_BANK_PARAM_REG(nandDev), board->nandFlashReadParams);
			MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(nandDev), board->nandFlashWriteParams);
			MV_REG_WRITE(DEV_NAND_CTRL_REG, board->nandFlashControl);
			/* Set Ready Polarity to Active High */
			syncCtrl |= SYNC_CTRL_READY_POL(nandDev);
		}
	} else if (mvBoardModuleConfigGet() & MV_MODULE_NOR) { /* init NOR only if Module NOR is detected */
		norDev = boardGetDevCSNum(0, BOARD_DEV_NOR_FLASH);
		if (norDev != 0xFFFFFFFF) {
			/* Set NOR interface access parameters */
			MV_REG_WRITE(DEV_BANK_PARAM_REG(norDev), board->norFlashReadParams);
			MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(norDev), board->norFlashWriteParams);
			/* Ignore Ready signal */
			syncCtrl |= SYNC_CTRL_READY_IGNORE(norDev);
		}
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
* mvBoardModelGet - Get Board model - (ECO version)
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
*
*******************************************************************************/
MV_U32 mvBoardRevGet(MV_VOID)
{

	MV_U32 boardECO;
	MV_U8 readValue;

	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, 1, &readValue, 1) != MV_OK) {
		mvOsPrintf("%s: Error: Read from TWSI failed\n", __func__);
		return MV_ERROR;
	}
	boardECO = (readValue & 0x0F) | ((readValue & 0x0f0) << 12);
	return boardECO;
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
* mvBoardModuleConfigGet - Get the module configuration
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit module configuration.
*
*******************************************************************************/
MV_32 mvBoardModuleConfigGet(MV_VOID)
{
	return board->boardOptionsModule;
}
/*******************************************************************************
* mvBoardModuleConfigSet - Set the module configuration
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit module configuration.
*
*******************************************************************************/
MV_VOID mvBoardModuleConfigSet(MV_U32 newCfg)
{
	/* The MII module cannot work together with the SGMII module */
	/* Ignore MII module if SGMII module is detected */
	if ((newCfg == MV_MODULE_MII) && (MV_MODULE_SGMII & board->boardOptionsModule)) {
		mvOsPrintf("%s: Warning: Conflict in module detect, (SGMII & MII module), MII module is ignored\n",
			    __func__);
		return;
	}
	if ((newCfg == MV_MODULE_SGMII) && (MV_MODULE_MII & board->boardOptionsModule)) {
		mvOsPrintf("%s: Warning: Conflict in module detect, (SGMII & MII module), MII module is ignored\n",
			   __func__);
		board->boardOptionsModule &= ~MV_MODULE_MII;
	}
	board->boardOptionsModule |= newCfg;
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
* mvBoardQuadPhyAddr0Get - Get the Quad PHY address
*
* DESCRIPTION:
*       This routine returns the Quad PHY address of a given Ethernet port.
*       Required to initialize QUAD PHY through a specific PHY address
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing Quad PHY address, -1 if the port number is wrong.
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
* mvBoardQuadPhyAddr0Set - Set the phy address
*
* DESCRIPTION:
*       This routine sets the Quad PHY address of a given Ethernet port.
*       Required to initialize QUAD PHY through a specific PHY address
*
* INPUT:
*       ethPortNum - Ethernet port number.
*       smiAddr    - requested Quad phy address
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardQuadPhyAddr0Set(MV_U32 ethPortNum, MV_U32 smiAddr)
{
	if (ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return;
	}

	board->pBoardMacInfo[ethPortNum].boardEthSmiAddr0 = smiAddr;
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
* mvBoardMacSpeedSet - Set the Mac speed
*
* DESCRIPTION:
*       This routine SET the Mac speed for a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*       ethSpeed. - 10/100/1000 eth speed
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardMacSpeedSet(MV_U32 ethPortNum, MV_BOARD_MAC_SPEED ethSpeed)
{
	if (ethPortNum >= board->numBoardMacInfo) {
		mvOsPrintf("%s: Error: wrong eth port (%d)\n", __func__, ethPortNum);
		return;
	}
	board->pBoardMacInfo[ethPortNum].boardMacSpeed = ethSpeed;
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
	tclk = (MV_REG_READ(MPP_SAMPLE_AT_RESET));
	tclk = ((tclk & (1 << 15)) >> 15);
	switch (tclk) {
	case 0:
		return MV_BOARD_TCLK_250MHZ;
	case 1:
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_250MHZ;
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
	return MV_BOARD_SLIC_DISABLED;
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
* mvBoardModuleAutoDetect - Update Board information structures according to auto-detection.
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
static MV_VOID mvBoardModuleAutoDetect(MV_VOID)
{
	MV_U8 readValue;
	MV_MODULE_TYPE_INFO configInfo;
	int i;
	MV_BOARD_BOOT_SRC bootSrc;
	/*Read all TWSI board module if exsist : */
	/* Save values board spec struct  */
	for (i = 0; i < MV_MODULE_TYPE_MAX_MODULE; i++) {
		if (mvBoardModuleTypeGet((1 << i), &configInfo) == MV_TRUE) {
			if (mvBoardTwsiGet(BOARD_TWSI_MODULE_DETECT, configInfo.twsiAddr,
					   configInfo.offset, &readValue, 1) != MV_OK) {
				DB(mvOsPrintf("%s: Error: Read from TWSI failed addr=0x%x\n",
					   __func__, configInfo.twsiAddr));
				continue;
			}
			readValue &= 0x0f;	/* only 4 bit relevant  */
			/* twsi ID represente  module configuration ID*/
			if (configInfo.twsiId == readValue)
				mvBoardModuleConfigSet(configInfo.configId);
		}
	}
	bootSrc = mvBoardBootDeviceGroupSet();
	if (MSAR_0_BOOT_NAND_NEW == bootSrc)
		mvBoardModuleConfigSet(MV_MODULE_NAND_ON_BOARD);

}

/*******************************************************************************
* mvBoardIsModuleConnected
*
* DESCRIPTION:
*	Checks if a given Module connected to board.
*
* INPUT:
*	Module ID
*
* OUTPUT:
*       None
*
* RETURN:
*	MV_TRUE if Module is connected, MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardIsModuleConnected(MV_U32 ModuleID)
{
	if (mvBoardModuleConfigGet() & ModuleID)
		return MV_TRUE;
	return MV_FALSE;
}
/*******************************************************************************
* mvModuleMppUpdate
*
* DESCRIPTION:
*
* INPUT:
*	num of grup.
*	pointer to arreay MV_BOARD_MPP_MODULE
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
void mvModuleMppUpdate(MV_U32 numGroup, struct _mvBoardMppModule *pMpp)
{
	int i;
	if (pMpp == NULL)
		return;
	for (i = 0; i < numGroup; i++) {
		mvBoardMppSet(pMpp->group, pMpp->mppValue);
		pMpp++;
	}
}

/*******************************************************************************
* mvBoardMppIdUpdate - Update MPP ID's according to modules auto-detection.
*
* DESCRIPTION:
*	Update MPP ID's according to on-board modules as detected using TWSI bus.
*	Update board information for changed mpp values
	Must run AFTER mvBoardNetComplexInfoUpdate
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
#ifdef CONFIG_ARMADA_38X
	struct _mvBoardMppModule miiModule[3] = MPP_MII_MODULE;
	struct _mvBoardMppModule norModule[6] = MPP_NOR_MODULE;
	struct _mvBoardMppModule nandModule[6] = MPP_NAND_MODULE;
	struct _mvBoardMppModule sdioModule[4] = MPP_SDIO_MODULE;
	struct _mvBoardMppModule tdmModule[2] = MPP_TDM_MODULE;
	struct _mvBoardMppModule i2sModule = MPP_I2S_MODULE;
	struct _mvBoardMppModule spdifModule = MPP_SPDIF_MODULE;
	struct _mvBoardMppModule nandOnBoard[4] = MPP_NAND_ON_BOARD;
	struct _mvBoardMppModule mini_pcie0_OnBoard = MPP_GP_MINI_PCIE0;
	struct _mvBoardMppModule mini_pcie1_OnBoard = MPP_GP_MINI_PCIE1;
	struct _mvBoardMppModule mini_pcie0_pcie1OnBoard = MPP_GP_MINI_PCIE0_PCIE1;
	MV_U8 miniPcie0_sata0_selector, miniPcie1_sata1_selector;


	switch (mvBoardIdGet()) {
	case DB_68XX_ID:
		if (mvBoardIsModuleConnected(MV_MODULE_MII))
			mvModuleMppUpdate(3, miiModule);

		if (mvBoardIsModuleConnected(MV_MODULE_NOR))
			mvModuleMppUpdate(6, norModule);

		if (mvBoardIsModuleConnected(MV_MODULE_NAND))
			mvModuleMppUpdate(6, nandModule);

		if (mvBoardIsModuleConnected(MV_MODULE_SDIO))
			mvModuleMppUpdate(4, sdioModule);

		if (mvBoardIsModuleConnected(MV_MODULE_SLIC_TDM_DEVICE))
			mvModuleMppUpdate(2, tdmModule);

		if (mvBoardIsModuleConnected(MV_MODULE_I2S_DEVICE))
			mvModuleMppUpdate(1, &i2sModule);

		if (mvBoardIsModuleConnected(MV_MODULE_SPDIF_DEVICE))
			mvModuleMppUpdate(1, &spdifModule);

		if (mvBoardIsModuleConnected(MV_MODULE_NAND_ON_BOARD))
			mvModuleMppUpdate(4, nandOnBoard);
		break;
	case DB_GP_68XX_ID:
		miniPcie0_sata0_selector = mvBoardSatRRead(MV_SATR_GP_SERDES1_CFG); /* 0 = SATA0 , 1 = PCIe0 */
		miniPcie1_sata1_selector = mvBoardSatRRead(MV_SATR_GP_SERDES2_CFG); /* 0 = SATA1 , 1 = PCIe1 */
		if (miniPcie0_sata0_selector == 1 && miniPcie1_sata1_selector == 1)
			mvModuleMppUpdate(1, &mini_pcie0_pcie1OnBoard);
		else if (miniPcie0_sata0_selector == 1)
			mvModuleMppUpdate(1, &mini_pcie0_OnBoard);
		else if (miniPcie1_sata1_selector == 1)
			mvModuleMppUpdate(1, &mini_pcie1_OnBoard);
		break;
	}
#endif
}

/*******************************************************************************
* mvBoardIoExpanderUpdate
*
* DESCRIPTION:
*	Update io expander via TWSI,
*
** INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - wriet to twsi failed.
*
*******************************************************************************/
MV_STATUS mvBoardIoExpanderUpdate(MV_VOID)
{
	MV_U32 i = 0;

#ifdef CONFIG_ARMADA_38X
	MV_U8 ioValue, ioValue2;
	MV_U8 boardId = mvBoardIdGet();
	MV_U32 serdesCfg = MV_ERROR;

	/* Verify existence of IO expander on board, and fetch 1st IO expander value to modify */
	if (mvBoardIoExpanderGet(0, 2, &ioValue) == MV_ERROR ||
		mvBoardIoExpanderGet(1, 6, &ioValue2) == MV_ERROR)
		return MV_OK;

	if (boardId == DB_GP_68XX_ID) {
		/* check SerDes lane 1,2,5 configuration ('gpserdes1/2/5') */
		serdesCfg = mvBoardSatRRead(MV_SATR_GP_SERDES5_CFG);
		if (serdesCfg != MV_ERROR) {
			if (serdesCfg == 0) /* 0 = USB3.  1 = SGMII. */
				ioValue |= 1 ;	/* Setting USB3.0 interface: configure IO as output '1' */
			else {
				ioValue &= ~1;		/* Setting SGMII interface:  configure IO as output '0' */
				ioValue2 &= ~BIT5;	/* Set Tx disable for SGMII */
			}
			mvBoardIoExpanderSet(1, 6, ioValue2);
		}

		serdesCfg = mvBoardSatRRead(MV_SATR_GP_SERDES1_CFG);
		if (serdesCfg == 1) { /* 0 = SATA0 , 1 = PCIe0 */
			ioValue |= BIT1 ;	/* Setting PCIe0 (mini): PCIe0_SATA0_SEL(out) = 1 (PCIe)  */
			ioValue &= ~BIT2;	/* disable SATA0 power: PWR_EN_SATA0(out) = 0 */
		}

		serdesCfg = mvBoardSatRRead(MV_SATR_GP_SERDES2_CFG);
		if (serdesCfg == 1) { /* 0 = SATA1 , 1 = PCIe1 */
			ioValue |= BIT6 ;	/* Setting PCIe1 (mini): PCIe1_SATA1_SEL(out) = 1 (PCIe)  */
			ioValue &= ~BIT3;	/* disable SATA1 power: PWR_EN_SATA1(out) = 0 */
		}

		mvBoardIoExpanderSet(0, 2, ioValue);
	}
#endif

	for (i = 0; i < board->numIoExp; i++) {
		if (MV_OK != mvBoardTwsiSet(BOARD_TWSI_IO_EXPANDER, board->pIoExp[i].addr,
					    board->pIoExp[i].offset, &board->pIoExp[i].val, 1)) {
			mvOsPrintf("%s: Write IO expander (addr=0x%x, offset=%d, value=0x%2x to  fail\n",
				   __func__,
				   mvBoardTwsiAddrGet(BOARD_TWSI_IO_EXPANDER, board->pIoExp[i].addr),
				   board->pIoExp[i].offset,
				   board->pIoExp[i].val);
			return MV_ERROR;
		}
	}
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
	MV_BOARD_BOOT_SRC bootSrc = mvBoardBootDeviceGet();

	switch (bootSrc) {
	case MSAR_0_BOOT_NOR_FLASH:
		break;
	case MSAR_0_BOOT_NAND_NEW:
		mvBoardModuleConfigSet(MV_MODULE_NAND_ON_BOARD);
		break;
	case MSAR_0_BOOT_SPI_FLASH:
		break;
	case MSAR_0_BOOT_SPI1_FLASH:
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
	MV_SATR_BOOT_TABLE satrBootEntry;

	satrBootDeviceValue = mvCtrlbootSrcGet();

	satrBootEntry = satrBootSrcTable[satrBootDeviceValue];

	if (-1 == (MV_U32)satrBootEntry.bootSrc)	/* return default for incorrect value */
		return MSAR_0_BOOT_SPI_FLASH;

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
	MV_SATR_BOOT_TABLE satrBootEntry = satrBootSrcTable[satrBootDeviceValue];

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
	return MV_FALSE;
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
	return 0;
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
	return 0;
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
	if ((mvBoardIsModuleConnected(MV_MODULE_SGMII)) && (ethPortNum == 0))
		return MV_FALSE;
	if (ethPortNum < mvCtrlEthMaxPortGet())
		return MV_TRUE;

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
* mvBoardTwsiAddrSet
*
* DESCRIPTION:
*	Set a new TWSI address for a given twsi device class.
*
* INPUT:
*	twsiClass - The TWSI device to return the address type for.
*	index	  - The TWSI device index (Pass 0 in case of a single
*		    device)
*	address   - The new TWSI address.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TWSI address.
*
*******************************************************************************/
MV_VOID mvBoardTwsiAddrSet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index, MV_U8 address)
{
	int i;

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if ((board->pBoardTwsiDev[i].devClass == twsiClass) \
				&& (board->pBoardTwsiDev[i].devClassId == index)) {
			board->pBoardTwsiDev[i].twsiDevAddr = address;
		}
	}
}

/*******************************************************************************
* mvBoardTwsiIsMore256Get
*
* DESCRIPTION:
*	Return flag if TWSI have more then 256 byte.
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
*	The flag from spec table.
*
*******************************************************************************/
MV_U8 mvBoardTwsiIsMore256Get(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index)
{
	int i;

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if ((board->pBoardTwsiDev[i].devClass == twsiClass) && (board->pBoardTwsiDev[i].devClassId == index))
			return board->pBoardTwsiDev[i].moreThen256;
	}

	return 0xFF;
}

/*******************************************************************************
* mvBoardNetComplexConfigGet - Return ethernet complex board configuration.
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
MV_U32 mvBoardNetComplexConfigGet(MV_VOID)
{
	return board->pBoardNetComplexInfo->netComplexOpt;
}

/*******************************************************************************
* mvBoardNetComplexConfigSet - Set ethernet complex board configuration.
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
MV_VOID mvBoardNetComplexConfigSet(MV_U32 ethConfig)
{
	board->pBoardNetComplexInfo->netComplexOpt = ethConfig;
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
MV_STATUS mvBoardSatrInfoConfig(MV_SATR_TYPE_ID satrClass, MV_BOARD_SATR_INFO *satrInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 boardIdIndex = mvBoardIdIndexGet(boardId);
	MV_BOARD_SATR_INFO *satrInfoTable = boardSatrInfo;

#ifdef CONFIG_ARMADA_38X
	/* A38x DB-GP board has different I2C mapping for SSCG, Core clock, and Device ID S@R fields */
	/* A381 DB-BP board has different I2C mapping for 'freq' and 'cpusnum' S@R fields */
	if ((boardId == DB_GP_68XX_ID && (satrClass == MV_SATR_CORE_CLK_SELECT ||
		satrClass == MV_SATR_SSCG_DISABLE || satrClass == MV_SATR_DEVICE_ID))
		|| (boardId == DB_BP_6821_ID && satrClass == MV_SATR_CPU_DDR_L2_FREQ))
			satrInfoTable = boardSatrInfo2;
#endif

	/* verify existence of requested SATR type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; satrInfoTable[i].satrId != MV_SATR_MAX_OPTION ; i++) {
		if (satrInfoTable[i].satrId == satrClass) {
			memcpy(satrInfo, &satrInfoTable[i], sizeof(MV_BOARD_SATR_INFO));
			if (satrInfoTable[i].isActiveForBoard[boardIdIndex])
				return MV_OK;
			else
				return MV_ERROR;
		}
	}
	DB(mvOsPrintf("%s: Error: requested MV_SATR_TYPE_ID was not found (%d)\n", __func__,satrClass));
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardModuleTypeGet
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
*	MV_MODULE_TYPE_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_BOOL mvBoardModuleTypeGet(MV_MODULE_TYPE_ID configClass, MV_MODULE_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdIndexGet(mvBoardIdGet());

	/* verify existence of requested config type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_MODULE_TYPE_MAX_OPTION ; i++)
		if (boardModuleTypesInfo[i].configId == configClass) {
			*configInfo = boardModuleTypesInfo[i];
			if (boardModuleTypesInfo[i].isActiveForBoard[boardId])
				return MV_TRUE;
			else
				return MV_FALSE;
		}
	mvOsPrintf("%s: Error: requested MV_MODULE_TYPE_ID was not found (%d)\n", __func__, configClass);
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
		gBoardId = CUSTOMER_BOARD_ID0;
	#elif CONFIG_CUSTOMER_BOARD_1
		gBoardId = CUSTOMER_BOARD_ID1;
	#endif
#else /* !CONFIG_CUSTOMER_BOARD_SUPPORT */
	/* Temporarily set generic board struct pointer, to set/get EEPROM i2c address, and read board ID */
	board = marvellBoardInfoTbl[mvBoardIdIndexGet(MV_DEFAULT_BOARD_ID)];

	/* Up to Z revision, A38x Marvell board ID was saved on EEPROM@0x50.
	 * due to i2c address conflict between the SFP transceiver,
	 * starting from A0 rev, the EEPROM address is shifted to 0x57.
	 * if running A38x Z rev, use the old address = 0x50*/
	if (mvCtrlDevFamilyIdGet(0) == MV_88F68XX && mvCtrlRevGet() == MV_88F68XX_Z1_ID)
		mvBoardTwsiAddrSet(BOARD_DEV_TWSI_SATR, 0, MV_A38X_Z_REV_BOARDID_I2C_ADDR);

	MV_U8 readValue;
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, 0, &readValue, 1) != MV_OK) {
		mvOsPrintf("%s: Error: Read from TWSI failed\n", __func__);
		mvOsPrintf("%s: Set default board ID to %s\n", __func__, board->boardName);
		readValue = MV_DEFAULT_BOARD_ID;
	}
	readValue = readValue & 0x07;

	if (readValue < MV_MARVELL_BOARD_NUM && readValue >= 0) {
		gBoardId = MARVELL_BOARD_ID_BASE + readValue;
	} else {
		mvOsPrintf("%s: Error: read wrong board (%d)\n", __func__, readValue);
		return MV_INVALID_BOARD_ID;
	}
#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

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
*	byteCnt - how many bytes to read/write
*       pData - type must correspond with byteCnt
*               (for example, if byteCnt = 4, pData must be a pointer for 32bit var)
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData, MV_U32 byteCnt)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(twsiClass, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(twsiClass, devNum);
	twsiSlave.moreThen256 = mvBoardTwsiIsMore256Get(twsiClass, devNum);
	DB(mvOsPrintf("Board: TWSI Read device addr=0x%x\n", twsiSlave.slaveAddr.address));

	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
	twsiSlave.offset = regNum;

	if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, byteCnt)) {
		DB(mvOsPrintf("%s: Twsi Read fail\n", __func__));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded\n"));

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
*	byteCnt - how many bytes to read/write
*	regVal - type must correspond with byteCnt
*		(for example, if byteCnt = 4, regVal must be a pointer for 32bit var)
*
* OUTPUT:
*	None.
*
* RETURN:
*	reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiSet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 *regVal, MV_U32 byteCnt)
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
		twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, *regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = mvBoardTwsiIsMore256Get(twsiClass, devNum);

	if (MV_OK != mvTwsiWrite(0, &twsiSlave, regVal, byteCnt)) {
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
	return mvBoardIsModuleConnected(MV_MODULE_SLIC_TDM_DEVICE);
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
	return -1;
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
	return mvBoardIsEthConnected(ethNum);
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
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	if (0xFF == twsiSlave.slaveAddr.address)
		return MV_ERROR;
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.moreThen256 = mvBoardTwsiIsMore256Get(BOARD_DEV_TWSI_SATR, devNum);
	DB(mvOsPrintf("Board: Read S@R device read ADDR=0x%X, regnum=%d\n",	\
		      twsiSlave.slaveAddr.address, regNum));

	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.validOffset = MV_TRUE;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);


	if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, 1)) {
		DB(mvOsPrintf("Board: Read S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded 0x%X\n", *pData));

	return MV_OK;
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
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	if (0xFF == twsiSlave.slaveAddr.address)
		return MV_ERROR;
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.validOffset = MV_TRUE;
	DB(mvOsPrintf("Board: Write S@R device addr %x, regNum %x, data 0x%X\n",
		      twsiSlave.slaveAddr.address, regNum, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = mvBoardTwsiIsMore256Get(BOARD_DEV_TWSI_SATR, devNum);
	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write S@R succeded addr=0x%X data=0x%X\n", twsiSlave.slaveAddr.address, regVal));

	return MV_OK;
}

/*******************************************************************************
* mvBoardSatRRead
*
* DESCRIPTION: Read S@R configuration Field
*
* INPUT: satrField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*	if field is valid - returns requested S@R field value
*       else if field is not relevant for running board, return 0xFFFFFFF.
*
*******************************************************************************/
MV_U32 mvBoardSatRRead(MV_SATR_TYPE_ID satrField)
{
	MV_BOARD_SATR_INFO satrInfo;
	MV_U8 data, data1;
	MV_U32 c;

	if (satrField >= MV_SATR_MAX_OPTION) {
		mvOsPrintf("%s: Error: wrong MV_SATR_TYPE_ID field value (%d).\n", __func__ , satrField);
		return MV_ERROR;
	}

	if (mvBoardSatrInfoConfig(satrField, &satrInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Requested S@R field (%d) is not relevant for this board\n", __func__, satrField);
		return MV_ERROR;
	}

	/* read */
	if (mvBoardTwsiSatRGet(satrInfo.devClassId , satrInfo.regOffset, &data) != MV_OK) {
		mvOsPrintf("%s: Error: Read from S@R failed\n", __func__);
		return MV_ERROR;
	}
	data &= satrInfo.mask;
	data = (data >> satrInfo.bitOffset);

	if (satrInfo.status & SATR_SWAP_BIT) {
		c = mvCountMaskBits(satrInfo.mask);
		data1 = mvReverseBits(data);
		data =  (data1 >> (8-c));
		DB(mvOsPrintf("%s: value after mvReverseBits = 0x%X\n", __func__, data));
	}

	/* BootSrc field is 6 bits: divided over 2 I2C registers */
	if (satrField == MV_SATR_BOOT_DEVICE) {
		/*  read boot mode MSV part from I2C */
		MV_U32 tmp = mvBoardSatRRead(MV_SATR_BOOT2_DEVICE);
		if (tmp == MV_ERROR) {
			mvOsPrintf("%s: Error: Read 2nd part (MSB) of 'bootsrc' from S@R failed\n", __func__);
			return MV_ERROR;
		}
		data = data | ((tmp & MV_SATR_BOOT2_VALUE_MASK) << MV_SATR_BOOT2_VALUE_OFFSET);
	}

	/* devId field for A38x DB-BP board it's divided over 2 I2C registers */
#ifdef CONFIG_ARMADA_38X
	if (mvBoardIdGet() == DB_68XX_ID && satrField == MV_SATR_DEVICE_ID) {
		/* read boot mode MSB part from I2C */
		MV_U32 tmp = mvBoardSatRRead(MV_SATR_DEVICE_ID2);
		if (tmp == MV_ERROR) {
			mvOsPrintf("%s: Error: Read 2nd part (MSB) of 'devid' from S@R failed\n", __func__);
			return MV_ERROR;
		}
		data = data | ((tmp & MV_SATR_DEVICE_ID2_VALUE_OFFSET) << MV_SATR_DEVICE_ID2_VALUE_MASK);
	}
#endif

	return data;
}

/*******************************************************************************
* mvBoardSatRWrite
*
* DESCRIPTION: Write S@R configuration Field
*
* INPUT: satrField - Field description enum
*        val       - value to write (if write action requested)
*
* OUTPUT: None
*
* RETURN:
*       write action:
*       if value is writen succesfully - returns the written value
*       else if write failed - returns MV_ERROR
*
*******************************************************************************/
MV_STATUS mvBoardSatRWrite(MV_SATR_TYPE_ID satrWriteField, MV_U8 val)
{
	MV_BOARD_SATR_INFO satrInfo;
	MV_U8 data, val1, data1;
	MV_U32 c;

	if (satrWriteField >= MV_SATR_MAX_OPTION) {
		mvOsPrintf("%s: Error: wrong MV_SATR_TYPE_ID field value (%d).\n", __func__ , satrWriteField);
		return MV_ERROR;
	}

	if (mvBoardSatrInfoConfig(satrWriteField, &satrInfo) != MV_OK) {
		mvOsPrintf("%s: Error: Requested S@R field is not relevant for this board\n", __func__);
		return MV_ERROR;
	}
	if (satrInfo.status & SATR_READ_ONLY) {
		mvOsPrintf("%s: Error: Requested S@R field is read only\n", __func__);
		return MV_ERROR;
	}
	/* BootSrc field is 6 bits: divided over 2 I2C registers */
	if (satrWriteField == MV_SATR_BOOT_DEVICE) {
		val1 = (val >> MV_SATR_BOOT2_VALUE_OFFSET) & MV_SATR_BOOT2_VALUE_MASK ;
		if (mvBoardSatRWrite(MV_SATR_BOOT2_DEVICE, val1) != MV_OK) {
			mvOsPrintf("%s: Error: write 2nd part (MSB) of boot device field\n", __func__);
			return MV_ERROR;
		}
	}

	/* devId field for DB-BP board it's divided over 2 I2C registers */
#ifdef CONFIG_ARMADA_38X
	if (mvBoardIdGet() == DB_68XX_ID && satrWriteField == MV_SATR_DEVICE_ID) {
		val1 = (val >> MV_SATR_DEVICE_ID2_VALUE_OFFSET) & MV_SATR_DEVICE_ID2_VALUE_MASK;
		if (mvBoardSatRWrite(MV_SATR_DEVICE_ID2, val1) != MV_OK) {
			mvOsPrintf("%s: Error: write 2nd part (MSB) of device ID field\n", __func__);
			return MV_ERROR;
		}
	}
#endif

	val &= (satrInfo.mask >> satrInfo.bitOffset); /* verify correct value */

	/* read */
	if (mvBoardTwsiSatRGet(satrInfo.devClassId , satrInfo.regOffset, &data) != MV_OK) {
		mvOsPrintf("%s: Error: Read from S@R failed\n", __func__);
		return MV_ERROR;
	}

	if (satrInfo.status & SATR_SWAP_BIT) {
		c = mvCountMaskBits(satrInfo.mask);
		data1 = mvReverseBits(val);
		val =  (data1 >> (8-c));
		DB(mvOsPrintf("%s: value after mvReverseBits = 0x%X\n", __func__, val));
	}

	/* modify */
	data &= ~(satrInfo.mask);             /* clean old value */
	data |= (val <<  satrInfo.bitOffset);    /* save new value */

	/* write */
	if (mvBoardTwsiSatRSet(satrInfo.devClassId , satrInfo.regOffset, data) != MV_OK) {
		mvOsPrintf("%s: Error: Write to S@R failed\n", __func__);
		return MV_ERROR;
	}
	return MV_OK;
}
/*******************************************************************************
* mvBoardNorFlashConnect
*
* DESCRIPTION:
*	This routine returns MV_TRUE for NOR module detected
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in GMII.
*       MV_FALSE - other.
*
*******************************************************************************/
int mvBoardNorFlashConnect(void)
{
	if (mvBoardIsModuleConnected(MV_MODULE_NOR))
		return MV_TRUE;
	return MV_FALSE;
}
/****************************************************************************************
* mvBoardIoExpanderGet
*
* DESCRIPTION:
*	Return the IO Expander value for a given address and offset.
*
* INPUT:
*	addr - IO expander address.
*	offs - IO expander offset
*	pValue - pointer saved value
*
* OUTPUT:
*       None.
*
* RETURN:
*       if addr and offset are exist save the value in input pointer and returns  MV_OK
*       else returns MV_ERROR
*
****************************************************************************************/
MV_STATUS mvBoardIoExpanderGet(MV_U8 addr, MV_U8 offs, MV_U8 *pVal)
{
	int i;

	if ((board->pIoExp == NULL) || (0 == board->numIoExp))
		return MV_ERROR;

	for (i = 0; i < board->numIoExp; i++) {
		if ((board->pIoExp[i].addr == addr) &&
		    (board->pIoExp[i].offset == offs)) {
			*pVal = board->pIoExp[i].val;
		    return MV_OK;
		}
	}
	return MV_ERROR;
}
/*******************************************************************************
* mvBoardIoExpanderSet
*
* DESCRIPTION:
*	Save the IO Expander value for a given index.
*
* INPUT:
*	index	  - The IO expander index
*	val	  - The IO expander value to save
*
* OUTPUT:
*       None.
*
* RETURN:
*       if addr and offset are exist save the value and returns  MV_OK
*       else returns MV_ERROR
*******************************************************************************/
MV_STATUS mvBoardIoExpanderSet(MV_U8 addr, MV_U8 offs, MV_U8 val)
{
	int i;

	if ((board->pIoExp == NULL) || (0 == board->numIoExp))
		return MV_ERROR;

	for (i = 0; i < board->numIoExp; i++) {
		if ((board->pIoExp[i].addr == addr) &&
		    (board->pIoExp[i].offset == offs)) {
			board->pIoExp[i].val = val;
			return MV_OK;
		}
	}
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardUartPortGet
*
* DESCRIPTION:
*       Return the UART port
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       return 1 if detect DB-AP-68xx board
*
*******************************************************************************/
MV_U32 mvBoardUartPortGet()
{
	if (mvBoardIdGet() != DB_AP_68XX_ID)
		return whoAmI();

	return (whoAmI() == 0 ? 1 : 0); /* CPU0 uses UART1 on DB-AP */
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
	MV_U32 satrBootDeviceValue = mvCtrlbootSrcGet();

	if (satrBootSrcTable[satrBootDeviceValue].bootSrc == MSAR_0_BOOT_NAND_NEW) {
		switch (satrBootSrcTable[satrBootDeviceValue].attr3) {
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

#ifdef CONFIG_CMD_BOARDCFG
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_EEPROM_CONFIG_INFO;
MV_U32 boardOptionsConfig[MV_CONFIG_TYPE_MAX_OPTION];

/*******************************************************************************
* mvBoardEepromWriteDefaultCfg - Write default configuration to EEPROM
*
* DESCRIPTION:
*       Write default configuration to EEPROM
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration
*       [0x8-0xB](32bits) - pattern
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
MV_STATUS mvBoardEepromWriteDefaultCfg(void)
{
	MV_U8 i;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_U32 defaultValue[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	/* write default board configuration, chunk of 4 bytes*/
	for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4; i++) {
		/* Swap byte's order to be from LSB to MSB:
			- When reading 32bit variables, the I2C commands display the bytes from LSB to MSB
			- mvBoardTwsiSet/Get writes/reads bytes according to their actual address location
			- in order to keep I2C output aligned with original written data
			we reverse the byte order to be from LSB to MSB before each read/write */
		defaultValue[i] = cpu_to_be32(defaultValue[i]);
		if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, 0, i * 4, (MV_U8 *)&defaultValue[i], 4) != MV_OK) {
			mvOsPrintf("%s: Error: Set default configuration to EEPROM failed\n", __func__);
			return MV_ERROR;
		}
	}

	if (mvBoardConfigTypeGet(MV_CONFIG_BOARDCFG_VALID, &configInfo) != MV_TRUE)
		DB(printf("failed reading board config valid data\n"));

	/* reset the valid counter */
	defaultValue[0] = 0;
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, 0, configInfo.offset, (MV_U8 *)&defaultValue[0], 4) != MV_OK) {
		mvOsPrintf("%s: Error: Set default configuration to EEPROM failed\n", __func__);
		return MV_ERROR;
	}


	return MV_OK;
}

/*******************************************************************************
* mvBoardEepromValidSet - reset the valid counter in the EEPROM.
*
* DESCRIPTION:
*       the valid counter is implemented by 2 bits in the EEPROM (offset 0x13
*»       bits 0 & 1). the counter is used to count 3 failures during loading,
*»       which happen because of bad configuration. when the counter reaches 3,
*»       the configuration on the EEPROM is set to default.
* INPUT:
*       None
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvBoardEepromValidSet(void)
{
	MV_U8 data = 0;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	/* disable the auto board config valid counter */
	if (mvBoardConfigTypeGet(MV_CONFIG_BOARDCFG_VALID, &configInfo) != MV_TRUE)
		DB(printf("failed reading board config valid data\n"));

	/* Read board configuration valid counter from the EEPROM */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, CALC_BYTE_OFFSET(configInfo.offset, configInfo.byteNum),
		&data, 1) != MV_OK)
		DB(printf("failed reading board config status from EEPROM\n"));

	data &= ~configInfo.mask;
	/* Write board configuration valid counter to the EEPROM */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, 0, CALC_BYTE_OFFSET(configInfo.offset, configInfo.byteNum),
		&data, 1) != MV_OK)
		DB(printf("failed writing board config status to EEPROM\n"));
}


/*******************************************************************************
* mvBoardEepromInit - Verify if the EEPROM have been initialized
*
* DESCRIPTION:
*       Verify if the EEPROM have been initialized (if not, initialize it):
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration section 1
*       [0x8-0xF](64bits) - board configuration section 2
*	[0x10-0x13](32bit) - board configuration internal data
*	[0x14-0x17](32bit) - magic pattern
*       None
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
MV_STATUS mvBoardEepromInit(void)
{
	MV_U8 data = 0;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	/* Check if EEPROM is enabled on board */
	if (mvBoardIsEepromEnabled() != MV_TRUE) {
		DB(mvOsPrintf("%s: EEPROM doesn't exists on board\n" , __func__));
		return MV_ERROR;
	}

	/* check if board auto configuration is enabled. if so, return MV_OK to
	   load configuration form the EEPROM. */
	if (mvBoardConfigTypeGet(MV_CONFIG_BOARDCFG_EN, &configInfo) != MV_TRUE)
		return MV_ERROR;

	/* Read board configuration enable bit from the EEPROM */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, CALC_BYTE_OFFSET(configInfo.offset, configInfo.byteNum),
		&data, 1) != MV_OK) {
		mvOsPrintf("%s: Error: Read from TWSI failed\n", __func__);
		return MV_ERROR;
	}

	if ((data & configInfo.mask) == configInfo.mask)
		return MV_OK;

	mvOsPrintf("%s: Warning: Board config is not enabled\n", __func__);
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardSysConfigInit
*
* DESCRIPTION: Initialize EEPROM configuration
*       1. initialize all board configuration fields
*       2. read relevant board configuration (using TWSI/EEPROM access)
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration
*       [0x8-0xB](32bits) - pattern
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_VOID mvBoardSysConfigInit(void)
{
	MV_U8 i, readValue;
	MV_U32 defaultVal[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_U32 configVal[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_BOOL readSuccess = MV_FALSE, readFlagError = MV_TRUE;

	memset(&boardOptionsConfig, 0x0, sizeof(MV_U32) * MV_CONFIG_TYPE_MAX_OPTION);

	if (mvBoardEepromInit() != MV_OK) {
		DB(mvOsPrintf("%s: Error: mvBoardEepromInit failed\n", __func__));
		readFlagError = MV_FALSE;
	}
	/* Read configuration data: 1st 8 bytes in  EEPROM, (read twice: each read of 4 bytes(32bit)) */
	for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4; i++) {
		if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, i * 4, (MV_U8 *)&configVal[i], 4) != MV_OK) {
			DB(mvOsPrintf("%s: Error: mvBoardTwsiGet from EEPROM failed\n", __func__));
			readFlagError = MV_FALSE;
		}
		/* Swap byte's order to be from LSB to MSB:
			- When reading 32bit variables, the I2C commands display the bytes from LSB to MSB
			- mvBoardTwsiSet/Get writes/reads bytes according to their actual address location
			- in order to keep I2C output aligned with original written data
				we reverse the byte order to be from LSB to MSB before each read/write */
		configVal[i] = be32_to_cpu(configVal[i]);
	}

	/* if the EEPROM init or the configurations reading fails, load default configuration. */
	if (readFlagError == MV_FALSE) {
		mvOsPrintf("%s: Warning Failed to init/read from EEPROM, set default configurations\n", __func__);
		for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4; i++)
			configVal[i] = defaultVal[i];
	}
	/* Save values Locally in configVal[] */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION; i++) {
		/* Get board configuration field information (Mask, offset, etc..) */
		if (mvBoardConfigTypeGet(i, &configInfo) != MV_TRUE) {
			mvOsPrintf("\nfailed getting board donfig for %d", i);
			continue;
		}

		readValue = (configVal[configInfo.byteNum/4] & configInfo.mask) >> configInfo.offset;
		boardOptionsConfig[configInfo.configId] =  readValue;
		readSuccess = MV_TRUE;
	}

	if (readSuccess == MV_FALSE)
		mvOsPrintf("%s: Error: Read board configuration from EEPROM failed\n", __func__);

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
MV_BOOL mvBoardIsEepromEnabled(void)
{
	MV_U8 addr = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, 0);

	if (addr == 0xFF)
		return MV_FALSE;

	DB(mvOsPrintf("%s probing for i2c chip 0x%x\n", __func__, addr));
	if (mvTwsiProbe((MV_U32)addr, mvBoardTclkGet()) == MV_TRUE)
		return MV_TRUE;  /* EEPROM enabled */
	else
		return MV_FALSE; /* EEPROM disabled */
}

/*******************************************************************************
* mvBoardConfigTypeGet
*
* DESCRIPTION:
*       Return the Config type fields information for a given Config type class.
*
* INPUT:
*       configClass - The Config type field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOARD_CONFIG_TYPE_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_BOOL mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdIndexGet(mvBoardIdGet());

	/* verify existence of requested config type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_CONFIG_TYPE_CMD_DUMP_ALL ; i++)
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
* mvBoardSysConfigGet
*
* DESCRIPTION: Read Board configuration Field
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*       if field is valid - returns requested Board configuration field value
*
*******************************************************************************/
MV_U32 mvBoardSysConfigGet(MV_CONFIG_TYPE_ID configField)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	if (configField < MV_CONFIG_TYPE_MAX_OPTION &&
		mvBoardConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		DB(mvOsPrintf("%s: Error: Requested board config is invalid for this board" \
						" (%d)\n", __func__, configField));
		return MV_ERROR;
	}

	return boardOptionsConfig[configField];
}

/*******************************************************************************
* mvBoardSysConfigSet
*
* DESCRIPTION: Write Board configuration Field to local array
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*       Write requested Board configuration field value to local array
*
*******************************************************************************/
MV_STATUS mvBoardSysConfigSet(MV_CONFIG_TYPE_ID configField, MV_U8 value)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	if (configField < MV_CONFIG_TYPE_MAX_OPTION &&
		mvBoardConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		DB(mvOsPrintf("Error: Requested board config is invalid for this board" \
						" (%d)\n", configField));
		return MV_ERROR;
	}

	boardOptionsConfig[configField] = value;

	return MV_OK;
}

/*******************************************************************************
* mvBoardEepromWrite - Write a new configuration value for a specific field
*
* DESCRIPTION:
*       Write configuration to EEPROM
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration
*       [0x8-0xB](32bits) - pattern
*
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
	MV_U32 readValue;

	if (mvBoardConfigTypeGet(configType, &configInfo) != MV_TRUE) {
		mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
		return MV_ERROR;
	}
	/* check if value is bigger then field's limit */
	if (value > configInfo.mask >> configInfo.offset) {
		mvOsPrintf("%s: Error: Requested write value is not valid (%d)\n", __func__, value);
		return MV_ERROR;
	}

	/* Read */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, 0, configInfo.byteNum, (MV_U8 *)&readValue, 4) != MV_OK) {
		mvOsPrintf("%s: Error: Read configuration from EEPROM failed\n", __func__);
		return MV_ERROR;
	}
	/* Swap byte's order to be from LSB to MSB:
		- When reading 32bit variables, the I2C commands display the bytes from LSB to MSB
		- mvBoardTwsiSet/Get writes/reads bytes according to their actual address location
		- in order to keep I2C output aligned with original written data
			we reverse the byte order to be from LSB to MSB before each read/write */
	readValue = cpu_to_be32(readValue);

	/* Modify */
	readValue &= ~configInfo.mask;
	readValue |= (value << configInfo.offset);

	readValue = be32_to_cpu(readValue);
	/* Write */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, 0, configInfo.byteNum, (MV_U8 *)&readValue, 4) != MV_OK) {
		mvOsPrintf("%s: Error: Write configuration to EEPROM failed\n", __func__);
		return MV_ERROR;
	}

	/* Update local array information */
	mvBoardSysConfigSet(configInfo.configId, value);

	return MV_OK;
}

/*******************************************************************************
* mvBoardConfigurationConflicts
*
* DESCRIPTION:
*       Check if there is any conflicts with the board configurations
*
* INPUT:
*       field = Field name of configuration
*       writeVal = option number
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK: if there is no conflicts
*       MV_ERROR: conflict in one configuration
*
*******************************************************************************/
MV_STATUS mvBoardConfigVerify(MV_CONFIG_TYPE_ID field, MV_U8 writeVal)
{
	return MV_OK;
}
#endif /* CONFIG_CMD_BOARDCFG */

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
				"MSAR configured to MSAR_0_BOOT_NAND_SPI\n",
				__func__);
		return NAND_IF_NONE;
#endif
	default:
		return board->nandIfMode;
	}
}
