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

/* #define	RTC2_DEBUG  */

#include <common.h>
#include <command.h>
#include <rtc.h>
#include "mv_rtc2.h"

#if defined(CONFIG_CMD_DATE)
/* This define for WA in rtc read */
#define SAMPLE_NR 100
static int rtc_ready = -1;


/*******************************************************/
void rtc_init(void)
{
	/* Update RTC-MBUS bridge timing parameters */
	#ifdef ERRATA_FE_3124064
	/* Functional Errata Ref #: FE-3124064 -  WA for failing time read attempts.
	 * Description:
	 * 	The device supports CPU write and read access to the RTC Time register.
	 * 	However, due to this erratum, Write to RTC TIME register may fail.
	 * 	Read from RTC TIME register may fail.
	 * Workaround:
	 * 	Before writing to RTC TIME register, issue a dummy write of 0x0 twice to RTC Status register.
	 * 	RTC TIME register should be read twice, the second read will return a proper value.
	 * 	Configure maximum value (0x3FF) in write clock period in RTC Mbus Bridge Timing Control register.
	 * Functional Impact After Workaround is applied:
	 * 	No functional impact after WA is applied
	 */
	MV_REG_WRITE(MV_RTC2_SOC_OFFSET, 0xFD4D4FFF);
#else
	MV_REG_WRITE(MV_RTC2_SOC_OFFSET, 0xFD4D4CFA);
#endif
	rtc_ready = 1;
}

#ifdef ERRATA_FE_3124064
struct _strTime2Freq {
	unsigned long nTime;
	unsigned int  nFreq;

};
#endif

/*******************************************************/
int rtc_get(struct rtc_time *tm)
{
#ifdef ERRATA_FE_3124064
	/* Functional Errata Ref #: FE-3124064 - WA for failing time read attempts.
	 * Description:
	 *      The device supports CPU write and read access to the RTC Time register.
	 *	However, due to this erratum, Write to RTC TIME register may fail.
	 *	Read from RTC TIME register may fail.
	 * Workaround:
	 *	Before writing to RTC TIME register, issue a dummy write of 0x0 twice to
	 *	RTC Status register.
	 *	Configure maximum value (0x3FF) in write clock period in RTC Mbus Bridge
	 *	Timing Control register.
	 *	Before writing to RTC TIME register, issue a dummy write of 0x0 twice to
	 *	RTC Status register.
	 *      RTC TIME register should be read 100 times, then find the result
	 *	that appear most frequently, use this result as the correct value.
	 */
	unsigned long nTimeArray[SAMPLE_NR], i, j, nTime, nMax = 0, indexMax = SAMPLE_NR - 1;
	struct _strTime2Freq sTimeToFreq[SAMPLE_NR];
#endif

	if (rtc_ready != 1)
		rtc_init();
#ifdef ERRATA_FE_3124064
	/* read RTC TIME register 100 times and save the values in array,
	   initialize the counters to zero */
	for (i = 0; i < SAMPLE_NR; i++) {
		sTimeToFreq[i].nFreq = 0;
		nTimeArray[i] = RTC_READ_REG(RTC_TIME_REG_OFFS);
	}
	for (i = 0; i < SAMPLE_NR; i++) {
		nTime = nTimeArray[i];
		/* if nTime appears in sTimeToFreq array so add the counter of nTime value,
		   if didn't appear yet in counters array then allocate new member of
		   sTimeToFreq array with counter = 1 */
		for (j = 0; j < SAMPLE_NR; j++) {
			if (sTimeToFreq[j].nFreq == 0 || sTimeToFreq[j].nTime == nTime)
				break;
		}
		if (sTimeToFreq[j].nFreq == 0)
			sTimeToFreq[j].nTime = nTime;
		sTimeToFreq[j].nFreq++;
		/*find the most common result*/
		if (nMax < sTimeToFreq[j].nFreq) {
			indexMax = j;
			nMax = sTimeToFreq[j].nFreq;
		}
	}

	to_tm(sTimeToFreq[indexMax].nTime, tm);
#else
	to_tm(RTC_READ_REG(RTC_TIME_REG_OFFS), tm);
#endif
	return 0;
}
/*******************************************************/
int rtc_set(struct rtc_time *tm)
{
	unsigned long time;

	if (rtc_ready != 1)
		rtc_init();

	time = mktime(tm->tm_year, tm->tm_mon,
				  tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

#ifdef ERRATA_FE_3124064
	/* Functional Errata Ref #: FE-3124064 -  WA for failing time read attempts.
	 * Description:
	 * 	The device supports CPU write and read access to the RTC Time register.
	 * 	However, due to this erratum, Write to RTC TIME register may fail.
	 * 	Read from RTC TIME register may fail.
	 * Workaround:
	 * 	Before writing to RTC TIME register, issue a dummy write of 0x0 twice to RTC Status register.
	 * 	RTC TIME register should be read twice, the second read will return a proper value.
	 * 	Configure maximum value (0x3FF) in write clock period in RTC Mbus Bridge Timing Control register.
	 * Functional Impact After Workaround is applied:
	 * 	No functional impact after WA is applied
	 */
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
	RTC_WRITE_REG(time, RTC_TIME_REG_OFFS);

	return 0;
}

/*******************************************************/
void rtc_reset(void)
{
	/* Reset Test register */
	RTC_WRITE_REG(0, RTC_TEST_CONFIG_REG_OFFS);
	mdelay(500); /* Oscillator startup time */

	/* Reset time register */
	RTC_WRITE_REG(0, RTC_TIME_REG_OFFS);
	udelay(62);

	/* Reset Status register */
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK), RTC_STATUS_REG_OFFS);
	udelay(62);

	/* Turn off Int1 and Int2 sources & clear the Alarm count */
	RTC_WRITE_REG(0, RTC_IRQ_1_CONFIG_REG_OFFS);
	RTC_WRITE_REG(0, RTC_IRQ_2_CONFIG_REG_OFFS);
	RTC_WRITE_REG(0, RTC_ALARM_1_REG_OFFS);
	RTC_WRITE_REG(0, RTC_ALARM_2_REG_OFFS);

	/* Setup nominal register access timing */
	RTC_WRITE_REG(RTC_NOMINAL_TIMING, RTC_CLOCK_CORR_REG_OFFS);

	/* Reset time register */
	RTC_WRITE_REG(0, RTC_TIME_REG_OFFS);
	udelay(10);

	/* Reset Status register */
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK), RTC_STATUS_REG_OFFS);
	udelay(50);
}

#endif	/* CONFIG_CMD_DATE */
