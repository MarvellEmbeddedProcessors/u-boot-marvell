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
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/tools.h>
#include <asm/arch-mvebu/ddr.h>
#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
#include <asm/arch-mvebu/dram_over_pci.h>
#endif
#ifdef CONFIG_MVEBU_CCU
#include <asm/arch-mvebu/ccu.h>
#endif
#ifdef CONFIG_MVEBU_RFU
#include <asm/arch-mvebu/rfu.h>
#endif
#ifdef CONFIG_MVEBU_FLC
#include <asm/arch-mvebu/flc.h>
#endif
#ifdef CONFIG_MVEBU_A3700_IO_ADDR_DEC
#include <asm/arch-mvebu/io_addr_dec.h>
#endif
#ifdef CONFIG_MVEBU_MBUS
#include <asm/arch-mvebu/mbus.h>
#endif
#ifdef CONFIG_MVEBU_SPL_A3700_GPIO
#include <asm/arch-mvebu/a3700_gpio.h>
#endif
#ifdef CONFIG_MVEBU_A3700_MISC_INIT
#include <asm/arch-mvebu/mvebu_misc.h>
#endif

#ifdef CONFIG_MVEBU_SPL_SAR_DUMP
extern void mvebu_sar_dump_reg(void);
#endif
#ifdef CONFIG_TARGET_ARMADA_LP
void (*ptr_uboot_start)(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

static int setup_fdt(void)
{
#ifdef CONFIG_OF_CONTROL
#ifdef CONFIG_OF_EMBED
	/* Get a pointer to the FDT */
	gd->fdt_blob = __dtb_dt_begin;
#else
	#error "Support only embedded FDT mode in SPL"
#endif
#endif
	return 0;
}

void board_init_f(ulong silent)
{
	gd = &gdata;
	gd->baudrate = CONFIG_BAUDRATE;
#if 0
	if (silent)
		gd->flags |= GD_FLG_SILENT;
#endif

	setup_fdt();
#ifdef CONFIG_MVEBU_SPL_A3700_GPIO
	mvebu_a3700_gpio();
#endif
#ifdef CONFIG_MVEBU_A3700_MISC_INIT
	misc_init_cci400();
#endif
	preloader_console_init();

#ifdef CONFIG_MVEBU_SPL_SAR_DUMP
	mvebu_sar_dump_reg();
#endif

#ifndef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
/* when DDR over PCIE is enabled, add delay before and after the comphy_init
   to verify that the PCIE card init done, before setting the comphy to avoid
   collisions. and no ddr init require */
#if CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifndef CONFIG_PALLADIUM
	mvebu_dram_init(gd->fdt_blob);
#endif

#else /* CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT */
	dram_over_pci_init(gd->fdt_blob);
#endif /* CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT */

#ifdef CONFIG_MVEBU_FLC
#ifdef CONFIG_MVEBU_CCU
	init_ccu(false);
#endif
#ifdef CONFIG_MVEBU_RFU
	init_rfu(false);
#endif
	init_flc();
#endif

#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
#endif
#ifdef CONFIG_MVEBU_A3700_IO_ADDR_DEC
	init_a3700_io_addr_dec();
#endif

#ifdef CONFIG_MVEBU_SPL_MEMORY_TEST
	if (run_memory_test())
		printf("**** DRAM test failed ****\n");
#endif

#ifdef CONFIG_TARGET_ARMADA_LP
	debug("SPL processing done. Jumping to u-boot\n\n");
	ptr_uboot_start = 0;
	/* Jump from SPL to u-boot start address */
	ptr_uboot_start();
#endif
}
