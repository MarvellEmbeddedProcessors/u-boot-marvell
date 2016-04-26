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

/* #define DEBUG */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/pinctl.h>
#include <asm/arch-mvebu/mpp.h>
#include <asm/arch-mvebu/system_info.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/fdt.h>
#include "board.h"
#ifdef CONFIG_DEVEL_BOARD
#include "devel-board.h"
#endif
#ifdef CONFIG_BOARD_CONFIG_EEPROM
#include "cfg_eeprom.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	return 0;
}
#endif

int __soc_early_init_f(void)
{
	return 0;
}
int soc_early_init_f(void)
		__attribute__((weak, alias("__soc_early_init_f")));

#ifdef CONFIG_BOARD_EARLY_INIT_F
/* Do very basic stuff like board and soc detection */
int board_early_init_f(void)
{
	debug_enter();
#ifdef CONFIG_BOARD_CONFIG_EEPROM
	/* set default FDT to work with:
	 ** - customer/regular mode: point to the defined FDT by CONFIG_DEFAULT_DEVICE_TREE.
	 ** - Marvell multi FDT mode: set the first compiled relevant device
	 **   tree for the SoC, required for i2c initialization to read EEPROM data */
	setup_fdt();
	cfg_eeprom_init();
#endif

#ifdef CONFIG_MULTI_DT_FILE
	mvebu_setup_fdt();
#endif
	soc_early_init_f();
#ifdef CONFIG_MVEBU_SYS_INFO
/* Call this function to transfer data from address 0x4000000
   into a global struct, before code relocation. */
	sys_info_init();
#endif
#ifdef CONFIG_MVEBU_MBUS
	/* mbus driver must be inited before dram_init,
	  * since mbus reads the DRAM window configuration
	  * from FDT, and dram_init needs it to get the DRAM
	  * size.
	  *
	  * the sequence of init_sequence_f is:
	  * 1. board_early_init_f --> init_mubs
	  * 2. dram_init
	  */
	init_mbus();
#endif
	debug_exit();
	return 0;
}
#endif

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	/* Boot progress. */
#if 0
	/* Initialize special environment variables. */
	misc_init_r_env();

	/* CPU Init (part of it depends on env varisables. */
	mv_cpu_init();

	/* USB init (according to usbMode).
	** XOR init
	*/
	// ACS-TODO: Why do we need this?

	/* Boot Progress. */

	/* Print of MPP configuration and modules. */
	mvBoardMppModuleTypePrint();
	mvBoardOtherModuleTypePrint();

	/* Boot Progress. */

	/* Phy Initialization. */
	/* Init the PHY or Switch of the board */
	mvebu_board_phy_switch_init();
#endif
	return 0;
}
#endif


int mvebu_io_init(void)
{
	return 0;
}

int mvebu_print_info(void)
{
	const void *blob = gd->fdt_blob;
	char *brd_name;

	fdt_get_string(blob, 0, "model", (const char **)&brd_name);
	printf("Board: %s\n", brd_name);
	mvebu_print_soc_info();

	return 0;
}

int mvebu_board_init(void)
{
	debug_enter();
	debug("Initializing board\n");

#ifdef CONFIG_MVEBU_MPP_BUS
	mpp_bus_probe();
#endif
#ifdef CONFIG_DEVEL_BOARD
	mvebu_devel_board_init();
#endif
	debug_exit();
	return 0;
}


int board_init(void)
{
	debug_enter();
	mvebu_print_info();

	mvebu_soc_init();

	mvebu_board_init();

	mvebu_io_init();

	debug_exit();
	return 0;
}

