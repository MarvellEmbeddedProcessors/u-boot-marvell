/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <asm/arch-armada8k/cache_llc.h>
#include <asm/io.h>
#include <common.h>
#include <mvebu/mvebu_chip_sar.h>

/* Print System cache (LLC) status and mode */
void soc_print_system_cache_info(void)
{
	u32 val;
	int llc_en = 0, excl = 0;

	val = readl(MVEBU_LLC_BASE + LLC_CTRL_REG_OFFSET);
	if (val & LLC_EN) {
		llc_en = 1;
		if (val & LLC_EXCL_EN)
			excl = 1;
	}

	printf("LLC %s%s\n", llc_en ? "Enabled" : "Disabled",
	       excl ? " (Exclusive Mode)" : "");
}

int soc_early_init_f(void)
{
#ifdef CONFIG_MVEBU_SAR
	/* Sample at reset register init
	 * TODO: add support for 2 more CP's in AP810
	 */
	mvebu_sar_init();
#endif

	return 0;
}
