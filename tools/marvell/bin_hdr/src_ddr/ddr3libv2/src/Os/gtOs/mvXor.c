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

#include "mvXor.h"
#include "mvXorRegs.h"
#include "printf.h"

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

static GT_U32 uiXorRegsCtrlBackup;
static GT_U32 uiXorRegsBaseBackup[MAX_CS + 1];
static GT_U32 uiXorRegsMaskBackup[MAX_CS + 1];

extern GT_U32 mvHwsDdr3TipMaxCSGet(GT_U32 devNum);

/*******************************************************************************
* mvSysXorInit
*
* DESCRIPTION:
*               This function initialize the XOR unit's windows.
* INPUT:
*       uiNumOfCS	- number of enabled chip select
*	uiCsEna		- bitmap of enabled chip selects:
*				bit[i] = 1 if chip select i is enabled (for 1 <= i <= 3)
*				bit[4] = if SRAM window is enabled
*	csSize		- size of DRAM chip select
*	baseDelta	- base start of the DRAM windows
*
*
*******************************************************************************/
GT_VOID mvSysXorInit(GT_U32 uiNumOfCS, GT_U32 uiCsEna, GT_U32 csSize, GT_U32 baseDelta)
{
	GT_U32 uiReg, ui, uiBase, uiCsCount, sizeMask;

	uiXorRegsCtrlBackup = MV_REG_READ(XOR_WINDOW_CTRL_REG(0, 0));
	for(ui = 0; ui < MAX_CS + 1; ui++)
		uiXorRegsBaseBackup[ui] = MV_REG_READ(XOR_BASE_ADDR_REG(0, ui));
	for(ui = 0; ui < MAX_CS + 1; ui++)
		uiXorRegsMaskBackup[ui] = MV_REG_READ(XOR_SIZE_MASK_REG(0, ui));

	uiReg = 0;
	for (uiCsCount = 0, ui = 0; uiCsCount < uiNumOfCS && ui < 8; ui++) {
		if(uiCsEna & (1 << ui)) {
			uiReg |= (0x1 << (ui));			/* Enable Window x for each CS */
			uiReg |= (0x3 << ((ui*2)+16)); 		/* Enable Window x for each CS */
			uiCsCount++;
		}
	}

	MV_REG_WRITE(XOR_WINDOW_CTRL_REG(0, 0), uiReg);

	for (uiCsCount = 0, ui = 0; uiCsCount < uiNumOfCS && ui < 8; ui++) {
		if(uiCsEna & (1<<ui)) {
			/* window x - Base - 0x00000000, Attribute 0x0E - DRAM */
			uiBase = csSize*ui + baseDelta;
			/* fixed size 2GB for each CS */
			sizeMask = 0x7FFF0000;
			switch(ui) {
				case 0:
					uiBase |= 0xE00;
					break;
				case 1:
					uiBase |= 0xD00;
					break;
				case 2:
					uiBase |= 0xB00;
					break;
				case 3:
					uiBase |= 0x700;
					break;
				case 4: /* SRAM */
					uiBase = 0x40000000;
					uiBase |= 0x1F00; /* configure as shared transaction */
					sizeMask = 0xF0000;
			}

			MV_REG_WRITE(XOR_BASE_ADDR_REG(0, ui), uiBase);

			/* window x - Size*/
			MV_REG_WRITE(XOR_SIZE_MASK_REG(0, ui), sizeMask);
			uiCsCount++;
		}
	}

	mvXorHalInit(1);
	return;
}

GT_VOID mvSysXorFinish(void)
{
	GT_U32 ui;

	MV_REG_WRITE(XOR_WINDOW_CTRL_REG(0, 0), uiXorRegsCtrlBackup);
	for(ui = 0; ui < MAX_CS + 1; ui++)
		MV_REG_WRITE(XOR_BASE_ADDR_REG(0, ui), uiXorRegsBaseBackup[ui]);
	for(ui = 0; ui < MAX_CS + 1; ui++)
		MV_REG_WRITE(XOR_SIZE_MASK_REG(0, ui), uiXorRegsMaskBackup[ui]);

	MV_REG_WRITE(XOR_ADDR_OVRD_REG(0, 0), 0);
}


/*******************************************************************************
* mvXorHalInit - Initialize XOR engine
*
* DESCRIPTION:
*               This function initialize XOR unit.
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       GT_BAD_PARAM if parameters to function invalid, GT_OK otherwise.
*******************************************************************************/
GT_VOID mvXorHalInit(GT_U32 xorChanNum)
{
	GT_U32 i;
	/* Abort any XOR activity & set default configuration */
	for (i = 0; i < xorChanNum; i++) {
		mvXorCommandSet(i, MV_STOP);
		mvXorCtrlSet(i, (1 << XEXCR_REG_ACC_PROTECT_OFFS) |
			     (4 << XEXCR_DST_BURST_LIMIT_OFFS) | (4 << XEXCR_SRC_BURST_LIMIT_OFFS)
#if defined(MV_CPU_BE)
/*				| (1 << XEXCR_DRD_RES_SWP_OFFS)
				| (1 << XEXCR_DWR_REQ_SWP_OFFS)
*/
			     | (1 << XEXCR_DES_SWP_OFFS)
#endif
		    );
	}

}

/*******************************************************************************
* mvXorCtrlSet - Set XOR channel control registers
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       GT_BAD_PARAM if parameters to function invalid, GT_OK otherwise.
* NOTE:
*    This function does not modify the OperationMode field of control register.
*
*******************************************************************************/
GT_STATUS mvXorCtrlSet(GT_U32 chan, GT_U32 xorCtrl)
{
	GT_U32 oldValue;

	/* update the XOR Engine [0..1] Configuration Registers (XExCR) */
	oldValue = MV_REG_READ(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)))
	    & XEXCR_OPERATION_MODE_MASK;
	xorCtrl &= ~XEXCR_OPERATION_MODE_MASK;
	xorCtrl |= oldValue;
	MV_REG_WRITE(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), xorCtrl);
	return GT_OK;
}

/*******************************************************************************
* mvXorMemInit -
*
* DESCRIPTION:
*
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*
*******************************************************************************/
GT_STATUS mvXorMemInit(GT_U32 chan, GT_U32 startPtr, GT_U32 blockSize, GT_U32 initValHigh, GT_U32 initValLow)
{
	GT_U32 temp;

	/* Parameter checking   */
	if (chan >= MV_XOR_MAX_CHAN) {
		return GT_BAD_PARAM;
	}
	if (MV_ACTIVE == mvXorStateGet(chan)) {
		return GT_BUSY;
	}
	if ((blockSize < XEXBSR_BLOCK_SIZE_MIN_VALUE) || (blockSize > XEXBSR_BLOCK_SIZE_MAX_VALUE)) {
		return GT_BAD_PARAM;
	}

	/* set the operation mode to Memory Init */
	temp = MV_REG_READ(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	temp &= ~XEXCR_OPERATION_MODE_MASK;
	temp |= XEXCR_OPERATION_MODE_MEM_INIT;
	MV_REG_WRITE(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), temp);

	/* update the startPtr field in XOR Engine [0..1] Destination Pointer
	   Register (XExDPR0) */
	MV_REG_WRITE(XOR_DST_PTR_REG(XOR_UNIT(chan), XOR_CHAN(chan)), startPtr);

	/* update the BlockSize field in the XOR Engine[0..1] Block Size
	   Registers (XExBSR) */
	MV_REG_WRITE(XOR_BLOCK_SIZE_REG(XOR_UNIT(chan), XOR_CHAN(chan)), blockSize);

	/* update the field InitValL in the XOR Engine Initial Value Register
	   Low (XEIVRL) */
	MV_REG_WRITE(XOR_INIT_VAL_LOW_REG(XOR_UNIT(chan)), initValLow);

	/* update the field InitValH in the XOR Engine Initial Value Register
	   High (XEIVRH) */
	MV_REG_WRITE(XOR_INIT_VAL_HIGH_REG(XOR_UNIT(chan)), initValHigh);

	/* start transfer */
	MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XESTART_MASK);

	return GT_OK;
}

/*******************************************************************************
* mvXorStateGet - Get XOR channel state.
*
* DESCRIPTION:
*       XOR channel activity state can be active, idle, paused.
*       This function retrunes the channel activity state.
*
* INPUT:
*       chan     - the channel number
*
* OUTPUT:
*       None.
*
* RETURN:
*       XOR_CHANNEL_IDLE    - If the engine is idle.
*       XOR_CHANNEL_ACTIVE  - If the engine is busy.
*       XOR_CHANNEL_PAUSED  - If the engine is paused.
*       MV_UNDEFINED_STATE  - If the engine state is undefind or there is no
*                             such engine
*
*******************************************************************************/
MV_STATE mvXorStateGet(GT_U32 chan)
{
	GT_U32 state;

	/* Parameter checking   */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(mvPrintf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return MV_UNDEFINED_STATE;
	}

	/* read the current state */
	state = MV_REG_READ(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	state &= XEXACTR_XESTATUS_MASK;

	/* return the state */
	switch (state) {
	case XEXACTR_XESTATUS_IDLE:
		return MV_IDLE;
	case XEXACTR_XESTATUS_ACTIVE:
		return MV_ACTIVE;
	case XEXACTR_XESTATUS_PAUSED:
		return MV_PAUSED;
	}
	return MV_UNDEFINED_STATE;
}

/*******************************************************************************
* mvXorCommandSet - Set command of XOR channel
*
* DESCRIPTION:
*       XOR channel can be started, idle, paused and restarted.
*       Paused can be set only if channel is active.
*       Start can be set only if channel is idle or paused.
*       Restart can be set only if channel is paused.
*       Stop can be set only if channel is active.
*
* INPUT:
*       chan     - The channel number
*       command  - The command type (start, stop, restart, pause)
*
* OUTPUT:
*       None.
*
* RETURN:
*       GT_OK on success , GT_BAD_PARAM on erroneous parameter, MV_ERROR on
*       undefind XOR engine mode
*
*******************************************************************************/
GT_STATUS mvXorCommandSet(GT_U32 chan, MV_COMMAND command)
{
	MV_STATE state;

	/* Parameter checking */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(mvPrintf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return GT_BAD_PARAM;
	}

	/* get the current state */
	state = mvXorStateGet(chan);

	/* command is start and current state is idle */
	if ((command == MV_START) && (state == MV_IDLE)) {
		MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XESTART_MASK);
		return GT_OK;
	}
	/* command is stop and current state is active */
	else if ((command == MV_STOP) && (state == MV_ACTIVE)) {
		MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XESTOP_MASK);
		return GT_OK;
	}
	/* command is paused and current state is active */
	else if (((MV_STATE)command == MV_PAUSED) && (state == MV_ACTIVE)) {
		MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XEPAUSE_MASK);
		return GT_OK;
	}
	/* command is restart and current state is paused */
	else if ((command == MV_RESTART) && (state == MV_PAUSED)) {
		MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XERESTART_MASK);
		return GT_OK;
	}

	/* command is stop and current state is active */
	else if ((command == MV_STOP) && (state == MV_IDLE))
		return GT_OK;

	/* illegal command */
	DB(mvPrintf("%s: ERR. Illegal command\n", __func__));

	return GT_BAD_PARAM;
}


/*******************************************************************************
* ddr3NewTipEccScrub - Scrub the DRAM
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
MV_VOID	ddr3NewTipEccScrub()
{
	MV_U32 cs_c,max_cs;
	MV_U32 uiCsEna = 0;
    mvPrintf("DDR Training Sequence - Start scrubbing \n");

	max_cs = mvHwsDdr3TipMaxCSGet(0);
	for (cs_c = 0; cs_c < max_cs; cs_c++)
		uiCsEna |= 1 << cs_c;

	mvSysXorInit(max_cs, uiCsEna, 0x80000000, 0);

	mvXorMemInit(0, 0x00000000, 0x80000000, 0xdeadbeef, 0xdeadbeef);
    /* wait for previous transfer completion */
    while (mvXorStateGet(0) != MV_IDLE);

	mvXorMemInit(0, 0x80000000, 0x40000000, 0xdeadbeef, 0xdeadbeef);
    /* wait for previous transfer completion */
    while (mvXorStateGet(0) != MV_IDLE);

    /* Return XOR State */
    mvSysXorFinish();

    mvPrintf("DDR Training Sequence - End scrubbing \n");
}

/*******************************************************************************
* mvXorTransfer - Transfer data from source to destination on one of
*                 three modes (XOR,CRC32,DMA)
*
* DESCRIPTION:
*       This function initiates XOR channel, according to function parameters,
*       in order to perform XOR or CRC32 or DMA transaction.
*       To gain maximum performance the user is asked to keep the following
*       restrictions:
*       1) Selected engine is available (not busy).
*       1) This module does not take into consideration CPU MMU issues.
*          In order for the XOR engine to access the appropreate source
*          and destination, address parameters must be given in system
*          physical mode.
*       2) This API does not take care of cache coherency issues. The source,
*          destination and in case of chain the descriptor list are assumed
*          to be cache coherent.
*       4) Parameters validity. For example, does size parameter exceeds
*          maximum byte count of descriptor mode (16M or 64K).
*
* INPUT:
*       chan          - XOR channel number. See GT_XOR_CHANNEL enumerator.
*       xorType       - One of three: XOR, CRC32 and DMA operations.
*       xorChainPtr   - address of chain pointer
*
* OUTPUT:
*       None.
*
* RETURS:
*       GT_BAD_PARAM if parameters to function invalid, GT_OK otherwise.
*
*******************************************************************************/
GT_STATUS mvXorTransfer(GT_U32 chan, MV_XOR_TYPE xorType, GT_U32 xorChainPtr)
{
	GT_U32 temp;

	/* Parameter checking */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(mvPrintf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return GT_BAD_PARAM;
	}
	if (MV_ACTIVE == mvXorStateGet(chan)) {
		DB(mvPrintf("%s: ERR. Channel is already active\n", __func__));
		return GT_BUSY;
	}
	if (0x0 == xorChainPtr) {
		DB(mvPrintf("%s: ERR. xorChainPtr is NULL pointer\n", __func__));
		return GT_BAD_PARAM;
	}

	/* read configuration register and mask the operation mode field */
	temp = MV_REG_READ(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	temp &= ~XEXCR_OPERATION_MODE_MASK;

	switch (xorType) {
	case MV_XOR:
		if (0 != (xorChainPtr & XEXDPR_DST_PTR_XOR_MASK)) {
			DB(mvPrintf("%s: ERR. Invalid chain pointer (bits [5:0] must "
				      "be cleared)\n", __func__));
			return GT_BAD_PARAM;
		}
		/* set the operation mode to XOR */
		temp |= XEXCR_OPERATION_MODE_XOR;
		break;

	case MV_DMA:
		if (0 != (xorChainPtr & XEXDPR_DST_PTR_DMA_MASK)) {
			DB(mvPrintf("%s: ERR. Invalid chain pointer (bits [4:0] must "
				      "be cleared)\n", __func__));
			return GT_BAD_PARAM;
		}
		/* set the operation mode to DMA */
		temp |= XEXCR_OPERATION_MODE_DMA;
		break;

	case MV_CRC32:
		if (0 != (xorChainPtr & XEXDPR_DST_PTR_CRC_MASK)) {
			DB(mvPrintf("%s: ERR. Invalid chain pointer (bits [4:0] must "
				      "be cleared)\n", __func__));
			return GT_BAD_PARAM;
		}
		/* set the operation mode to CRC32 */
		temp |= XEXCR_OPERATION_MODE_CRC;
		break;

	default:
		return GT_BAD_PARAM;
	}

	/* write the operation mode to the register */
	MV_REG_WRITE(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), temp);
	/* update the NextDescPtr field in the XOR Engine [0..1] Next Descriptor
	   Pointer Register (XExNDPR) */
	MV_REG_WRITE(XOR_NEXT_DESC_PTR_REG(XOR_UNIT(chan), XOR_CHAN(chan)), xorChainPtr);

	/* start transfer */
	MV_REG_BIT_SET(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)), XEXACTR_XESTART_MASK);

	return GT_OK;
}
