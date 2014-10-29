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

static int rtc_ready = -1;

static void rtc_init(void);

/*******************************************************/
int rtc_get(struct rtc_time *tm)
{
	unsigned long time;

	if (rtc_ready != 1)
		rtc_init();

	to_tm((time = RTC_READ_REG(RTC_TIME_REG_OFFS)), tm);

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

	/* WA for failing time set attempts. The HW ERRATA information should be added here */
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	mdelay(100);
	/* End of SW WA */
	RTC_WRITE_REG(time, RTC_TIME_REG_OFFS);
	RTC_WRITE_REG(time, RTC_TIME_REG_OFFS);

	return 0;
}

/*******************************************************/
void rtc_reset(void)
{
	if (rtc_ready != 1)
		rtc_init();
}

/*******************************************************/
static void rtc_init(void)
{
	uint32_t stat, alrm1, alrm2, int1, int2, tstcfg;

	/* Update RTC-MBUS bridge timing parameters */
	MV_REG_WRITE(MV_RTC2_SOC_OFFSET, 0xFD4D4CFA);

	/* Setup nominal register access timing */
	RTC_WRITE_REG(RTC_NOMINAL_TIMING, RTC_CLOCK_CORR_REG_OFFS);

	/* Make sure we are not in any test mode */
	RTC_WRITE_REG(0, RTC_TEST_CONFIG_REG_OFFS);
	mdelay(500);

	/* Turn off Int1 sources & clear the Alarm count */
	RTC_WRITE_REG(0, RTC_IRQ_1_CONFIG_REG_OFFS);
	RTC_WRITE_REG(0, RTC_ALARM_1_REG_OFFS);

	/* Turn off Int2 sources & clear the Periodic count */
	RTC_WRITE_REG(0, RTC_IRQ_2_CONFIG_REG_OFFS);
	RTC_WRITE_REG(0, RTC_ALARM_2_REG_OFFS);

	/* Clear any pending Status bits */
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK), RTC_STATUS_REG_OFFS);
	stat   = RTC_READ_REG(RTC_STATUS_REG_OFFS) & 0xFF;
	alrm1  = RTC_READ_REG(RTC_ALARM_1_REG_OFFS);
	int1   = RTC_READ_REG(RTC_IRQ_1_CONFIG_REG_OFFS) & 0xFF;
	alrm2  = RTC_READ_REG(RTC_ALARM_2_REG_OFFS);
	int2   = RTC_READ_REG(RTC_IRQ_2_CONFIG_REG_OFFS) & 0xFF;
	tstcfg = RTC_READ_REG(RTC_TEST_CONFIG_REG_OFFS) & 0xFF;

	if ((0xFC == stat)  &&
		(0 == alrm1) && (0xC0 == int1) &&
		(0 == alrm2) && (0xC0 == int2) &&
		(0 == tstcfg)) {
		/* Setup the loosest register access timing possible */
		RTC_WRITE_REG(~RTC_SZ_TIMING_RESERVED1_MASK, RTC_CLOCK_CORR_REG_OFFS);
	}

	rtc_ready = 1;
}

#endif	/* CONFIG_CMD_DATE */
