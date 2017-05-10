/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <power/regulator.h>
#ifdef CONFIG_BOARD_CONFIG_EEPROM
#include <mvebu/cfg_eeprom.h>
#endif

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

int board_early_init_r(void)
{
#ifdef CONFIG_DM_REGULATOR
	/* Check if any existing regulator should be turned down */
	regulators_enable_boot_off(false);
#endif

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

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
