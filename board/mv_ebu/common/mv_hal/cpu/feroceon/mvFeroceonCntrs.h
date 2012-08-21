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
#ifndef __mvFeroceonCntrs_h__
#define __mvFeroceonCntrs_h__

#include "mvTypes.h"
#include "mvOs.h"

#define MV_FEROCEON_CNTRS_NUM       4
#define MV_FEROCEON_CNTRS_OPS_NUM   32

#define MV_CPU_CNTR_SIZE			64 /* bits */


/* internal */
static INLINE int mvCpuCntrsStart(int counter, int op)
{
	MV_U32 reg = (1 << op) | 0x1;	/*enable */

	switch (counter) {
	case 0:
__asm__ __volatile__("mcr p15, 0, %0, c15, c12, 0" : : "r"(reg));
		return 0;

	case 1:
__asm__ __volatile__("mcr p15, 0, %0, c15, c12, 1" : : "r"(reg));
		return 0;

	case 2:
__asm__ __volatile__("mcr p15, 0, %0, c15, c12, 2" : : "r"(reg));
		return 0;

	case 3:
__asm__ __volatile__("mcr p15, 0, %0, c15, c12, 3" : : "r"(reg));
		return 0;

	default:
		mvOsPrintf("error in program_counter: bad counter number (%d)\n", counter);
	}
	return 1;
}

static INLINE void mvCpuCntrsStop(const int counter)
{
	MV_U32 ll = 0;

	switch (counter) {
	case 0:
MV_ASM("mcr p15, 0, %0, c15, c12, 0" : : "r"(ll));
		break;

	case 1:
MV_ASM("mcr p15, 0, %0, c15, c12, 1" : : "r"(ll));
		break;

	case 2:
MV_ASM("mcr p15, 0, %0, c15, c12, 2" : : "r"(ll));
		break;

	case 3:
MV_ASM("mcr p15, 0, %0, c15, c12, 3" : : "r"(ll));
		break;

	default:
		mvOsPrintf("mv_cpu_cntrs_read: bad counter number (%d)\n", counter);
	}
}

static INLINE MV_U64 mvCpuCntrsRead(const int counter)
{
	MV_U32 low = 0, high = 0;

	switch (counter) {
	case 0:
MV_ASM("mrc p15, 0, %0, c15, c13, 0" : "=r"(low));
MV_ASM("mrc p15, 0, %0, c15, c13, 1" : "=r"(high));
		break;

	case 1:
MV_ASM("mrc p15, 0, %0, c15, c13, 2" : "=r"(low));
MV_ASM("mrc p15, 0, %0, c15, c13, 3" : "=r"(high));
		break;

	case 2:
MV_ASM("mrc p15, 0, %0, c15, c13, 4" : "=r"(low));
MV_ASM("mrc p15, 0, %0, c15, c13, 5" : "=r"(high));
		break;

	case 3:
MV_ASM("mrc p15, 0, %0, c15, c13, 6" : "=r"(low));
MV_ASM("mrc p15, 0, %0, c15, c13, 7" : "=r"(high));
		break;

	default:
		mvOsPrintf("mv_cpu_cntrs_read: bad counter number (%d)\n", counter);
	}
	return (((MV_U64) high << 32) | low);
}

static INLINE void mvCpuCntrsReset(void)
{
	MV_U32 reg = 0;

MV_ASM("mcr p15, 0, %0, c15, c13, 0" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 1" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 2" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 3" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 4" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 5" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 6" : : "r"(reg));
MV_ASM("mcr p15, 0, %0, c15, c13, 7" : : "r"(reg));
}


void mvCpuCntrsInit(void);

#endif /* __mvFeroceonCntrs_h__ */

