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
#include <errno.h>
#include <pci.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/driver_interface.h>

void pci_init_board(void)
{
	int host_cnt = unit_info_get_count(PCIE_UNIT_ID);
	u16 active_mask = unit_info_get_mask(PCIE_UNIT_ID);
	int ep_mask = 0;

#ifdef CONFIG_MVEBU_PCI_EP
	/*
	 * Currently set all hosts as EP. In case we want to support
	 * mixed mode (RC & EP hosts) we need to define a way to
	 * select it per port
	 */
	ep_mask = active_mask;
#endif

	mvebu_pcie_init_board(host_cnt, active_mask, ep_mask);
}
