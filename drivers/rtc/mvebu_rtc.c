/*
 * ***************************************************************************
 * Copyright (C) 2016 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <rtc.h>
#include <asm/arch-mvebu/fdt.h>
#include "mvebu_rtc.h"

DECLARE_GLOBAL_DATA_PTR;

void __attribute__((section(".data"))) __iomem *rtc_base = NULL;

/*******************************************************/
void rtc_init(void)
{
	int node;
	unsigned long reg;

	node = fdt_node_offset_by_compatible(gd->fdt_blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_RTC_REG));
	if (node < 0) {
		debug("No rtc node found in FDT blob\n");
		return;
	}

	/* Get the rtc register base address */
	rtc_base = (void *)fdt_get_regs_offs(gd->fdt_blob, node, "reg");

	/* Update RTC-MBUS bridge timing parameters */
#ifdef ERRATA_FE_3124064
	/* Functional Errata Ref #: FE-3124064 -  WA for failing time read attempts.
	 * Description:
	 *	The device supports CPU write and read access to the RTC Time register.
	 *	However, due to this erratum, Write to RTC TIME register may fail.
	 *	Read from RTC TIME register may fail.
	 * Workaround:
	 * 1. Configure the RTC Mbus Bridge Timing Control register (offset 0x284080 and 0x284084)
	 *	- Write RTC WRCLK Period 0x3FF (default value is 0xFA)
	 *	- Write RTC WRCLK setup to 0x29 (default value is 0x53)
	 *	- Write RTC Read Output Delay to 0x3F (default value is 0x10)
	 *	- Write RTC WRCLK High Time to 0x53 (default value)
	 *	- Mbus - Read All Byte Enable to 0x1 (default value)
	 * 2. Configure the RTC Test Configuration Register (offset 0x28401C) bit3 to '1' (Reserved, Marvell internal)
	 *
	 * RTC Time register write operation:
	 *	- Issue two dummy writes of 0x0 to the RTC Status register (offset 0x284000).
	 *	- Write the time to the RTC Time register (offset 0x28400C).
	*/
	reg = readl(rtc_base + MV_RTC0_SOC_OFFSET);
	reg &= ~RTC_WRCLK_PERIOD_MASK;
	reg |= 0x3FF << RTC_WRCLK_PERIOD_OFFS;
	reg &= ~RTC_WRCLK_SETUP_MASK;
	reg |= 0x29 << RTC_WRCLK_SETUP_OFFS;
	writel(reg, rtc_base + MV_RTC0_SOC_OFFSET);

	reg = readl(rtc_base + MV_RTC1_SOC_OFFSET);
	reg &= ~RTC_READ_OUTPUT_DELAY_MASK;
	reg |= 0x3F << RTC_READ_OUTPUT_DELAY_OFFS;
	writel(reg, rtc_base + MV_RTC1_SOC_OFFSET);

	reg = RTC_READ_REG(RTC_TEST_CONFIG_REG_OFFS);
	reg |= 0x8;
	RTC_WRITE_REG(reg, RTC_TEST_CONFIG_REG_OFFS);
#else
	reg = readl(rtc_base + MV_RTC1_SOC_OFFSET);
	reg &= ~RTC_READ_OUTPUT_DELAY_MASK;
	reg |= 0x1F << RTC_READ_OUTPUT_DELAY_OFFS;
	writel(reg, rtc_base + MV_RTC1_SOC_OFFSET);
#endif
}

int rtc_get(struct rtc_time *tm)
{
	if (!rtc_base)
		rtc_init();
	to_tm(RTC_READ_REG(RTC_TIME_REG_OFFS), tm);
	return 0;
}

int rtc_set(struct rtc_time *tm)
{
	unsigned long time;

	if (!rtc_base)
		rtc_init();

	time = mktime(tm->tm_year, tm->tm_mon,
				  tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

#ifdef ERRATA_FE_3124064
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
	RTC_WRITE_REG(time, RTC_TIME_REG_OFFS);

	return 0;
}

void rtc_reset(void)
{
	/* Reset Test register */
	RTC_WRITE_REG(0, RTC_TEST_CONFIG_REG_OFFS);
	mdelay(500); /* Oscillator startup time */

	/* Reset time register */
#ifdef ERRATA_FE_3124064
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
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
#ifdef ERRATA_FE_3124064
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
	RTC_WRITE_REG(0, RTC_TIME_REG_OFFS);
	udelay(10);

	/* Reset Status register */
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK), RTC_STATUS_REG_OFFS);
	udelay(50);
}
