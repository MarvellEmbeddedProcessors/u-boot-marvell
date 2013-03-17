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

	MV_REG_WRITE(MV_RUNIT_PMU_REGS_OFFSET + 0x4, board->pmuPwrUpPolarity);
	MV_REG_WRITE(MV_RUNIT_PMU_REGS_OFFSET + 0x14, board->pmuPwrUpDelay);

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
MV_STATUS mvBoardNameGet(char *pNameBuff)
{
	mvOsSPrintf(pNameBuff, "%s", board->boardName);
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
	if (ethPortNum >= board->numBoardMacInfo)
		return MV_ERROR;

#if defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return 8;
#endif

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
* mvBoardQuadPhyAddr0Get - Get the phy address
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
MV_32 mvBoardQuadPhyAddr0Get(MV_U32 ethPortNum)
{
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
	return board->pBoardMacInfo[ethPortNum].boardEthSmiAddr0;
#else
	return 0;
#endif

}

/*******************************************************************************
* mvBoardPhyLinkCryptPortAddrGet
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
	if (ethPortNum >= board->numBoardMacInfo)
		return MV_ERROR;

	return board->pBoardMacInfo[ethPortNum].LinkCryptPortAddr;
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
*       This routine extract the controller core clock.
*
* INPUT:
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
		DB(mvOsPrintf("%s: Error : Board: Read from S@R fail\n", __func__));
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
	/* empty */
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
	return board->pBoardMppConfigValue[0].mppGroup[mppGroupNum];
}

MV_32 mvBoardMppTypeGet(MV_U32 mppGroupNum)
{
	return (MV_U32)board->pBoardMppGroupValue[mppGroupNum];
}

MV_VOID mvBoardMppTypeSet(MV_U32 mppGroupNum, MV_U32 groupType)
{
	board->pBoardMppGroupValue[mppGroupNum] = groupType;
}

/*******************************************************************************
* mvBoardConfigInit - Build board configuration : Eth-Complex & MPP types
*
* DESCRIPTION:
*   read S@R & board configuration From: satrOptionsConfig[] & boardOptionsConfig[],
*   (initialized  by mvCtrlSatrInit ) and :
*   1. build etherntComplex configuration
*   2. decide the relevant MPP group types
*
* INPUT:  None.
*
* OUTPUT:  None.
*
* RETURN:
*       if initialized correct return MV_OK, else MV_ERROR
*
*******************************************************************************/
MV_VOID mvBoardConfigInit(void)
{
	MV_U32 ethSataComplexOptions = 0x0;
	MV_ETH_COMPLEX_TOPOLOGY mac0con, mac1con;
	MV_BOARD_BOOT_SRC bootDev;
	MV_TDM_UNIT_TYPE tdmDev;
	MV_BOOL SW_SMI;

	/* Ethernet Complex initialization : */
	if ( (mac0con = mvBoardMac0ConfigGet()) != MV_ERROR)
		ethSataComplexOptions |= mac0con;
	if ( (mac1con = mvBoardMac1ConfigGet()) != MV_ERROR)
		ethSataComplexOptions |= mac1con;

	if (mac0con == 0x0) /* 00 - MAC0 connected to Switch P6 */
		ethSataComplexOptions |= MV_ETH_COMPLEX_SW_P6_RGMII0;

	if (mac1con == 0x2) /* 10  MAC1 connected to Switch P4 */
		ethSataComplexOptions |= MV_ETH_COMPLEX_SW_P4_RGMII1;

	if (mvCtrlConfigGet(MV_CONFIG_PON_SERDES) == 0x0)  /* 0 - PON SerDes connected to PON MAC */
		ethSataComplexOptions |= MV_ETH_COMPLEX_P2P_MAC_PON_ETH_SERDES;

	board->pBoardModTypeValue->ethSataComplexOpt = ethSataComplexOptions;

	/* MPP Groups initialization : */
	/* Group 0-1 - Boot device (or if SPI1 boot : Groups 3-4) */
	bootDev = mvBoardBootDeviceGroupSet(mvCtrlSatRRead(MV_SATR_BOOT_DEVICE));

	/* Group 2 - TDM unit */
	tdmDev = mvCtrlTdmUnitTypeGet();
	switch (tdmDev) {
	case SLIC_LANTIQ_ID:
		mvBoardMppTypeSet(2, SLIC_LANTIQ_UNIT);
		break;
	case SLIC_SILABS_ID:
		mvBoardMppTypeSet(2, SLIC_SILABS_UNIT);
		break;
	case SLIC_ZARLINK_ID:
		mvBoardMppTypeSet(2, SLIC_ZARLINK_UNIT);
		break;
	case SLIC_EXTERNAL_ID:
		mvBoardMppTypeSet(2, SLIC_EXTERNAL_UNIT);
		break;
	}

	/* Groups 3-4  - (only if not Booting from SPI1)*/
	if (bootDev != MSAR_0_BOOT_SPI1_FLASH) {
		if (mac1con == 0x0) /* 00 - MAC1 connected to RGMII-1 */
			mvBoardMppTypeSet(3, GE1_UNIT);
		else
			mvBoardMppTypeSet(3, SDIO_UNIT);

		SW_SMI = MV_TRUE; // test if SW or CPU SMI control (omriii : how to decide ?)
		if (mvCtrlIsLantiqTDM())
			mvBoardMppTypeSet(4, (SW_SMI ? GE1_SW_SMI_CTRL_TDM_LQ_UNIT : GE1_CPU_SMI_CTRL_TDM_LQ_UNIT));
		else    /*REF_CLK_OUT*/
			mvBoardMppTypeSet(4, (SW_SMI ? GE1_SW_SMI_CTRL_REF_CLK_OUT : GE1_CPU_SMI_CTRL_REF_CLK_OUT));
	}

	/* Groups 5-6-7  - */
	if (mac0con == 0x2) { /* 10 - MAC0 connected to RGMII-0  */
		/* omriii: what scenario leads to enable PON_CLK_OUT insted of PON_TX_FAULT?? */
		mvBoardMppTypeSet(5, GE0_UNIT_PON_TX_FAULT);
		mvBoardMppTypeSet(6, GE0_UNIT);
		mvBoardMppTypeSet(7, GE0_UNIT_LED_MATRIX);  /* omriii :when to use GE0_UNIT_UA1_PTP */
	}else if (mac1con == 0x2) { /* 10  MAC1 connected to Switch P4 */
		mvBoardMppTypeSet(5, SWITCH_P4_PON_TX_FAULT);
		mvBoardMppTypeSet(6, SWITCH_P4);
		mvBoardMppTypeSet(7, SWITCH_P4_LED_MATRIX); /* omriii :when to use SWITCH_P4_UA1_PTP */
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
* INPUT:  sarBootDevice - BOOT_DEVICE value from S@R.
*
* OUTPUT:  None.
*
* RETURN:
*       the selected MV_BOARD_BOOT_SRC
*
*******************************************************************************/
MV_BOARD_BOOT_SRC mvBoardBootDeviceGroupSet(MV_U32 sarBootDeviceValue)
{
	MV_U32 groupType;
	MV_BOOL SW_SMI;
	MV_BOARD_BOOT_SRC bootSrc= mvBoardBootDeviceGet(sarBootDeviceValue);

	switch (bootSrc) {
	case MSAR_0_BOOT_NAND_NEW:
		mvBoardMppTypeSet(0, NAND_BOOT_V2);
		mvBoardMppTypeSet(1, NAND_BOOT_V2);
		break;
	case MSAR_0_BOOT_SPI_FLASH:
		groupType = ((mvCtrlConfigGet(MV_CONFIG_DEVICE_BUS_MODULE) == 0x2) ? SPI0_BOOT_SPDIF_AUDIO : SPI0_BOOT);
		mvBoardMppTypeSet(0, groupType);
		mvBoardMppTypeSet(1, groupType);
	case MSAR_0_BOOT_SPI1_FLASH:	/* MSAR_0_SPI1 - update Groups 3-4 */
		mvBoardMppTypeSet(3, SDIO_SPI1_UNIT);
		SW_SMI = MV_TRUE;       // test if SW or CPU SMI control (omriii : how to decide ?)
		if (mvCtrlIsLantiqTDM())
			mvBoardMppTypeSet(4, (SW_SMI ? SPI1_SW_SMI_CTRL_TDM_LQ_UNIT : SPI1_CPU_SMI_CTRL_TDM_LQ_UNIT));
		else    /*REF_CLK_OUT*/
			mvBoardMppTypeSet(4, (SW_SMI ? SPI1_SW_SMI_CTRL_REF_CLK_OUT : SPI1_CPU_SMI_CTRL_REF_CLK_OUT));
		// omriii : if nand/spi0 not for BOOT, enable Audio-SPDIF by default?
		mvBoardMppTypeSet(0, SPI0_BOOT_SPDIF_AUDIO);
		mvBoardMppTypeSet(1, SPI0_BOOT_SPDIF_AUDIO);
		break;
/* omriii : what to do with the next Boot devices  ?
        case MSAR_0_BOOT_NOR_FLASH:
                break;
        case MSAR_0_BOOT_PROMPT:
                break;
        case MSAR_0_BOOT_UART:
                break;
        case MSAR_0_BOOT_SATA:
                break;
        case MSAR_0_BOOT_PEX:
                break;
 */
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
* INPUT:  sarBootDevice - BOOT_DEVICE value from S@R.
*
* OUTPUT:  None.
*
* RETURN:
*       the selected MV_BOARD_BOOT_SRC
*
*******************************************************************************/
MV_BOARD_BOOT_SRC mvBoardBootDeviceGet(MV_U32 sarBootDeviceValue)
{
	MV_SAR_BOOT_TABLE sarTable[] = MV_SAR_TABLE_VAL;
	MV_SAR_BOOT_TABLE sarBootEntry = sarTable[sarBootDeviceValue];
	if (sarBootEntry.bootSrc!=MSAR_0_BOOT_SPI_FLASH)
		return sarBootEntry.bootSrc;
	else /* if boot source is SPI ,verify which CS (0/1) */
		if (mvBoardBootAttrGet(sarBootDeviceValue,1) == MSAR_0_SPI0)
			return MSAR_0_BOOT_SPI_FLASH;
		else return MSAR_0_BOOT_SPI1_FLASH;
}

/*******************************************************************************
* mvBoardBootAttrGet -  Get the selected S@R Boot device attributes[1/2/3]
*
* DESCRIPTION:
*   read board BOOT configuration and return attributes accordingly
*
* INPUT:  sarBootDevice - BOOT_DEVICE value from S@R.*
* 	  attrNum - attribute number [1/2/3]
* OUTPUT:  None.
*
* RETURN:
*       the selected attribute value
*
*******************************************************************************/
MV_U32 mvBoardBootAttrGet(MV_U32 sarBootDeviceValue, MV_U8 attrNum)
{
	MV_SAR_BOOT_TABLE sarTable[] = MV_SAR_TABLE_VAL;
	MV_SAR_BOOT_TABLE sarBootEntry = sarTable[sarBootDeviceValue];
	switch (attrNum) {
	case 1:
		return sarBootEntry.attr1;
		break;
	case 2:
		return sarBootEntry.attr2;
		break;
	case 3:
		return sarBootEntry.attr3;
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
	switch (mvCtrlConfigGet(MV_CONFIG_MAC0)) {
	case 0x0:
		return MV_ETH_COMPLEX_GE_MAC0_SW_P6;
		break;
	case 0x1:
		return MV_ETH_COMPLEX_GE_MAC0_QUAD_PHY_P0;
		break;
	case 0x2:
		return MV_ETH_COMPLEX_GE_MAC0_RGMII0;
		break;
	case 0x3:
		return mvBoardLaneSGMIIGet();
		break;
	default:
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
	if (mvCtrlConfigGet(MV_CONFIG_PON_SERDES) == 0x1)
		return MV_ETH_COMPLEX_GE_MAC1_PON_ETH_SERDES;
	/* else Scan MAC1 config to decide its connection */
	switch (mvCtrlConfigGet(MV_CONFIG_MAC1)) {
	case 0x0:
		return MV_ETH_COMPLEX_GE_MAC1_RGMII1;
		break;
	case 0x1:
		return MV_ETH_COMPLEX_GE_MAC1_SW_P4;
		break;
	case 0x2:
		return MV_ETH_COMPLEX_GE_MAC1_QUAD_PHY_P3;
		break;
	default:
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
	if (mvCtrlConfigGet(MV_CONFIG_LANE1) == 0x1)
		return MV_ETH_COMPLEX_GE_MAC0_COMPHY_1;
	else if (mvCtrlConfigGet(MV_CONFIG_LANE2) == 0x0)
		return MV_ETH_COMPLEX_GE_MAC0_COMPHY_2;
	else if (mvCtrlConfigGet(MV_CONFIG_LANE3) == 0x1)
		return MV_ETH_COMPLEX_GE_MAC0_COMPHY_3;
	else return MV_ERROR;
}

MV_VOID mvBoardConfigWrite(void)
{
	MV_U32 mppGroup, mppType, mppVal;
	MV_U32 mppGroups[][10] = MPP_GROUP_TYPES;

	for (mppGroup = 0; mppGroup < MV_MPP_MAX_GROUP; mppGroup++) {
		mppType = mvBoardMppTypeGet(mppGroup);
		mppVal = mppGroups[mppGroup][mppType];
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
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
	MV_U32 indexFound = 0;

	for (i = 0; i < board->numBoardTwsiDev; i++) {
		if (board->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return board->pBoardTwsiDev[i].twsiDevAddr;
			else
				indexFound++;
		}
	}

	return 0xFF;
}

/*******************************************************************************
* mvBoardSarInfoGet
*
* DESCRIPTION:
*	Return the SAR fields information for a given SAR class.
*
* INPUT:
*	sarClass - The SAR field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_BOARD_SAR_INFO struct with mask, offset and register number.
*
*******************************************************************************/
MV_STATUS mvBoardSarInfoGet(MV_SATR_TYPE_ID sarClass, MV_BOARD_SAR_INFO *sarInfo)
{
	int i;

	for (i = 0; i < board->numBoardSarInfo; i++)
		if (board->pBoardSarInfo[i].sarid == sarClass) {
			*sarInfo = board->pBoardSarInfo[i];
			return MV_OK;
		}
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

	for (i = 0; i < board->numBoardConfigTypes; i++)
		if (board->pBoardConfigTypes[i].configid == configClass) {
			*configInfo = board->pBoardConfigTypes[i];
			return MV_OK;
		}
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

	/* NAND wasn't found */
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
	if (boardId >= MV_MAX_BOARD_ID) {
		mvOsPrintf("%s: Error: wrong boardId (%d)\n", __func__, boardId);
	}

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
	MV_U32 boardId, tempVal;

#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	boardId = MV_BOARD_ID_AVANTA_LP_FPGA;
#else
	tempVal = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	boardId = ((tempVal & (0xF0)) >> 4);
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
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write S@R succeded\n"));

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
	return 0;
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
	return 0;
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
	if (board->pBoardSerdesConfigValue)
		return board->pBoardSerdesConfigValue->enableSerdesConfiguration;
	else
		return MV_FALSE;
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
MV_STATUS mvBoardSerdesConfigurationEnableSet(MV_BOOL enableSerdesConfiguration)
{
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardSerdesCfgGet
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
	return NULL;
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
* mvBoardModuleAutoDetectEnabled
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
MV_BOOL mvBoardModuleAutoDetectEnabled(void)
{
	return board->moduleAutoDetect;
}
