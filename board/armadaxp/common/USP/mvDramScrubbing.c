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


#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "xor/mvXor.h"
#include "xor/mvXorRegs.h"
#include "ddr2_3/mvDramIfRegs.h"
#include "ddr2_3/mvDramIf.h"
#include "cpu/mvCpu.h"

#define SCRB_XOR_UNIT	0
#define SCRB_XOR_CHAN	1
#define SCRB_XOR_WIN   	0

static MV_U32 uiXorRegsCtrlBackup=1;
static MV_U32 uiXorRegsBaseBackup=1;
static MV_U32 uiXorRegsMaskBackup=1;
/*************************************************************************/
MV_U32 mvDramCsSizeGet(MV_U32 cs)
{
	MV_U32 size;
	size = MV_REG_READ(SDRAM_SIZE_REG(cs)) & SDRAM_ADDR_MASK;
	if (size > 0) 
		size |= ~(SDRAM_ADDR_MASK);
	return size;
}

MV_VOID mvSysXorInit2( MV_U32 uiCS)
{
	MV_U32 uiReg, uiBase, size, base;
	MV_U32 bankAttr[4]={0xe00, 0xd00, 0xb00,0x700};

	uiXorRegsCtrlBackup = MV_REG_READ(XOR_WINDOW_CTRL_REG(SCRB_XOR_UNIT, SCRB_XOR_CHAN));
	uiXorRegsBaseBackup = MV_REG_READ(XOR_BASE_ADDR_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN));
	uiXorRegsMaskBackup = MV_REG_READ(XOR_SIZE_MASK_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN));
//	printf(" regs 20250=0x%08x, 20240=0x%08x\n", MV_REG_READ(0x20250), MV_REG_READ(0x20240));
	uiReg = 0x1; 					/* 	Enable Window x for each CS */
	uiReg |= (0x3 << 16); 					/* 	Enable Window x for each CS */

	MV_REG_WRITE(XOR_WINDOW_CTRL_REG(SCRB_XOR_UNIT, SCRB_XOR_CHAN), uiReg);

	base=0;
	size = mvDramCsSizeGet(uiCS);
	if(size) {
//			if ((base+size) > SDRAM_MAX_ADDR)
//				size = SDRAM_MAX_ADDR - base -1;

			uiBase = ((base / _64K) << XEBARX_BASE_OFFS) | bankAttr[uiCS];
	
			MV_REG_WRITE(XOR_BASE_ADDR_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN), uiBase);
			base += size +1;
			size = (size / _64K) << 16;
			/* window x - Size - 256 MB */
			MV_REG_WRITE(XOR_SIZE_MASK_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN), size);
	}
//	printf("** XOR chanel=%d, WIN %d, CTRL=0x%08x, base=0x%08x, size=0x%08x\n", SCRB_XOR_CHAN, SCRB_XOR_WIN,
//		   MV_REG_READ(XOR_WINDOW_CTRL_REG(SCRB_XOR_UNIT, SCRB_XOR_CHAN)),
//		   MV_REG_READ(XOR_BASE_ADDR_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN)), 
//		   MV_REG_READ(XOR_SIZE_MASK_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN)));
	
	mvXorHalInit(0);
	
	return;
}

MV_VOID mvSysXorFinish2(void)
{
	MV_REG_WRITE(XOR_WINDOW_CTRL_REG(SCRB_XOR_UNIT, SCRB_XOR_CHAN), uiXorRegsCtrlBackup);
	MV_REG_WRITE(XOR_BASE_ADDR_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN), uiXorRegsBaseBackup);
	MV_REG_WRITE(XOR_SIZE_MASK_REG(SCRB_XOR_UNIT, SCRB_XOR_WIN), uiXorRegsMaskBackup);
}

MV_VOID mvDramScrubbing(void)
{
	int cs;
	MV_U32 size;
	MV_U32 totalMem;
	MV_U64 total;
	MV_U32 start_addr;

	if (0 == mvCtrlDDRECC())  /* read SatR enable ECC */
		return;
	if (whoAmI())	/* only cpu 0 will make Scrubbing */
		return;

	printf("Dram Scrubbing starting (use XOR chanel %d)\n",SCRB_XOR_CHAN);
	totalMem = 0;
	/* DDR training scrub DDR till 0x1000000 */
	for (cs=0; cs < CONFIG_NR_DRAM_BANKS; cs++) {
		size = mvDramIfHwCsSizeGet(cs);
		if (size == 0)
			continue;
		total = (MV_U64)size + 1;
		totalMem += (MV_U32)(total / _1G);
		start_addr = 0;
		mvSysXorInit2(cs);
		if (0 == cs) {
			start_addr = 0x1000000;
			size -= start_addr;
//			if ((size + start_addr) > SDRAM_MAX_ADDR)
//				size = (SDRAM_MAX_ADDR-1) - start_addr;
		}
//		if ((size + start_addr) > SDRAM_MAX_ADDR)
//			size = SDRAM_MAX_ADDR - start_addr -1;
		printf("       CS %d Dram Scrubbing  0x%x:0x%x\n", cs, start_addr,(start_addr+size));

		mvXorMemInit(SCRB_XOR_CHAN, start_addr, size, 0xbeefdead, 0xbeefdead);
		/* wait for previous transfer completion */
		while (mvXorStateGet(SCRB_XOR_CHAN) != MV_IDLE);
		mvSysXorFinish2();
	}
	printf("       Total physical DRAM size = %d GB\n\n", (int)totalMem);
}

