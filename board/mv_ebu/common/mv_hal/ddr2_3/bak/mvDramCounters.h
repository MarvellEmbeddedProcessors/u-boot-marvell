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

#ifndef __INCmvDramCountersh
#define __INCmvDramCountersh

/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysDdrConfig.h"
#include "mvDramIfRegs.h"



#define SDRAM_STAT_CNTRS_CTRL  			(MV_DDR_REGS_BASE + 0x1590)
#define SDRAM_STAT_CNTR_SELECT_OFFSET(c)	((c == 0) ? 0 : 8)
#define SDRAM_STAT_CNTR_SELECT_MASK(c)		(0x1F << SDRAM_STAT_CNTR_SELECT_OFFSET(c))
#define SDRAM_STAT_CNTR_RESET_OFFSET		16
#define SDRAM_STAT_CNTR_RESET_MASK		(0x1 << SDRAM_STAT_CNTR_RESET_OFFSET)
#define SDRAM_STAT_CNTR_START_OFFSET		17
#define SDRAM_STAT_CNTR_START_MASK		(0x1 << SDRAM_STAT_CNTR_START_OFFSET)

#define SDRAM_STAT_CNTRS_VAL(set, idx)		(MV_DDR_REGS_BASE + 0x1594 + (set << 3) + (idx << 2))
#define SDRAM_STAT_HCLK_VAL(idx)		(MV_DDR_REGS_BASE + 0x15A4 + (idx << 2))


typedef enum {
	MBUSL_CACHE_READ = 0,
	MBUSL_BURST_READ,
	MBUSL_PAR_READ,
	MBUSL_CACHE_WRITE,
	MBUSL_BURST_WRITE,
	MBUSL_UNSPEC_WRITE,
	MBUSL_FULL_PAR_WRITE,
	MBUSL_SEMI_PAR_WRITE,
	MBUSL_TOTAL_READS,
	MBUSL_TOTAL_WRITES,
	MBUSL_TOTAL_TRANS,
	MBUSL0_TOTAL_TRANS,
	MBUSL1_TOTAL_TRANS,
	MBUSL_LOOKUP_HIT,
	MBUS_CACHE_READ,
	MBUS_BURST_READ,
	MBUS_PAR_READ,
	MBUS_CACHE_WRITE,
	MBUS_CACHE_WRITE_RMW,
	MBUS_BURST_WRITE,
	MBUS_BURST_WRITE_RMW,
	MBUS_PAR_WRITE,
	MBUS_PAR_WRITE_RMW,
	MBUS_TOTAL_READS,
	MBUS_TOTAL_WRITES,
	MBUS_TOTAL_TRANS,
	MBUS0_TOTAL_TRANS,
	MBUS1_TOTAL_TRANS,
	MBUS_BURST_CHOP,
	NUM_BURSTS_128BIT,
	DRAM_ACTIVE,
	DRAM_PRECHARGE
} MV_DRAM_STAT_MODE;

/*******************************************************************************
* mvDramStatStart
*
* DESCRIPTION:
*       Start DRAM statistics counters.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static inline void mvDramStatStart(void)
{
	MV_REG_BIT_SET(SDRAM_STAT_CNTRS_CTRL, SDRAM_STAT_CNTR_START_MASK);
}

/*******************************************************************************
* mvDramStatStop
*
* DESCRIPTION:
*       Stop DRAM statistics counters.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static inline void mvDramStatStop(void)
{
	MV_REG_BIT_RESET(SDRAM_STAT_CNTRS_CTRL, SDRAM_STAT_CNTR_START_MASK);
}

/*******************************************************************************
* mvDramStatClear
*
* DESCRIPTION:
*       Clear the DRAM statistics counters.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static inline void mvDramStatClear(void)
{
	MV_U32 reg;

	MV_REG_BIT_SET(SDRAM_STAT_CNTRS_CTRL, SDRAM_STAT_CNTR_RESET_MASK);
	do {
		reg = MV_REG_READ(SDRAM_STAT_CNTRS_CTRL);
	} while(reg & SDRAM_STAT_CNTR_RESET_MASK);

	return;
}


/*******************************************************************************
* mvDramStatConfig
*
* DESCRIPTION:
*       Configure the DRAM statistics counters.
*
* INPUT:
*       cntIdx	- The counter index to configure.
*	mode	- The mode to configure the counter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success,
*	MV_FAIL otherwise.
*
*******************************************************************************/
static inline MV_STATUS mvDramStatConfig(MV_U8 cntIdx, MV_DRAM_STAT_MODE mode)
{
	MV_U32 reg;

	reg = MV_REG_READ(SDRAM_STAT_CNTRS_CTRL);
	reg &= ~SDRAM_STAT_CNTR_SELECT_MASK(cntIdx);
	reg |= (mode << SDRAM_STAT_CNTR_SELECT_OFFSET(cntIdx));
	MV_REG_WRITE(SDRAM_STAT_CNTRS_CTRL, reg);

	return MV_OK;
}

/*******************************************************************************
* mvDramStatRead
*
* DESCRIPTION:
*       Read the current DRAM statistics value.
*
* INPUT:
*	None.
*
* OUTPUT:
*       counter0 - Value of DRAM statistics counter #0.
*       counter1 - Value of DRAM statistics counter #1.
*	hclk	 - Value of HCLK counter.
*
* RETURN:
*       MV_OK on success,
*	MV_FAIL otherwise.
*
*******************************************************************************/
static inline MV_STATUS mvDramStatRead(MV_U64 *counter0, MV_U64 *counter1, MV_U64 *hclk)
{
	MV_U32 val;

	if(counter0 != NULL) {
		val = MV_REG_READ(SDRAM_STAT_CNTRS_VAL(0, 0));
		*counter0 = val;
		val = MV_REG_READ(SDRAM_STAT_CNTRS_VAL(0, 1));
		*counter0 |= ((MV_U64)val << 32);
	}

	if(counter1 != NULL) {
		val = MV_REG_READ(SDRAM_STAT_CNTRS_VAL(1, 0));
		*counter1 = val;
		val = MV_REG_READ(SDRAM_STAT_CNTRS_VAL(1, 1));
		*counter1 |= ((MV_U64)val << 32);
	}

	if(hclk != NULL) {
		val = MV_REG_READ(SDRAM_STAT_HCLK_VAL(0));
		*hclk = val;
		val = MV_REG_READ(SDRAM_STAT_HCLK_VAL(1));
		*hclk |= ((MV_U64)val << 32);
	}

	return MV_OK;
}

#endif /* __INCmvDramCountersh */

