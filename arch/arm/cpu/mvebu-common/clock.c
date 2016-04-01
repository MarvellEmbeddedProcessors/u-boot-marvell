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
#include <fdtdec.h>
#include <asm/arch-mvebu/clock.h>

__weak u32 get_fdt_tclk(const void *blob, int node)
{
	if (node == -1)
		node = fdt_node_offset_by_compatible(blob, -1, "marvell,t-clock");
	return fdtdec_get_int(blob, node, "clock-frequency", -1);
}

__weak u32 get_fdt_refclk(const void *blob, int node)
{
	if (node == -1)
		node = fdt_node_offset_by_compatible(blob, -1, "marvell,ref-clock");
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
	case COMPAT_MVEBU_REFLOCK:
		return get_fdt_refclk(blob, ptr_node);
	}
	return -1;
}
