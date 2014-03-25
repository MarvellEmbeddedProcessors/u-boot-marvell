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

#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "mvSysEthConfig.h"

#include "pex/mvPex.h"
#include "pex/mvPexRegs.h"

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
#define DB(x)   x
#else
#define DB(x)
#endif

/*
 * Control Environment internal data structure
 * Note: it should be initialized dynamically only once.
 */
#define MV_INVALID_CTRL_REV     0xff

typedef struct _ctrlEnvInfo {
	MV_U16 ctrlModel;
	MV_U8 ctrlRev;
} CTRL_ENV_INFO;

CTRL_ENV_INFO ctrlEnvInfo = {};

MV_U32 satrOptionsConfig[MV_SATR_READ_MAX_OPTION];
MV_U32 boardOptionsConfig[MV_CONFIG_TYPE_MAX_OPTION];

MV_BOARD_SATR_INFO boardSatrInfo[] = MV_SAR_INFO;

/*******************************************************************************
* mvCtrlGetCpuNum
*
* DESCRIPTION: Read number of cores enabled by SatR
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*        Number of cores enabled
*
*******************************************************************************/
MV_U32 mvCtrlGetCpuNum(MV_VOID)
{
	MV_U32 cpu1Enabled;

	cpu1Enabled = mvCtrlSatRRead(MV_SATR_CPU1_ENABLE);
	if (cpu1Enabled == MV_ERROR) {
		DB(mvOsPrintf("%s: Error: MV_SATR_CPU1_ENABLE is not active for board (using default)\n", __func__));
		return 0;
	} else
		return cpu1Enabled;
}

/*******************************************************************************
* mvCtrlIsValidSatR
*
* DESCRIPTION: check frequency modes table and verify current mode is supported
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*        MV_TRUE - if current cpu/ddr/l2 frequency mode is supported for board
*
*******************************************************************************/
MV_BOOL mvCtrlIsValidSatR(MV_VOID)
{
	MV_FREQ_MODE cpuFreqMode;
	MV_U32 cpuFreqSatRMode =  mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ);

	/* Verify SatR Mode exists in user frequency modes table */
	if (mvCtrlFreqModeGet(cpuFreqSatRMode, &cpuFreqMode) == MV_OK)
		return MV_TRUE;
	else
		return MV_FALSE;
}
/*******************************************************************************
* mvCtrlFreqModeGet
*
* DESCRIPTION: scan frequency modes table (CPU/L2/DDR) and return requested mode
*
* INPUT: freqModeSatRValue - Sample at reset value (represent a frequency mode)
*
* OUTPUT: MV_FREQ_MODE which describes the frequency mode (CPU/L2/DDR)
*
* RETURN:
*        MV_OK if frequency mode is supported , else MV_ERROR
*
*******************************************************************************/
MV_STATUS mvCtrlFreqModeGet(MV_U32 freqModeSatRValue, MV_FREQ_MODE *freqMode)
{
	MV_FREQ_MODE freqTable[] = MV_USER_SAR_FREQ_MODES;
	MV_U32 i, maxFreqModes = mvBoardFreqModesNumGet();

	for (i = 0; i < maxFreqModes; i++) {
		if (freqModeSatRValue == freqTable[i].id) {
			*freqMode = freqTable[i];
			return MV_OK;
		}
	}

	return MV_ERROR;
}

#ifdef MV_INCLUDE_PEX
MV_STATUS mvCtrlUpdatePexId(MV_VOID)
{
	return MV_ERROR;
}

#endif

#define MV_6720_INDEX		0
#define MV_67xx_INDEX_MAX	1

static MV_U32 mvCtrlDevIdIndexGet(MV_U32 devId)
{
	return MV_6720_INDEX;
}

static MV_VOID mvCtrlPexConfig(MV_VOID)
{
	MV_U8 pexUnit;
	MV_U32 pexIfNum = mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);

	MV_BOARD_PEX_INFO *boardPexInfo = mvBoardPexInfoGet();

	memset(boardPexInfo, 0, sizeof(MV_BOARD_PEX_INFO));

	for (pexUnit = 0; pexUnit < pexIfNum; pexUnit++) {
		boardPexInfo->pexUnitCfg[pexUnit] = PEX_BUS_MODE_X1;
		boardPexInfo->pexMapping[pexUnit] = pexUnit;
	}

	boardPexInfo->boardPexIfNum = pexIfNum;
}


MV_UNIT_ID mvCtrlSocUnitNums[MAX_UNITS_ID][MV_67xx_INDEX_MAX] = {
/*                          6720 */
/* DRAM_UNIT_ID         */ { 1, },
/* PEX_UNIT_ID          */ { 2, },
/* ETH_GIG_UNIT_ID      */ { 2, },
/* USB_UNIT_ID          */ { 1, },
/* USB3_UNIT_ID          */ { 1, },
/* IDMA_UNIT_ID         */ { 0, },
/* XOR_UNIT_ID          */ { 2, },
/* SATA_UNIT_ID         */ { 2, },
/* TDM_32CH_UNIT_ID     */ { 1, },
/* UART_UNIT_ID         */ { 2, },
/* CESA_UNIT_ID         */ { 1, },
/* SPI_UNIT_ID          */ { 2, },
/* AUDIO_UNIT_ID        */ { 1, },
/* SDIO_UNIT_ID         */ { 1, },
/* TS_UNIT_ID           */ { 0, },
/* XPON_UNIT_ID         */ { 1, },
/* BM_UNIT_ID           */ { 1, },
/* PNC_UNIT_ID          */ { 1, },
/* I2C_UNIT_ID          */ { 2, },
};

MV_U32 mvCtrlSocUnitInfoNumGet(MV_UNIT_ID unit)
{
	MV_U32 devId, devIdIndex;

	if (unit >= MAX_UNITS_ID) {
		mvOsPrintf("%s: Error: Wrong unit type (%u)\n", __func__, unit);
		return 0;
	}

	devId = mvCtrlModelGet();
	devIdIndex = mvCtrlDevIdIndexGet(devId);
	return mvCtrlSocUnitNums[unit][devIdIndex];
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
	MV_U32 i, gppMask;

	/* Set I2C MPP's(MPP Group 1), before reading board configuration, using TWSI read */
	MV_REG_WRITE(mvCtrlMppRegGet(1), GROUP1_DEFAULT_MPP_SPI_I2C);

	mvCtrlSatrInit();

	/* If set to Auto detect, read board config info, update MPP group types*/
	if (mvBoardConfigAutoDetectEnabled()) {
		mvBoardInfoUpdate();
	}

	mvCtrlPexConfig();

	/* write MPP's config and Board general config */
	mvBoardConfigWrite();

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
		mvGppTypeSet(i, gppMask, (MV_GPP_IN & gppMask));
		mvGppPolaritySet(i, gppMask, (MV_GPP_IN_INVERT & gppMask));
	}

	/*Disabled for kernel compilation*/
	/*mvEthComplexInit(mvBoardEthComplexConfigGet());*/

	/*
	 * Enable NAND Flash PUP (Pack-Unpack)
	 * HW machanism to accelerate transactions (controlled by SoC register)
	 */
	MV_REG_BIT_SET(PUP_EN_REG, BIT4);

#ifdef MV_NOR_BOOT
	/*Enable PUP bit for NOR*/
	MV_REG_BIT_SET(PUP_EN_REG, BIT6);
#endif
	/* XXX: Following setting should be configured by u-boot */
	MV_REG_BIT_SET(SOC_DEV_MUX_REG, BIT0); /* Configure NAND flush enabled */

	/* Set NfArbiterEn to NAND Flash (Bootrom accidently Set NfArbiterEn to Device) */
	/* Disable arbitration between device and NAND */
	MV_REG_BIT_RESET(SOC_DEV_MUX_REG, BIT27);

	/* Disable MBUS Err Prop - inorder to avoid data aborts */
	MV_REG_BIT_RESET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT8);

	return MV_OK;
}

/*******************************************************************************
* mvCtrlSatRWrite
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
MV_STATUS mvCtrlSatRWrite(MV_SATR_TYPE_ID satrWriteField, MV_SATR_TYPE_ID satrReadField, MV_U8 val)
{
	MV_BOARD_SATR_INFO satrInfo;
	MV_U8 readValue, verifyValue, i2cRegNum = 0;

	if (satrReadField >= MV_SATR_READ_MAX_OPTION ||
		satrWriteField >= MV_SATR_WRITE_MAX_OPTION) {
		mvOsPrintf("%s: Error: wrong MV_SATR_TYPE_ID field value (%d).\n", __func__ ,satrWriteField);
		return MV_ERROR;
	}

	if (mvBoardSatrInfoConfig(satrWriteField, &satrInfo, MV_FALSE) != MV_OK) {
		mvOsPrintf("%s: Error: Requested S@R field is not relevant for this board\n", __func__);
		return MV_ERROR;
	}

	/* ddr bus width field is the only sample at reset field saved on the 2nd I2C register */
	if (satrWriteField == MV_SATR_WRITE_DDR_BUS_WIDTH)
		i2cRegNum = 1;

	/* read */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, i2cRegNum, &readValue) != MV_OK) {
		mvOsPrintf("%s: Error: Read from S@R failed\n", __func__);
		return MV_ERROR;
	}

	/* #1 Workaround for mirrored bits bug (for freq. mode SatR value only!)
	 * Bug: all freq. mode bits are reversed when sampled at reset from I2C
	 *		(caused due to a bug in board design)
	 * Solution: reverse them before write to I2C
	 *		(reverse only 5 bits - size of SatR field) */
	if (satrWriteField == MV_SATR_WRITE_CPU_FREQ)
		val = mvReverseBits(val) >> 3 ;

	/* modify */
	readValue &= ~(satrInfo.mask);             /* clean old value */
	readValue |= (val <<  satrInfo.offset);    /* save new value */

	/* write */
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, i2cRegNum, readValue) != MV_OK) {
		mvOsPrintf("%s: Error: Write to S@R failed\n", __func__);
		return MV_ERROR;
	}

	/* verify */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, i2cRegNum, &verifyValue) != MV_OK) {
		mvOsPrintf("%s: Error: 2nd Read from S@R failed\n", __func__);
		return MV_ERROR;
	}

	if (readValue != verifyValue) {
		mvOsPrintf("%s: Error: Write to S@R failed : written value doesn't match\n", __func__);
		return MV_ERROR;
	}

	/* #2 Workaround for mirrored bits bug (for freq. mode SatR value only!)
	 * Reverse bits again to locally save them properly */
	if (satrWriteField == MV_SATR_WRITE_CPU_FREQ)
		val = mvReverseBits(val) >> 3 ;

	/* Save written value in global array */
	satrOptionsConfig[satrReadField] = val;
	return MV_OK;
}

/*******************************************************************************
* mvCtrlSatRRead
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
MV_U32 mvCtrlSatRRead(MV_SATR_TYPE_ID satrField)
{
	MV_BOARD_SATR_INFO satrInfo;
	if (satrField < MV_SATR_READ_MAX_OPTION &&
			mvBoardSatrInfoConfig(satrField, &satrInfo, MV_TRUE) == MV_OK)
		return satrOptionsConfig[satrField];
	else
		return MV_ERROR;
}

/*******************************************************************************
* mvCtrlCpuDdrL2FreqGet - Get the selected S@R Frequency mode
*
* DESCRIPTION:
*   read board BOOT configuration and return the selcted S@R Frequency mode
*
* INPUT:  freqMode - MV_FREQ_MODE struct to return the freq mode
*
* OUTPUT: None,
*
* RETURN:
*       MV_STATUS to indicate a successful read.
*
*******************************************************************************/
MV_STATUS mvCtrlCpuDdrL2FreqGet(MV_FREQ_MODE *freqMode)
{
	MV_U32 freqModeSatRValue = mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ);

	if (freqMode == NULL) {
		mvOsPrintf("%s: Error: NULL pointer parameter\n", __func__);
		return MV_ERROR;
	}

	if (MV_ERROR != freqModeSatRValue)
		return mvCtrlFreqModeGet(freqModeSatRValue, freqMode);

	DB(mvOsPrintf("%s: Error Read from S@R fail\n", __func__));
	return MV_ERROR;

}

/*******************************************************************************
* mvCtrlSysConfigGet
*
* DESCRIPTION: Read Board configuration Field
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*	if field is valid - returns requested Board configuration field value
*
*******************************************************************************/
MV_U32 mvCtrlSysConfigGet(MV_CONFIG_TYPE_ID configField)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	if (!mvBoardConfigAutoDetectEnabled()) {
		mvOsPrintf("%s: Error reading board configuration", __func__);
		mvOsPrintf("- Auto detection is disabled\n");
		return MV_ERROR;
	}

	if (configField < MV_CONFIG_TYPE_MAX_OPTION &&
		mvBoardConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		mvOsPrintf("%s: Error: Requested board config", __func__);
		mvOsPrintf("is not valid for this board(%d)\n", configField);
		return -1;
	}

	return boardOptionsConfig[configField];

}

/*******************************************************************************
* mvCtrlSatrInit
* DESCRIPTION: Initialize S@R configuration
*               1. initialize all S@R and fields
*               2. read relevant S@R fields (direct memory access)
*               **from this point, all reads from S@R will use mvCtrlSatRRead/Write functions**
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_VOID mvCtrlSatrInit(void)
{
	MV_U32 satrVal[2];
	MV_BOARD_SATR_INFO satrInfo;
	MV_U32 i;
	MV_U8 readValue;

	/* initialize all S@R & Board configuration fields to -1 (MV_ERROR) */
	memset(&satrOptionsConfig, 0x0, sizeof(MV_U32) * MV_SATR_READ_MAX_OPTION );

	/* Read Sample @ Reset configuration, memory access read : */
	satrVal[0] = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	satrVal[1] = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));

	for (i = 0; i < MV_SATR_READ_MAX_OPTION; i++)
		if (mvBoardSatrInfoConfig(i, &satrInfo, MV_TRUE) == MV_OK)
			satrOptionsConfig[satrInfo.satrId] = ((satrVal[satrInfo.regNum]  & (satrInfo.mask)) >> (satrInfo.offset));

	/* Read DDR Bus width configuration:
	   - DDR_BUS_WIDTH - only S@R field which is not sampled at reset to any internal register
	   - Need to read it separately from S@R I2C	*/
	if (mvBoardSatrInfoConfig(MV_SATR_WRITE_DDR_BUS_WIDTH, &satrInfo, MV_FALSE) != MV_OK)
		mvOsPrintf("%s: Error: DDR_BUS_WIDTH field is not relevant for this board\n", __func__);

	/* read DDR_BUS_WIDTH from 2nd register (regNum = 1) */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, 1, &readValue) != MV_OK)
		mvOsPrintf("%s: Error: Read DDR_BUS_WIDTH from S@R failed\n", __func__);

	satrOptionsConfig[MV_SATR_DDR_BUS_WIDTH] = ((readValue  & (satrInfo.mask)) >> (satrInfo.offset));
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
	return MV_88F67X0;
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
* mvCtrlPexMaxIfGet
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

#endif

/*******************************************************************************
* mvCtrlPexMaxUnitGet
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
*       Marvell controller number of PEX units. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexActiveUnitNumGet(MV_VOID)
{
	return mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);
}

#if defined(MV_INCLUDE_PCI)
/*******************************************************************************
* mvCtrlPciMaxIfGet
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
#ifndef mvCtrlPciMaxIfGet
MV_U32 mvCtrlPciMaxIfGet(MV_VOID)
{
	return 1;
}

#endif
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
	return MV_ETH_MAX_PORTS;
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
	return mvCtrlSocUnitInfoNumGet(SATA_UNIT_ID);
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

#if defined(MV_INCLUDE_USB)
/*******************************************************************************
* mvCtrlUsbMaxGet - Get number of Marvell Usb  controllers
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
	return mvCtrlSocUnitInfoNumGet(USB_UNIT_ID);
}

/*******************************************************************************
* mvCtrlUsb3MaxGet - Get number of Marvell USB 3.0 controllers
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
*       returns number of Marvell USB 3.0 controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsb3MaxGet(void)
{
	return mvCtrlSocUnitInfoNumGet(USB3_UNIT_ID);
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
	return mvCtrlSocUnitInfoNumGet(SDIO_UNIT_ID) ? MV_TRUE : MV_FALSE;
}

#endif

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
	return mvCtrlSocUnitInfoNumGet(TDM_UNIT_ID) ? MV_TRUE : MV_FALSE;
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
	return mvCtrlSocUnitInfoNumGet(TDM_UNIT_ID);
}

/*******************************************************************************
* mvCtrlTdmUnitTypeGet - return the TDM unit type being used
*
* DESCRIPTION:
*	if auto detection enabled, read TDM unit from board configuration
*	else , read pre-defined TDM unit from board information struct.
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
MV_TDM_UNIT_TYPE mvCtrlTdmUnitTypeGet(MV_VOID)
{
	return TDM_UNIT_2CH;
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

/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in Vendor ID configuration register
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
	return MV_6720_DEV_ID;
}

/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       Revision ID Register.
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
	MV_U32 value;

	value = MV_REG_READ(DEV_VERSION_ID_REG);
	return  ((value & (REVISON_ID_MASK) ) >> REVISON_ID_OFFS);
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
	return (mvCtrlModelGet() << 16) | mvCtrlRevGet();
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
	/* mvCtrlModelRevGet(); */
	mvCtrlNameGet(pNameBuff);
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
#if defined(MV_INCLUDE_PEX)
static MV_VOID mvCtrlPexAddrDecShow(MV_VOID)
{
	MV_PEX_BAR pexBar;
	MV_PEX_DEC_WIN win;
	MV_U32 pexIf;
	MV_U32 bar, winNum;
	MV_BOARD_PEX_INFO *boardPexInfo = mvBoardPexInfoGet();
	MV_U32 pexHWInf = 0;

	for (pexIf = 0; pexIf < boardPexInfo->boardPexIfNum; pexIf++) {
		pexHWInf = pexIf;


		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexHWInf))
			continue;
		mvOsOutput("\n");
		mvOsOutput("PEX%d:\n", pexHWInf);
		mvOsOutput("-----\n");

		mvOsOutput("\nPex Bars\n\n");

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
static void mvUnitAddrDecShow(MV_U8 numUnits, MV_UNIT_ID unitId,
			      const char *name, MV_WIN_GET_FUNC_PTR winGetFuncPtr)
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
	alignment--;                    /* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */
	/* and size is 0x1000000 (16MB) for example     */
	while (alignment & 1) {         /* Check that alignmet LSB is set       */
		size = (size >> 1);     /* If LSB is set, move 'size' one bit to right      */
		alignment = (alignment >> 1);
	}

	/* If after the alignment first '0' was met we still have '1' in                */
	/* it then aligment is invalid (not power of 2)                                 */
	if (alignment) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", (MV_U32)alignment));
		return -1;
	}

	/* Now the size is shifted right according to aligment: 0x0100                  */
	size--;                 /* Now the size is a sequance of '1': 0x00ff                    */
	retVal = size;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's         */
	while (size & 1)                /* Check that LSB is set    */
		size = (size >> 1);     /* If LSB is set, move one bit to the right         */

	if (size) {                     /* Sequance of 1's is over. Check that we have no other 1's         */
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
	temp = regSize;                 /* Now the size is a sequance of '1': 0x00ff            */

	while (temp & 1)                /* Check that LSB is set                                    */
		temp = (temp >> 1);     /* If LSB is set, move one bit to the right         */

	if (temp) {                     /* Sequance of 1's is over. Check that we have no other 1's         */
		DB(mvOsPrintf("%s: ERR: Size parameter 0x%x invalid.\n", __func__, regSize));
		return -1;
	}

	/* Check that aligment is a power of two                                        */
	temp = alignment - 1;           /* Now the alignmet is a sequance of '1' (0xffff)          */

	while (temp & 1)                /* Check that alignmet LSB is set                           */
		temp = (temp >> 1);     /* If LSB is set, move 'size' one bit to right      */

	/* If after the 'temp' first '0' was met we still have '1' in 'temp'            */
	/* then 'temp' is invalid (not power of 2)                                      */
	if (temp) {
		DB(mvOsPrintf("%s: ERR: Alignment parameter 0x%x invalid.\n", __func__, alignment));
		return -1;
	}

	regSize++;              /* Now the size is 0x0100                                       */

	/* Add in the "alignment" portion to the register size parameter                */
	alignment--;                            /* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */

	while (alignment & 1) {                 /* Check that alignmet LSB is set                       */
		regSize = (regSize << 1);       /* LSB is set, move 'size' one bit left     */
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
	return MV_TRUE;
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
	return MV_TRUE; // omriii : return to false
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
	/* Clock gating is not supported on FPGA */
	if (mvCtrlModelGet() == MV_FPGA_DEV_ID)
		return;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PEX_STOP_CLK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PEX_STOP_CLK_MASK(index));

		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SATA_STOP_CLK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SATA_STOP_CLK_MASK);

		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_USB_STOP_CLK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_USB_STOP_CLK_MASK);

		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SDIO_STOP_CLK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SDIO_STOP_CLK_MASK);

		break;
#endif
	case TDM_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_TDM_STOP_CLK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_TDM_STOP_CLK_MASK);
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
	MV_BOOL state = MV_TRUE;

	/* Clock gating is not supported on FPGA */
	if (mvCtrlModelGet() == MV_FPGA_DEV_ID)
		return MV_TRUE;

	MV_U32 reg = MV_REG_READ(POWER_MNG_CTRL_REG);
	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEX_STOP_CLK_MASK(index)) == PMC_PEX_STOP_CLK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATA_STOP_CLK_MASK) == PMC_SATA_STOP_CLK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USB_STOP_CLK_MASK) == PMC_USB_STOP_CLK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & PMC_SDIO_STOP_CLK_MASK) == PMC_SDIO_STOP_CLK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_UNIT_ID:
		if ((reg & PMC_TDM_STOP_CLK_MASK) == PMC_TDM_STOP_CLK_STOP)
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

	return (reg & (0x1 << REG_SDRAM_CONFIG_DDR_BUS_OFFS)) ? 32 : 16;
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
	/*Used Hard Coded values, TODO sync with Spec*/
	MV_32 reg = 0;

	/* init the TSEN sensor once */
	if ((MV_REG_READ(TSEN_STATE_REG) & TSEN_STATE_MASK) == 0) {
		MV_REG_BIT_RESET(TSEN_STATE_REG, TSEN_STATE_MASK);

		MV_REG_WRITE(TSEN_STATE_REG, 0x8011E214);

		reg = MV_REG_READ(TSEN_CONF_REG);

		reg = 0x00a80909;
		MV_REG_WRITE(TSEN_CONF_REG, reg);
		mvOsDelay(10);
	}

	reg = MV_REG_READ(TSEN_STATUS_REG);
	reg = (reg & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;

	return (3171900 - (10000 * reg)) / 13553;
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
void mvCtrlNandClkSet(int nClock)
{
	/* Set the division ratio of ECC Clock 0x00018748[13:8] (by default it's double of core clock) */
	MV_U32 nVal = MV_REG_READ(CORE_DIV_CLK_CTRL(1));
	nVal &= ~(NAND_ECC_DIVCKL_RATIO_MASK);
	nVal |= (nClock << NAND_ECC_DIVCKL_RATIO_OFFS);
	MV_REG_WRITE(CORE_DIV_CLK_CTRL(1), nVal);

	/* Set reload force of ECC clock 0x00018740[7:0] to 0x2 (meaning you will force only the ECC clock) */
	nVal = MV_REG_READ(CORE_DIV_CLK_CTRL(0));
	nVal &= ~(CORE_DIVCLK_RELOAD_FORCE_MASK);
	nVal |= CORE_DIVCLK_RELOAD_FORCE_VAL;
	MV_REG_WRITE(CORE_DIV_CLK_CTRL(0), nVal);

	/* Set reload ratio bit 0x00018740[8] to 1'b1 */
	MV_REG_BIT_SET(CORE_DIV_CLK_CTRL(0), CORE_DIVCLK_RELOAD_RATIO_MASK);
	mvOsDelay(1); /*  msec */
	/* Set reload ratio bit 0x00018740[8] to 0'b1 */
	MV_REG_BIT_RESET(CORE_DIV_CLK_CTRL(0), CORE_DIVCLK_RELOAD_RATIO_MASK);
}
