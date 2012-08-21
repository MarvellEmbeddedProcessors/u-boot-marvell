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

#include "mvCommon.h"		/* Should be included before mvSysHwConfig */
#include "mvTypes.h"
#include "mvDebug.h"
#include "mvOs.h"

#include "gbe/mvNeta.h"

#include "mvPmt.h"

/*#define PMT_DBG mvOsPrintf*/
#define PMT_DBG(X...)

/*******************************************************************************
* mvNetaPmtWrite - Add entry to Packet Modification Table
* DESCRIPTION:
*
* INPUT:
*       int			port - NETA port number
*
* RETURN:   MV_STATUS
*               MV_OK - Success, Others - Failure
*
* NOTE:
*******************************************************************************/
MV_STATUS mvNetaPmtWrite(int port, int idx, MV_NETA_PMT *pEntry)
{
	if (mvNetaPortCheck(port))
		return MV_OUT_OF_RANGE;

	if (mvNetaMaxCheck(idx, NETA_TX_PMT_SIZE))
		return MV_OUT_OF_RANGE;

	PMT_DBG("%s: 0x%08x <-- 0x%x\n", __func__, NETA_TX_PMT_REG(port), idx);
	MV_REG_WRITE(NETA_TX_PMT_REG(port), (MV_U32) idx);
	MV_REG_WRITE(NETA_TX_PMT_W0_REG(port), pEntry->mt_w0 & NETA_TX_PMT_W0_MASK);
	MV_REG_WRITE(NETA_TX_PMT_W1_REG(port), pEntry->mt_w1 & NETA_TX_PMT_W1_MASK);
	MV_REG_WRITE(NETA_TX_PMT_W2_REG(port), pEntry->mt_w2 & NETA_TX_PMT_W2_MASK);

	return MV_OK;
}
/*******************************************************************************
* mvNetaPmtRead - Read entry from Packet Modification Table
* DESCRIPTION:
*
* INPUT:
*       int			port - NETA port number
*       int			inx - PMT entry index to read from
* OUTPUT:
*       MV_NETA_PMT	pEntry - PMT entry
*
* RETURN:   MV_STATUS
*               MV_OK - Success, Others - Failure
*
* NOTE:
*******************************************************************************/
MV_STATUS mvNetaPmtRead(int port, int idx, MV_NETA_PMT *pEntry)
{
	if (mvNetaPortCheck(port))
		return MV_OUT_OF_RANGE;

	if (mvNetaMaxCheck(idx, NETA_TX_PMT_SIZE))
		return MV_OUT_OF_RANGE;

	PMT_DBG("%s: 0x%08x <-- 0x%x\n", __func__, NETA_TX_PMT_REG(port), idx);

	MV_REG_WRITE(NETA_TX_PMT_REG(port), (MV_U32) idx);
	pEntry->mt_w0 = MV_REG_READ(NETA_TX_PMT_W0_REG(port));
	pEntry->mt_w1 = MV_REG_READ(NETA_TX_PMT_W1_REG(port));
	pEntry->mt_w2 = MV_REG_READ(NETA_TX_PMT_W2_REG(port));

	return MV_OK;
}

/*******************************************************************************
* mvNetaPmtInit - Clear Packet Modification Table
* DESCRIPTION:
*
* INPUT:
*       int			port - NETA port number
*
* RETURN:   MV_STATUS
*               MV_OK - Success, Others - Failure
*
* NOTE:
*******************************************************************************/
MV_STATUS mvNetaPmtInit(int port)
{
	int idx;
	MV_NETA_PMT entry;

	if (mvNetaPortCheck(port))
		return MV_OUT_OF_RANGE;

	memset(&entry, 0, sizeof(MV_NETA_PMT));

	for (idx = 0; idx < NETA_TX_PMT_SIZE; idx++)
		mvNetaPmtWrite(port, idx, &entry);

	return MV_OK;
}

/*******************************************************************************
* mvNetaPmtDump - Dump Packet Modification Table
* DESCRIPTION:
*
* INPUT:
*       int			port - NETA port number
*
* RETURN:   MV_STATUS
*               MV_OK - Success, Others - Failure
*
* NOTE:
*******************************************************************************/
MV_STATUS mvNetaPmtDump(int port)
{
	int idx, count = 0;
	MV_NETA_PMT entry;

	if (mvNetaPortCheck(port))
		return MV_OUT_OF_RANGE;

	for (idx = 0; idx < NETA_TX_PMT_SIZE; idx++) {
		mvNetaPmtRead(port, idx, &entry);
		if (entry.mt_w0 || entry.mt_w1 || entry.mt_w2) {
			count++;
			mvOsPrintf("[%2x] 0x%08x 0x%08x 0x%08x\n", idx, entry.mt_w0, entry.mt_w1, entry.mt_w2);
		}
	}

	if (!count)
		mvOsPrintf("PMT is empty, %d entries\n", NETA_TX_PMT_SIZE);

	return MV_OK;
}

/* Set MH register */
MV_STATUS mvNetaPmtMhRegSet(int port, int txp, int reg, MV_U16 val)
{
	if (mvNetaTxpCheck(port, txp))
		return MV_OUT_OF_RANGE;

	if (mvNetaMaxCheck(reg, NETA_TX_MAX_MH_REGS))
		return MV_OUT_OF_RANGE;

	MV_REG_WRITE(NETA_TX_MH_REG(port, txp, reg), val);

	return MV_OK;
}

/* Set ETH_TYPE register */
MV_STATUS mvNetaPmtEthTypeRegSet(int port, int txp, int reg, MV_U16 val)
{
	if (mvNetaTxpCheck(port, txp))
		return MV_OUT_OF_RANGE;

	if (mvNetaMaxCheck(reg, NETA_TX_MAX_ETH_TYPE_REGS))
		return MV_OUT_OF_RANGE;

	MV_REG_WRITE(NETA_TX_ETH_TYPE_REG(port, txp, reg), val);

	return MV_OK;
}
