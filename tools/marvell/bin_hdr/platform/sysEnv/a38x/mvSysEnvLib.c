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
