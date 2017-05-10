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
#ifdef CONFIG_BOARD_CONFIG_EEPROM
#include <mvebu_cfg_eeprom.h>
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
#ifdef CONFIG_MVEBU_SYS_INFO

	soc_early_init_f();

	/*
	 * Call this function to transfer data from address 0x4000000
	 * into a global struct, before code relocation.
	 */
	sys_info_init();
#endif
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
