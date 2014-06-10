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

#include "mv_seq_exec_ext.h"

#ifdef MV88F68XX
#include "ddr3_a38x.h"
#endif

/* Forward declarations of local functions */
static MV_STATUS writeOpExecuteExt
(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
);

static MV_STATUS delayOpExecuteExt
(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
);

static MV_STATUS pollOpExecuteExt
(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
);

/* Array for mapping the operation (write, poll or delay) functions */
static opExecuteFuncExtPtr opExecuteFuncExtArr[] =
{
	writeOpExecuteExt,
	delayOpExecuteExt,
	pollOpExecuteExt
};

extern MV_CFG_EXT_SEQ serdesSeqDb[];

/***************************************************************************/
static MV_STATUS writeOpExecuteExt(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
)
{
	MV_U32			offset, data, mask;
	MV_HWS_UNITS_ID	unit;

	/* Getting write op params from the input parameter */

	unit   = params->unitId;
	offset = params->offset;
	data   = params->data[dataArrIdx];
	mask   = params->mask;

	if (unitNum >= MAX_SERDES_LANES) {
		DEBUG_INIT_S("writeOpExecuteExt: bad serdes number\n");
		return MV_BAD_PARAM;
	}

	/* an empty operation */
	if (data == NO_DATA)
		return MV_OK;

	CHECK_STATUS(mvGenUnitRegisterSet(unit, unitNum, offset, data, mask));

	return MV_OK;
}

/***************************************************************************/
static MV_STATUS delayOpExecuteExt
(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
)
{
	mvOsDelay(params->waitTime);
	return MV_OK;
}

/***************************************************************************/
static MV_STATUS pollOpExecuteExt(
	MV_U32				unitNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
)
{
	MV_U32			offset, data, mask, numOfLoops, waitTime;
	MV_U32			pollCounter = 0;
	MV_U32			regData;
	MV_HWS_UNITS_ID	unit;
	MV_STATUS		rc;

	/* Getting poll op params from the input parameter */
	unit   = params->unitId;
	offset = params->offset;
	data   = params->data[dataArrIdx];
	mask   = params->mask;
	numOfLoops = params->numOfLoops;
	waitTime   = params->waitTime;

	if (unitNum >= MAX_SERDES_LANES) {
		DEBUG_INIT_S("pollOpExecuteExt: bad serdes number\n");
		return MV_BAD_PARAM;
	}

	/* an empty operation */
	if (data == NO_DATA)
		return MV_OK;

	/* Polling */

#ifdef WIN32

	DEBUG_INIT_FULL_S("\n### pollOpExecuteExt ###\n");
	DEBUG_INIT_FULL_C("     unitNum = ", unitNum, 2);
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
		rc = mvGenUnitRegisterGet(unit, unitNum, offset, &regData, mask);
		if (rc != MV_OK)
			return rc;

		pollCounter++;
		mvOsDelay(waitTime);

	} while ((regData != data) && (pollCounter < numOfLoops));

	if ((pollCounter >= numOfLoops) && (regData != data)) {
		DEBUG_INIT_S("pollOpExecuteExt: TIMEOUT\n");
		return MV_TIMEOUT;
	}

	return MV_OK;
}

/***************************************************************************/
static MV_EXT_OP getCfgSeqExtOp(MV_OP_EXT_PARAMS *params)
{
	if (params->waitTime == 0)
		return WRITE_EXT_OP;
	else if (params->numOfLoops == 0)
		return DELAY_EXT_OP;
	return POLL_EXT_OP;
}

/***************************************************************************/
MV_STATUS mvSeqExecExt(
	MV_U32	unitNum,
	MV_U32	seqId
)
{
	MV_U32				seqIdx;
	MV_OP_EXT_PARAMS	*seqArr;
	MV_U32				seqSize;
	MV_U32				dataArrIdx;
	MV_EXT_OP			currOp;

	DEBUG_INIT_FULL_S("\n### mvSeqExtExec ###\n");
	DEBUG_INIT_FULL_C("seq id = ", seqId, 2);

	seqArr     = serdesSeqDb[seqId].opParamsPtr;
	seqSize    = serdesSeqDb[seqId].cfgSeqSize;
	dataArrIdx = serdesSeqDb[seqId].dataArrIdx;

	DEBUG_INIT_FULL_C("seqSize= ", seqSize, 2);
	DEBUG_INIT_FULL_C("dataArrIdx = ", dataArrIdx, 2);

	/* Executing the sequence operations */
	for (seqIdx = 0; seqIdx < seqSize; seqIdx++) {
		currOp = getCfgSeqExtOp(&seqArr[seqIdx]);
		CHECK_STATUS(opExecuteFuncExtArr[currOp](unitNum, &seqArr[seqIdx], dataArrIdx));
	}

	return MV_OK;
}
