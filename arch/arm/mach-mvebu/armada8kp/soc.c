/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <asm/arch-armada8k/cache_llc.h>
#include <asm/io.h>
#include <common.h>
#include <dm.h>
#include <mvebu/mvebu_chip_sar.h>

int soc_get_ap_cp_num(void *ap_num, void *cp_num)
{
	/* TODO: Handle Quad AP case */
	if (of_machine_is_compatible("marvell,armada8162") ||
	    of_machine_is_compatible("marvell,armada8164"))
		*((u32 *)ap_num) = 2;
	else
		*((u32 *)ap_num) = 1;

	if (of_machine_is_compatible("marvell,armada8082") ||
	    of_machine_is_compatible("marvell,armada8162"))
		*((u32 *)cp_num) = 2;
	else if (of_machine_is_compatible("marvell,armada8084") ||
		 of_machine_is_compatible("marvell,armada8164"))
		*((u32 *)cp_num) = 4;
	else
		*((u32 *)cp_num) = 0;

	return 0;
}

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
