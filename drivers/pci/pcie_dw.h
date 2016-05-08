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

#ifndef __PCIE_DW_
#define __PCIE_DW_

#include <common.h>

struct pcie_win {
	u32 base;
	u32 size;
};

struct dw_host_params {
	uintptr_t regs_base;
	struct pcie_win mem_win;
	struct pcie_win cfg_win;
	int is_endpoint;
};

int dw_pcie_link_up(uintptr_t regs_base, u32 cap_speed);
int dw_pcie_init(int host_id, uintptr_t regs_base, struct pcie_win *mem_win,
		struct pcie_win *cfg_win, int first_busno);
int dw_pcie_set_endpoint(u32 hid, u32 regs_base);
void dw_pcie_configure(uintptr_t regs_base, u32 cap_speed);
int dw_pcie_get_link_speed(uintptr_t regs_base);
int dw_pcie_get_link_width(uintptr_t regs_base);

/* Must be implemented by SoC wrapper layer. */
int soc_pcie_link_up(uintptr_t regs_base);

#endif /* __PCIE_DW_ */
