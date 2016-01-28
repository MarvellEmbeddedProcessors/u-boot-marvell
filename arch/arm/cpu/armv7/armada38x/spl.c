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
#include <asm/arch-mvebu/tools.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/fdt.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

void board_init_f(ulong silent)
{
	gd = &gdata;
	gd->baudrate = CONFIG_BAUDRATE;

	/* Update the pointer to the default FDT, this is necessary only to config i2c*/
	setup_fdt();

	preloader_console_init();

	/* Init all relevant drivers (e.g. DDR, comphy...) */
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifndef CONFIG_PALLADIUM
	mvebu_dram_init(gd->fdt_blob);
#endif
}
