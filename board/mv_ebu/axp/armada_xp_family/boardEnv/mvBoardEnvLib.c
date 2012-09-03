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

#define CODE_IN_ROM		MV_FALSE
#define CODE_IN_RAM		MV_TRUE

extern MV_BOARD_INFO *boardInfoTbl[];
#define BOARD_INFO(boardId)	boardInfoTbl[boardId - BOARD_ID_BASE]

/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);

MV_U32 tClkRate = -1;
extern MV_U8 mvDbDisableModuleDetection;

MV_U32 gSerdesZ1AMode = 0;

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
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 nandDev;
	MV_U32 norDev;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardEnvInit:Board unknown.\n");
		return;
	}

	nandDev = boardGetDevCSNum(0, BOARD_DEV_NAND_FLASH);
	if (nandDev != 0xFFFFFFFF) {
		/* Set NAND interface access parameters */
		nandDev = BOOT_CS;
		MV_REG_WRITE(DEV_BANK_PARAM_REG(nandDev), BOARD_INFO(boardId)->nandFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(nandDev), BOARD_INFO(boardId)->nandFlashWriteParams);
		MV_REG_WRITE(DEV_NAND_CTRL_REG, BOARD_INFO(boardId)->nandFlashControl);
	}

	norDev = boardGetDevCSNum(0, BOARD_DEV_NOR_FLASH);
	if (norDev != 0xFFFFFFFF) {
		/* Set NOR interface access parameters */
		MV_REG_WRITE(DEV_BANK_PARAM_REG(norDev), BOARD_INFO(boardId)->norFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(norDev), BOARD_INFO(boardId)->norFlashWriteParams);
		MV_REG_WRITE(DEV_BUS_SYNC_CTRL, 0x11);
	}

	MV_REG_WRITE(MV_RUNIT_PMU_REGS_OFFSET + 0x4, BOARD_INFO(boardId)->pmuPwrUpPolarity);
	MV_REG_WRITE(MV_RUNIT_PMU_REGS_OFFSET + 0x14, BOARD_INFO(boardId)->pmuPwrUpDelay);

	/* Set GPP Out value */
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), BOARD_INFO(boardId)->gppOutValLow);
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), BOARD_INFO(boardId)->gppOutValMid);
	MV_REG_WRITE(GPP_DATA_OUT_REG(2), BOARD_INFO(boardId)->gppOutValHigh);

	/* set GPP polarity */
	mvGppPolaritySet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValLow);
	mvGppPolaritySet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValMid);
	mvGppPolaritySet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValHigh);

	/* Set GPP Out Enable */
	mvGppTypeSet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValLow);
	mvGppTypeSet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValMid);
	mvGppTypeSet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValHigh);
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
	return (mvBoardIdGet() >> 16);
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
	return (mvBoardIdGet() & 0xFFFF);
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
MV_STATUS mvBoardNameGet(char *pNameBuff)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsSPrintf(pNameBuff, "Board unknown.\n");
		return MV_ERROR;
	}
	if (mvCtrlModelRevGet() ==  MV_6710_Z1_ID)
		mvOsSPrintf(pNameBuff, "%s", "DB-6710-Z1");
	else
		mvOsSPrintf(pNameBuff, "%s", BOARD_INFO(boardId)->boardName);


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
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	switch (boardId) {
	case DB_88F78XX0_BP_REV2_ID:
	case DB_88F78XX0_BP_ID:  /* need to update since Gunit0 can be SGMII */
		if (ethPortNum > 1)
			return MV_TRUE;
		break;
	case DB_78X60_AMC_ID:
		if (ethPortNum > 0)
			return MV_TRUE;
		break;
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
		if (ethPortNum > 0)
			return MV_TRUE;
		break;
	case DB_78X60_PCAC_ID:
	case RD_78460_GP_ID:
	case RD_78460_NAS_ID:
	case RD_78460_CUSTOMER_ID:
	case DB_78X60_PCAC_REV2_ID:
		return MV_TRUE;
		break;

	default:
		DB(mvOsPrintf("mvBoardSerdesCfgGet: Unsupported board!\n"));
		return MV_FALSE;
	}

	return MV_FALSE;
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
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum)
{
	if (mvBoardIsGMIIModuleConnected() && (ethPortNum ==0))
		return MV_TRUE;
	else
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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardPhyAddrGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardEthSmiAddr;
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
MV_32 mvBoardPhyLinkCryptPortAddrGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardPhyLinkCryptPortAddrGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].LinkCryptPortAddr;
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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardMacSpeedGet: Board unknown.\n");
		return MV_ERROR;
	}

	if (boardId == RD_78460_NAS_ID) {
		if (mvBoardIsSwitchModuleConnected())
			return BOARD_MAC_SPEED_1000M;
	}
	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardMacSpeed;
}

/*******************************************************************************
* mvBoardSpecInitGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: Return MV_TRUE and parameters in case board need spesific phy init,
*	  otherwise return MV_FALSE.
*
*
*******************************************************************************/
MV_BOOL mvBoardSpecInitGet(MV_U32 *regOff, MV_U32 *data)
{
	return MV_FALSE;
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
	if (mvBoardIdGet() == FPGA_88F78XX0_ID)
		return MV_FPGA_CLK; /* FPGA is limited to 25Mhz */

	if ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_TCLK_MASK) != 0)
		return MV_BOARD_TCLK_200MHZ;
	else
		return MV_BOARD_TCLK_250MHZ;
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
	MV_U32 idx;
	MV_U32 cpuFreqMhz, ddrFreqMhz;
	MV_CPU_ARM_CLK_RATIO clockRatioTbl[] = MV_DDR_L2_CLK_RATIO_TBL;

	if (mvBoardIdGet() == FPGA_88F78XX0_ID)
		return MV_FPGA_CLK; /* FPGA is limited to 25Mhz */

	idx = MSAR_DDR_L2_CLK_RATIO_IDX(MV_REG_READ(MPP_SAMPLE_AT_RESET(0)),
									MV_REG_READ(MPP_SAMPLE_AT_RESET(1)));

	if (clockRatioTbl[idx].vco2cpu != 0) {	/* valid ratio ? */
		cpuFreqMhz = mvCpuPclkGet() / 1000000;	/* obtain CPU freq */
		cpuFreqMhz *= clockRatioTbl[idx].vco2cpu;	/* compute VCO freq */
		ddrFreqMhz = cpuFreqMhz / clockRatioTbl[idx].vco2ddr;
		/* round up to integer MHz */
		if (((cpuFreqMhz % clockRatioTbl[idx].vco2ddr) * 10 / clockRatioTbl[idx].vco2ddr) >= 5)
			ddrFreqMhz++;

		return ddrFreqMhz * 1000000;
	} else
		return 0;
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
	return BOARD_INFO(boardId)->activeLedsNumber;
}

/*******************************************************************************
* mvBoardDebugLeg - Set the board debug Leds
*
* DESCRIPTION: turn on/off status leds.
* 	       Note: assume MPP leds are part of group 0 only.
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
	MV_U32 val[MV_GPP_MAX_GROUP] = {0};
	MV_U32 mask[MV_GPP_MAX_GROUP] = {0};
	MV_U32 digitMask;
	MV_U32 i, pinNum, gppGroup;
	MV_U32 boardId = mvBoardIdGet();

	if (BOARD_INFO(boardId)->pLedGppPin == NULL)
		return;

	hexNum &= (1 << BOARD_INFO(boardId)->activeLedsNumber) - 1;

	for (i = 0, digitMask = 1; i < BOARD_INFO(boardId)->activeLedsNumber; i++, digitMask <<= 1) {
			pinNum = BOARD_INFO(boardId)->pLedGppPin[i];
			gppGroup = pinNum / 32;
			if (hexNum & digitMask)
				val[gppGroup]  |= (1 << (pinNum - gppGroup * 32));
			mask[gppGroup] |= (1 << (pinNum - gppGroup * 32));
	}

	for (gppGroup = 0; gppGroup < MV_GPP_MAX_GROUP; gppGroup++) {
		/* If at least one bit is set in the mask, update the whole GPP group */
		if (mask[gppGroup])
			mvGppValueSet(gppGroup, mask[gppGroup], BOARD_INFO(boardId)->ledsPolarity == 0 ?
					val[gppGroup] : ~val[gppGroup]);
	}
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
	MV_U32 boardId, i;
	MV_U32 indexFound = 0;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardRTCGpioPinGet:Board unknown.\n");
		return MV_ERROR;
	}

	for (i = 0; i < BOARD_INFO(boardId)->numBoardGppInfo; i++) {
		if (BOARD_INFO(boardId)->pBoardGppInfo[i].devClass == gppClass) {
			if (indexFound == index)
				return (MV_U32) BOARD_INFO(boardId)->pBoardGppInfo[i].gppPinNum;
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
	else
	{
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
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGpioIntMaskGet:Board unknown.\n");
		return MV_ERROR;
	}

	switch (gppGrp) {
	case (0):
		return BOARD_INFO(boardId)->intsGppMaskLow;
		break;
	case (1):
		return BOARD_INFO(boardId)->intsGppMaskMid;
		break;
	case (2):
		return BOARD_INFO(boardId)->intsGppMaskHigh;
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
	MV_U32 boardId;
	MV_U32 mppMod;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
		return MV_ERROR;
	}

	mppMod = BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod;
	if (mppMod >= BOARD_INFO(boardId)->numBoardMppConfigValue)
		mppMod = 0; /* default */

	return BOARD_INFO(boardId)->pBoardMppConfigValue[mppMod].mppGroup[mppGroupNum];
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
	MV_U32 boardId, i;
	MV_U32 result = 0;
	MV_U32 gpp;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGppConfigGet: Board unknown.\n");
		return 0;
	}

	for (i = 0; i < BOARD_INFO(boardId)->numBoardGppInfo; i++) {
		if (BOARD_INFO(boardId)->pBoardGppInfo[i].devClass == BOARD_GPP_CONF) {
			gpp = BOARD_INFO(boardId)->pBoardGppInfo[i].gppPinNum;
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
	return DUAL_CHIP_SELECT_MODE;
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
	MV_U32 boardId = mvBoardIdGet();
	MV_16 index;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardTdmDevicesCountGet: Board unknown.\n");
		return 0;
	}

	index = BOARD_INFO(boardId)->boardTdmInfoIndex;
	if (index == -1)
		return 0;

	return BOARD_INFO(boardId)->numBoardTdmInfo[index];
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
	MV_U32 boardId = mvBoardIdGet();
	MV_16 index;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardTdmDevicesCountGet: Board unknown.\n");
		return -1;
	}

	index = BOARD_INFO(boardId)->boardTdmInfoIndex;
	if (index == -1)
		return 0;

	if (devId >= BOARD_INFO(boardId)->numBoardTdmInfo[index])
		return -1;

	return BOARD_INFO(boardId)->pBoardTdmInt2CsInfo[index][devId].spiCs;
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

	return;
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
		if (mvBoardIsLcdDviModuleConnected())
			return MV_FALSE;
		break;
	case 1:
		if (mvBoardIsLcdDviModuleConnected())
			return MV_FALSE;
		else if (mvBoardIsGMIIModuleConnected())
			return MV_FALSE;
		break;
	case 2:
		if ( (mvBoardIsPexModuleConnected()) || (mvBoardIsSetmModuleConnected()) )
			return MV_FALSE;
		break;
	case 3:
		break;
	default:
		break;
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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGetDeviceNumber:Board unknown.\n");
		return 0xFFFFFFFF;
	}

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++) {
		if (BOARD_INFO(boardId)->pDevCsInfo[devNum].devClass == devClass)
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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("boardGetDevEntry: Board unknown.\n");
		return NULL;
	}

	for (devIndex = START_DEV_CS; devIndex < BOARD_INFO(boardId)->numBoardDeviceIf; devIndex++) {
		if (BOARD_INFO(boardId)->pDevCsInfo[devIndex].devClass == devClass) {
			if (foundIndex == devNum)
				return &(BOARD_INFO(boardId)->pDevCsInfo[devIndex]);
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
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

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
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++) {
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddrType;
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
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++) {
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddr;
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
	MV_U32 boardId = mvBoardIdGet();

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++) {
		devWidth = mvBoardGetDeviceWidth(devNum, BOARD_DEV_NAND_FLASH);
		if (devWidth != MV_ERROR)
			return (devWidth / 8);
	}

	/* NAND wasn't found */
	return MV_ERROR;
}

MV_U32 gBoardId = -1;
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
MV_VOID mvBoardIdSet(MV_VOID)
{
	if (gBoardId == -1) {
#if defined(DB_88F78X60)
		gBoardId = DB_88F78XX0_BP_ID;
#elif defined(RD_88F78460_SERVER)
		gBoardId = RD_78460_SERVER_ID;
#elif defined(RD_78460_SERVER_REV2)
		gBoardId = RD_78460_SERVER_REV2_ID;
#elif defined(DB_78X60_PCAC)
		gBoardId = DB_78X60_PCAC_ID;
#elif defined(DB_88F78X60_REV2)
		gBoardId = DB_88F78XX0_BP_REV2_ID;
#elif defined(RD_78460_NAS)
		gBoardId = RD_78460_NAS_ID;
#elif defined(DB_78X60_AMC)
		gBoardId = DB_78X60_AMC_ID;
#elif defined(DB_78X60_PCAC_REV2)
		gBoardId = DB_78X60_PCAC_REV2_ID;
#elif defined(RD_78460_GP)
		gBoardId = RD_78460_GP_ID;
#elif defined(RD_78460_CUSTOMER)
		gBoardId = RD_78460_CUSTOMER_ID;
#else
		mvOsPrintf("mvBoardIdSet: Board ID must be defined!\n");
		while (1) {
			continue;
		}
#endif
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
	if (gBoardId == -1) {
		mvOsWarning();
		return INVALID_BAORD_ID;
	}

	return gBoardId;
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
MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
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
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);

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

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.validOffset = MV_TRUE;
	DB(mvOsPrintf("Board: Write S@R device addr %x, type %x, data %x\n",
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB1(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write S@R succeded\n"));

	return MV_OK;
}

/*******************************************************************************
* SatR Configuration functions
*******************************************************************************/
MV_U8 mvBoardFabFreqGet(MV_VOID)
{
	MV_U8 sar0;
	MV_U8 sar1;
	MV_U32 boardId = mvBoardIdGet();

	sar0 = mvBoardTwsiSatRGet(2, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	if (RD_78460_GP_ID == boardId)
		return (sar0 & 0x0f);

	sar1 = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar1)
		return MV_ERROR;

	return ( ((sar1 & 0x1) << 4) | ((sar0 & 0x1E) >> 1) );
}

/*******************************************************************************/
MV_STATUS mvBoardFabFreqSet(MV_U8 freqVal)
{
	MV_U8 sar0;
	MV_U32 boardId = mvBoardIdGet();

	sar0 = mvBoardTwsiSatRGet(2, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;
	if (RD_78460_GP_ID == boardId) {
		sar0 &= ~(0x0F);
		sar0 |= (freqVal & 0xF);
		if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar0)) {
			DB1(mvOsPrintf("Board: Write FreqOpt S@R fail\n"));
			return MV_ERROR;
		}
		return MV_OK;
	}

	sar0 &= ~(0xF << 1);
	sar0 |= (freqVal & 0xF) << 1;
	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar0)) {
		DB1(mvOsPrintf("Board: Write FreqOpt S@R fail\n"));
		return MV_ERROR;
	}

	sar0 = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	sar0 &= ~(0x1);
	sar0 |= ( (freqVal >> 4) & 0x1);
	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar0)) {
		DB1(mvOsPrintf("Board: Write FreqOpt S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write FreqOpt S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardFabFreqModeGet(MV_VOID)
{
	MV_U8 sar0;

	sar0 = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	return (sar0 & 0x1);
}

/*******************************************************************************/
MV_STATUS mvBoardFabFreqModeSet(MV_U8 freqVal)
{
	MV_U8 sar0;

	sar0 = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	sar0 &= ~(0x1);
	sar0 |= (freqVal & 0x1);
	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar0)) {
		DB1(mvOsPrintf("Board: Write FreqOpt S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write FreqOpt S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardCpuFreqModeGet(MV_VOID)
{
	MV_U8 sar0;

	sar0 = mvBoardTwsiSatRGet(2, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	return (sar0 & 0x1);
}

/*******************************************************************************/
MV_STATUS mvBoardCpuFreqModeSet(MV_U8 freqVal)
{
	MV_U8 sar0;

	sar0 = mvBoardTwsiSatRGet(2, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	sar0 &= ~(0x1);
	sar0 |= (freqVal & 0x1);
	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar0)) {
		DB1(mvOsPrintf("Board: Write FreqOpt S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write FreqOpt S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardCpuFreqGet(MV_VOID)
{
	MV_U8 sar;
	MV_U8 sarMsb;
	MV_U32 boardId = mvBoardIdGet();

	sar = mvBoardTwsiSatRGet(1, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
	if (RD_78460_GP_ID == boardId) {
		return (sar & 0x0f);
	}

	sarMsb = mvBoardTwsiSatRGet(2, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	return (  ((sarMsb & 0x1) << 3) | ((sar & 0x1C) >> 2));
}

/*******************************************************************************/
MV_STATUS mvBoardCpuFreqSet(MV_U8 freqVal)
{
	MV_U8 sar;
	MV_U32 boardId = mvBoardIdGet();

	sar = mvBoardTwsiSatRGet(1, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	if (RD_78460_GP_ID == boardId) {
		sar &= ~0x0f;
		sar |= (freqVal & 0x0f);
		if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
			DB1(mvOsPrintf("Board: Write CpuFreq S@R fail\n"));
			return MV_ERROR;
		}
	}
	else{
		sar &= ~(0x7 << 2);
		sar |= (freqVal & 0x7) << 2;
		if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
			DB1(mvOsPrintf("Board: Write CpuFreq S@R fail\n"));
			return MV_ERROR;
		}
		sar = mvBoardTwsiSatRGet(2, 0);
		if ((MV_8)MV_ERROR == (MV_8)sar)
			return MV_ERROR;
		sar &= ~(0x1);
		sar |= ( (freqVal >> 3) & 0x1);
		if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar)) {
			DB1(mvOsPrintf("Board: Write CpuFreq S@R fail\n"));
			return MV_ERROR;
		}

		sar = mvBoardTwsiSatRGet(2, 0);
		if ((MV_8)MV_ERROR == (MV_8)sar)
			return MV_ERROR;

		sar &= ~(0x1);
		sar |= ( (freqVal >> 3) & 0x1);
		if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar)) {
			DB1(mvOsPrintf("Board: Write CpuFreq S@R fail\n"));
			return MV_ERROR;
		}
	}

	DB(mvOsPrintf("Board: Write CpuFreq S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_U8 mvBoardBootDevGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(0, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
	if (RD_78460_GP_ID == mvBoardIdGet())
		sar = (sar >> 1);

	return (sar & 0x7);
}
/*******************************************************************************/
MV_STATUS mvBoardBootDevSet(MV_U8 val)
{
	MV_U8 sar;
	MV_U32 boardId = mvBoardIdGet();

	sar = mvBoardTwsiSatRGet(0, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	if (RD_78460_GP_ID == boardId) {
		sar &= ~(0x7 << 1);
		sar |= ((val & 0x7) << 1);
	}
	else {
		sar &= ~(0x7);
		sar |= (val & 0x7);
	}
	if (MV_OK != mvBoardTwsiSatRSet(0, 0, sar)) {
		DB1(mvOsPrintf("Board: Write BootDev S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write BootDev S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardBootDevWidthGet(MV_VOID)
{
	MV_U8 sar;
	MV_U32 boardId = mvBoardIdGet();

	sar = mvBoardTwsiSatRGet(0, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
	if (RD_78460_GP_ID == boardId)
		return (sar & 1);

	return (sar & 0x18) >> 3;
}
/*******************************************************************************/
MV_STATUS mvBoardBootDevWidthSet(MV_U8 val)
{
	MV_U8 sar;
	MV_U32 boardId = mvBoardIdGet();

	sar = mvBoardTwsiSatRGet(0, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
	if (RD_78460_GP_ID == boardId) {
		sar &= ~(1);
		sar |= (val & 0x1);
	}
	else {
		sar &= ~(0x3 << 3);
		sar |= ((val & 0x3) << 3);
	}

	if (MV_OK != mvBoardTwsiSatRSet(0, 0, sar)) {
		DB1(mvOsPrintf("Board: Write BootDevWidth S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write BootDevWidth S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
#ifdef MV88F78X60_Z1
MV_U8 mvBoardCpu0CoreModeGet(MV_VOID)
#else
MV_U8 mvBoardCpu0EndianessGet(MV_VOID)
#endif
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
#ifdef MV88F78X60_Z1
	return (sar & 0x18) >> 3;
#else
	return (sar & 0x08) >> 3;
#endif
}
/*******************************************************************************/
#ifdef MV88F78X60_Z1
MV_STATUS mvBoardCpu0CoreModeSet(MV_U8 val)
#else
MV_STATUS mvBoardCpu0EndianessSet(MV_U8 val)
#endif
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
#ifdef MV88F78X60_Z1
	sar &= ~(0x3 << 3);
	sar |= ((val & 0x3) << 3);
#else
	sar &= ~(0x1 << 3);
	sar |= ((val & 0x1) << 3);
#endif
	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar)) {
		DB1(mvOsPrintf("Board: Write Cpu0CoreMode S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Cpu0CoreMode S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardL2SizeGet(MV_VOID)
{
	MV_U8 sar;
	MV_U32 boardId = mvBoardIdGet();
	if (RD_78460_GP_ID == boardId) {
		sar = mvBoardTwsiSatRGet(0, 0);
		if ((MV_8)MV_ERROR == (MV_8)sar)
			return MV_ERROR;
		return ((sar & 0x10)>>4);
	}

	sar = mvBoardTwsiSatRGet(1, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	return (sar & 0x3);
}
/*******************************************************************************/
MV_STATUS mvBoardL2SizeSet(MV_U8 val)
{
	MV_U8 sar;
	if (RD_78460_GP_ID == mvBoardIdGet()) {
		sar = mvBoardTwsiSatRGet(0, 0);
		if ((MV_8)MV_ERROR == (MV_8)sar)
			return MV_ERROR;
		sar &= ~(0x1 << 4);
		sar |= ((val & 0x1) << 4);
		if (MV_OK != mvBoardTwsiSatRSet(0, 0, sar)) {
			DB1(mvOsPrintf("Board: Write L2Size S@R fail\n"));
			return MV_ERROR;
		}
		return MV_OK;
	}

	sar = mvBoardTwsiSatRGet(1, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar &= ~(0x3);
	sar |= (val & 0x3);
	if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
		DB1(mvOsPrintf("Board: Write L2Size S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write L2Size S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U8 mvBoardCpuCoresNumGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar = (sar & 0x6) >> 1;
	if (sar == 1)
		sar = 2;
	else if (sar == 2)
		sar =1;
	return sar;
}
/*******************************************************************************/
MV_STATUS mvBoardCpuCoresNumSet(MV_U8 val)
{
	MV_U8 sar;
	sar = mvBoardTwsiSatRGet(3, 0);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;
	/* MSB and LSB are swapped on DB board */
	if (val == 1)
		val = 2;
	else if (val == 2)
		val =1;

	sar &= ~(0x3 << 1);
	sar |= ((val & 0x3) << 1);
	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar)) {
		DB1(mvOsPrintf("Board: Write CpuCoreNum S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write CpuCoreNum S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvBoardConfIdSet(MV_U16 conf)
{
	if (MV_OK != mvBoardTwsiSatRSet(0, 1, conf)) {
		DB1(mvOsPrintf("Board: Write confID S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write confID S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_U16 mvBoardConfIdGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(0, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	return (sar & 0xFF);
}
/*******************************************************************************/
MV_STATUS mvBoardPexCapabilitySet(MV_U16 conf)
{
	MV_U8 sar;
	sar = mvBoardTwsiSatRGet(1, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar &= ~(0x1);
	sar |= (conf & 0x1);

	if (MV_OK != mvBoardTwsiSatRSet(1, 1, sar)) {
		DB(mvOsPrintf("Board: Write confID S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write confID S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U16 gPexCap = 0;
MV_U16 mvBoardPexCapabilityGet(MV_VOID)
{
	MV_U8 sar;
	MV_U32 boardId;

	if (gPexCap)
		return gPexCap;

	boardId = mvBoardIdGet();
	switch (boardId) {
	case DB_78X60_PCAC_ID:
	case RD_78460_NAS_ID:
	case RD_78460_GP_ID:
	case RD_78460_CUSTOMER_ID:
	case DB_78X60_AMC_ID:
	case DB_78X60_PCAC_REV2_ID:
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
		sar = 0x1; /* Gen2 */
		break;
	case DB_88F78XX0_BP_ID:
	case FPGA_88F78XX0_ID:
	case DB_88F78XX0_BP_REV2_ID:
	default:
		sar = mvBoardTwsiSatRGet(1, 1);
		break;
	}
	gPexCap = sar & 0x1;
	
	return (gPexCap);
}
/*******************************************************************************/
MV_STATUS mvBoardPexModeSet(MV_U16 conf)
{
	MV_U8 sar;
	sar = mvBoardTwsiSatRGet(1, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar &= ~(0x3 << 1);
	sar |= ((conf & 0x3) << 1);

	if (MV_OK != mvBoardTwsiSatRSet(1, 1, sar)) {
		DB(mvOsPrintf("Board: Write confID S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write confID S@R succeeded\n"));
	return MV_OK;
}
/*******************************************************************************/
MV_U16 mvBoardPexModeGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(1, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	return (sar & 0x6) >> 1;

}
/*******************************************************************************/
MV_STATUS mvBoardDramEccSet(MV_U16 conf)
{
	MV_U8 sar;
	sar = mvBoardTwsiSatRGet(3, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar &= ~(0x2);
	sar |= ((conf & 0x1) << 1);

	if (MV_OK != mvBoardTwsiSatRSet(3, 1, sar)) {
		DB(mvOsPrintf("Board: Write confID S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write confID S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_U16 mvBoardDramEccGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 1);
	return ((sar & 0x2) >> 1);
}

/*******************************************************************************/
MV_STATUS mvBoardDramBusWidthSet(MV_U16 conf)
{
	MV_U8 sar;
	sar = mvBoardTwsiSatRGet(3, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar)
		return MV_ERROR;

	sar &= ~(0x1);
	sar |= (conf & 0x1);

	if (MV_OK != mvBoardTwsiSatRSet(3, 1, sar)) {
		DB(mvOsPrintf("Board: Write confID S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write confID S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************/
MV_U16 mvBoardDramBusWidthGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 1);
	return (sar & 0x1);
}

/*******************************************************************************/
MV_U8 mvBoardAltFabFreqGet(MV_VOID)
{
	MV_U8 sar0;

	sar0 = mvBoardTwsiSatRGet(2, 1);
	if ((MV_8)MV_ERROR == (MV_8)sar0)
		return MV_ERROR;

	return (sar0 & 0x1F);
}
/*******************************************************************************/
MV_STATUS mvBoardAltFabFreqSet(MV_U8 freqVal)
{
	if (MV_OK != mvBoardTwsiSatRSet(2, 1, freqVal)) {
		DB1(mvOsPrintf("Board: Write Alt FreqOpt S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write Alt FreqOpt S@R succeeded\n"));
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
	MV_U8 regVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 boardId = mvBoardIdGet();

	/* Perform scan only for DB board */
	if ( (boardId == DB_88F78XX0_BP_ID) || (boardId == DB_88F78XX0_BP_REV2_ID) ) {
		twsiSlave.slaveAddr.address = MV_BOARD_MPP_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_MPP_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
			switch (regVal) {
			case MV_BOARD_LCD_DVI_MODULE_ID:
				BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod = MV_BOARD_LCD_DVI;
				return MV_OK;

			case MV_BOARD_MII_GMII_MODULE_ID:
				BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod = MV_BOARD_MII_GMII;
			    BOARD_INFO(boardId)->pBoardMacInfo[0].boardEthSmiAddr = 0x8;

				return MV_OK;

			case MV_BOARD_TDM_MODULE_ID:
				/*
				   TODO - how to distinguish between SLIC types?
				   BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_3215;
				   BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_880;
				   BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_792;
				   return MV_OK;
				 */
				BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_OTHER;
				BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod = MV_BOARD_TDM_32CH;
				return MV_OK;

			default:
				BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod = MV_BOARD_OTHER;
				DB1(mvOsPrintf("mvBoardMppModulesScan: Unsupported module!\n"));
				break;
			}
		}
	}

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
	MV_U8 regVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U32 boardId = mvBoardIdGet();

	/* Perform scan only for DB board */
	if ( (boardId == DB_88F78XX0_BP_ID) || (boardId == DB_88F78XX0_BP_REV2_ID) ) {
		/* reset modules flags */
		BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_NONE;

        /* TWSI init */
        slave.type = ADDR7_BIT;
        slave.address = 0;
        mvTwsiInit(0, TWSI_SPEED , mvBoardTclkGet(), &slave, 0);

 		/* SERDES module (PEX module and SETM module are supported now) */
		twsiSlave.slaveAddr.address = MV_BOARD_PEX_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_PEX_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
			if (regVal == MV_BOARD_PEX_MODULE_ID) {
				DB(mvOsPrintf("mvBoardOtherModulesScan: " "PEX module DETECTED!\n"));
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_PEX;
			} else {
				DB(mvOsPrintf("mvBoardOtherModulesScan: " "Unknown ID @ PEX module address!\n"));
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_UNKNOWN;
			}
		}

		/* SERDES module (PEX module and SETM module are supported now) */
		twsiSlave.slaveAddr.address = MV_BOARD_SETM_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_SETM_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
			if (regVal == MV_BOARD_SETM_MODULE_ID) {
				DB(mvOsPrintf("mvBoardOtherModulesScan: " "SETM module DETECTED!\n"));
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_SETM;
			} else {
				DB(mvOsPrintf("mvBoardOtherModulesScan: " "Unknown ID @ PEX module address!\n"));
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_UNKNOWN;
			}
		}

		/* LVDS (LCD) module */
		twsiSlave.slaveAddr.address = MV_BOARD_LVDS_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_LVDS_MODULE_ADDR_TYPE;
		if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
			if (regVal == MV_BOARD_LVDS_MODULE_ID) {
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_LVDS;
				mvCpuIfLvdsPadsEnable(MV_TRUE);
			} else {
				DB(mvOsPrintf("mvBoardOtherModulesScan: " "Unknown ID @ LVDS module address!\n"));
				BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_UNKNOWN;
			}
		}
	} else if (boardId == RD_78460_NAS_ID) {
		if ((MV_REG_READ(GPP_DATA_IN_REG(2)) & MV_GPP66) == 0x0) {
			DB(mvOsPrintf("mvBoardOtherModulesScan: SWITCH module DETECTED!\n"));
			BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod |= MV_BOARD_SWITCH;
		}
	}

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
	MV_U32 boardId = mvBoardIdGet();

	if ( (boardId != DB_88F78XX0_BP_ID) && (boardId != DB_88F78XX0_BP_REV2_ID) )
		DB(mvOsPrintf("mvBoardIsPexModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod & MV_BOARD_PEX)
		return MV_TRUE;

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
	MV_U32 boardId = mvBoardIdGet();

	if ( (boardId != DB_88F78XX0_BP_ID) && (boardId != DB_88F78XX0_BP_REV2_ID) )
		DB(mvOsPrintf("mvBoardIsSetmModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod & MV_BOARD_SETM)
		return MV_TRUE;
	return MV_FALSE;
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
MV_BOOL mvBoardIsSwitchModuleConnected(void)
{
	MV_U32 boardId = mvBoardIdGet();

	if (boardId != RD_78460_NAS_ID)
		DB(mvOsPrintf("mvBoardIsSwitchModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod & MV_BOARD_SWITCH)
		return MV_TRUE;

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
	MV_U32 boardId = mvBoardIdGet();

	if ( (boardId != DB_88F78XX0_BP_ID) && (boardId != DB_88F78XX0_BP_REV2_ID) )
		DB(mvOsPrintf("mvBoardIsLvdsModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardOtherMod & MV_BOARD_LVDS)
		return MV_TRUE;

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
	MV_U32 boardId = mvBoardIdGet();

	if ( (boardId != DB_88F78XX0_BP_ID) && (boardId != DB_88F78XX0_BP_REV2_ID) )
		DB(mvOsPrintf("mvBoardIsLcdDviModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod == MV_BOARD_LCD_DVI)
		return MV_TRUE;

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
	MV_U32 boardId = mvBoardIdGet();

	if ( (boardId != DB_88F78XX0_BP_ID) && (boardId != DB_88F78XX0_BP_REV2_ID) )
		DB(mvOsPrintf("mvBoardIsGMIIModuleConnected: Unsupported board!\n"));
	else if (BOARD_INFO(boardId)->pBoardModTypeValue->boardMppMod == MV_BOARD_MII_GMII)
		return MV_TRUE;

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
	static MV_U8 currChNum = 0xFF;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	if (currChNum == muxChNum)
		return MV_OK;

	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_TWSI_MUX, 0);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_TWSI_MUX, 0);
	twsiSlave.validOffset = 0;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	muxChNum += 4;
	return mvTwsiWrite(0, &twsiSlave, &muxChNum, 1);
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
MV_STATUS mvBoardTwsiReadByteThruMux(MV_U8 muxChNum, MV_U8 chNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *data)
{
	MV_STATUS res;

	/* Set Mux channel */
	res = mvBoardTwsiMuxChannelSet(muxChNum);
	if (res == MV_OK)
		res = mvTwsiRead(chNum, pTwsiSlave, data, 1);

	return res;
}

/*******************************************************************************
* mvBoardSerdesZ1ASupport - Support Z1A silicon serdes configuration
*
* DESCRIPTION:
* 	Default is fixed silicone Z1B. This routine should be called before
*       calling the serdes initialization in ctrlEnvinit
*
* INPUT:
*	None.
* OUTPUT:
*       None.
* RETURN:
*       SERDES configuration structure or NULL on error
*
*******************************************************************************/
MV_VOID mvBoardSerdesZ1ASupport(void)
{
	gSerdesZ1AMode = 1;
}
/*******************************************************************************
* mvBoardIsSerdesConfigurationEnabled
*
* DESCRIPTION:
*       Check if Serdes configuration is enabled on this board.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_BOOL mvBoardIsSerdesConfigurationEnabled(void)
{

	 MV_U32 boardId = mvBoardIdGet();

	 if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
                mvOsPrintf("mvBoardIsSerdesConfigurationEnabled:Board unknown.\n");
                return MV_ERROR;
        }
	return (BOARD_INFO(boardId)->pBoardSerdesConfigValue->enableSerdesConfiguration);
}


/*******************************************************************************
* mvBoardSerdesConfigurationEnableSet
*
* DESCRIPTION:
*	Check if Serdes configuration is enabled on this board.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS  mvBoardSerdesConfigurationEnableSet(MV_BOOL enableSerdesConfiguration)
{

	MV_U32 boardId = mvBoardIdGet();
	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardIsSerdesConfigurationEnabled:Board unknown.\n");
		return MV_ERROR;
        }
	BOARD_INFO(boardId)->pBoardSerdesConfigValue->enableSerdesConfiguration=enableSerdesConfiguration;
	return MV_OK;
}


/*******************************************************************************
* mvBoardSledCpuNumGet - Get board SERDES configuration
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       SERDES configuration structure or NULL on error
*
*******************************************************************************/
MV_U32 mvBoardSledCpuNumGet(MV_VOID)
{
	MV_U32 reg;

	reg = MV_REG_READ(GPP_DATA_IN_REG(0));

	return ((reg & 0xF0000) >> 16);
}

/*******************************************************************************
* mvBoardMppGet - Get board SERDES configuration
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       SERDES configuration structure or NULL on error
*
*******************************************************************************/
MV_SERDES_CFG *mvBoardSerdesCfgGet(void)
{
	MV_U32 boardId;
	MV_U32 serdesCfg = 0; /* default */
	int pex0 = 1;
	int pex1 = 1;

	MV_BOOL moduleConnected = mvBoardIsPexModuleConnected();
	MV_U16 pexMode = mvBoardPexModeGet();

	switch (pexMode) {
	case 0:
		pex0 = 1;
		pex1 = 1;
		break;
	case 1:
		pex0 = 4;
		pex1 = 1;
		break;
	case 2:
		pex0 = 1;
		pex1 = 4;
		break;
	case 3:
		pex0 = 4;
		pex1 = 4;
		break;
	}

	boardId = mvBoardIdGet();

	switch (boardId) {
	case DB_88F78XX0_BP_ID:
		if (moduleConnected)
			serdesCfg = 1;
		/* If backword compatability for Z1A is needed */
		if (gSerdesZ1AMode)
			serdesCfg += 2;
		break;
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
		if (mvBoardSledCpuNumGet() > 0)
			serdesCfg = 1;
		break;
	case DB_88F78XX0_BP_REV2_ID:
		if ( (!moduleConnected) && (pex0 == 1)) /*if the module is not connected the PEX1 mode is not relevant*/
			serdesCfg = 0;
		if ( (moduleConnected) && (pex0 == 1) && (pex1 == 1))
			serdesCfg = 1;
		if ( (!moduleConnected) && (pex0 == 4))  /*if the module is not connected the PEX1 mode is not relevant*/
			serdesCfg = 2;
		if ( (moduleConnected) && (pex0 == 4) && (pex1 == 1))
			serdesCfg = 3;
		if ( (moduleConnected) && (pex0 == 1) && (pex1 == 4))
			serdesCfg = 4;
		if ( (moduleConnected) && (pex0 == 4) && (pex1 == 4))
			serdesCfg = 5;
		break;
	case RD_78460_NAS_ID:
		if (mvBoardIsSwitchModuleConnected())
			serdesCfg = 1;
		break;
	}

	return &BOARD_INFO(boardId)->pBoardSerdesConfigValue[serdesCfg];
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
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	switch (boardId) {
	case DB_88F78XX0_BP_ID:
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
	case DB_78X60_PCAC_ID:
	case FPGA_88F78XX0_ID:
	case DB_88F78XX0_BP_REV2_ID:
	case RD_78460_NAS_ID:
	case RD_78460_GP_ID:
	case RD_78460_CUSTOMER_ID:
	case DB_78X60_AMC_ID:
	case DB_78X60_PCAC_REV2_ID:
		return &BOARD_INFO(boardId)->boardPexInfo;
		break;
	default:
		DB(mvOsPrintf("mvBoardSerdesCfgGet: Unsupported board!\n"));
		return NULL;
	}
}
