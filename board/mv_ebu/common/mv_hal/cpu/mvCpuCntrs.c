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
#include "mvCpuCntrs.h"

MV_CPU_CNTRS_ENTRY mvCpuCntrsTbl[MV_CPU_CNTRS_NUM];

void mvCpuCntrsEventClear(MV_CPU_CNTRS_EVENT *pEvent)
{
	int i;

	for (i = 0; i < MV_CPU_CNTRS_NUM; i++)
		pEvent->counters_sum[i] = 0;
	pEvent->num_of_measurements = 0;
}

MV_CPU_CNTRS_EVENT *mvCpuCntrsEventCreate(char *name, MV_U32 print_threshold)
{
	int i;
	MV_CPU_CNTRS_EVENT *event = mvOsMalloc(sizeof(MV_CPU_CNTRS_EVENT));

	if (event) {
		strncpy(event->name, name, sizeof(event->name));
		event->num_of_measurements = 0;
		event->avg_sample_count = print_threshold;
		for (i = 0; i < MV_CPU_CNTRS_NUM; i++) {
			event->counters_before[i] = 0;
			event->counters_after[i] = 0;
			event->counters_sum[i] = 0;
		}
	}
	return event;
}

void mvCpuCntrsEventDelete(MV_CPU_CNTRS_EVENT *event)
{
	if (event != NULL)
		mvOsFree(event);
}

MV_STATUS mvCpuCntrsProgram(int counter, MV_CPU_CNTRS_OPS op, char *name, MV_U32 overhead)
{
	int	opIdx, size = MV_CPU_CNTR_SIZE;

	strncpy(mvCpuCntrsTbl[counter].name, name, sizeof(mvCpuCntrsTbl[counter].name));
	mvCpuCntrsTbl[counter].operation = op;
	mvCpuCntrsTbl[counter].overhead = overhead;

	if (size == 64) {
		MV_U64 max_val = ~0;
		mvCpuCntrsTbl[counter].max_value = max_val;
	} else if (size == 32) {
		MV_U32 max_val = ~0;
		mvCpuCntrsTbl[counter].max_value = max_val;
	} else
		mvOsPrintf("%s: Unexpected counter size = %d\n", __func__, size);

	opIdx = mvCpuCntrsMap(counter, op);
	if (opIdx == -1) {
		mvOsPrintf("%s: Can't map CPU counter %d, to %d event\n", __func__, counter, op);
		return MV_FAIL;
	}

	if (mvCpuCntrsStart(counter, opIdx)) {
		mvOsPrintf("%s: Can't start CPU counter %d, op=%d, opIdx=%d\n", __func__, counter, op, opIdx);
		return MV_FAIL;
	}

	mvOsPrintf("%s: Counter=%d, opIdx=%d, overhead=%d, max=0x%llx\n",
		   mvCpuCntrsTbl[counter].name, counter, opIdx, mvCpuCntrsTbl[counter].overhead,
		   mvCpuCntrsTbl[counter].max_value);

	return MV_OK;
}

void mvCpuCntrsShow(MV_CPU_CNTRS_EVENT *pEvent)
{
	int			i;
	MV_U64		counters_avg;
	static int	title = 0;

	if ((title % 64) == 0) {

		mvOsPrintf("\n%16s  ", " ");
		for (i = 0; i < MV_CPU_CNTRS_NUM; i++) {
			if (mvCpuCntrsTbl[i].operation == MV_CPU_CNTRS_INVALID)
				continue;

			mvOsPrintf(" %8s, ", mvCpuCntrsTbl[i].name);
		}
		mvOsPrintf("\n");
	}

	mvOsPrintf("%16s: ", pEvent->name);
	for (i = 0; i < MV_CPU_CNTRS_NUM; i++) {
		if (mvCpuCntrsTbl[i].operation == MV_CPU_CNTRS_INVALID)
			continue;


		counters_avg = mvOsDivMod64(pEvent->counters_sum[i], pEvent->num_of_measurements, NULL);
/*		counters_avg = pEvent->counters_sum[i] / pEvent->num_of_measurements;*/
		if (counters_avg >= mvCpuCntrsTbl[i].overhead)
			counters_avg -= mvCpuCntrsTbl[i].overhead;
		else
			counters_avg = 0;
		mvOsPrintf(" %8llu, ", counters_avg);

/*
		mvOsPrintf("%s = %5llu / %u, ",
				mvCpuCntrsTbl[i].name, pEvent->counters_sum[i], pEvent->num_of_measurements);
*/
	}
	mvOsPrintf("\n");
	mvCpuCntrsEventClear(pEvent);
	mvCpuCntrsReset();

	title++;
}

void mvCpuCntrsStatus(void)
{
	int i;

	for (i = 0; i < MV_CPU_CNTRS_NUM; i++)
		mvOsPrintf("#%d: %s, overhead=%d\n", i, mvCpuCntrsTbl[i].name, mvCpuCntrsTbl[i].overhead);
}

void mvCpuCntrsInitialize(void)
{
	memset(mvCpuCntrsTbl, 0, sizeof(mvCpuCntrsTbl));
	mvCpuCntrsInit();
}

