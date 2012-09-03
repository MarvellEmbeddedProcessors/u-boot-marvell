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

/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysDdrConfig.h"
#include "ddr2_3/mvDramIf.h"
#include "mvDramIfRegs.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/*******************************************************************************
* mvDramIfBankSizeGet - Get DRAM interface bank size.
*
* DESCRIPTION:
*       This function returns the size of a given DRAM bank.
*
* INPUT:
*       bankNum - Bank number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM bank size. If bank is disabled the function return '0'. In case
*		or paramter is invalid, the function returns -1.
*
*******************************************************************************/
MV_U32 mvDramIfBankSizeGet(MV_U32 bankNum)
{
	MV_DRAM_DEC_WIN addrDecWin;
	MV_U32 result = 0;

	if (mvDramIfWinGet(SDRAM_CS0 + bankNum, &addrDecWin) == MV_OK) {
		if (addrDecWin.enable == MV_TRUE)
			result = addrDecWin.addrWin.size;
	}

	DB(mvOsPrintf("Dram: mvDramIfBankSizeGet Bank %d size is %x \n", bankNum, result));
	return result;
}

/*******************************************************************************
* mvDramIfSizeGet - Get DRAM interface total size.
*
* DESCRIPTION:
*       This function get the DRAM total size.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM total size. In case or paramter is invalid, the function
*		returns -1.
*
*******************************************************************************/
MV_U32 mvDramIfSizeGet(MV_VOID)
{
	MV_U32 size = 0, i;

	for (i = 0; i < MV_DRAM_MAX_CS; i++)
		size += mvDramIfBankSizeGet(i);

	DB(mvOsPrintf("Dram: mvDramIfSizeGet size is %x \n", size));
	return size;
}

/*******************************************************************************
* sdramIfWinOverlap - Check if an address window overlap an SDRAM address window
*
* DESCRIPTION:
*		This function scan each SDRAM address decode window to test if it
*		overlapps the given address windoow
*
* INPUT:
*       target      - SDRAM target where the function skips checking.
*       pAddrDecWin - The tested address window for overlapping with
*					  SDRAM windows.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlaps any enabled address
*       decode map, MV_FALSE otherwise.
*
*******************************************************************************/
static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin)
{
	MV_TARGET targetNum;
	MV_DRAM_DEC_WIN addrDecWin;

	for (targetNum = SDRAM_CS0; targetNum < MV_DRAM_MAX_CS; targetNum++) {
		/* don't check our winNum or illegal targets */
		if (targetNum == target)
			continue;

		/* Get window parameters        */
		if (MV_OK != mvDramIfWinGet(targetNum, &addrDecWin)) {
			mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
			return MV_ERROR;
		}

		/* Do not check disabled windows        */
		if (MV_FALSE == addrDecWin.enable)
			continue;

		if (MV_TRUE == mvWinOverlapTest(pAddrWin, &addrDecWin.addrWin)) {
			mvOsPrintf("sdramIfWinOverlap: Required target %d overlap winNum %d\n", target, targetNum);
			return MV_TRUE;
		}
	}

	return MV_FALSE;
}

/*******************************************************************************
* mvDramIfWinSet - Set DRAM interface address decode window
*
* DESCRIPTION:
*       This function sets DRAM interface address decode window.
*
* INPUT:
*	    target      - System target. Use only SDRAM targets.
*       pAddrDecWin - SDRAM address window structure.
*
* OUTPUT:
*       None
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinSet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg = 0, ctrlReg = 0;
	MV_U32 sizeToReg = 0;

	/* Check if the requested window overlaps with current enabled windows  */
	if (MV_TRUE == sdramIfWinOverlap(target, &pAddrDecWin->addrWin)) {
		mvOsPrintf("mvDramIfWinSet: ERR. Target %d overlaps\n", target);
		return MV_BAD_PARAM;
	}

	/* check if address is aligned to the size */
	if (MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size)) {
		mvOsPrintf("mvDramIfWinSet:Error setting DRAM interface window %d."
			   "\nAddress 0x%08x is unaligned to size 0x%llx.\n",
			   target, pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	baseReg = MV_REG_READ(SDRAM_WIN_BASE_REG(target));
	ctrlReg = MV_REG_READ(SDRAM_WIN_CTRL_REG(target));

	/* Write to address decode Base Address Register                  */
	baseReg &= ~SDRAMWBR_BASE_MASK;
	baseReg |= (pAddrDecWin->addrWin.baseLow & SDRAMWBR_BASE_MASK);

	/* Translate the given window size to register format                   */
	sizeToReg = (pAddrDecWin->addrWin.size / SDRAMWCR_SIZE_ALLIGNMENT) - 1;

	/* set size */
	ctrlReg &= ~SDRAMWCR_SIZE_MASK;
	ctrlReg |= (sizeToReg << SDRAMWCR_SIZE_OFFS);

	/* enable/Disable */
	if (MV_TRUE == pAddrDecWin->enable)
		ctrlReg |= SDRAMWCR_ENABLE;
	else
		ctrlReg &= ~SDRAMWCR_ENABLE;

	/* Set back registers */
	MV_REG_WRITE(SDRAM_WIN_BASE_REG(target), baseReg);
	MV_REG_WRITE(SDRAM_WIN_CTRL_REG(target), ctrlReg);

	return MV_OK;
}

/*******************************************************************************
* mvDramIfWinGet - Get DRAM interface address decode window
*
* DESCRIPTION:
*       This function gets DRAM interface address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*       pAddrDecWin - SDRAM address window structure.
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinGet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg, sizeReg;
	MV_U32 sizeRegVal;

	/* Read base and size registers */
	sizeReg = MV_REG_READ(SDRAM_WIN_CTRL_REG(target));
	baseReg = MV_REG_READ(SDRAM_WIN_BASE_REG(target));

	sizeRegVal = (sizeReg & SDRAMWCR_SIZE_MASK) >> SDRAMWCR_SIZE_OFFS;

	pAddrDecWin->addrWin.size = (MV_U64)((MV_U64)(sizeRegVal + 1) * (MV_U64)SDRAMWCR_SIZE_ALLIGNMENT);

	/* Check if ctrlRegToSize returned OK */
	if (-1 == pAddrDecWin->addrWin.size) {
		mvOsPrintf("mvDramIfWinGet: size of target %d is Illigal\n", target);
		return MV_ERROR;
	}

	/* Extract base address                                         */
	/* Base register [31:16] ==> baseLow[31:16]             */
	pAddrDecWin->addrWin.baseLow = baseReg & SDRAMWBR_BASE_MASK;
	pAddrDecWin->addrWin.baseHigh = baseReg & SDRAMWBR_BASE_EXT_MASK;

	if (sizeReg & SDRAMWCR_ENABLE)
		pAddrDecWin->enable = MV_TRUE;
	else
		pAddrDecWin->enable = MV_FALSE;

	return MV_OK;
}

/*******************************************************************************
* mvDramIfWinEnable - Enable/Disable SDRAM address decode window
*
* DESCRIPTION:
*		This function enable/Disable SDRAM address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*		None.
*
* RETURN:
*		MV_ERROR in case function parameter are invalid, MV_OK otherewise.
*
*******************************************************************************/
MV_STATUS mvDramIfWinEnable(MV_TARGET target, MV_BOOL enable)
{
	MV_DRAM_DEC_WIN addrDecWin;

	if (enable == MV_TRUE) {
		/* First check for overlap with other enabled windows                           */
		if (MV_OK != mvDramIfWinGet(target, &addrDecWin)) {
			mvOsPrintf("mvDramIfWinEnable:ERR. Getting target %d failed.\n", target);
			return MV_ERROR;
		}
		/* Check for overlapping */
		if (MV_FALSE == sdramIfWinOverlap(target, &(addrDecWin.addrWin))) {
			/* No Overlap. Enable address decode winNum window              */
			MV_REG_BIT_SET(SDRAM_WIN_CTRL_REG(target), SDRAMWCR_ENABLE);
		} else {	/* Overlap detected */
			mvOsPrintf("mvDramIfWinEnable: ERR. Target %d overlap detect\n", target);
			return MV_ERROR;
		}
	} else {		/* Disable address decode winNum window                             */
		MV_REG_BIT_RESET(SDRAM_WIN_CTRL_REG(target), SDRAMWCR_ENABLE);
	}

	return MV_OK;
}

/*******************************************************************************
* mvDdrPhyRegRead - Read the content of a DDR PHY register
*
* DESCRIPTION:
*	Perform register read of the indicated DDR Phy register
*
* INPUT:
*	regNum: register offset
*	pupNum: PUP number
*
* OUTPUT:
*	value: 16bit register value
*
* RETURN:
*	None
*
*******************************************************************************/
MV_VOID mvDdrPhyRegRead(MV_U8 regNum, MV_U8 pupNum, MV_U16 *value)
{
	MV_U32 regAddr;
	MV_U32 reg;

	regAddr = (BIT31 | ((regNum & 0x3F) << 16) | ((pupNum & 0xF) << 22));
	MV_REG_WRITE(SDRAM_PHY_REGISTER_FILE_ACCESS, regAddr);

	do {
		reg = MV_REG_READ(SDRAM_PHY_REGISTER_FILE_ACCESS);
	} while ((reg & BIT31) != 0);

	*value = (reg & 0xFFFF);
}

/*******************************************************************************
* mvDdrPhyRegWrite - Update the content of a DDR PHY register
*
* DESCRIPTION:
*	Perform register write of the indicated DDR Phy register
*
* INPUT:
*	regNum: register offset
*	pupNum: PUP number
*	value: 16bit register value
*
* OUTPUT:
*	None
*
* RETURN:
*	None
*
*******************************************************************************/
MV_VOID mvDdrPhyRegWrite(MV_U8 regNum, MV_U8 pupNum, MV_U16 value)
{
	MV_U32 regAddr;
	MV_U32 reg;

	regAddr = (BIT31 | BIT30 | ((regNum & 0x3F) << 16) | ((pupNum & 0xF) << 22) | value);
	MV_REG_WRITE(SDRAM_PHY_REGISTER_FILE_ACCESS, regAddr);

	do {
		reg = MV_REG_READ(SDRAM_PHY_REGISTER_FILE_ACCESS);
	} while ((reg & BIT31) != 0);
}

/*******************************************************************************
* mvDramIfSingleBitErrThresholdSet - Set single bit ECC threshold.
*
* DESCRIPTION:
*       The ECC single bit error threshold is the number of single bit
*       errors to happen before the Dunit generates an interrupt.
*       This function set single bit ECC threshold.
*
* INPUT:
*       threshold - threshold.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if threshold is to big, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvDramIfSingleBitErrThresholdSet(MV_U32 threshold)
{
	MV_U32 regVal;

	if (threshold > SECR_THRECC_MAX)
		return MV_BAD_PARAM;

	regVal = MV_REG_READ(SDRAM_ECC_CONTROL_REG);
	regVal &= ~SECR_THRECC_MASK;
	regVal |= ((SECR_THRECC(threshold) & SECR_THRECC_MASK));
	MV_REG_WRITE(SDRAM_ECC_CONTROL_REG, regVal);

	return MV_OK;
}
