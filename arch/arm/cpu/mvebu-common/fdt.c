/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., on the worldwide web at
 * http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
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
