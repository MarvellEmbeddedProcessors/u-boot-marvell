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

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include "twsi/mvTwsi.h"
#include "boardEnv/mvBoardEnvLib.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#define DB1(x)  x
#else
#define DB(x)
#define DB1(x)
#endif

/* SAR defines when Connected to MSYS (Only Bc2 & BOBK) */
#define MV_BOARD_CTRL_I2C_ADDR_MSYS	0x0
#define TWSI_CHANNEL_MSYS			0
#define TWSI_SPEED_MSYS		20000 /* wa for bits 1,2 in 0x4c. Mmust lower
					 100000 -> 20000 . adiy, erez*/

MV_U8 tread_msys(MV_U8 addr, int reg, MV_BOOL moreThen256)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	DB(printf("tread_msys, DevAddr = 0x%x\n", addr));

	/* TWSI init */
	slave.address = MV_BOARD_CTRL_I2C_ADDR_MSYS;
	slave.type = ADDR7_BIT;

	mvTwsiInit(TWSI_CHANNEL_MSYS, TWSI_SPEED_MSYS, mvBoardTclkGet(), &slave, 0);

	/* read SatR */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = addr ;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = reg;
	twsiSlave.moreThen256 = moreThen256;

	if (MV_OK != mvTwsiRead(TWSI_CHANNEL_MSYS, &twsiSlave, &data, 1)) {
		DB(printf("tread_msys : twsi read fail\n"));
		return MV_ERROR;
	}
	DB(printf("tread_msys: twsi read succeeded, data = 0x%x\n", data));

	return data;
}

MV_STATUS twrite_msys(MV_U8 addr, int reg, MV_U8 regVal, MV_BOOL moreThen256)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	/* printf(">>> in twrite_msys, addr=0x%x, reg = 0x%x, val=0x%x\n", addr, reg, regVal);*/
	/* TWSI init */
	slave.address = MV_BOARD_CTRL_I2C_ADDR_MSYS;
	slave.type = ADDR7_BIT;

	mvTwsiInit(TWSI_CHANNEL_MSYS, TWSI_SPEED_MSYS, mvBoardTclkGet(), &slave, 0);

	/* write SatR */
	twsiSlave.slaveAddr.address = addr;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = reg;
	twsiSlave.moreThen256 = moreThen256;

	data = regVal;
	if (MV_OK != mvTwsiWrite(TWSI_CHANNEL_MSYS, &twsiSlave, &data, 1)) {
		DB(mvOsPrintf("twrite_msys: twsi write fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("twrite_msys: twsi write succeeded\n"));

	return MV_OK;
}

MV_STATUS check_twsi_msys(void)
{
	MV_U8 reg = tread_msys(0x4c, 0, MV_FALSE);
	DB(printf("\ncheck_twsi_msys: read_MSYS= 0x%x\n", reg));
	if (reg == 0xff)
		return MV_ERROR;
	else
		return MV_OK;
}
