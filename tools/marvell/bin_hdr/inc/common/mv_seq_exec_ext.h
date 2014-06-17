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
#ifndef _MV_SEQ_EXEC_EXT_H
#define _MV_SEQ_EXEC_EXT_H

#include "mv_os.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "soc_spec.h"
#include "mvSiliconIf.h"

#if defined(MV_MSYS_AC3)	/* TBD - create a platform-specifica function for obtaining these parameters */
#define MAX_SERDES_LANES		3
#define MAX_SERDES_UNITS		12
#else
#define MAX_SERDES_LANES		6
#define MAX_SERDES_UNITS		0 /* Not used so far, should be re-defined later */
#endif

#define NA                      8
#define DEFAULT_PARAM           0
#define MV_BOARD_TCLK_ERROR     0xFFFFFFFF

#define NO_DATA                 0xFFFFFFFF
#define MAX_DATA_ARRAY          5
#define FIRST_CELL              0

/* Operation parameters */
typedef struct {
	MV_HWS_UNITS_ID	unitId;
	MV_U32			offset;
	MV_U32			mask;
	MV_U32			data[MAX_DATA_ARRAY];            /* data array */
	MV_U8			waitTime;                         /* msec */
	MV_U16			numOfLoops;                      /* for polling only */
} MV_OP_EXT_PARAMS;

/*
    Sequence parameters. Each sequence contains:
        1. Sequence id.
        2. Sequence size (total amount of operations during the sequence)
        3. a series of operations. operations can be write, poll or delay
        4. index in the data array (the entry where the relevant data sits)
 */
typedef struct {
	MV_OP_EXT_PARAMS	*opParamsPtr;
	MV_U8				cfgSeqSize;
	MV_U8				dataArrIdx;
} MV_CFG_EXT_SEQ;

/* Operation types */
typedef enum {
	WRITE_EXT_OP,
	DELAY_EXT_OP,
	POLL_EXT_OP
} MV_EXT_OP;

/* A generic function type for executing an operation (write, poll or delay) */
typedef MV_STATUS (*opExecuteFuncExtPtr)
(
	MV_U32				serdesNum,
	MV_OP_EXT_PARAMS	*params,
	MV_U32				dataArrIdx
);

/**************************************************************************
 * mvSeqExtExec -
 *
 * DESCRIPTION:          Executes a serdes sequence
 * INPUT:                unitNum         - the unit which the sequence
 *                                         is executed for
 *                       seqId           - the id of the executed sequence
 * OUTPUT:               None.
 * RETURNS:              MV_OK           - for success
 *                       MV_BAD_PARAM    - for bad sequence id or serdes num
 *                       MV_TIMEOUT      - for an unsuccessful polling
 ***************************************************************************/
MV_STATUS mvSeqExecExt(MV_U32 unitNum, MV_U32 seqId);


#endif /*_MV_SEQ_EXEC_EXT_H*/
