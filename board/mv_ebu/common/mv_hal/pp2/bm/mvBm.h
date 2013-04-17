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

#ifndef __mvBm_h__
#define __mvBm_h__

/* includes */
#include "mvTypes.h"
#include "mvCommon.h"
#include "mvStack.h"
#include "mv802_3.h"
#include "pp2/common/mvPp2Common.h"
#include "mvBmRegs.h"

typedef struct {
	int valid;
	int longPool;
	int shortPool;
	int longBufNum;
	int shortBufNum;
	int hwfLongPool;
	int hwfShortPool;
	int hwfLongBufNum;
	int hwfShortBufNum;

} MV_BM_CONFIG;

typedef struct {
	int         pool;
	int         capacity;
	int         bufNum;
	int         bufSize;
	MV_U32      *pVirt;
	MV_ULONG    physAddr;
} MV_BM_POOL;

/* defines */

/* bits[8-9] of address define pool 0-3 */
#define BM_POOL_ACCESS_OFFS     8

/* INLINE functions */
static INLINE void mvBmPoolPut(int pool, MV_U32 bufPhysAddr, MV_U32 bufVirtAddr)
{
	mvPp2WrReg(MV_BM_VIRT_RLS_REG, bufVirtAddr);
	mvPp2WrReg(MV_BM_PHY_RLS_REG(pool), bufPhysAddr);
}

static INLINE void mvBmPoolMcPut(int pool, MV_U32 bufPhysAddr, MV_U32 bufVirtAddr, int mcId, int isForce)
{
	MV_U32 regVal = 0;

	regVal |= ((mcId << MV_BM_MC_ID_OFFS) & MV_BM_MC_ID_MASK);
	if (isForce)
		regVal |= MV_BM_FORCE_RELEASE_MASK;

	mvPp2WrReg(MV_BM_MC_RLS_REG, regVal);
	mvBmPoolPut(pool, bufPhysAddr | MV_BM_PHY_RLS_MC_BUFF_MASK, bufVirtAddr);
}

static INLINE MV_U32 mvBmPoolGet(int pool)
{
	MV_U32 bufVirtAddr;

	mvPp2RdReg(MV_BM_PHY_ALLOC_REG(pool)); /* read physical address */
	bufVirtAddr = mvPp2RdReg(MV_BM_VIRT_ALLOC_REG); /* return virtual address */

	return bufVirtAddr;
}

/* prototypes */
MV_STATUS mvBmInit(void);
MV_STATUS mvBmPoolControl(int pool, MV_COMMAND cmd);
MV_STATE  mvBmPoolStateGet(int pool);
void      mvBmPoolEnable(int pool);
void      mvBmPoolDisable(int pool);
MV_BOOL   mvBmPoolIsEnabled(int pool);
MV_STATUS mvBmPoolInit(int pool, MV_ULONG physPoolBase, int capacity);
MV_STATUS mvBmPoolBufNumUpdate(int pool, int buf_num);
MV_STATUS mvBmPoolBufSizeSet(int pool, int buf_size);
void      mvBmRegs(void);
void      mvBmStatus(void);
void      mvBmPoolDump(int pool, int mode);
void      mvBmPoolPrint(int pool);

#endif /* __mvBm_h__ */

