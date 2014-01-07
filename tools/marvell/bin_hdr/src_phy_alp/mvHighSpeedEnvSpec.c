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
#include "mvHighSpeedEnvSpec.h"
#include "mvUart.h"

/***************************   defined ******************************/

#define MV_IO_EXP_MAX_REGS    3

#define BOARD_DEV_TWSI_EEPROM               0x55
#define BOARD_DEV_TWSI_IO_EXPANDER          0x21
#define BOARD_DEV_TWSI_IO_EXPANDER_JUMPER1  0x24

/**************************** globals *****************************/

MV_BOARD_TOPOLOGY_CONFIG boardTopologyConfig[] =
{
/*  board name					Lane 1				Lane 2					    Lane3     				Sgmii Speed*/
	{"RD_88F6650_BP",		{SERDES_UNIT_PEX,	SERDES_UNIT_UNCONNECTED,	SERDES_UNIT_UNCONNECTED},	MV_SGMII_NA},
	{"DB_88F6650_BP",		{SERDES_UNIT_PEX,	SERDES_UNIT_UNCONNECTED,	SERDES_UNIT_UNCONNECTED},	MV_SGMII_NA},
	{"RD_88F6660_BP",		{SERDES_UNIT_PEX,	SERDES_UNIT_SATA,			SERDES_UNIT_USB3},			MV_SGMII_NA},
	{"DB_88F6660_BP",		{SERDES_UNIT_PEX,	SERDES_UNIT_UNCONNECTED,	SERDES_UNIT_UNCONNECTED},	MV_SGMII_NA},/*Lane 2 and Lane 3 configuration is set in run time*/
	{"AVANTA_LP_CUSTOMER",	{SERDES_UNIT_PEX,	SERDES_UNIT_SATA,			SERDES_UNIT_USB3	   },	MV_SGMII_GEN1},
};

/****************************  function implementation *****************************************/

/*******************************************************************************
* mvReverseBits
*
* DESCRIPTION:
*       This function Reverts the direction of the bits (LSB to MSB and vice versa)
*
* INPUT:
*	num - MV_U8 number to revert
*
* OUTPUT:
*       Reverted number
*
* RETURN:
*	None
*
*******************************************************************************/
static MV_U8 mvReverseBits(MV_U8 num)
{
	num = (num & 0xF0) >> 4 | (num & 0x0F) << 4;
	num = (num & 0xCC) >> 2 | (num & 0x33) << 2;
	num = (num & 0xAA) >> 1 | (num & 0x55) << 1;
	return num;
}

/*******************************************************************************
* mvBoardDb6660IsEepromEnabled - read jumper and verify if EEPROM is enabled
*
* DESCRIPTION:
*       This function returns MV_TRUE if board configuration jumper is set to EEPROM.
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
MV_BOOL mvBoardDb6660IsEepromEnabled()
{
	MV_U8 regVal;

	if (MV_OK == mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER_JUMPER1, 0, 0, MV_FALSE, &regVal))
		return ((regVal & 0x80)? MV_FALSE : MV_TRUE);
	return MV_FALSE;
}

/*******************************************************************************
* mvCtrlBoardTopologyConfigGet - read Board Configuration, from EEPROM / Dip Switch
*
* DESCRIPTION:
*       This function reads all board configuration from EEPROM / Dip Switch:
*           1. read the EEPROM enable jumper, and read from configured device
*           2. read first 2 registers for all boards
*           3. read specific registers for specific boards
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
MV_STATUS mvBoardDb6660LaneConfigGet(MV_U8 *tempVal)
{
	MV_STATUS rc1, rc2;
	MV_BOOL isEepromEnabled = mvBoardDb6660IsEepromEnabled();
	MV_U32 address = (isEepromEnabled ? BOARD_DEV_TWSI_EEPROM : BOARD_DEV_TWSI_IO_EXPANDER);
	MV_BOOL isMoreThen256 = (address == BOARD_DEV_TWSI_EEPROM) ? MV_TRUE : MV_FALSE;
	MV_U8 temp0,temp1;

	rc1 = mvBoardTwsiGet(address, 0, 0, isMoreThen256, &tempVal[0]); /* EEPROM/Dip Switch Reg#0 */
	rc2 = mvBoardTwsiGet(address, 0, 1, isMoreThen256, &tempVal[1]);  /* EEPROM/Dip Switch Reg#1 */

	/*
	* Workaround for DIP Switch IO Expander 0x21 bug in DB-6660 board
	* Bug: Pins at IO expander 0x21 are reversed (only on DB-6660)
	* Solution: after reading IO expander, reverse bits of both registers
	*/
	if (isEepromEnabled != MV_TRUE) {
		/* Reverse all BITS */
		tempVal[0] = mvReverseBits(tempVal[0]);
		tempVal[1] = mvReverseBits(tempVal[1]);

		/* Swap field's MSB with LSB */
		temp0 = (tempVal[0] & 0x18) >> 3;
		temp0 = mvReverseBits(temp0) >> 6;
		tempVal[0] &= ~0x18;
		tempVal[0] |= temp0 << 3;
		temp1 = (tempVal[1] & 0x18) >> 3;
		temp1 = mvReverseBits(temp1) >> 6;
		tempVal[1] &= ~0x18;
		tempVal[1] |= temp1 << 3;
	}
	/* verify that all TWSI reads were successfully */
	if ((rc1 != MV_OK) || (rc2 != MV_OK))
		return MV_ERROR;

	return MV_OK;
}

/*****************************************************************:**************
* mvDb6660UpdateBoardTopologyConfig
*
* DESCRIPTION: read DB-6660 board configuration (using TWSI/EEPROM access)
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_STATUS mvDb6660UpdateBoardTopologyConfig(void)
{
	MV_U8 configVal[MV_IO_EXP_MAX_REGS];
	MV_U8 tmp;

	/*Read rest of Board Configuration, EEPROM / Dip Switch access read : */

	/* lane 1 config */
	if (mvBoardDb6660LaneConfigGet(configVal) == MV_OK) {
		tmp = ((configVal[1] & 0x18) >> 3);

		switch (tmp) {
		case 0:
			boardTopologyConfig[DB_88F6660_BP_ID].serdesTopology.lane1 = SERDES_UNIT_PEX;
			break;
		case 1:
			boardTopologyConfig[DB_88F6660_BP_ID].serdesTopology.lane1 = SERDES_UNIT_SGMII;
			break;
		case 2:
			boardTopologyConfig[DB_88F6660_BP_ID].serdesTopology.lane1 = SERDES_UNIT_SATA;
			break;
		case 3:
		default:
			DEBUG_INIT_S("Error: Read board configuration (SERDES LANE1) from EEPROM/Dip Switch failed\n");
			return MV_INIT_ERROR;

		}

		/* lane 2 config */
		boardTopologyConfig[DB_88F6660_BP_ID].serdesTopology.lane2 = ((configVal[1] & 0x20) >> 5)? SERDES_UNIT_SGMII:SERDES_UNIT_SATA;

		/* lane 3 config */
		boardTopologyConfig[DB_88F6660_BP_ID].serdesTopology.lane3 = ((configVal[1] & 0x40) >> 6)? SERDES_UNIT_SGMII:SERDES_UNIT_USB3;
	}
	else{
		DEBUG_INIT_S("Error: Read board configuration from EEPROM/Dip Switch failed \n");
		DEBUG_INIT_S("setting default Serdes lanes configuration: PCIe-0, PCIe-1 SATA2, USB3\n");
	}

	/* update SGMII speed config */
	boardTopologyConfig[DB_88F6660_BP_ID].sgmiiSpeed = (configVal[0] & 0x40) ? MV_SGMII_GEN2 : MV_SGMII_GEN1;

	return MV_OK;
}

/*****************************************************************:**************
* mvBoardUpdateSerdesTopology
*
* DESCRIPTION: Updates the topology structure of a specific board according to dynamic configuration
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_STATUS mvBoardUpdateBoardTopologyConfig(MV_U32 boardId)
{
	/* this routine must be implemented, in order to use dynamic Serdes /SGMII settings configuration.
		1. scan requested configuration 2. update boardTopologyConfig[] according to config */
	switch(boardId) {
	case DB_88F6660_BP_ID:
		return mvDb6660UpdateBoardTopologyConfig();
	case AVANTA_LP_CUSTOMER:
		return MV_OK;
	default:
		return MV_OK;
	}

	return MV_OK;
}
