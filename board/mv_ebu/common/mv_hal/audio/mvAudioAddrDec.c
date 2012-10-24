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
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvAudioRegs.h"

#ifndef MV_AUDIO_SKIP_WIN_DECODING
extern MV_STATUS mvAudioSetAddDecMap(MV_UNIT_WIN_INFO *pAddrDecWinMap);
#endif

/*******************************************************************************
* mvAudioWinWrite
*
* DESCRIPTION:
*	This function writes the address decoding registers according to the
*	given window configuration.
*
* INPUT:
*       unit	    - The Audio unit number to configure.
*       winNum	    - AUDIO target address decode window number.
*       pAddrDecWin - AUDIO target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*   MV_OK on success,
*	MV_BAD_PARAM if winNum is invalid.
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvAudioWinWrite(MV_U32 unit, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
	MV_U32 baseReg;
	MV_U32 sizeReg;
	MV_U32 size;
	MV_U32 alignment;

	/* Parameter checking   */
	if (winNum >= MV_AUDIO_MAX_ADDR_DECODE_WIN) {
		mvOsPrintf("%s: ERR. Invalid win num %d\n", __func__, winNum);
		return MV_BAD_PARAM;
	}

	/* check if address is aligned to the size */
	if (MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size)) {
		mvOsPrintf("mvAudioWinWrite:Error setting AUDIO window %d to "
			   "Address 0x%08x is unaligned to size 0x%x.\n",
			   winNum, pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	size = pAddrDecWin->addrWin.size;
	if (!MV_IS_POWER_OF_2(size)) {
		mvOsPrintf("mvAudioWinWrite: Error setting AUDIO window %d. "
			   "Window size is not a power to 2.", winNum);
		return MV_BAD_PARAM;
	}

	alignment = 1 << MV_AUDIO_WIN_SIZE_OFFSET;

	sizeReg = (size / alignment - 1) << MV_AUDIO_WIN_SIZE_OFFSET;

	/* BaseLow[31:16] => base register [31:16]          */
	baseReg = pAddrDecWin->addrWin.baseLow & MV_AUDIO_WIN_BASE_MASK;

	/* set attributes */
	sizeReg &= ~MV_AUDIO_WIN_ATTR_MASK;
	sizeReg |= (pAddrDecWin->attrib << MV_AUDIO_WIN_ATTR_OFFSET);

	/* set target ID */
	sizeReg &= ~MV_AUDIO_WIN_TARGET_MASK;
	sizeReg |= (pAddrDecWin->targetId << MV_AUDIO_WIN_TARGET_OFFSET);

	if (pAddrDecWin->enable == MV_TRUE)
		sizeReg |= MV_AUDIO_WIN_ENABLE_MASK;
	else
		sizeReg &= ~MV_AUDIO_WIN_ENABLE_MASK;

	MV_REG_WRITE(MV_AUDIO_WIN_CTRL_REG(unit, winNum), sizeReg);
	MV_REG_WRITE(MV_AUDIO_WIN_BASE_REG(unit, winNum), baseReg);
	return MV_OK;
}

/*******************************************************************************
* mvAudioWinRead
*
* DESCRIPTION:
*       Read AUDIO peripheral target address window.
*
* INPUT:
*       winNum - AUDIO target address decode window number.
*
* OUTPUT:
*       pAddrDecWin - AUDIO target window data structure.
*
* RETURN:
*	MV_BAD_PARAM if winNum is invalid.
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvAudioWinRead(MV_U32 unit, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
	MV_U32 baseReg, sizeReg;
	MV_U32 sizeRegVal;
	MV_U32 alignment;

	if (winNum >= MV_AUDIO_MAX_ADDR_DECODE_WIN) {
		mvOsPrintf("%s : ERR. Invalid winNum %d\n", __func__, winNum);
		return MV_BAD_PARAM;
	}

	baseReg = MV_REG_READ(MV_AUDIO_WIN_BASE_REG(unit, winNum));
	sizeReg = MV_REG_READ(MV_AUDIO_WIN_CTRL_REG(unit, winNum));

	alignment = 1 << MV_AUDIO_WIN_SIZE_OFFSET;
	sizeRegVal = (sizeReg & MV_AUDIO_WIN_SIZE_MASK) >> MV_AUDIO_WIN_SIZE_OFFSET;

	pAddrDecWin->addrWin.size = (sizeRegVal + 1) * alignment;

	/* Extract base address                                             */
	/* Base register [31:16] ==> baseLow[31:16]                 */
	pAddrDecWin->addrWin.baseLow = baseReg & MV_AUDIO_WIN_BASE_MASK;
	pAddrDecWin->addrWin.baseHigh = 0;

	/* attrib and targetId */
	pAddrDecWin->attrib = (sizeReg & MV_AUDIO_WIN_ATTR_MASK) >> MV_AUDIO_WIN_ATTR_OFFSET;
	pAddrDecWin->targetId = (sizeReg & MV_AUDIO_WIN_TARGET_MASK) >> MV_AUDIO_WIN_TARGET_OFFSET;

	/* Check if window is enabled   */
	if (sizeReg & MV_AUDIO_WIN_ENABLE_MASK)
		pAddrDecWin->enable = MV_TRUE;
	else
		pAddrDecWin->enable = MV_FALSE;

	return MV_OK;
}

/*******************************************************************************
* mvAudioWinInit
*
* DESCRIPTION:
*       Initialize the AUDIO peripheral target address window.
*
* INPUT:
*	unit	 : The unit number to initialize the address decoding for.
*	addWinMap: An array holding the address decoding information for the
*		    system.
*
* OUTPUT:
*     None.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvAudioWinInit(MV_U32 unit, MV_UNIT_WIN_INFO *addrWinMap)
{
	MV_32 winNum;
	MV_UNIT_WIN_INFO *cpuAddrDecWin;

	/* Initiate Audio address decode */

	/* First disable all address decode windows */
	for (winNum = 0; winNum < MV_AUDIO_MAX_ADDR_DECODE_WIN; winNum++) {
		MV_U32 regVal = MV_REG_READ(MV_AUDIO_WIN_CTRL_REG(unit, winNum));
		regVal &= ~MV_AUDIO_WIN_ENABLE_MASK;
		MV_REG_WRITE(MV_AUDIO_WIN_CTRL_REG(unit, winNum), regVal);
	}

	for (winNum = 0; winNum < MV_AUDIO_MAX_ADDR_DECODE_WIN; winNum++) {
		/* We will set the Window to DRAM_CS0 in default */
		/* first get attributes from CPU If */
		cpuAddrDecWin = &addrWinMap[SDRAM_CS0];

		if (cpuAddrDecWin->enable == MV_TRUE) {
			if (MV_OK != mvAudioWinWrite(unit, winNum, cpuAddrDecWin))
				return MV_ERROR;
		}
	}

#ifndef MV_AUDIO_SKIP_WIN_DECODING
	/* Initialize the address decoding map in the audio driver */
	mvAudioSetAddDecMap(addrWinMap);
#endif

	return MV_OK;
}
