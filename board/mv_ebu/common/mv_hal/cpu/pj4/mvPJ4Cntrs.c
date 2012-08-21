/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "mvOs.h"
#include "../mvCpuCntrs.h"

int		mvCpuCntrsMap(int counter, MV_CPU_CNTRS_OPS op)
{
	int	event;

	switch (op) {
	case MV_CPU_CNTRS_CYCLES:
		event = MRVL_PJ4B_CYCLE_COUNT;
		break;

	case MV_CPU_CNTRS_INSTRUCTIONS:
		event = MRVL_PJ4B_INSN_EXECUTED;
		break;

	case MV_CPU_CNTRS_ICACHE_READ_MISS:
		event = MRVL_PJ4B_ICACHE_READ_MISS;
		break;

	case MV_CPU_CNTRS_DCACHE_READ_MISS:
		event = MRVL_PJ4B_DCACHE_READ_MISS;
		break;

	case MV_CPU_CNTRS_DCACHE_WRITE_MISS:
		event = MRVL_PJ4B_DCACHE_WRITE_MISS;
		break;

	case MV_CPU_CNTRS_DTLB_MISS:
		event = MRVL_PJ4B_DTLB_MISS;
		break;

	default:
		mvOsPrintf("invalid CPU counters event %d\n", op);
		return -1;
	}
	return event;
}


int	mvCpuCntrsStart(int idx, int event)
{
	MV_U32 enable;

	if (idx == MRVL_PJ4B_CCNT)
		enable = (1 << MRVL_PJ4B_CCNT_BIT_OFFSET);
	else if (idx < MRVL_PJ4B_MAX_COUNTERS)
		enable = (1 << (idx - MRVL_PJ4B_PMN0));
	else {
		mvOsPrintf("invalid counter number (%d)\n", idx);
		return 1;
	}
	mrvl_pj4b_pmu_cntr_disable(enable);
	if (idx != MRVL_PJ4B_CCNT) {
		/* select event */
		if (event >= MRVL_PJ4B_EVT_UNUSED)
			return 1;

		mrvl_pj4b_pmu_select_event((idx - MRVL_PJ4B_PMN0), event);
	}
	mrvl_pj4b_pmu_cntr_enable(enable);
	return 0;
}

MV_VOID	mvCpuCntrsInit(MV_VOID)
{
	MV_U32	pmnc;

	mvCpuCntrsReset();

	pmnc = mrvl_pj4b_read_pmnc();

	pmnc &= ~MRVL_PJ4B_PMU_RESET;
	pmnc |= MRVL_PJ4B_PMU_ENABLE;

	mrvl_pj4b_write_pmnc(pmnc);
}

