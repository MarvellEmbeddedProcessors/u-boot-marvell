/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <mvebu/cfg_eeprom.h>

DECLARE_GLOBAL_DATA_PTR;

int __soc_early_init_f(void)
{
	return 0;
}

int soc_early_init_f(void)
	__attribute__((weak, alias("__soc_early_init_f")));

int board_early_init_f(void)
{
	soc_early_init_f();
	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
#ifdef CONFIG_OF_CONTROL
	printf("U-Boot DT blob at : %p\n", gd->fdt_blob);
#endif

#ifdef CONFIG_BOARD_CONFIG_EEPROM
	cfg_eeprom_init();
#endif

	return 0;
}

int board_late_init(void)
{
	/* Pre-configure the USB ports (overcurrent, VBus) */
	return 0;
}
