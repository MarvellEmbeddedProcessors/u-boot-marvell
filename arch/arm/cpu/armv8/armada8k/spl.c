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
#include <asm/arch-mvebu/tools.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/dram_over_pci.h>
#include <asm/arch-mvebu/ccu.h>
#include <asm/arch-mvebu/flc.h>
#include <asm/arch-mvebu/fdt.h>
#include <mvebu_chip_sar.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

void board_init_f(ulong silent)
{
	gd = &gdata;
	gd->baudrate = CONFIG_BAUDRATE;

	if (silent)
		gd->flags |= GD_FLG_SILENT;

	/* isetup_fdt set default FDT to work with:
	   - customer/regular mode: point to the defined
	     FDT by CONFIG_DEFAULT_DEVICE_TREE.
	   - Marvell multi FDT mode: set the first compiled relevant device
	     tree for the SoC, required for i2c initialization to read EEPROM data */
	setup_fdt();

#ifdef CONFIG_BOARD_CONFIG_EEPROM
	cfg_eeprom_init();
#endif

#ifdef CONFIG_MULTI_DT_FILE
	/* Update gd->fdt_blob according to multi-fdt data in eeprom */
	mvebu_setup_fdt();
#endif

	preloader_console_init();


#ifdef CONFIG_MVEBU_CHIP_SAR
	/* Sample at reset register init */
	mvebu_sar_init(gd->fdt_blob);
#ifdef CONFIG_MVEBU_SPL_CHIP_SAR_DUMP
	/* Sample at reset dump register */
	mvebu_sar_dump();
#endif /* CONFIG_MVEBU_SPL_SAR_DUMP */
#endif /* CONFIG_MVEBU_CHIP_SAR */

	/* Init all relevant drivers (e.g. DDR, comphy...) */
#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
	dram_over_pci_init(gd->fdt_blob);
#endif /* CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT */

#ifdef CONFIG_MVEBU_FLC
	init_flc();
#endif

#ifdef CONFIG_MVEBU_SPL_MEMORY_TEST
	/* Momory test */
	if (run_memory_test())
		printf("**** DRAM test failed ****\n");
#endif
}


