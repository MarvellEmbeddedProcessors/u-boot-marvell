/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/io.h>
#include <rtc.h>
#include <dm.h>
#include <dm/device-internal.h>
#include "mvebu_rtc.h"

static int mvebu_rtc_get(struct udevice *dev, struct rtc_time *time)
{
	struct rtc_unit_config *rtc_cfg = dev_get_priv(dev);
	void __iomem *rtc_base = rtc_cfg->rtc_base;

	rtc_to_tm(RTC_READ_REG(RTC_TIME_REG_OFFS), time);

	return 0;
}

static int mvebu_rtc_set(struct udevice *dev, const struct rtc_time *time)
{
	unsigned long tm;
	struct rtc_unit_config *rtc_cfg = dev_get_priv(dev);
	void __iomem *rtc_base = rtc_cfg->rtc_base;

	tm = rtc_mktime(time);

#ifdef ERRATA_FE_3124064
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
	RTC_WRITE_REG(tm, RTC_TIME_REG_OFFS);

	/* Give registers time to stabilize */
	mdelay(100);

	return 0;
}

static int mvebu_rtc_reset(struct udevice *dev)
{
	struct rtc_unit_config *rtc_cfg = dev_get_priv(dev);
	void __iomem *rtc_base = rtc_cfg->rtc_base;

	/* Reset Test register */
	RTC_WRITE_REG(0, RTC_TEST_CONFIG_REG_OFFS);
	/* Oscillator startup time */
	mdelay(500);

	/* Reset time register */
#ifdef ERRATA_FE_3124064
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
	RTC_WRITE_REG(0, RTC_STATUS_REG_OFFS);
#endif
	RTC_WRITE_REG(0, RTC_TIME_REG_OFFS);
	udelay(62);

	/* Reset Status register */
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK),
		      RTC_STATUS_REG_OFFS);
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
	RTC_WRITE_REG((RTC_SZ_STATUS_ALARM1_MASK | RTC_SZ_STATUS_ALARM2_MASK),
		      RTC_STATUS_REG_OFFS);
	udelay(50);

	return 0;
}

static int mvebu_rtc_probe(struct udevice *dev)
{
	unsigned long reg;

	/* Get the rtc register base address */
	struct rtc_unit_config *rtc_cfg = dev_get_priv(dev);
	void __iomem *rtc_base = (void *)dev_get_addr_index(dev, 0);

	rtc_cfg->rtc_base = rtc_base;

	/* Update RTC-MBUS bridge timing parameters */
#ifdef ERRATA_FE_3124064
	/* Functional Errata Ref #:
	 * FE-3124064 - WA for failing time read attempts.
	 * Description:
	 *	The device supports CPU write and read access
	 *	to the RTC Time register.
	 *	However, due to this erratum,
	 *	Write to RTC TIME register may fail.
	 *	Read from RTC TIME register may fail.
	 * Workaround:
	 * 1. Configure the RTC Mbus Bridge Timing Control register
	 *    (offset 0x284080 and 0x284084)
	 *	- Write RTC WRCLK Period 0x3FF (default value is 0xFA)
	 *	- Write RTC WRCLK setup to 0x29 (default value is 0x53)
	 *	- Write RTC Read Output Delay to 0x3F (default value is 0x10)
	 *	- Write RTC WRCLK High Time to 0x53 (default value)
	 *	- Mbus - Read All Byte Enable to 0x1 (default value)
	 * 2. Configure the RTC Test Configuration Register (offset 0x28401C)
	 *    bit3 to '1' (Reserved, Marvell internal)
	 *
	 * RTC Time register write operation:
	 *	- Issue two dummy writes of 0x0 to the RTC Status register
	 *	  (offset 0x284000).
	 *	- Write the time to the RTC Time register (offset 0x28400C).
	 */
	reg = RTC_READ_REG(MV_RTC0_SOC_OFFSET);
	reg &= ~RTC_WRCLK_PERIOD_MASK;
	reg |= 0x3FF << RTC_WRCLK_PERIOD_OFFS;
	reg &= ~RTC_WRCLK_SETUP_MASK;
	reg |= 0x29 << RTC_WRCLK_SETUP_OFFS;
	RTC_WRITE_REG(reg, MV_RTC0_SOC_OFFSET);

	reg = RTC_READ_REG(MV_RTC1_SOC_OFFSET);
	reg &= ~RTC_READ_OUTPUT_DELAY_MASK;
	reg |= 0x3F << RTC_READ_OUTPUT_DELAY_OFFS;
	RTC_WRITE_REG(reg, MV_RTC1_SOC_OFFSET);

	reg = RTC_READ_REG(RTC_TEST_CONFIG_REG_OFFS);
	reg |= 0x8;
	RTC_WRITE_REG(reg, RTC_TEST_CONFIG_REG_OFFS);
#else
	reg = RTC_READ_REG(MV_RTC1_SOC_OFFSET);
	reg &= ~RTC_READ_OUTPUT_DELAY_MASK;
	reg |= 0x1F << RTC_READ_OUTPUT_DELAY_OFFS;
	RTC_WRITE_REG(reg, MV_RTC1_SOC_OFFSET);
#endif

	return 0;
}

static const struct rtc_ops mvebu_rtc_ops = {
	.get = mvebu_rtc_get,
	.set = mvebu_rtc_set,
	.reset = mvebu_rtc_reset,
};

static const struct udevice_id mvebu_rtc_ids[] = {
	{ .compatible = "marvell,real-time-clock" },
	{ }
};

U_BOOT_DRIVER(mvebu_rtc) = {
	.name	= "mvebu_rtc",
	.id	= UCLASS_RTC,
	.of_match = mvebu_rtc_ids,
	.ops	= &mvebu_rtc_ops,
	.probe = mvebu_rtc_probe,
	.priv_auto_alloc_size = sizeof(struct rtc_unit_config),
};
