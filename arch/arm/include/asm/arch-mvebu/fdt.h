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

#ifndef _MVEBU_FDT_H_
#define _MVEBU_FDT_H_

struct fdt_range {
	u32 child_bus_address;
	u32 parent_bus_address;
	u32 size;
};


void *fdt_get_regs_offs(const void *blob, int node, const char *prop_name);
void *fdt_get_regs_base(const void *blob, int node, uintptr_t reg);

#endif /* _MVEBU_FDT_H_ */

