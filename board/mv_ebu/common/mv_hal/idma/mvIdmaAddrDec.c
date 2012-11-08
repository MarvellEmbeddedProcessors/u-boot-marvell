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
#include "mvSysHwConfig.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvIdmaRegs.h"
#include "mvIdma.h"
#include "mv_hal_if/mvSysIdma.h"
#include "armadaxp.h"
#include "mvXorRegs.h"

/* defines  */

/* #define MV_DEBUG */

#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

MV_STATUS mvDmaTargetWinEnable(MV_U32 winNum, MV_BOOL enable);
MV_STATUS mvDmaTargetWinWrite(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);

MV_TARGET idmaAddrDecPrioTap[] = {
/* Note: the size of this array should be limited according to IDMA_MAX_ADDR_DEC_WIN */
#if defined(MV_INCLUDE_PEX)
#if PEX0_MEM_SIZE
	PEX0_MEM,
#endif
#if PEX1_MEM_SIZE
	PEX1_MEM,
#endif
#if PEX2_MEM_SIZE
	PEX2_MEM,
#endif
#if PEX3_MEM_SIZE
	PEX3_MEM,
#endif
#if PEX8_MEM_BASE
	PEX8_MEM,
#endif
#if PEX9_MEM_BASE
	PEX9_MEM,
#endif
#endif
#if defined(MV_INCLUDE_SDRAM_CS0)
	SDRAM_CS0,
#endif
#if defined(MV_INCLUDE_SDRAM_CS1)
	SDRAM_CS1,
#endif
#if defined(MV_INCLUDE_SDRAM_CS2)
	SDRAM_CS2,
#endif
#if defined(MV_INCLUDE_SDRAM_CS3)
	SDRAM_CS3,
#endif
#if defined(MV_INCLUDE_DEVICE_CS0)
	DEVICE_CS0,
#endif
#if defined(MV_INCLUDE_DEVICE_CS1)
	DEV_CS1,
#endif
#if defined(MV_INCLUDE_CESA)
	CRYPT1_ENG,
#endif
	TBL_TERM
};

/*******************************************************************************
* mvDmaWinInit
*
* DESCRIPTION:
*   This function sets the default address decode windows of the unit.
*   Note that if the address window is disabled in idmaAddrDecMap, the
*   window parameters will be set but the window will remain disabled.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*******************************************************************************/
MV_STATUS mvDmaWinInit(MV_UNIT_WIN_INFO *addrWinMap)
{
	MV_U32 winNum;
	MV_UNIT_WIN_INFO *addrDecWin;
	MV_U32 winPrioIndex = 0;

	/* First disable all address decode windows */
	for (winNum = 0; winNum < IDMA_MAX_ADDR_DEC_WIN; winNum++)
		mvDmaTargetWinEnable(winNum, MV_FALSE);

	/* Go through all windows in user table until table terminator */
	for (winNum = 0; ((idmaAddrDecPrioTap[winPrioIndex] != TBL_TERM) && (winNum < IDMA_MAX_ADDR_DEC_WIN));) {
		addrDecWin = &addrWinMap[idmaAddrDecPrioTap[winPrioIndex]];
		if (addrDecWin->enable == MV_TRUE) {
			if (mvDmaTargetWinWrite(winNum, addrDecWin) != MV_OK) {
				DB(mvOsPrintf("mvDmaInit: ERR. mvDmaTargetWinSet failed\n"));
				return MV_ERROR;
			}
			if (mvDmaTargetWinEnable(winNum, MV_TRUE) != MV_OK) {
				DB(mvOsPrintf("mvDmaInit: ERR. mvDmaTargetWinSet failed\n"));
				return MV_ERROR;
			}
			winNum++;
		}
		winPrioIndex++;
	}

	return MV_OK;
}

/*******************************************************************************
* mvDmaTargetWinWrite - Set IDMA target address window
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0)
*       address window. After setting this target window, the IDMA will be
*       able to access the target within the address window.
*
* INPUT:
*	winNum - One of the possible IDMA memory decode windows.
*	pAddrDecWin - describes the window
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvDmaTargetWinWrite(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
	MV_U32 sizeReg, baseReg;

	if (winNum >= IDMA_MAX_ADDR_DEC_WIN) {
		DB(mvOsPrintf("%s: ERR. Invalid win num %d\n",  __func__, winNum));
		return MV_BAD_PARAM;
	}

	if (pAddrDecWin == NULL) {
		DB(mvOsPrintf("%s: ERR. pAddrDecWin is NULL pointer\n",  __func__));
		return MV_BAD_PTR;
	}

	if (!MV_IS_POWER_OF_2(pAddrDecWin->addrWin.size)) {
		mvOsPrintf("%s: ERR. Illegal window size.\n",  __func__);
		return MV_BAD_PARAM;
	}

	baseReg = pAddrDecWin->addrWin.baseLow & XEBARX_BASE_MASK;
	sizeReg = (pAddrDecWin->addrWin.size / XOR_WIN_SIZE_ALIGN) - 1;
	sizeReg = (sizeReg << XESMRX_SIZE_MASK_OFFS) & XESMRX_SIZE_MASK_MASK;

	/* set attributes */
	baseReg &= ~XEBARX_ATTR_MASK;
	baseReg |= pAddrDecWin->attrib << XEBARX_ATTR_OFFS;
	/* set target ID */
	baseReg &= ~XEBARX_TARGET_MASK;
	baseReg |= pAddrDecWin->targetId << XEBARX_TARGET_OFFS;

	/* Write to address decode Base Address Register */
	MV_REG_WRITE(IDMA_BASE_ADDR_REG(winNum), baseReg);

	/* Write to Size Register */
	MV_REG_WRITE(IDMA_SIZE_MASK_REG(winNum), sizeReg);

	return MV_OK;
}


/*******************************************************************************
* mvDmaTargetWinEnable - Enable/disable a Idma address decode window
*
* DESCRIPTION:
*       This function enable/disable a IDMA address decode window.
*       if parameter 'enable' == MV_TRUE the routine will enable the
*       window, thus enabling IDMA accesses (before enabling the window it is
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - Decode window number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvDmaTargetWinEnable(MV_U32 winNum, MV_BOOL enable)
{
	if (winNum >= IDMA_MAX_ADDR_DEC_WIN) {
		DB(mvOsPrintf("%s: ERR. Invalid winNum%d\n",  __func__, winNum));
		return MV_ERROR;
	}

	if (enable == MV_TRUE)
		MV_REG_BIT_RESET(IDMA_BASE_ADDR_ENABLE_REG, IBAER_ENABLE(winNum));
	else
		MV_REG_BIT_SET(IDMA_BASE_ADDR_ENABLE_REG, IBAER_ENABLE(winNum));

	return MV_OK;
}



