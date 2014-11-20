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
#include "config_marvell.h"     /* Required to identify SOC and Board */
#include "mv_os.h"
#include "mvSysEnvLib.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "mv_seq_exec.h"
#include "printf.h"
#include "ddr3_a38x.h"

#ifdef WIN32
#define mvPrintf    printf
#endif

MV_U32 gBoardId = -1;
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
#else
	/* For Marvell Boards: read board ID from TWSI*/
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardId;

	DEBUG_INIT_FULL_S("\n### mvBoardIdGet ###\n");

	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading board id */
	DEBUG_INIT_FULL_S("mvBoardIdGet: getting board id\n");
	if (mvTwsiRead(0, &twsiSlave, &boardId, 1) != MV_OK) {
		mvPrintf("\n\n%s: TWSI Read for Marvell Board ID failed (%x) \n", __func__, EEPROM_I2C_ADDR);
		mvPrintf("\tUsing default board ID\n\n");
		gBoardId = MV_DEFAULT_BOARD_ID;
		return gBoardId;
	}

	DEBUG_INIT_FULL_S("boardId from HW = 0x");
	DEBUG_INIT_FULL_D(boardId, 2);
	DEBUG_INIT_FULL_S("\n");

	boardId &= 0x7; /* bits 0-2 */

	if (boardId < MV_MARVELL_BOARD_NUM && boardId >= 0)
		gBoardId = MARVELL_BOARD_ID_BASE + boardId;
	else {
		DEBUG_INIT_S("mvBoardIdGet: board id 0x");
		DEBUG_INIT_FULL_D(boardId, 8);
		DEBUG_INIT_S("is out of range. Using default board ID\n");
		gBoardId = MV_DEFAULT_BOARD_ID;
	}
#endif
	return gBoardId;
}

MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 value;

	value = (MV_MEMIO_LE32_READ(INTER_REGS_BASE | DEVICE_SAMPLE_AT_RESET1_REG) >> 15) & 0x1;

	switch (value) {
	case (0x0):
		return MV_BOARD_TCLK_250MHZ;
	case (0x1):
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_ERROR;
	}
}

MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 as base for Board ID: mask MSB to receive index for board ID*/
	return boardId & (MARVELL_BOARD_ID_MASK - 1);
}

/* Use flagTwsiInit global flag to init the Twsi once */
static int flagTwsiInit = -1;
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID)
{
	MV_TWSI_ADDR slave;
	MV_U32 tClock;
	if (flagTwsiInit == -1) {
		DEBUG_INIT_FULL_S("\n### mvHwsTwsiInitWrapper ###\n");
		slave.type = ADDR7_BIT;
		slave.address = 0;
		tClock = mvBoardTclkGet();
		if (tClock == MV_BOARD_TCLK_ERROR) {
			DEBUG_INIT_FULL_S("mvHwsTwsiInitWrapper: TClk read from the board is not supported\n");
			return MV_NOT_SUPPORTED;
		}

		mvTwsiInit(0, TWSI_SPEED, tClock, &slave, 0);
		flagTwsiInit = 1;
	}
	return MV_OK;
}

/************************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:	 	Reads GPIO input for suspend-wakeup indication.
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 indicating suspend wakeup status:
* 		0 - normal initialization, otherwise - suspend wakeup.
 ***************************************************************************/
MV_U32 mvSysEnvSuspendWakeupCheck(void)
{
	MV_U32 reg, boardIdIndex, gpio;
	MV_BOARD_WAKEUP_GPIO boardGpio[] = MV_BOARD_WAKEUP_GPIO_INFO;

	boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());
	if (!(sizeof(boardGpio)/sizeof(MV_BOARD_WAKEUP_GPIO) > boardIdIndex)) {
		mvPrintf("\nFailed loading Suspend-Wakeup information (invalid board ID)\n");
		return 0;
	}

	/* - Detect if Suspend-Wakeup is supported on current board
	 * - Fetch the GPIO number for wakeup status input indication */
	if (boardGpio[boardIdIndex].gpioNum == -1)
		return 0; /* suspend to RAM is not supported */
	else if (boardGpio[boardIdIndex].gpioNum == -2)
		return 1; /* suspend to RAM is supported but GPIO indication is not implemented - Skip */
	else
		gpio = boardGpio[boardIdIndex].gpioNum;

	/* Initialize MPP for GPIO (set MPP = 0x0) */
	reg = MV_REG_READ(MPP_CONTROL_REG(MPP_REG_NUM(gpio)));
	reg &= ~MPP_MASK(gpio);		/* reset MPP21 to 0x0, keep rest of MPP settings*/
	MV_REG_WRITE(MPP_CONTROL_REG(MPP_REG_NUM(gpio)), reg);

	/* Initialize GPIO as input */
	reg = MV_REG_READ(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)));
	reg |= GPP_MASK(gpio);
	MV_REG_WRITE(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)), reg);

	/* Check GPP for input status from PIC: 0 - regular init, 1 - suspend wakeup */
	reg = MV_REG_READ(GPP_DATA_IN_REG(GPP_REG_NUM(gpio)));

	return reg & GPP_MASK(gpio);
}

/************************************************************************************
* mvSysEnvModelGet
* DESCRIPTION:	 	Returns 16bit describing the device model (ID) as defined
*       		in Vendor ID configuration register
 ***************************************************************************/
MV_U16 mvSysEnvModelGet(MV_VOID)
{
	MV_U32	defaultCtrlId, ctrlId = MV_REG_READ(DEV_ID_REG);
	ctrlId = (ctrlId & (DEV_ID_REG_DEVICE_ID_MASK)) >> DEV_ID_REG_DEVICE_ID_OFFS;

	switch (ctrlId) {
	case MV_6820_DEV_ID:
	case MV_6810_DEV_ID:
	case MV_6811_DEV_ID:
	case MV_6828_DEV_ID:
	case MV_6920_DEV_ID:
	case MV_6928_DEV_ID:
		return ctrlId;
	default: /*Device ID Default for A38x: 6820 , for A39x: 6920 */
	#ifdef MV88F68XX
		defaultCtrlId =  MV_6820_DEV_ID;
	#else
		defaultCtrlId = MV_6920_DEV_ID;
	#endif
		mvPrintf("%s:Error retrieving device ID (%x), using default ID = %x \n", __func__, ctrlId, defaultCtrlId);
		return defaultCtrlId;
	}
}

/************************************************************************************
* mvSysEnvDeviceIdGet
* DESCRIPTION:	 	Returns enum (0..7) index of the device model (ID)
 ***************************************************************************/
MV_U32 gDevId = -1;
MV_U32 mvSysEnvDeviceIdGet(MV_VOID)
{
	char *deviceIdStr[7] = { "6810", "6820", "6811", "6828",
				"NONE", "6920", "6928" };

	if (gDevId != -1)
		return gDevId;

	gDevId = MV_REG_READ(DEVICE_SAMPLE_AT_RESET1_REG);
	gDevId = gDevId >> SAR_DEV_ID_OFFS & SAR_DEV_ID_MASK;
	mvPrintf("Detected Device ID %s\n" ,deviceIdStr[gDevId]);
	return gDevId;
}

/*******************************************************************************
* mvSysEnvDeviceRevGet - Get Marvell controller device revision number
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
MV_U8 mvSysEnvDeviceRevGet(MV_VOID)
{
    MV_U32 value;

    value = MV_REG_READ(DEV_VERSION_ID_REG);
    return ((value & (REVISON_ID_MASK)) >> REVISON_ID_OFFS);
}
/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
*
* DESCRIPTION: defines pointer to to DLB COnfiguration table
*
* INPUT: none
*
* OUTPUT: pointer to DLB COnfiguration table
*
* RETURN:
*       returns pointer to DLB COnfiguration table
*
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID)
{
#ifdef MV88F69XX
	return (&ddr3DlbConfigTable_A0[0]);
#else
	if (mvSysEnvDeviceRevGet() == MV_88F68XX_A0_ID)
		return (&ddr3DlbConfigTable_A0[0]);
	else
		return (&ddr3DlbConfigTable[0]);
#endif
}


#ifdef CONFIG_CMD_BOARDCFG
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_EEPROM_CONFIG_INFO;
MV_U32 boardOptionsConfig[MV_CONFIG_TYPE_MAX_OPTION];

/*******************************************************************************
* mvSysEnvConfigTypeGet
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
static MV_BOOL mvSysEnvConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
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

	DEBUG_INIT_FULL_S("mvSysEnvConfigTypeGet: Error: requested MV_CONFIG_TYPE_ID was not found\n");
	return MV_FALSE;
}

/*******************************************************************************
* mvSysEnvEpromRead -
*
* DESCRIPTION:
*	routine to read more that 1 byte from EEPROM
* INPUT:
*	byteNum - byteNumber to read
*	byteCnt - how many bytes to read/write
*	pData - pointer for 32bit
*
* OUTPUT:
*               None.
*
* RETURN:
*               reg value
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromRead(MV_U8 byteNum, MV_U32 *pData, MV_U32 byteCnt, MV_U8 addr)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 i;
	MV_U8 readVal;

	twsiSlave.slaveAddr.address = addr;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;

	*pData = 0x0;
	for ( i = 0 ; i < byteCnt; i++) {
		twsiSlave.offset = byteNum + i;
		if (mvTwsiRead(0, &twsiSlave, &readVal, 1) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEepromRead: Error: Read pattern from EEPROM failed\n");
			return MV_FAIL;
		}
		readVal = MV_32BIT_LE_FAST(readVal);
		(*pData) |= (readVal & 0x000000FF) << (8*byteCnt - 8*(i+1));
		readVal = 0x0;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvEpromWrite -
*
* DESCRIPTION:
*       routine to write more that 1 byte to EEPROM
* INPUT:
*       byteNum - byteNumber to write
*       byteCnt - how many bytes to read/write
*       pData - pointer for 32bit
*
* OUTPUT:
*               None.
*
* RETURN:
*               returns MV_OK on success, MV_ERROR on failure.
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromWrite(MV_U8 byteNum, MV_U32 *pData, MV_U32 byteCnt, MV_U8 addr)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 i;
	MV_U8 writeVal;

	twsiSlave.slaveAddr.address = addr;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;

	for (i = 0 ; i < byteCnt ; i++) {
		twsiSlave.offset = byteNum + i;
		writeVal = (*pData >> (8*i)) & 0xFF;
		if (mvTwsiWrite(0, &twsiSlave, &writeVal, 1) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEpromWrite: Error: write data to EEPROM failed\n");
			return MV_ERROR;
		}
		/* add delay to wait for the bus to be ready. */
		/* this is a temporary delay which will be removed once twsi bugs will be fixed */
		mvOsUDelay(5000);
	}
	return MV_OK;
}

/*******************************************************************************
* mvSysEnvTwsiProbe - Probe the given I2C chip address
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
static MV_STATUS mvSysEnvTwsiProbe(MV_U32 chip)
{
	MV_TWSI_ADDR eepromAddress;

	/* TWSI init */
	mvHwsTwsiInitWrapper();

	if (mvTwsiStartBitSet(0)) {
		DEBUG_INIT_S("mvSysEnvTwsiProbe: Transaction start failed\n");
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}

	eepromAddress.type = ADDR7_BIT;
	eepromAddress.address = chip;

	if (mvTwsiAddrSet(0, &eepromAddress, MV_TWSI_WRITE)) {
		DEBUG_INIT_S("mvSysEnvTwsiProbe: Failed to set slave address\n");
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}
	DEBUG_INIT_FULL_S("address 0x");
	DEBUG_INIT_FULL_D(chip, 2);
	DEBUG_INIT_FULL_S(" returned 0x");
	DEBUG_INIT_FULL_D(MV_REG_READ(TWSI_STATUS_BAUDE_RATE_REG(0)), 8);
	DEBUG_INIT_FULL_S("\n");

	/* issue a stop bit */
	mvTwsiStopBitSet(0);

	DEBUG_INIT_FULL_S("mvSysEnvTwsiProbe: successful I2C probe\n");
	return MV_TRUE; /* successful completion */
}

/*******************************************************************************
* mvSysEnvIsEepromEnabled - read EEPROM and verify if EEPROM exists
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
static MV_STATUS mvSysEnvIsEepromEnabled(void)
{
	MV_U8 addr = EEPROM_I2C_ADDR;

	DEBUG_INIT_FULL_S("mvSysEnvIsEepromEnabled probing for i2c chip 0x");
	DEBUG_INIT_FULL_D(addr, 2);
	DEBUG_INIT_FULL_S("\n");

	if (mvSysEnvTwsiProbe((MV_U32)addr) == MV_TRUE)
		return MV_OK;  /* EEPROM enabled */
	else
		return MV_FAIL; /* EEPROM disabled */
}

/*******************************************************************************
* mvSysEnvEpromReset - reset EEPROM to default content.
*
* DESCRIPTION:
*       This function resets the EEPROM content. used for first board boot.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if EEPROM is reset, else return MV_FAIL.
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromReset(void)
{
	MV_U32 data[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_U32 i;

	/* write default configuration to the EEPROM */
	for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4 ; i++) {
		data[i] = MV_BYTE_SWAP_32BIT(data[i]);
		if (mvSysEnvEpromWrite(i * 4, &data[i], 4, EEPROM_I2C_ADDR) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEpromReset: Error: Write default configuration to EEPROM failed\n");
			return MV_FAIL;
		}
	}

	/* write magic pattern to the EEPROM */
	data[0] = MV_BYTE_SWAP_32BIT(EEPROM_VERIFICATION_PATTERN);
	if (mvSysEnvEpromWrite(MV_BOARD_CONFIG_PATTERN_OFFSET, &data[0], 4, EEPROM_I2C_ADDR) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEpromReset: failed to write magic pattern to EEPROM\n");
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvEepromInit - Verify if the EEPROM have been initialized
*
* DESCRIPTION:
*       Verify if the EEPROM have been initialized:
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration section 1
*       [0x8-0xF](64bits) - board configuration section 2
*	[0x10-0x13](32bit) - board configuration internal data
*			     offset 0x13,  bits 0:1 - board configuration valid counter.
*	[0x14-0x17](32bit) - magic pattern
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
static MV_STATUS mvSysEnvEepromInit(void)
{
	MV_U32 data = 0;
	MV_U32 validCount = 0;
	MV_BOARD_CONFIG_TYPE_INFO configEnableInfo;
	MV_BOARD_CONFIG_TYPE_INFO configValidInfo;

	/* check if EEPROM is enabled */
	if (mvSysEnvIsEepromEnabled() != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: EEPROM doesn't exists on board\n");
		return MV_FAIL;
	}

	/* check for EEPROM pattern to see if this is the board's first boot */
	if (mvSysEnvEpromRead(MV_BOARD_CONFIG_PATTERN_OFFSET, &data, 4, EEPROM_I2C_ADDR) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read pattern from EEPROM failed.\n");
		return MV_FAIL;
	}

	/* if the magic pattern was not found, reset the EEPROM to default configuration. */
	if (data != EEPROM_VERIFICATION_PATTERN) {
		DEBUG_INIT_S("mvSysEnvEepromInit: First init of the board. loadind default configuration\n");
		if (mvSysEnvEpromReset() != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEepromInit: Error: failed resetting EEPROM\n");
			return MV_FAIL;
		}
		return MV_OK;
	}

	if (mvSysEnvConfigTypeGet(MV_CONFIG_BOARDCFG_EN, &configEnableInfo) != MV_TRUE)
		return MV_FAIL;

	/* the folowing formula is used to calculate the byte offset in the EEPROM according
	   to the byte number and bit offset as they are defined in the config info table. */
	configEnableInfo.offset = CALC_BYTE_OFFSET(configEnableInfo.offset, configEnableInfo.byteNum);

	/* check if board auto configuration is enabled */
	if (mvSysEnvEpromRead(configEnableInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.\n");
		return MV_FAIL;
	}

	/* if board auto configuration is disabled, return MV_ERROR to load default configuration */
	if ((data & configEnableInfo.mask) == 0x0) {
		DEBUG_INIT_S("mvSysEnvEepromInit: board auto configuration is not enabled.\n");
		return MV_FAIL;
	}

	if (mvSysEnvConfigTypeGet(MV_CONFIG_BOARDCFG_VALID, &configValidInfo) != MV_TRUE)
		return MV_FAIL;

	/* the folowing formula is used to calculate the byte offset in the EEPROM according
	   to the byte number and bit offset as they are defined in the config info table. */
	configValidInfo.offset = CALC_BYTE_OFFSET(configValidInfo.offset, configValidInfo.byteNum);

	/* board configuration is enabled. */
	/* bits 0&1 in offset 11 in the EEPROM are used as counters for board configuration validation.
	   each load of the board by EEPROM configuration, the counter is incremented, and when
	   it reaches 3 times, the configuration is set to default */
	if (mvSysEnvEpromRead(configValidInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.");
		return MV_FAIL;
	}

	validCount = (data & configValidInfo.mask);

	/* if the valid counter has reached 3, reset the counter, disable the board auto configuration
	   enable bit, and return MV_FAIL to load default configuration. */
	if (validCount == 3) {
		DEBUG_INIT_S("mvSysEnvEepromInit: board configuration from the EEPROM is not valid\n");

		/* reset the valid counter to 0 */
		data &= ~configValidInfo.mask;
		if (mvSysEnvEpromWrite(configValidInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK)
			DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");

		if (mvSysEnvEpromRead(configEnableInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.");
			return MV_FAIL;
		}

		/* disable the board auto config */
		DEBUG_INIT_S("mvSysEnvEepromInit: Disable board config.\n");
		data &= ~configEnableInfo.mask;
		if (mvSysEnvEpromWrite(configEnableInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK)
			DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");

		return MV_FAIL;
	}

	/* incremeant the valid counter by 1, and return MV_OK to load configuration from the EEPROM */
	validCount++;
	data &= ~configValidInfo.mask;
	data |= (validCount);

	if (mvSysEnvEpromWrite(configValidInfo.offset, &data, 1, EEPROM_I2C_ADDR) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvConfigInit
*
* DESCRIPTION: Initialize EEPROM configuration
*       1. initialize all board configuration fields
*       3. read relevant board configuration (using TWSI/EEPROM access)
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN:
*       MV_OK if initialize pass, MV_INIT_ERROR if read board topology fail,
*
*******************************************************************************/
static int flagConfigInit = -1;
MV_STATUS mvSysEnvConfigInit(void)
{
	MV_U8 i, readValue;
	MV_U32 defaultVal[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_U32 configVal[2] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_BOOL readSuccess = MV_FALSE, readFlagError = MV_TRUE;
	MV_STATUS res;

	if (flagConfigInit != -1)
		return MV_OK;

	flagConfigInit = 1;

	/* Read Board Configuration*/
	res = mvSysEnvEepromInit();
	if (res == MV_OK) {
		/* Read configuration data: 1st 8 bytes in  EEPROM, (read twice: each read of 4 bytes(32bit)) */
		for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4 ; i++)
			if (mvSysEnvEpromRead(i * 4, &configVal[i], 4, EEPROM_I2C_ADDR) != MV_OK) {
				DEBUG_INIT_S("mvSysEnvConfigInit: Error: Read board configuration from EEPROM failed\n");
				readFlagError = MV_FALSE;
			}
	}
	/* if mvSysEnvEepromInit or the EEPROM reading fails, load default configuration. */
	if ((res != MV_OK) || (readFlagError == MV_FALSE)) {
		DEBUG_INIT_S("mvSysEnvConfigInit: Setting default configurations\n");
		for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4; i++)
			configVal[i] = defaultVal[i];
	}

	/* Save values Locally in configVal[] */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION; i++) {
		/* Get board configuration field information (Mask, offset, etc..) */
		if (mvSysEnvConfigTypeGet(i, &configInfo) != MV_TRUE)
			continue;

		readValue = (configVal[configInfo.byteNum/4] & configInfo.mask) >> configInfo.offset;
		boardOptionsConfig[configInfo.configId] = readValue;
		readSuccess = MV_TRUE;
	}

	if (readSuccess == MV_FALSE) {
		DEBUG_INIT_FULL_S("mvSysEnvConfigInit: Error: Read board configuration from EEPROM failed\n");
		return MV_INIT_ERROR;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvConfigGet
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
MV_U32 mvSysEnvConfigGet(MV_CONFIG_TYPE_ID configField)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	if (configField < MV_CONFIG_TYPE_MAX_OPTION &&
		mvSysEnvConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		DEBUG_INIT_S("mvSysEnvConfigGet: Error: Requested board config is invalid for this board\n");
		return MV_ERROR;
	}

	return boardOptionsConfig[configField];
}

#endif /* CONFIG_CMD_BOARDCFG */

/*******************************************************************************
* mvSysEnvGetTopologyUpdateInfo
*
* DESCRIPTION: Read TWSI fields to update DDR topology structure
*
* INPUT: None
*
* OUTPUT: None, 0 means no topology update
*
* RETURN:
*       Bit mask of changes topology features
*
*******************************************************************************/
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo)
{
	MV_U8	configVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardEccModeArray[A38X_MV_MAX_MARVELL_BOARD_ID-A38X_MARVELL_BOARD_ID_BASE][5] = MV_TOPOLOGY_UPDATE;
	MV_U8 boardId = mvBoardIdGet();

	boardId = mvBoardIdIndexGet(boardId);

	/*Fix the topology for A380 by SatR values*/
	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading DDR topology configuration from EEPROM */
	if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvGetTopologyUpdateInfo: TWSI Read failed\n");
		return 0;
	}

	/*Set 16/32 bit configuration*/
	if( 0 == (configVal & DDR_SATR_CONFIG_MASK_WIDTH) ){
		/*16bit*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT] != 0)){
			topologyUpdateInfo->mvUpdateWidth = MV_TRUE;
			topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_16BIT;
		}
	}
	else{
		/*32bit*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT] !=0 )){
			topologyUpdateInfo->mvUpdateWidth = MV_TRUE;
			topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_32BIT;
		}
	}
	
	/*Set ECC/no ECC bit configuration*/
	if( 0 == (configVal & DDR_SATR_CONFIG_MASK_ECC) ){
		/*NO ECC*/
		topologyUpdateInfo->mvUpdateECC = MV_TRUE;
		topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_OFF;
	}
	else{
		/*ECC*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC] !=0) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC_PUP3] !=0) ){
			topologyUpdateInfo->mvUpdateECC = MV_TRUE;
			topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_ON;
			topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
			topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
		}
	}

	/*Set ECC pup bit configuration*/
	if( 0 == (configVal & DDR_SATR_CONFIG_MASK_ECC_PUP) ){
		/*PUP3*/
		if(	(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC_PUP3] !=0 )){
			if(topologyUpdateInfo->mvWidth == MV_TOPOLOGY_UPDATE_WIDTH_16BIT){
				topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
				topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP3;
			}
			else{
				if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0 ) ){
					mvPrintf("DDR Topology Update: ECC PUP3 not valid for 32bit mode, force ECC in PUP4\n");
					topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
					topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
				}
			}
		}
	}
	else{
		/*PUP4*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0 ) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC] !=0 )){
			topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
			topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
		}
	}

	return MV_OK;
}

