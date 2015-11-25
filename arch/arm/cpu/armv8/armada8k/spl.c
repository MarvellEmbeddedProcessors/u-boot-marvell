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
#include <asm/arch-mvebu/dram_over_pci.h>
#include <asm/arch-mvebu/ccu.h>
#include <asm/arch-mvebu/rfu.h>
#include <asm/arch-mvebu/flc.h>

void early_spl_init(void)
{
}

void late_spl_init(void)
{
#ifndef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
/* when DDR over PCIE is enabled, add delay before and after the comphy_init
   to verify that the PCIE card init done, before setting the comphy to avoid
   collisions. and no ddr init require */
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifdef CONFIG_MVEBU_SPL_DDR_SUPPORT
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
}

void soc_spl_jump_uboot(void)
{
	/* Armada-8k return to bootrom */
	return;
}
