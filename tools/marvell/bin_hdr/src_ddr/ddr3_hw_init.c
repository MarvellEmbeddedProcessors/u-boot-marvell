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
#include "config_marvell.h"     /* Required to identify SOC and Board */

#if defined(MV88F68XX) // this file is relevant for ARMADA-380 only, using new Training IP

#include "ddr3_init.h"
#include "ddr3_spd.h"

#include "ddr3_spd.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "ddr3_hws_hw_training.h"

#if defined(MV88F68XX)
#include "ddr3_a38x.h"
#include "ddr3_a38x_config.h"
#endif


static MV_VOID ddr3RestoreAndSetFinalWindows(MV_U32 *auWinBackup)
{
	MV_U32 ui, uiReg, uiCs;
    MV_U32 winCtrlReg, numOfWinRegs;
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();

    winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
    numOfWinRegs = 16;


	/* Return XBAR windows 4-7 or 16-19 init configuration */
	for (ui = 0; ui < numOfWinRegs; ui++)
		MV_REG_WRITE((winCtrlReg + 0x4 * ui), auWinBackup[ui]);

	DEBUG_INIT_FULL_S("DDR3 Training Sequence - Switching XBAR Window to FastPath Window \n");

	uiReg = 0x1FFFFFE1;

	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			uiReg |= (uiCs << 2);
			break;
		}
	}
	MV_REG_WRITE(REG_FASTPATH_WIN_0_CTRL_ADDR, uiReg); /*Open fast path Window to - 0.5G */
}

static MV_VOID ddr3SaveAndSetTrainingWindows(MV_U32 *auWinBackup)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiReg, uiTempCount, uiCs, ui;
    MV_U32 winCtrlReg, winBaseReg, winRemapReg;
    MV_U32 numOfWinRegs, winJumpIndex;

    winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
    winBaseReg  = REG_XBAR_WIN_4_BASE_ADDR;
    winRemapReg = REG_XBAR_WIN_4_REMAP_ADDR;
    winJumpIndex = 0x10;
    numOfWinRegs = 16;

    /*  Close XBAR Window 19 - Not needed */
	/*{0x000200e8}  -   Open Mbus Window - 2G */
	MV_REG_WRITE(REG_XBAR_WIN_19_CTRL_ADDR, 0);

	/* Save XBAR Windows 4-19 init configurations */
	for (ui = 0; ui < numOfWinRegs; ui++)
		auWinBackup[ui] = MV_REG_READ(winCtrlReg + 0x4 * ui);

/*  Open XBAR Windows 4-7 or 16-19 for other CS */
	uiReg = 0;
	uiTempCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			switch (uiCs) {
			case 0:
				uiReg = 0x0E00;
				break;
			case 1:
				uiReg = 0x0D00;
				break;
			case 2:
				uiReg = 0x0B00;
				break;
			case 3:
				uiReg = 0x0700;
				break;
			}
			uiReg |= (1 << 0);
			uiReg |= (SDRAM_CS_SIZE & 0xFFFF0000);

            MV_REG_WRITE(winCtrlReg + winJumpIndex * uiTempCount, uiReg);
			uiReg = (((SDRAM_CS_SIZE + 1) * (uiTempCount)) & 0xFFFF0000);
			MV_REG_WRITE(winBaseReg + winJumpIndex * uiTempCount, uiReg);
            if(winRemapReg <= REG_XBAR_WIN_7_REMAP_ADDR) {
                MV_REG_WRITE(winRemapReg + winJumpIndex * uiTempCount, 0);
            }
			uiTempCount++;
		}
	}
}

MV_U32 ddr3Init(void)
{
	MV_U32 uiReg = 0;
	MV_U32 auWinBackup[16];
	MV_STATUS status;

    DEBUG_INIT_S("DDR3 training version: 2013_Q3_14T1.1\n");
	/* SoC/Board special Initializtions */

	/* Set X-BAR windows for the training sequence */
    ddr3SaveAndSetTrainingWindows(auWinBackup);

    /*Start New Training IP*/
	status = ddr3HwsHwTraining();
	if (MV_OK != status) {
		DEBUG_INIT_FULL_S("DDR3 Training Sequence - FAILED\n");
		return status;
	}

	/* Restore and set windows */
	ddr3RestoreAndSetFinalWindows(auWinBackup);

	/* Update DRAM init indication in bootROM register */
	uiReg = MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR);
	MV_REG_WRITE(REG_BOOTROM_ROUTINE_ADDR, uiReg | (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS));

#ifdef DEBUG_MEM_TEST
	if (fullMemTest() != MV_OK){
		DEBUG_INIT_S(" memory test - failed\n");
		return MV_FAIL;
	}
#endif

	DEBUG_INIT_S("DDR3 Training Sequence - Ended Successfully \n");

	return MV_OK;
}

/******************************************************************************
 * Name:     ddr3GetCSNumFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSNumFromReg(void)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiCsCount = 0;
	MV_U32 uiCs;

	for (uiCs = 0; uiCs < MAX_CS; uiCs++)
		if (uiCsEna & (1 << uiCs))
			uiCsCount++;

	return uiCsCount;
}

/******************************************************************************
 * Name:     ddr3GetCSEnaFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSEnaFromReg(void)
{
	return MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & REG_DDR3_RANK_CTRL_CS_ENA_MASK;
}
#endif
