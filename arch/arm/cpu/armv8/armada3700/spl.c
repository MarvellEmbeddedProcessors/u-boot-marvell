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
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/io_addr_dec.h>
#include <asm/arch/clock.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch/gpio.h>
#include <asm/arch-mvebu/mvebu_misc.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/fdt.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

void (*ptr_oslo_start)(void);

void board_init_f(ulong silent)
{
#ifdef CONFIG_MVEBU_BOOTMODE_SWITCH_SUPPORT
	uchar boot_mode_read;
#endif

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

/* multi FDT feature reads eeprom, which requires I2C support
 * but for Armada3700, I2C feature depends on GPIO configuration,
 * so mvebu_setup_fdt need to be invoked after it.
 *
 * but GPIO driver also need to read FDT file, for reg base and
 * other configurations. For reg base it is OK, since all the fdt
 * files for all the boards should have the same value.
 *
 * as far as the I2C related GPIO settings, we will demand from HW that
 * all Marvell boards will use the same set of I2C pins setup.
 *
 * GPIO need to be split into two stages: static shared (for I2C),
 * and then dynamic-fdt-based. the first step will be done before
 * mvebu_setup_fdt(), and the second one will be after it.
 */
#ifdef CONFIG_MVEBU_SPL_A3700_GPIO
	mvebu_init_gpio();
#endif

#ifdef CONFIG_BOARD_CONFIG_EEPROM
	cfg_eeprom_init();
#endif

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

#ifdef CONFIG_MVEBU_BOOTMODE_SWITCH_SUPPORT
	/* Armada3700-Z chip doesn't support escape string to enter the uart mode.
	 * So that the u-boot cannot be recovered via uart on a boot failure. For Marvell
	 * board, it supports to switch the boot_src by modifying the sample at reset
	 * value in PCA9560.
	 *
	 * 1. in early stage of SPL, boot mode from SatR device will be read, kept
	 *    the original value, I2C mem will be written to boot from UART, so if
	 *    SPL/u-boot crashed, board stays in boot from UART mode.
	 * 2. in the last stage of u-boot, boot mode will be written back to
	 *    oringal value.
	 */
	mvebu_boot_mode_get(&boot_mode_read);

	/* Pass BOOT_MODE from SPL to u-boot */
	set_info(BOOT_MODE, boot_mode_read);

	/* switch to uart boot mode */
	mvebu_boot_mode_set(BOOT_MODE_UART);
#endif

	/* Clock should be enabeld before initialize the I/O units */
#if defined(CONFIG_MVEBU_A3700_CLOCK) && !defined(SPL_IS_IN_DRAM)
	/* Dynamic clocks configuration is only supported for SPL running from SRAM
	   Changing the DDR clock is not possible when the SPL code is located in DDR.
	   When SPL is running from DRAM, all clocks should be set by TIM at boot
	   time, since TIM code is is executed by secure CPU (CM3) from internal SRAM.
	*/
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

	debug("SPL processing done. Jumping to OSLO image\n\n");
	ptr_oslo_start = (void *)CONFIG_OSLO_START_ADDR;
	/* Jump from SPL to OSLO start address, which could be u-boot or ATF */
	ptr_oslo_start();
}
