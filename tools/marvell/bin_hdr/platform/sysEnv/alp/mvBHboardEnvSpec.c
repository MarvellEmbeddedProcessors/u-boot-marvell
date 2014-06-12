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

#include "mvBHboardEnvSpec.h"

/***************************   defined ******************************/
#define FREQ_MODES_NUM_6610		0
#define FREQ_MODES_NUM_6650		4
#define FREQ_MODES_NUM_6660		5


#define MV_USER_SAR_FREQ_MODES { \
		{ 6,  400,  400, 200 }, \
		{ 14, 600,  400, 300 }, \
		{ 20, 800,  400, 400 }, \
		{ 21, 800,  534, 400 }, \
		{ 25, 1000, 500, 500 }, \
};

/******************   Structures  ******************************/
typedef struct {
	MV_U8 id;
	MV_U32 cpuFreq;
	MV_U32 ddrFreq;
	MV_U32 l2Freq;
} MV_FREQ_MODE;

/****************************  Local function *****************************************/

/****************************  function implementation *****************************************/

/*******************************************************************************
* mvBoardFreqModesNumGet
*
* DESCRIPTION: Return the number of supported frequency modes for this SoC
*
*
* INPUT:
*      None.
*
* OUTPUT:
*      None.
*
* RETURN:
*      Number of supported frequency modes
*
*******************************************************************************/
MV_U32 mvBoardFreqModesNumGet(MV_VOID)
{
	MV_U32 freqNum;

	switch (mvCtrlModelGet()) {
	case MV_6610_DEV_ID:
		freqNum = FREQ_MODES_NUM_6610;
		break;
	case MV_6650_DEV_ID:
		freqNum = FREQ_MODES_NUM_6650;
		break;
	case MV_6660_DEV_ID:
		freqNum = FREQ_MODES_NUM_6660;
		break;
	default:
		return MV_ERROR;
	}

	return freqNum;
}

/*******************************************************************************
* mvCpuL2ClkGet - Get the CPU L2 (CPU bus clock)
*
* DESCRIPTION:
*       This routine extract the CPU L2 clock from Sample at reset
*       Read S@R frequency mode value, and translate vvalue to coresponding L2 clock
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvCpuL2ClkGet(MV_VOID)
{
	MV_FREQ_MODE freqTable[] = MV_USER_SAR_FREQ_MODES;
	MV_U32 i, freqSatR, maxFreqModes = mvBoardFreqModesNumGet();

	/* read SatR value for frequency mode */
	freqSatR = MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR); /* 0xE8200 */
	freqSatR = ((freqSatR & REG_SAMPLE_RESET_CPU_FREQ_MASK) >> REG_SAMPLE_RESET_CPU_FREQ_OFFS);

	/* Get mode values for CPU,L2,DDR frequency */
	for (i = 0; i < maxFreqModes; i++)
		if (freqSatR == freqTable[i].id)
			return (MV_U32)(1000000 * freqTable[i].l2Freq);
	return MV_ERROR;
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
static MV_U32 gBoardId = -1;
MV_U32 mvBoardIdGet(MV_VOID)
{
	if (gBoardId != -1)
		return gBoardId;

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	#ifdef CONFIG_CUSTOMER_BOARD_0
		gBoardId = AVANTA_LP_CUSTOMER_BOARD_ID0;
	#elif CONFIG_CUSTOMER_BOARD_1
		gBoardId = AVANTA_LP_CUSTOMER_BOARD_ID1;
	#endif
#else

	MV_U32 readValue;

	readValue = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	readValue = ((readValue & (0xF0)) >> 4);

	if (readValue < MV_MARVELL_BOARD_NUM && readValue >= 0)
		gBoardId = MARVELL_BOARD_ID_BASE + readValue;
	else {
		DEBUG_INIT_S("mvBoardIdGet: board id 0x");
		DEBUG_INIT_FULL_D(readValue, 8);
		DEBUG_INIT_S("is out of range.\n");
		return MV_INVALID_BOARD_ID;
	}
#endif

	return gBoardId;
}

/*******************************************************************************
* mvBoardTclkGet -
*
* DESCRIPTION: this function read the TCLK frequency from S@R register and return
*              the TCLK frequency
*
* INPUT:
*        None
*
* OUTPUT:
*        None.
*
* RETURN:
*        TCLK frequency
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
    MV_U32 value = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
    value= ((value & (MSAR_TCLK_MASK)) >> MSAR_TCLK_OFFS);

    switch (value) {
        case 0:
            return MV_BOARD_TCLK_166MHZ;
        case 1:
            return MV_BOARD_TCLK_200MHZ;
        default:
            DEBUG_INIT_S("Error : Board: Read from S@R fail\n");
            return MV_BOARD_TCLK_200MHZ;
    }
}
/*******************************************************************************
* mvBoardTwsiGet -
*
* DESCRIPTION:
*
* INPUT:
*           device num - one of three devices
*           reg num - 0 or 1
*
* OUTPUT:
*                         None.
*
* RETURN:
*                         reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiGet(MV_U32 address, MV_U8 devNum, MV_U8 regNum, MV_BOOL isMoreThen256, MV_U8 *pData)
{
     MV_TWSI_SLAVE twsiSlave;
     MV_TWSI_ADDR slave;

     /* TWSI init */
     slave.type = ADDR7_BIT;
     slave.address = 0;

     DEBUG_INIT_FULL_C(">>> in mvBoardTwsiGet 1, address=0x",address, 2);
     DEBUG_INIT_FULL_C(" devmum=0x", devNum, 2);
     DEBUG_INIT_FULL_C(" regnum=0x", regNum, 2);
     DEBUG_INIT_FULL_S("\n");

     mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

     /* Read MPP module ID */
     DEBUG_INIT_FULL_S("Board: Read S@R device read\n");

     twsiSlave.slaveAddr.address = address;
     twsiSlave.slaveAddr.type = ADDR7_BIT;
     twsiSlave.validOffset = MV_TRUE;
     /* Use offset as command */
     twsiSlave.offset = regNum;
	 twsiSlave.moreThen256 = isMoreThen256;

     if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, 1)) {
            DEBUG_INIT_S("TWSI Read failed\n");
            return MV_ERROR;
     }
     DEBUG_INIT_FULL_C("Board: Read TWSI succeeded data=0x",*pData, 2);

     return MV_OK;
}
/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit describing Marvell controller ID
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
    MV_U32 ctrlId, satr0;

    ctrlId = MV_REG_READ(DEV_ID_REG);
    ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;

    if (ctrlId == 0x6660)
	return MV_6660_DEV_ID;

    satr0 = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
    satr0 &= SATR_DEVICE_ID_2_0_MASK;
    if (satr0 == 0)
	return MV_6650_DEV_ID;
    return MV_6610_DEV_ID;
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
MV_U8 mvCtrlRevisionGet(MV_VOID)
{
	static MV_U8 ctrlRev = 0xFF;
	MV_U32 value;

	if(ctrlRev != 0xFF) {
		return ctrlRev;
	}

	value = MV_REG_READ(DEV_VERSION_ID_REG);
	ctrlRev = ((value & (REVISON_ID_MASK) ) >> REVISON_ID_OFFS);
	return ctrlRev;
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
	return boardId & (MARVELL_BOARD_ID_BASE - 1);
}
