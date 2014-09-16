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

#include "mv_seq_exec.h"
#include "soc_spec.h"
#include "mvHighSpeedEnvSpec.h"

#ifdef REGISTER_TRACE_DEBUG
static MV_U32 _MV_REG_READ(MV_U32 regAddr)
{
	putstring("\n >>>       MV_REG_READ.  regAddr=0x");
	putdata(INTER_REGS_BASE | (regAddr), 8);
	putstring(" regData=0x");
	MV_U32 regData = MV_MEMIO_LE32_READ((INTER_REGS_BASE | (regAddr)));
	putdata(regData, 8);

	return regData;
}

static MV_VOID _MV_REG_WRITE(MV_U32 regAddr, MV_U32 regData)
{
	putstring("\n >>>       MV_REG_WRITE. regAddr=0x");
	putdata(INTER_REGS_BASE | (regAddr), 8);
	putstring(" regData=0x");
	putdata(regData, 8);
	MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (regAddr)), (regData));
}
#undef MV_REG_WRITE
#undef MV_REG_READ

#define MV_REG_WRITE    _MV_REG_WRITE
#define MV_REG_READ     _MV_REG_READ
#endif /*REGISTER_TRACE_DEBUG*/

/* Array for mapping the operation (write, poll or delay) functions */
opExecuteFuncPtr opExecuteFuncArr[] =
{
	writeOpExecute,
	delayOpExecute,
	pollOpExecute
};

extern MV_CFG_SEQ serdesSeqDb[];

/***************************************************************************/
MV_STATUS writeOpExecute
(
	MV_U32 serdesNum,
	MV_OP_PARAMS    *params,
	MV_U32 dataArrIdx
)
{
	MV_U32 unitBaseReg, unitOffset, data, mask, regData, regAddr;

	if (serdesNum >= mvHwsSerdesGetMaxLane()) {
		DEBUG_INIT_S("writeOpExecute: bad serdes number\n");
		return MV_BAD_PARAM;
	}

	/* Getting write op params from the input parameter */
	/*
	unitBaseReg = params->unitBaseReg;
	unitOffset = params->unitOffset;
	*/
	data = params->data[dataArrIdx];
	mask = params->mask;

	/* an empty operation */
	if (data == NO_DATA)
		return MV_OK;

	/* get updated base address since it can be different between Serdes */
	CHECK_STATUS(mvHwsGetExtBaseAddr(serdesNum, params->unitBaseReg, params->unitOffset,
									 &unitBaseReg, &unitOffset));

	/* Address calculation */
	regAddr = unitBaseReg + unitOffset * serdesNum;

#ifdef SEQ_DEBUG
	mvPrintf("Write: 0x%x: 0x%x (mask 0x%x) - ", regAddr, data, mask);
#endif
	/* Reading old value */
	regData = MV_REG_READ(regAddr);
	regData &= (~mask);

	/* Writing new data */
	data &= mask;
	regData |= data;
	MV_REG_WRITE(regAddr, regData);

#ifdef SEQ_DEBUG
	mvPrintf(" - 0x%x\n", regData);
#endif

	return MV_OK;
}

/***************************************************************************/
MV_STATUS delayOpExecute
(
	MV_U32 serdesNum,
	MV_OP_PARAMS    *params,
	MV_U32 dataArrIdx
)
{
	MV_U32 delay;

	/* Getting delay op params from the input parameter */
	delay = params->waitTime;
#ifdef SEQ_DEBUG
	mvPrintf("Delay: %d\n", delay);
#endif
	mvOsDelay(delay);
	return MV_OK;
}

/***************************************************************************/
MV_STATUS pollOpExecute
(
	MV_U32 serdesNum,
	MV_OP_PARAMS    *params,
	MV_U32 dataArrIdx
)
{
	MV_U32 unitBaseReg, unitOffset, data, mask, numOfLoops, waitTime;
	MV_U32 pollCounter = 0;
	MV_U32 regAddr, regData;

	/* Getting poll op params from the input parameter */
	/*
	unitBaseReg = params->unitBaseReg;
	unitOffset = params->unitOffset;
	*/
	data = params->data[dataArrIdx];
	mask = params->mask;
	numOfLoops = params->numOfLoops;
	waitTime = params->waitTime;

	if (serdesNum >= mvHwsSerdesGetMaxLane()) {
		DEBUG_INIT_S("pollOpExecute: bad serdes number\n");
		return MV_BAD_PARAM;
	}

	/* an empty operation */
	if (data == NO_DATA)
		return MV_OK;

	/* get updated base address since it can be different between Serdes */
	CHECK_STATUS(mvHwsGetExtBaseAddr(serdesNum, params->unitBaseReg, params->unitOffset,
									 &unitBaseReg, &unitOffset));

	/* Address calculation */
	regAddr = unitBaseReg + unitOffset * serdesNum;

	/* Polling */
#ifdef SEQ_DEBUG
	mvPrintf("Poll:  0x%x: 0x%x (mask 0x%x)\n", regAddr, data, mask);
#endif

#ifdef WIN32

	DEBUG_INIT_FULL_S("\n### pollOpExecute ###\n");
	DEBUG_INIT_FULL_C("     serdesNum = ", serdesNum, 2);
	DEBUG_INIT_FULL_C("     dataArrIdx = ", dataArrIdx, 2);
	DEBUG_INIT_FULL_C("     unitBaseReg = ", unitBaseReg, 2);
	DEBUG_INIT_FULL_C("     unitOffset = ", unitOffset, 2);
	DEBUG_INIT_FULL_C("     data = ", data, 2);
	DEBUG_INIT_FULL_C("     mask = ", mask, 2);
	DEBUG_INIT_FULL_C("     numOfLoops = ", numOfLoops, 2);
	DEBUG_INIT_FULL_C("     waitTime = ", waitTime, 2);

	return MV_OK;
#endif
	do {
		regData = MV_REG_READ(regAddr) & mask;
		pollCounter++;
		mvOsUDelay(waitTime);
	} while ((regData != data) && (pollCounter < numOfLoops));

	if ((pollCounter >= numOfLoops) && (regData != data)) {
		DEBUG_INIT_S("pollOpExecute: TIMEOUT\n");
		return MV_TIMEOUT;
	}

	return MV_OK;
}

/***************************************************************************/
MV_OP getCfgSeqOp(MV_OP_PARAMS *params)
{
	if (params->waitTime == 0)
		return WRITE_OP;
	else if (params->numOfLoops == 0)
		return DELAY_OP;
	return POLL_OP;
}

/***************************************************************************/
MV_STATUS mvSeqExec
(
	MV_U32 serdesNum,
	MV_U32 seqId
)
{
	MV_U32 seqIdx;
	MV_OP_PARAMS    *seqArr;
	MV_U32 seqSize;
	MV_U32 dataArrIdx;
	MV_OP currOp;

	DEBUG_INIT_FULL_S("\n### mvSeqExec ###\n");
	DEBUG_INIT_FULL_C("seq id = ", seqId, 2);

	seqArr = serdesSeqDb[seqId].opParamsPtr;
	seqSize = serdesSeqDb[seqId].cfgSeqSize;
	dataArrIdx = serdesSeqDb[seqId].dataArrIdx;

	DEBUG_INIT_FULL_C("seqSize= ", seqSize, 2);
	DEBUG_INIT_FULL_C("dataArrIdx = ", dataArrIdx, 2);

	/* Executing the sequence operations */
	for (seqIdx = 0; seqIdx < seqSize; seqIdx++) {
		currOp = getCfgSeqOp(&seqArr[seqIdx]);
		opExecuteFuncArr[currOp](serdesNum, &seqArr[seqIdx], dataArrIdx);
	}

	return MV_OK;
}
