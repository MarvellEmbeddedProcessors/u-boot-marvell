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
#include <asm/arch-mvebu/fdt.h>
#include <fdtdec.h>

DECLARE_GLOBAL_DATA_PTR;

void *fdt_get_regs_offs(const void *blob, int node, const char *prop_name)
{
	void *base;
	u32 reg;

	base = fdt_get_regs_base();
	reg = fdtdec_get_addr(blob, node, prop_name);
	if (reg == FDT_ADDR_T_NONE)
		return NULL;

	return base + reg;
}

void *fdt_get_regs_base(void)
{
	int node;

	if (gd->arch.reg_base)
		return gd->arch.reg_base;

	node = fdt_node_offset_by_compatible(gd->fdt_blob, -1, "marvell,internal-regs");
	gd->arch.reg_base = (void *)((uintptr_t)fdtdec_get_addr(gd->fdt_blob, node, "reg"));
	return gd->arch.reg_base;
}
