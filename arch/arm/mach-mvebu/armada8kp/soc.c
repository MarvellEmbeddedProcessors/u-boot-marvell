/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <mvebu/mvebu_chip_sar.h>

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
