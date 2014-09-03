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
#include "mv_os.h"
#include "mvSysEnvLib.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "mv_seq_exec.h"

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
		gBoardId = ARMADA_38x_CUSTOMER_BOARD_ID0;
	#elif CONFIG_CUSTOMER_BOARD_1
		gBoardId = ARMADA_38x_CUSTOMER_BOARD_ID1;
	#endif
#else
	/* For Marvell Boards: read board ID from TWSI*/
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardId;

	DEBUG_INIT_FULL_S("\n### mvHwsBoardIdGet ###\n");

	twsiSlave.slaveAddr.address = BOARD_ID_GET_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading board id */
	DEBUG_INIT_FULL_S("mvHwsBoardIdGet: getting board id\n");
	if (mvTwsiRead(0, &twsiSlave, &boardId, 1) != MV_OK) {
		DEBUG_INIT_S("mvHwsBoardIdGet: TWSI Read failed\n");
		return (MV_U8)MV_INVALID_BOARD_ID;
	}

	DEBUG_INIT_FULL_S("boardId from HW = 0x");
	DEBUG_INIT_FULL_D(boardId, 2);
	DEBUG_INIT_FULL_S("\n");

	boardId &= 0x7; /* bits 0-2 */

	if (boardId < MV_MARVELL_BOARD_NUM && boardId >= 0)
		gBoardId = MARVELL_BOARD_ID_BASE + boardId;
	else {
		DEBUG_INIT_S("mvHwsBoardTopologyLoad: board id 0x");
		DEBUG_INIT_FULL_D(boardId, 8);
		DEBUG_INIT_S("is out of range. Using default board ID (DB-88F6820-BP)\n");
		gBoardId = DB_68XX_ID;
	}
#endif
	return gBoardId;
}

MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 value;

	value = MV_MEMIO_LE32_READ((INTER_REGS_BASE | DEVICE_SAMPLE_AT_RESET1_REG)) & (0x3 << 22);

	switch (value) {
	case 0:
		return MV_BOARD_TCLK_250MHZ;
	case (0x1 << 22):
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_ERROR;
	}
}

MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 as base for Board ID: mask MSB to receive index for board ID*/
	return boardId & (MARVELL_BOARD_ID_BASE - 1);
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

	/* - Detect if Suspend-Wakeup is supported on current board
	 * - Fetch the GPIO number for wakeup status input indication */
	boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());
	if (boardGpio[boardIdIndex].gpioNum == -1)
		return 0;
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

#ifdef CONFIG_CMD_BOARDCFG
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_EEPROM_CONFIG_INFO;
MV_U32 boardOptionsConfig[MV_CONFIG_TYPE_MAX_OPTION];

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
			DEBUG_INIT_S("mvSysEnvEepromInit: Error: Read pattern from EEPROM failed\n");
			return MV_ERROR;
		}
		readVal = MV_32BIT_LE_FAST(readVal);
		(*pData) |= (readVal & 0x000000FF) << (32 - 8*(i+1));
		readVal = 0x0;
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
static MV_BOOL mvSysEnvIsEepromEnabled(void)
{
	MV_U8 addr = MV_INFO_TWSI_EEPROM_DEV;

	/* TBD - check if jumper exists */
	/* by default EEPROM is enabled */
	DEBUG_INIT_FULL_S("mvSysEnvIsEepromEnabled: EEPROM Jumper is enabled\n");

	DEBUG_INIT_FULL_S("mvSysEnvIsEepromEnabled probing for i2c chip 0x");
	DEBUG_INIT_FULL_D(addr, 2);
	DEBUG_INIT_FULL_S("\n");

	if (mvSysEnvTwsiProbe((MV_U32)addr) == MV_TRUE)
		return MV_TRUE;  /* EEPROM enabled */
	else
		return MV_FALSE; /* EEPROM disabled */
}

/*******************************************************************************
* mvSysEnvEepromInit - Verify if the EEPROM have been initialized
*
* DESCRIPTION:
*       Verify if the EEPROM have been initialized:
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
static MV_STATUS mvSysEnvEepromInit(void)
{
	MV_U32 pattern = 0;

	if (mvSysEnvIsEepromEnabled() != MV_TRUE) {
		DEBUG_INIT_S("mvSysEnvEepromInit: EEPROM doesn't exists on board\n");
		return MV_ERROR;
	}

	/* verify EEPROM: read 4 bytes at byte number 0x8 (read magic pattern) */
	if (mvSysEnvEpromRead(8, &pattern, 4, MV_INFO_TWSI_EEPROM_DEV) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: Read pattern from EEPROM failed\n");
		return MV_ERROR;
	}

	/* If EEPROM is initialized with magic pattern, continue and exit*/
	if (pattern == EEPROM_VERIFICATION_PATTERN)
		return MV_OK;

	DEBUG_INIT_S("\nmvSysEnvEepromInit: first use of EEPROM\n");
	return MV_ERROR;
}

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

	if (flagConfigInit != -1)
		return MV_OK;

	flagConfigInit = 1;

	/* Read Board Configuration*/
	if (mvSysEnvEepromInit() != MV_OK) {
		DEBUG_INIT_S("mvSysEnvConfigInit: Error: mvSysEnvEepromInit failed\n");
		readFlagError = MV_FALSE;
	}

	/* Read configuration data: 1st 8 bytes in  EEPROM, (read twice: each read of 4 bytes(32bit)) */
	for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT/4 ; i++)
		if (mvSysEnvEpromRead(i * 4, &configVal[i], 4, MV_INFO_TWSI_EEPROM_DEV) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvConfigInit: Error: Read pattern from EEPROM failed\n");
			readFlagError = MV_FALSE;
		}

	if (readFlagError == MV_FALSE) {
		DEBUG_INIT_S("mvSysEnvConfigInit: Warning Failed to init/read from EEPROM, set default configurations\n");
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
