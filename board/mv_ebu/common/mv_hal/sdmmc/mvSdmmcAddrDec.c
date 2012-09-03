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
#include "mvSysSdmmcConfig.h"
#include "mvSdmmc.h"

#define MV_SDMMC_MAX_ADDR_DECODE_WIN 4

#define MV_SDMMC_WIN_CTRL_REG(dev, win)        (MV_SDMMC_REGS_BASE + 0x108 + ((win)<<3))
#define MV_SDMMC_WIN_BASE_REG(dev, win)        (MV_SDMMC_REGS_BASE + 0x10c + ((win)<<3))

/* BITs in Windows 0-3 Control and Base Registers */
#define MV_SDMMC_WIN_ENABLE_BIT               0
#define MV_SDMMC_WIN_ENABLE_MASK              (1<<MV_SDMMC_WIN_ENABLE_BIT)

#define MV_SDMMC_WIN_TARGET_OFFSET            4
#define MV_SDMMC_WIN_TARGET_MASK              (0xF<<MV_SDMMC_WIN_TARGET_OFFSET)

#define MV_SDMMC_WIN_ATTR_OFFSET              8
#define MV_SDMMC_WIN_ATTR_MASK                (0xFF<<MV_SDMMC_WIN_ATTR_OFFSET)

#define MV_SDMMC_WIN_SIZE_OFFSET              16
#define MV_SDMMC_WIN_SIZE_MASK                (0xFFFF<<MV_SDMMC_WIN_SIZE_OFFSET)

#define MV_SDMMC_WIN_BASE_OFFSET              16
#define MV_SDMMC_WIN_BASE_MASK                (0xFFFF<<MV_SDMMC_WIN_BASE_OFFSET)

#define MV_SDMMC_WIN_SIZE_ALIGN		    _64K

MV_TARGET sdmmcAddrDecPrioTab[] = {
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
#if defined(MV_INCLUDE_PEX)
	PEX0_MEM,
#endif
	TBL_TERM
};

/*******************************************************************************
* sdmmcWinOverlapDetect - Detect SDMMC address windows overlapping
*
* DESCRIPTION:
*       An unpredicted behaviur is expected in case SDMMC address decode
*       windows overlapps.
*       This function detects SDMMC address decode windows overlapping of a
*       specified window. The function does not check the window itself for
*       overlapping. The function also skipps disabled address decode windows.
*
* INPUT:
*       winNum      - address decode window number.
*       pAddrDecWin - An address decode window struct.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlap current address
*       decode map, MV_FALSE otherwise, MV_ERROR if reading invalid data
*       from registers.
*
*******************************************************************************/
static MV_STATUS sdmmcWinOverlapDetect(int dev, MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
	MV_U32 winNumIndex;
	MV_UNIT_WIN_INFO addrDecWin;

	for (winNumIndex = 0; winNumIndex < MV_SDMMC_MAX_ADDR_DECODE_WIN; winNumIndex++) {
		/* Do not check window itself       */
		if (winNumIndex == winNum)
			continue;

		/* Get window parameters    */
		if (MV_OK != mvSdmmcWinRead(dev, winNumIndex, &addrDecWin)) {
			mvOsPrintf("%s: ERR. mvSdmmcWinRead failed\n", __func__);
			return MV_ERROR;
		}

		/* Do not check disabled windows    */
		if (addrDecWin.enable == MV_FALSE)
			continue;

		if (MV_TRUE == mvWinOverlapTest(pAddrWin, &(addrDecWin.addrWin)))
			return MV_TRUE;
	}
	return MV_FALSE;
}

/*******************************************************************************
* mvSdmmcWinWrite - Set SDMMC target address window
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0)
*       address window, also known as address decode window.
*       After setting this target window, the SDMMC will be able to access the
*       target within the address window.
*
* INPUT:
*       winNum      - SDMMC target address decode window number.
*       pAddrDecWin - SDMMC target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if address window overlapps with other address decode windows.
*       MV_BAD_PARAM if base address is invalid parameter or target is
*       unknown.
*
*******************************************************************************/
MV_STATUS mvSdmmcWinWrite(MV_U32 dev, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
	MV_U32 sizeReg, baseReg;

	/* Parameter checking   */
	if (winNum >= MV_SDMMC_MAX_ADDR_DECODE_WIN) {
		mvOsPrintf("%s: ERR. Invalid win num %d\n", __func__, winNum);
		return MV_BAD_PARAM;
	}

	/* Check if the requested window overlapps with current windows         */
	if (MV_TRUE == sdmmcWinOverlapDetect(dev, winNum, &pAddrDecWin->addrWin)) {
		mvOsPrintf("%s: ERR. Window %d overlap\n", __func__, winNum);
		return MV_ERROR;
	}

	/* check if address is aligned to the size */
	if (MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size)) {
		mvOsPrintf("mvSdmmcWinWrite:Error setting SDMMC window %d.\n"
			   "Address 0x%08x is unaligned to size 0x%llx.\n",
			   winNum, pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	baseReg = pAddrDecWin->addrWin.baseLow & MV_SDMMC_WIN_BASE_MASK;
	sizeReg = (pAddrDecWin->addrWin.size / MV_SDMMC_WIN_SIZE_ALIGN) - 1;
	sizeReg = sizeReg << MV_SDMMC_WIN_SIZE_OFFSET;

	/* set attributes */
	sizeReg &= ~MV_SDMMC_WIN_ATTR_MASK;
	sizeReg |= (pAddrDecWin->attrib << MV_SDMMC_WIN_ATTR_OFFSET);

	/* set target ID */
	sizeReg &= ~MV_SDMMC_WIN_TARGET_MASK;
	sizeReg |= (pAddrDecWin->targetId << MV_SDMMC_WIN_TARGET_OFFSET);

	if (pAddrDecWin->enable == MV_TRUE)
		sizeReg |= MV_SDMMC_WIN_ENABLE_MASK;
	else
		sizeReg &= ~MV_SDMMC_WIN_ENABLE_MASK;

	MV_REG_WRITE(MV_SDMMC_WIN_CTRL_REG(dev, winNum), sizeReg);
	MV_REG_WRITE(MV_SDMMC_WIN_BASE_REG(dev, winNum), baseReg);

	return MV_OK;
}

/*******************************************************************************
* mvSdmmcWinRead - Read SDMMC peripheral target address window.
*
* DESCRIPTION:
*       Get SDMMC peripheral target address window.
*
* INPUT:
*       winNum - SDMMC target address decode window number.
*
* OUTPUT:
*       pAddrDecWin - SDMMC target window data structure.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvSdmmcWinRead(MV_U32 dev, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
	MV_U32 sizeReg, baseReg;
	MV_U32 sizeRegVal;

	/* Parameter checking   */
	if (winNum >= MV_SDMMC_MAX_ADDR_DECODE_WIN) {
		mvOsPrintf("%s (dev=%d): ERR. Invalid winNum %d\n", __func__, dev, winNum);
		return MV_NOT_SUPPORTED;
	}

	baseReg = MV_REG_READ(MV_SDMMC_WIN_BASE_REG(dev, winNum));
	sizeReg = MV_REG_READ(MV_SDMMC_WIN_CTRL_REG(dev, winNum));

	/* Extract base address and size    */
	sizeRegVal = (sizeReg & MV_SDMMC_WIN_SIZE_MASK) >> MV_SDMMC_WIN_SIZE_OFFSET;
	pAddrDecWin->addrWin.size = (sizeRegVal + 1) * MV_SDMMC_WIN_SIZE_ALIGN;
	pAddrDecWin->addrWin.baseLow = baseReg & MV_SDMMC_WIN_BASE_MASK;
	pAddrDecWin->addrWin.baseHigh = 0;

	/* attrib and targetId              */
	pAddrDecWin->attrib = (sizeReg & MV_SDMMC_WIN_ATTR_MASK) >> MV_SDMMC_WIN_ATTR_OFFSET;
	pAddrDecWin->targetId = (sizeReg & MV_SDMMC_WIN_TARGET_MASK) >> MV_SDMMC_WIN_TARGET_OFFSET;

	/* Check if window is enabled       */
	if (sizeReg & MV_SDMMC_WIN_ENABLE_MASK)
		pAddrDecWin->enable = MV_TRUE;
	else
		pAddrDecWin->enable = MV_FALSE;

	return MV_OK;
}

/*******************************************************************************
* mvSdmmcWinInit - Initialize the integrated SDMMC target address window.
*
* DESCRIPTION:
*       Initialize the SDMMC peripheral target address window.
*
* INPUT:
*
*
* OUTPUT:
*
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvSdmmcWinInit(MV_UNIT_WIN_INFO *addrWinMap)
{
	MV_U32 winNum;
	MV_UNIT_WIN_INFO *addrDecWin;
	MV_U32 winPrioIndex = 0;

	/* Initiate Sdmmc address decode */

	/* First disable all address decode windows */
	for (winNum = 0; winNum < MV_SDMMC_MAX_ADDR_DECODE_WIN; winNum++) {
		MV_U32 regVal = MV_REG_READ(MV_SDMMC_WIN_CTRL_REG(0, winNum));
		regVal &= ~MV_SDMMC_WIN_ENABLE_MASK;
		MV_REG_WRITE(MV_SDMMC_WIN_CTRL_REG(0, winNum), regVal);
	}

	winNum = 0;
	while ((sdmmcAddrDecPrioTab[winPrioIndex] != TBL_TERM) && (winNum < MV_SDMMC_MAX_ADDR_DECODE_WIN)) {
		addrDecWin = &addrWinMap[sdmmcAddrDecPrioTab[winPrioIndex]];
		if (addrDecWin->enable == MV_TRUE) {
			if (MV_OK != mvSdmmcWinWrite(0, winNum, addrDecWin))
				return MV_ERROR;
			winNum++;
		}
		winPrioIndex++;
	}
	return MV_OK;
}
