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
#include <asm/arch-mvebu/spl.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/io_addr_dec.h>
#include <asm/arch-armadalp/clock.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-armadalp/gpio.h>
#include <asm/arch-mvebu/mvebu_misc.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/fdt.h>
#include <spl.h>

void (*ptr_uboot_start)(void);

void board_init_f(ulong silent)
{
	gd = &gdata;
	gd->baudrate = CONFIG_BAUDRATE;

	/* Silence flag is not supported by CM3 WTP BootROM */
	gd->flags &= ~GD_FLG_SILENT;

	/* isetup_fdt set default FDT to work with:
	   - customer/regular mode: point to the defined
	     FDT by CONFIG_DEFAULT_DEVICE_TREE.
	   - Marvell multi FDT mode: set the first compiled relevant device
	     tree for the SoC, required for i2c initialization to read EEPROM data */
	setup_fdt();

#ifdef CONFIG_MULTI_DT_FILE
	/* Update gd->fdt_blob according to multi-fdt data in eeprom */
	mvebu_setup_fdt();
#endif

	/* UART1 and UART2 clocks are sourced from XTAL by default
	* (see RD0012010 register for the details). Additionally the GPIO
	* control (RD0013830) sets the GPIO1[26:25] as the UART1 pins by default.
	* Therefore it is safe to start using UART before call to early_spl_init()
	*/
	preloader_console_init();

	/* Init all drivers requred at early stage (clocks, GPIO...) */
#ifdef CONFIG_MVEBU_SPL_A3700_GPIO
	mvebu_init_gpio();
#endif

	/* Clock should be enabeld before initialize the I/O units */
#ifdef CONFIG_MVEBU_A3700_CLOCK
	init_clock();
#endif

#ifdef CONFIG_MVEBU_A3700_MISC_INIT
	misc_init_cci400();
#endif

	/* Init all relevant drivers (e.g. DDR, comphy...) */
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifdef CONFIG_MVEBU_SPL_DDR_SUPPORT
	mvebu_dram_init(gd->fdt_blob);
#endif

#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
#endif

#ifdef CONFIG_MVEBU_A3700_IO_ADDR_DEC
	init_io_addr_dec();
#endif

	debug("SPL processing done. Jumping to u-boot\n\n");
	ptr_uboot_start = 0;
	/* Jump from SPL to u-boot start address */
	ptr_uboot_start();
}

