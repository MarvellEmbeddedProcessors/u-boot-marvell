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

#if defined(CONFIG_MV_ETH_PP2)
#include "pp2/gbe/mvPp2Gbe.h"
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

#if defined(MV_INCLUDE_TDM)
static MV_VOID mvCtrlTdmCtrlRegSet(MV_VOID);
#endif

MV_U32 mvCtrlGetCpuNum(MV_VOID)
{
	MV_U32 cpu1Enabled;

	if (mvCtrlSatRRead(MV_SATR_CPU1_ENABLE, &cpu1Enabled) != MV_OK) {
		mvOsPrintf("%s: Error: failed to read CPU#1 status\n", __func__);
		return 0; /* use single CPU */
	} else
		return cpu1Enabled;
}

/*
 * Note: this function is called at the very early stage
 * in Linux Kernel, hence, it has to read from SoC register, not
 * from pre-built database.
 */
MV_BOOL mvCtrlIsSscgEnabled(MV_VOID)
{
	MV_BOARD_SATR_INFO satrInfo;
	MV_U32 sscgDisabled;

	if (mvBoardSatrInfoConfig(MV_SATR_SSCG_DISABLE, &satrInfo, MV_TRUE) != MV_OK) {
		mvOsPrintf("%s: mvBoardSatrInfoConfig failed\n", __func__);
		return MV_FALSE;
	}

	sscgDisabled = MV_REG_READ(MPP_SAMPLE_AT_RESET(satrInfo.regNum));
	sscgDisabled = (sscgDisabled & (satrInfo.mask)) >> (satrInfo.offset);

	if (sscgDisabled == 0)
		return MV_TRUE; /* SSCG mechanism is enabled */
	else
		return MV_FALSE; /* SSCG mechanism is disabled */
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
	MV_U32 cpuFreqSatRMode;
	MV_FREQ_MODE cpuFreqMode;

	if (mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ, &cpuFreqSatRMode) != MV_OK) {
		mvOsPrintf("%s: Error: failed to read Frequency status\n", __func__);
		return MV_FALSE;
	}

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

/* Avanta LP family linear id */
#define MV_6660_INDEX		0
#define MV_6650_INDEX		1 /* used also for MV6650F */
#define MV_6610_INDEX		2 /* used also for MV6610F */
#define MV_6665_INDEX		3
#define MV_6658_INDEX		4
#define MV_66xx_INDEX_MAX	5

static MV_U32 mvCtrlDevIdIndexGet(MV_U32 devId)
{
	MV_U32 index;

	switch (devId) {
	case MV_6660_DEV_ID:
		index = MV_6660_INDEX;
		break;
	case MV_6650_DEV_ID:
	case MV_6650F_DEV_ID:
		index = MV_6650_INDEX;
		break;
	case MV_6610_DEV_ID:
	case MV_6610F_DEV_ID:
		index = MV_6610_INDEX;
		break;
	case MV_6665_DEV_ID:
		index = MV_6665_INDEX;
	case MV_6658_DEV_ID:
		index = MV_6658_INDEX;
	default:
		index = MV_6650_INDEX;
	}

	return index;
}

MV_UNIT_ID mvCtrlSocUnitNums[MAX_UNITS_ID][MV_66xx_INDEX_MAX] = {
/*                        6660      650/650F 610/610F  6665   6658 */
/* DRAM_UNIT_ID         */ { 1,		1,	1,	1,	1, },
/* PEX_UNIT_ID          */ { 2,		1,	0,	2,	2, },
/* ETH_GIG_UNIT_ID      */ { 2,		2,	2,	2,	2, },
/* USB_UNIT_ID          */ { 1,		2,	0,	0,	2, },
/* USB3_UNIT_ID         */ { 1,		0,	0,	1,	0, },
/* IDMA_UNIT_ID         */ { 0,		0,	0,	0,	0, },
/* XOR_UNIT_ID          */ { 2,		2,	0,	2,	2, },
/* SATA_UNIT_ID         */ { 2,		0,	0,	2,	0, },
/* TDM_UNIT_ID          */ { 1,		1,	1,	1,	1, },
/* UART_UNIT_ID         */ { 2,		2,	2,	2,	2, },
/* CESA_UNIT_ID         */ { 2,		0,	0,	2,	0, },
/* SPI_UNIT_ID          */ { 2,		2,	2,	2,	2, },
/* AUDIO_UNIT_ID        */ { 1,		0,	0,	1,	0, },
/* SDIO_UNIT_ID         */ { 1,		1,	1,	1,	1, },
/* TS_UNIT_ID           */ { 0,		0,	0,	0,	0, },
/* XPON_UNIT_ID         */ { 1,		1,	1,	1,	1, },
/* BM_UNIT_ID           */ { 1,		1,	1,	1,	1, },
/* PNC_UNIT_ID          */ { 1,		1,	1,	1,	1, },
/* I2C_UNIT_ID          */ { 2,		2,	1,	2,	1, },
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

	/* If set to Auto detect, read board config information,
	 * Accordingly update Eth-Complex config, MPP group types and switch info */
	if (mvBoardConfigAutoDetectEnabled()) {
		mvCtrlSysConfigInit();
		mvBoardInfoUpdate();
	}

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

	mvEthComplexInit(mvBoardEthComplexConfigGet());
	/*
	 * Enable NAND Flash PUP (Pack-Unpack)
	 * HW machanism to accelerate transactions (controlled by SoC register)
	 */
	MV_REG_BIT_SET(PUP_EN_REG, BIT4);

	/* XXX: Following setting should be configured by u-boot */
	MV_REG_BIT_SET(SOC_DEV_MUX_REG, BIT0); /* Configure NAND flush enabled */

	/* Set NfArbiterEn to NAND Flash (Bootrom accidently Set NfArbiterEn to Device) */
	/* Disable arbitration between device and NAND */
	MV_REG_BIT_RESET(SOC_DEV_MUX_REG, BIT27);

#if defined(MV_INCLUDE_TDM)
	mvCtrlTdmCtrlRegSet();
	mvCtrlTdmClkCtrlConfig();
#endif

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
MV_STATUS mvCtrlSatRWrite(MV_SATR_TYPE_ID satrReadField, MV_U8 val, MV_BOOL restoreDefault)
{
	MV_BOARD_SATR_INFO satrInfo;
	MV_U8 readValue, verifyValue;

	/* S@R Write field enums are following the read field enums,
	 * with equal field order as the read fields, adaptively.
	 * The write fields starts right after the last Read field */
	MV_SATR_TYPE_ID satrWriteField = satrReadField + MV_SATR_READ_MAX_OPTION + 1;

	if (satrReadField >= MV_SATR_READ_MAX_OPTION ||
		satrWriteField >= MV_SATR_WRITE_MAX_OPTION) {
		mvOsPrintf("%s: Error: wrong MV_SATR_TYPE_ID field value (%d).\n"
				, __func__, satrWriteField);
		return MV_ERROR;
	}

	if (mvBoardSatrInfoConfig(satrWriteField, &satrInfo, MV_FALSE) != MV_OK) {
		if (restoreDefault == MV_FALSE)
			mvOsPrintf("%s: Error: Requested field is not writeable for this board\n", __func__);
		return MV_ERROR;
	}

	/* read */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, 0, &readValue) != MV_OK) {
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
	if (mvBoardTwsiSet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, 0, readValue) != MV_OK) {
		mvOsPrintf("%s: Error: Write to S@R failed\n", __func__);
		return MV_ERROR;
	}

	/* verify */
	if (mvBoardTwsiGet(BOARD_DEV_TWSI_SATR, satrInfo.regNum, 0, &verifyValue) != MV_OK) {
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
*	 value - pointer for returned value
*
* OUTPUT: None
*
* RETURN:
*	if field is valid - returns requested S@R field value
*       else if field is not relevant for running board, return 0xFFFFFFF.
*
*******************************************************************************/
MV_STATUS mvCtrlSatRRead(MV_SATR_TYPE_ID satrField, MV_U32 *value)
{
	if (value == NULL) {
		DB(mvOsPrintf("%s: Error: NULL pointer parameter\n", __func__));
		return MV_ERROR;
	}

	*value = satrOptionsConfig[satrField];
	return MV_OK;
}

/*******************************************************************************
* mvCtrlSmiMasterSet - alter Group 4 MPP type, between CPU SMI control and SWITCH SMI control
*
* DESCRIPTION: Read board configuration which is relevant to MPP group 4 interfaces,
* 		to derive the correct group type, and according to input SMI conrtol,
* 		write the correct MPP value.
*
* INPUT: smiCtrl - enum to select between SWITCH/CPU SMI controll
*
* OUTPUT: None
*
* RETURN: None
*
*******************************************************************************/
MV_VOID mvCtrlSmiMasterSet(MV_SMI_CTRL smiCtrl)
{
	MV_BOOL isSPI1Enabled, isRefClkOut;
	MV_U32 slicDev, ethComplex, groupTypeSelect = 0;

	ethComplex = mvBoardEthComplexConfigGet();
	slicDev = mvBoardSlicUnitTypeGet();

	/* if not using Lantiq TDM, define REF_CLK_OUT (both utilize the same gpio) */
	isRefClkOut   = !(slicDev == SLIC_LANTIQ_ID);

	/*SPI1 is in use when:
	 * 1. Boot source is SPI1
	 * 2. RGMII-1 is disabled (SPI-1 MPP's are shared with RGMII-1 MPP's) */
	if (mvBoardBootDeviceGet() == MSAR_0_BOOT_SPI1_FLASH ||
		(!(ethComplex & MV_ETHCOMP_GE_MAC1_2_RGMII1)))
		isSPI1Enabled = MV_TRUE;
	else
		isSPI1Enabled = MV_FALSE;

	if (smiCtrl == NO_SMI_CTRL)
		groupTypeSelect = NO_SW_SMI_CTRL_REF_CLK_OUT;
	else {
		/* MPP settings :
		* Test board configuration relevant to MPP group 4, and derive the correct group type */

		if (isRefClkOut)	/* add first REF_CLK_OUT group type */
			groupTypeSelect += GE1_CPU_SMI_CTRL_REF_CLK_OUT;

		if (smiCtrl == SWITCH_SMI_CTRL)	/* add first SW_SMI group type */
			groupTypeSelect += GE1_SW_SMI_CTRL_TDM_LQ_UNIT;

		if (isSPI1Enabled)	/* add first SPI1 group type */
			groupTypeSelect += SPI1_CPU_SMI_CTRL_TDM_LQ_UNIT;
	}

	mvBoardMppTypeSet(4, groupTypeSelect);	/* Set MPP value according to group type */
	MV_REG_WRITE(mvCtrlMppRegGet(4), mvBoardMppGet(4));

	/* Mux settings :
	 * Add mux configuration setup here ! */
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
	MV_U32 freqModeSatRValue;

	if (freqMode == NULL) {
		mvOsPrintf("%s: Error: NULL pointer parameter\n", __func__);
		return MV_ERROR;
	}

	if (mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ, &freqModeSatRValue) != MV_OK) {
		mvOsPrintf("%s: Error: failed to read frequency status\n", __func__);
		return MV_ERROR;
	}

	return mvCtrlFreqModeGet(freqModeSatRValue, freqMode);
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
		mvOsPrintf("%s: Error  Failed to read board config (Auto detection disabled)\n", __func__);
		return MV_ERROR;
	}

	if (configField < MV_CONFIG_TYPE_MAX_OPTION &&
		mvBoardConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		DB(mvOsPrintf("%s: Error: Requested board config is invalid for this board" \
				" (%d)\n", __func__, configField));
		return MV_ERROR;
	}

	return boardOptionsConfig[configField];

}

/*******************************************************************************
* mvCtrlSysConfigSet
*
* DESCRIPTION: Write Board configuration Field to local array
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*	Write requested Board configuration field value to local array
*
*******************************************************************************/
MV_STATUS mvCtrlSysConfigSet(MV_CONFIG_TYPE_ID configField, MV_U8 value)
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

	/* initialize all S@R & Board configuration fields to -1 (MV_ERROR) */
	memset(&satrOptionsConfig, 0x0, sizeof(MV_U32) * MV_SATR_READ_MAX_OPTION );

	/* Read Sample @ Reset configuration, memory access read : */
	satrVal[0] = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	satrVal[1] = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));

	for (i = 0; i < MV_SATR_READ_MAX_OPTION; i++)
		if (mvBoardSatrInfoConfig(i, &satrInfo, MV_TRUE) == MV_OK)
			satrOptionsConfig[satrInfo.satrId] = ((satrVal[satrInfo.regNum]  & (satrInfo.mask)) >> (satrInfo.offset));

}

/*******************************************************************************
* mvCtrlSysConfigInit
*
* DESCRIPTION: Initialize S@R configuration
*               1. initialize all board configuration fields
*               3. read relevant board configuration (using TWSI/EEPROM access)
*               **from this point, all reads from S@R & board config will use mvCtrlSatRRead/Write functions**
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_VOID mvCtrlSysConfigInit()
{
	MV_U8 regNum, i, configVal[MV_IO_EXP_MAX_REGS], readValue, bitsNum;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_BOOL readSuccess = MV_FALSE;
	MV_BOOL isEepromEnabled = mvBoardIsEepromEnabled();

	memset(&boardOptionsConfig, 0x0, sizeof(MV_U32) * MV_CONFIG_TYPE_MAX_OPTION );

	/*Read rest of Board Configuration, EEPROM / Dip Switch access read : */
	if (mvCtrlBoardConfigGet(configVal) != MV_OK) {
		mvOsPrintf("%s: Error: mvCtrlBoardConfigGet failed\n", __func__);
		return;
	}

	/* Save values Locally in configVal[] */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION; i++) {
		/* Get board configuration field information (Mask, offset, etc..) */
		if (mvBoardConfigTypeGet(i, &configInfo) != MV_TRUE)
			continue;

		/* each Expander conatins 2 registers */
		regNum = configInfo.expanderNum * 2 + configInfo.regNum;
		readValue = (configVal[regNum] & configInfo.mask) >> configInfo.offset;

		/*
		 * Workaround for DIP Switch IO Expander 0x21 bug in DB-6660 board
		 * Bug: Pins at IO expander 0x21 are reversed (only on DB-6660)
		 * example : instead of reading 00000110, we read 01100000
		 * WA step 1 (mvCtrlBoardConfigGet)
		 *  after reading IO expander, reverse bits of both registers
		 * WA step 2 (in mvCtrlSysConfigInit):
		 *  after reversing bits, swap MSB and LSB - due to Dip-Switch reversed mapping
		 */
		if (!isEepromEnabled && configInfo.expanderNum == 0)  {
			bitsNum = mvCountMaskBits(configInfo.mask);
			readValue = mvReverseBits(readValue) >> (8-bitsNum);
		}

		boardOptionsConfig[configInfo.configId] =  readValue;
		readSuccess = MV_TRUE;
	}

	if (readSuccess == MV_FALSE)
		mvOsPrintf("%s: Error: Read board configuration from EEPROM/Dip Switch failed\n", __func__);
}

/*******************************************************************************
* mvCtrlBoardConfigGet - read Board Configuration, from EEPROM / Dip Switch
*
* DESCRIPTION:
*       This function reads all board configuration from EEPROM / Dip Switch:
*	1. read the EEPROM enable jumper, and read from configured device
*	2. read first 2 registers for all boards
*	3. read specific registers for specific boards
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM enabled, else return MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvCtrlBoardConfigGet(MV_U8 *config)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_STATUS status1, status2;
	MV_BOOL isEepromEnabled = (mvBoardEepromInit() == MV_OK) ? MV_TRUE : MV_FALSE;

	config[0] = config[1] = config[2] = 0x0;

	MV_BOARD_TWSI_CLASS twsiClass = (isEepromEnabled ? BOARD_DEV_TWSI_EEPROM : BOARD_DEV_TWSI_IO_EXPANDER);

	status1 = mvBoardTwsiGet(twsiClass, 0, 0, &config[0]);		/* EEPROM/Dip Switch Reg#0 */
	status2 = mvBoardTwsiGet(twsiClass, 0, 1, &config[1]);		/* EEPROM/Dip Switch Reg#1 */

	if (status1 != MV_OK || status2 != MV_OK) {
		DB(mvOsPrintf("%s: Error: mvBoardTwsiGet from EEPROM/Dip Switch failed\n", __func__));
		return MV_ERROR;
	}
	if (boardId == DB_6660_ID) { /* DB-6660 has another register for board configuration */
		if (isEepromEnabled == MV_TRUE)
			status1 = mvBoardTwsiGet(BOARD_DEV_TWSI_EEPROM, 0, 2, &config[2]);	/* EEPROM Reg#2 */
		else {
			status1 = mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER, 1, 0, &config[2]);	/* Dip Switch Reg#1 */
			/*
			 * Workaround for DIP Switch IO Expander 0x21 bug in DB-6660 board
			 * Bug: Pins at IO expander 0x21 are reversed (only on DB-6660)
			 * example : instead of reading 00000110, we read 01100000
			 * WA step 1 (mvCtrlBoardConfigGet)
			 *  after reading IO expander, reverse bits of both registers
			 * WA step 2 (in mvCtrlSysConfigInit):
			 *  after reversing bits, swap MSB and LSB - due to Dip-Switch reversed mapping
			 */
			config[0] = mvReverseBits(config[0]);
			config[1] = mvReverseBits(config[1]);
		}

		if (status1 != MV_OK) {
			DB(mvOsPrintf("%s: Error: mvBoardTwsiGet from EEPROM/Dip Switch Reg#2 failed\n", __func__));
			return MV_ERROR;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvCtrlEepromEnable
*
* DESCRIPTION:
*       This function enable/disable the Eeprom usage
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM enabled, else return MV_FALSE.
*
*******************************************************************************/
MV_STATUS mvCtrlEepromEnable(MV_BOOL enable)
{
	MV_BOARD_IO_EXPANDER_TYPE_INFO *ioInfo = NULL;

	if (mvBoardIoExpanderTypeGet(MV_IO_EXPANDER_JUMPER2_EEPROM_ENABLED, ioInfo))
		return 	(mvBoardIoExpValSet(ioInfo, (enable? 0x1 : 0x0)));

	mvOsPrintf("%s: Error: Read from IO expander failed (EEPROM enabled jumper)\n", __func__);
	return MV_ERROR;
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
	return MV_88F66X0;
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
*       Marvell controller number of PEX units.
*
*******************************************************************************/
MV_U32 mvCtrlPexActiveUnitNumGet(MV_VOID)
{
	/* check board configuration for DB-6660:
	 * if PEX1 is disabled , only PEX0 is active (return constant 1)
	 * (only if MV_CONFIG_LANE1=0 --> then LANE1=PEX) */
	if (mvBoardIdGet() == DB_6660_ID && mvCtrlSysConfigGet(MV_CONFIG_LANE1) != 0x0)
		return 1;

	/* else, all PEX interfaces are active */
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
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	return MV_FPGA_ETH_MAX_PORT;
#else
	return 4;
	/* fixme : #error "ETH_MAX_PORT should be defined!" */
#endif
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
	MV_U32 sata_ports = mvCtrlSocUnitInfoNumGet(SATA_UNIT_ID);
	MV_U32 revID = mvCtrlRevGet();
	/* Z1, Z2, Z3 support 1 SATA port */
	if (revID <= MV_88F66X0_Z3_ID)
		return sata_ports - 1;
	else
		return sata_ports;
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
	MV_U32 usbNums = mvCtrlSocUnitInfoNumGet(USB_UNIT_ID);
	/* Z1, Z2, and Z3 revisions support 1 USB unit port in 6650 boards */
	if (mvCtrlRevGet() <= MV_88F66X0_Z3_ID && mvCtrlModelGet() == MV_6650_DEV_ID)
		return usbNums - 1;
	return usbNums;
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
	/* In Z1, Z2, and Z3 default HW setup is with SDIO only for RD-6660*/
	if (mvCtrlRevGet() <= MV_88F66X0_Z3_ID)
		return 0;
	else
		return mvCtrlSocUnitInfoNumGet(SDIO_UNIT_ID);
}

#endif

#if defined(MV_INCLUDE_TDM)
/*******************************************************************************
* mvCtrlTdmCtrlRegSet - Set TDM configuration register for: TDM, TDM+ISI, TDM+ZSI
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
*       None.
*
*******************************************************************************/
static MV_VOID mvCtrlTdmCtrlRegSet(MV_VOID)
{
	MV_U32 tdmCtrl;

	switch (mvBoardSlicUnitTypeGet()) {
	case MV_BOARD_SLIC_DISABLED:
	case MV_BOARD_SLIC_SSI_ID:
		return;
	case MV_BOARD_SLIC_ISI_ID:
		tdmCtrl = (ISI_MODE | ISI_MODE_CS_DEASSERT_BIT_COUNT_VAL | SPI_B_MODE_ISI_ENABLE_MASK);
		break;
	case MV_BOARD_SLIC_ZSI_ID:
		tdmCtrl = ZSI_MODE;
		break;
	case MV_BOARD_SLIC_EXTERNAL_ID:
		tdmCtrl = TDM_MODE;
		break;
	default:
		mvOsPrintf("%s: Error: wrong SLIC type\n", __func__);
		return;
	}

	/* Reset TDM Control register field/s */
	MV_REG_WRITE(MV_TDM_CTRL_REG, (MV_REG_READ(MV_TDM_CTRL_REG) & ~TDM_MODE_MASK));

	/* Set updated value to TDM Control register */
	MV_REG_BIT_SET(MV_TDM_CTRL_REG, tdmCtrl);

	return;
}

/*******************************************************************************
* mvCtrlTdmClkCtrlSet - Set TDM Clock Out Divider Control register
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
*       None.
*
*******************************************************************************/
MV_VOID mvCtrlTdmClkCtrlConfig(MV_VOID)
{
	MV_U32 clkReg, pcmClkFreq;
	MV_U32 dcoMask, dcoVal;

#if defined(MV_TDM_PCM_CLK_8MHZ)
	pcmClkFreq = DCO_CLK_DIV_RATIO_8M;
#elif defined(MV_TDM_PCM_CLK_4MHZ)
	pcmClkFreq = DCO_CLK_DIV_RATIO_4M;
#elif defined(MV_TDM_PCM_CLK_2MHZ)
	pcmClkFreq = DCO_CLK_DIV_RATIO_2M;
#else
	pcmClkFreq = DCO_CLK_DIV_RATIO_8M;
#endif

	/* Reload new DCO source ratio */
	clkReg = MV_REG_READ(CORE_DIVCLK_CTRL_REG);
	MV_REG_WRITE(CORE_DIVCLK_CTRL_REG,
		     MV_BIT_CLEAR(clkReg, DCO_CLK_DIV_MOD_OFFS));
	mvOsUDelay(1);

	clkReg = MV_REG_READ(CORE_DIVCLK_CTRL_REG);
	MV_REG_WRITE(CORE_DIVCLK_CTRL_REG,
		     MV_BIT_SET(clkReg, DCO_CLK_DIV_MOD_OFFS));
	mvOsUDelay(1);

	clkReg = MV_REG_READ(CORE_DIVCLK_CTRL_REG);
	MV_REG_WRITE(CORE_DIVCLK_CTRL_REG,
		     MV_BIT_CLEAR(clkReg, DCO_CLK_DIV_MOD_OFFS));
	mvOsUDelay(1);

	clkReg = MV_REG_READ(CORE_DIVCLK_CTRL_REG);
	MV_REG_WRITE(CORE_DIVCLK_CTRL_REG,
		     MV_BIT_SET(clkReg, DCO_CLK_DIV_RESET_OFFS));
	mvOsUDelay(1);

	/* Set DCO correction to 0PPM */
	dcoMask = (DCO_MOD_CTRL_MASK | DCO_CTRLFS_MASK);
	dcoVal = (DCO_MOD_CTRL_BASE_VAL | DCO_CTRLFS_24MHZ);
	clkReg = MV_REG_READ(DCO_MOD_CTRL_REG);
	MV_REG_WRITE(DCO_MOD_CTRL_REG, ((clkReg & ~dcoMask) | dcoVal));
	mvOsUDelay(1);

	/* Set DCO source ratio */
	clkReg = MV_REG_READ(CORE_DIVCLK_CTRL_REG);
	MV_REG_WRITE(CORE_DIVCLK_CTRL_REG,
		     (clkReg & ~DCO_CLK_DIV_RATIO_MASK) | pcmClkFreq);
	mvOsUDelay(1);
}

#if defined(MV_TDM_USE_DCO)
/*******************************************************************************
* mvCtrlTdmClkCtrlGet - Get DCO correction ratio
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
*       None.
*
*******************************************************************************/
MV_32 mvCtrlTdmClkCtrlGet(MV_VOID)
{
	MV_32 val;

	/* Get DCO correction */
	val = ((((MV_REG_READ(DCO_MOD_CTRL_REG)) & DCO_MOD_CTRL_MASK) >> DCO_MOD_CTRL_OFFS) - DCO_MOD_CTRL_BASE);

	return val;
}

/*******************************************************************************
* mvCtrlTdmClkCtrlSet - Set DCO correction ratio
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
*       Current DCO correction.
*
*******************************************************************************/
MV_VOID mvCtrlTdmClkCtrlSet(MV_32 correction)
{
	/* Set DCO correction to correction * 1PPM */
	MV_REG_WRITE(DCO_MOD_CTRL_REG,
		     ((MV_REG_READ(DCO_MOD_CTRL_REG) & ~DCO_MOD_CTRL_MASK) |
		     (((DCO_MOD_CTRL_BASE_VAL >> DCO_MOD_CTRL_OFFS) + correction) << DCO_MOD_CTRL_OFFS)));
}
#endif /* MV_TDM_USE_DCO */

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
#endif /* MV_INCLUDE_TDM */

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
#ifdef CONFIG_MACH_AVANTA_LP_FPGA
	return MV_88F66X0;
#else
	MV_U32 ctrlId, satr0;
	MV_U32 rev = mvCtrlRevGet();

	ctrlId = MV_REG_READ(DEV_ID_REG);
	ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;
	satr0 = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	satr0 &= SATR_DEVICE_ID_2_0_MASK;
	/* Device ID mapping differs between Z1-Z3 and A0, since new flavors
	** are added for A0 */
	if (rev <= MV_88F66X0_Z3_ID) {
		if (ctrlId == 0x6660)
			return MV_6660_DEV_ID;
		if (satr0 == 0)
			return MV_6650_DEV_ID;
		return MV_6610_DEV_ID;
	}
	switch (satr0) {
	case 0:
		if (ctrlId == 0x6660)
			return  MV_6660_DEV_ID;
		else if (ctrlId == 0x6610)
			return  MV_6650_DEV_ID;
		break;
	case 1:
		if (ctrlId == 0x6660)
			return  MV_6658_DEV_ID;
		else if (ctrlId == 0x6610)
			return  MV_6650F_DEV_ID;
		break;
	case 2:
		if (ctrlId == 0x6610)
			return  MV_6610_DEV_ID;
		break;
	case 3:
		if (ctrlId == 0x6660)
			return  MV_6665_DEV_ID;
		else if (ctrlId == 0x6610)
			return  MV_6610F_DEV_ID;
		break;
	}
	mvOsPrintf("%s: Error: Failed to Ctrl model ID\n", __func__);
	return MV_6660_DEV_ID;
#endif
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
MV_U32 mvCtrlRevGet(MV_VOID)
{
	MV_U32 value;

	value = MV_REG_READ(DEV_VERSION_ID_REG);
	value = (value & REVISON_ID_MASK) >> REVISON_ID_OFFS;

	switch (value) {
	case MV_88F66X0_Z1_ID:
	case MV_88F66X0_Z2_ID:
	case MV_88F66X0_Z3_ID:
	case MV_88F66XX_A0_ID:
			return value;
	default:
			mvOsPrintf("%s: Error: Failed to read Revision ID\n", __func__);
			return 0x0;
	}
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
MV_VOID mvCtrlNameGet(char *pNameBuff)
{
	mvOsSPrintf(pNameBuff, "%s%x", SOC_NAME_PREFIX, mvCtrlModelGet());
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
	MV_U32 revId;
	char *revArray[] = MV_88F66X0_ID_ARRAY;

	revId = mvCtrlRevGet();

	switch (revId) {
	case MV_88F66X0_Z1_ID:
	case MV_88F66X0_Z2_ID:
	case MV_88F66X0_Z3_ID:
	case MV_88F66XX_A0_ID:
			mvOsSPrintf(pNameBuff, " Rev %s", revArray[revId]);
			return;
	default:
		mvOsPrintf("%s: Error: Failed to read Revision ID\n", __func__);
	}
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
MV_VOID mvCtrlModelRevNameGet(char *pNameBuff)
{
	mvCtrlNameGet(pNameBuff);
	mvCtrlRevNameGet(pNameBuff + strlen(pNameBuff));
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
	MV_U32 boardPexIfNum = mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);
	MV_U32 pexHWInf = 0;

	for (pexIf = 0; pexIf < boardPexIfNum; pexIf++) {
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
