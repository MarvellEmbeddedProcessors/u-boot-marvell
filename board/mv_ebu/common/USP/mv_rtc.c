/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/


/* #define	RTC_DEBUG  */

#include <common.h>
#include <command.h>
#include <rtc.h>

#ifdef MV_INCLUDE_RTC
#include "rtc/integ_rtc/mvRtc.h"
#elif defined(CONFIG_RTC_DS1338_DS1339)
#include "rtc/ext_rtc/mvDS133x.h"
#endif

#if defined(CONFIG_CMD_DATE)

/* since this rtc dosen't support century we will use 20 as default */
#define CENTURY 20

int rtc_get( struct rtc_time *tmp )
{
	MV_RTC_TIME time;
#ifdef MV_INCLUDE_RTC
	mvRtcTimeGet(&time);
#elif defined(CONFIG_RTC_DS1338_DS1339)
	mvRtcDS133xTimeGet(&time);
#endif
	tmp->tm_year = (CENTURY * 100) + time.year;
	tmp->tm_mon = time.month;
	tmp->tm_mday = time.date;
	tmp->tm_wday = time.day;
	tmp->tm_hour = time.hours;
	tmp->tm_min = time.minutes;
	tmp->tm_sec = time.seconds;
        tmp->tm_yday = 0;
        tmp->tm_isdst= 0;
#ifdef RTC_DEBUG
	printf( "Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec );
#endif
	return 0;
}

int rtc_set( struct rtc_time *tmp )
{
	MV_RTC_TIME time;
#ifdef RTC_DEBUG
	printf( "Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	time.month = tmp->tm_mon;
	time.date = tmp->tm_mday;
	time.day = tmp->tm_wday;
	time.hours = tmp->tm_hour;
	time.minutes = tmp->tm_min;
	time.seconds = tmp->tm_sec;
	if((tmp->tm_year/100) != CENTURY)
		printf("Warning: century isn't supported to be set(always %d) \n",CENTURY);
	time.year = tmp->tm_year%100;
#ifdef MV_INCLUDE_RTC
	mvRtcTimeSet(&time);
#elif defined(CONFIG_RTC_DS1338_DS1339)
	mvRtcDS133xTimeSet(&time);
#endif
	return 0;
}

void rtc_reset (void)
{
	/* No need for init in any of the devices */
	/* The OSC enable bit in the DS1338 is clear at the first date set */
}

#endif	/* CONFIG_CMD_DATE */
