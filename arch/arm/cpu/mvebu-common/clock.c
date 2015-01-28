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
#include <fdtdec.h>
#include <asm/arch-mvebu/clock.h>

__weak u32 get_fdt_tclk(const void *blob, int node)
{
	if (node == -1)
		node = fdt_node_offset_by_compatible(blob, -1, "marvell,t-clock");
	return fdtdec_get_int(blob, node, "clock-frequency", -1);
}

u32 soc_clock_get(const void *blob, int node)
{
	int ptr_node, id;

	ptr_node = fdtdec_lookup_phandle(blob, node, "clock");
	id = fdtdec_lookup(blob, ptr_node);
	switch (id) {
	case COMPAT_MVEBU_TCLOCK:
		return get_fdt_tclk(blob, ptr_node);
	}
	return -1;
}
