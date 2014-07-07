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
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvHighSpeedEnvSpec.h"
#include "mvSysEnvLib.h"
#include "mvCtrlPex.h"
#include "mv_seq_exec_ext.h"


#if defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#endif

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "printf.h"

static MV_U32 gBoardId = -1;
MV_U32 mvBoardIdGet(MV_VOID)
{
#if defined(CONFIG_ALLEYCAT3)
	MV_U8 readValue = 0;
#endif
	if (gBoardId != -1)
		return gBoardId;

/* Customer board ID's */
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	#ifdef CONFIG_BOBCAT2
	#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = BC2_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = BC2_CUSTOMER_BOARD_ID1;
		#endif
	#elif defined CONFIG_ALLEYCAT3
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = AC3_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = AC3_CUSTOMER_BOARD_ID1;
		#endif
	#endif
#else
/* BobCat2 Board ID's */
	#if defined(DB_BOBCAT2)
		gBoardId = DB_DX_BC2_ID;
	#elif defined(RD_BOBCAT2)
		gBoardId = RD_DX_BC2_ID;
	#elif defined(RD_MTL_BOBCAT2)
		gBoardId = RD_MTL_BC2;
	#else
		/* AlleyCat3 Board ID's */
		if (mvBoardSarBoardIdGet(&readValue) != MV_OK) {
			mvPrintf("%s: Error obtaining Board ID\n", __func__);
			mvPrintf("%s: Set default board ID to DB-DXAC3-MM\n", __func__);
			readValue = DB_AC3_ID - AC3_MARVELL_BOARD_ID_BASE;
		}

		if (readValue < AC3_MARVELL_BOARD_NUM)
			gBoardId = AC3_MARVELL_BOARD_ID_BASE + readValue;
		else {
			mvPrintf("%s: Error: read wrong Board ID (%d)\n", __func__, readValue);
			mvPrintf("%s: Set default board ID to DB-DXAC3-MM\n", __func__);
			gBoardId = DB_AC3_ID;
		}
	#endif
#endif

	return gBoardId;
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
	return boardId & (BOARD_ID_INDEX_MASK - 1);
}


/*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	return MV_BOARD_TCLK_200MHZ;
}

/*******************************************************************************/
MV_U8 mvHwsBoardIdGet(MV_VOID)
{
	return 0;
}

/*******************************************************************************/
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *value)
{
	MV_U8			boardId;
	MV_TWSI_SLAVE	twsiSlave;

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = 0x4D;	/* TODO - change to 0x50 - address of init EEPROM */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 1;	/* TODO change to 7 - the LSB of the first address-value pair) */
	twsiSlave.moreThen256 = MV_FALSE;

	if (MV_ERROR == mvTwsiRead(0, &twsiSlave, &boardId, 1))
		return MV_ERROR;

	*value = (boardId & 0x7);

	return MV_OK;
}

