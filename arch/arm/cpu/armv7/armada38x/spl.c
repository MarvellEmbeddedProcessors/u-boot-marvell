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
#include <asm/arch-mvebu/tools.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/mbus.h>

void early_spl_init(void)
{
}

void late_spl_init(void)
{
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifndef CONFIG_PALLADIUM
	mvebu_dram_init(gd->fdt_blob);
#endif

#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
#endif
}

void soc_spl_jump_uboot(void)
{
	/* Armada-8k return to bootrom */
	return;
}
