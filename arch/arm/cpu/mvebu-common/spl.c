/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
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
#include <spl.h>
#include <fdtdec.h>
#include <i2c.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/spl.h>

#ifdef CONFIG_MVEBU_SPL_SAR_DUMP
extern void mvebu_sar_dump_reg(void);
#endif

void board_init_f(ulong silent)
{
	gd = &gdata;
	gd->baudrate = CONFIG_BAUDRATE;

#ifdef CONFIG_MVEBU_ROM_SILENCE_FLAG_SUPPORT
	if (silent)
		gd->flags |= GD_FLG_SILENT;
#else
	/* Silence flag is not supported by CM3 WTP BootROM */
	gd->flags &= ~GD_FLG_SILENT;
#endif
	/* Update the pointer to the default FDT, this is necessary only to config i2c*/
	setup_fdt();
#ifdef CONFIG_MULTI_DT_FILE
	/* Update the pointer to the FDT */
	mvebu_setup_fdt();
#endif

	/* UART1 and UART2 clocks are sourced from XTAL by default
	* (see RD0012010 register for the details). Additionally the GPIO
	* control (RD0013830) sets the GPIO1[26:25] as the UART1 pins by default.
	* Therefore it is safe to start using UART before call to early_spl_init()
	*/
	preloader_console_init();

	/* Init all drivers requred at early stage (clocks, GPIO...) */
	early_spl_init();

#ifdef CONFIG_MVEBU_SPL_SAR_DUMP
	/* Sample at reset dump register */
	mvebu_sar_dump_reg();
#endif

	/* Init all relevant drivers (e.g. DDR, comphy...) */
	late_spl_init();

#ifdef CONFIG_MVEBU_SPL_MEMORY_TEST
	/* Momory test */
	if (run_memory_test())
		printf("**** DRAM test failed ****\n");
#endif

	/* Jump to U-Boot if needed */
	soc_spl_jump_uboot();
}
