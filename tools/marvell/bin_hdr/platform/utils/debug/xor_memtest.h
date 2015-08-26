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

#define SRAM_WIN_ID		0x4
#define SRAM_WIN_BASE		0x40000000

#define XOR_ENGINE_TIMEOUT	0x5000000

#define ST_KP_LEN		59
#define ONE			0xffffffff
#define ZERO			0
#define BUS_STATUS(nValue)	((nValue) ? ONE : ZERO)
#define BIT(X)			(0x1 << X)

#define NUM_OF_DESC		8
#define DESC_SIZE		_8M

#define SOURCE_ADDRESS_BASE    0x83000000
#define BLOCK_SIZE             _1M
#define TOTAL_BLOCK_SIZE       (BLOCK_SIZE*2)
#define KILLER_64BIT_BASE      SOURCE_ADDRESS_BASE
#define KILLER_128BIT_BASE     (KILLER_64BIT_BASE + BLOCK_SIZE)
#define CPU_BURST_SIZE         _1K

#define DESTANTION_ADDRESS_BASE 0x84000000
#define DESTANTION_SIZE         _128M

#define NUM_OF_XOR_ENGINE              4
#define XOR_BLOCK_SIZE                 (_16M - 1)
#define XOR_SOURCE_ADDRESS(nXorNum)    (0x10000000 + _128M*nXorNum)
#define XOR_DEST_ADDRESS(nXorNum)      (XOR_SOURCE_ADDRESS(nXorNum) + (DESC_SIZE*NUM_OF_DESC))

#define MV_PLD(Address)\
{\
	__asm__ __volatile__("pld [%0]" : : "r" (Address));\
}

#define MV_PLD_RANGE(from, to)\
{\
	MV_U32 addr;\
	for (addr = from; addr < to; addr += 32) {\
		MV_PLD(addr);\
	} \
}

#define CHECK_CRC_STATUS(status)\
{\
	if (status != MV_OK) {\
		mvPrintf("Failed to calculate CRC32\n");\
		return 1;\
	} \
}

void cache_inv(MV_U32 dest);
void flush_l1_v7(MV_U32 dest);

#define MV_FLUSH_CACHE_L1(from, to)\
{\
	MV_U32 addr;\
	for (addr = from; addr < to; addr += 32) {\
		cache_inv(addr);\
		flush_l1_v7(addr);\
	} \
}

#define KILLER_PATTERN_BUS_STATE\
	{\
		0, 0, 0, 1, 1, 1, 0, 1, 0, 1,\
		0, 0, 1, 0, 1, 0, 1, 0, 1, 0,\
		1, 0, 1, 0, 0, 1, 0, 1, 0, 0,\
		0, 1, 1, 1, 0, 0, 1, 1, 0, 1,\
		0, 1, 1, 0, 0, 1, 0, 1, 1, 0,\
		1, 1, 0, 1, 1, 0, 0\
	}

#define KILLER_PATTERN_BIT_STATE\
	{\
		1, 0, 1, 1, 0, 0, 1, 0, 1, 0,\
		1, 1, 0, 1, 0, 0, 1, 0, 1, 0,\
		1, 0, 1, 1, 0, 1, 0, 1, 0, 0,\
		0, 0, 0, 1, 0, 0, 1, 1, 0, 1,\
		0, 1, 1, 0, 0, 1, 0, 1, 0, 0,\
		0, 0, 0, 1, 1, 1, 1\
	}
