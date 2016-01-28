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

DECLARE_GLOBAL_DATA_PTR;

int setup_fdt(void)
{
#ifdef CONFIG_OF_CONTROL
#ifdef CONFIG_OF_EMBED
	/* Get a pointer to the FDT */
	gd->fdt_blob = __dtb_dt_begin;
#else
	#error "Support only embedded FDT mode in SPL"
#endif
#endif
	return 0;
}

#ifdef CONFIG_MULTI_DT_FILE
int mvebu_setup_fdt(void)
{
	gd->fdt_blob = cfg_eeprom_get_fdt();
	return 0;
}
#endif


void *fdt_get_regs_offs(const void *blob, int node, const char *prop_name)
{
	uintptr_t reg;
	reg = fdtdec_get_addr(blob, node, prop_name);
	if (reg == FDT_ADDR_T_NONE)
		return NULL;
	return fdt_get_regs_base(blob, node, reg);
}

void *fdt_get_regs_base(const void *blob, int node, uintptr_t reg)
{
	int parent;
	struct fdt_range bus_info, *ranges;

	parent = fdt_parent_offset(blob, node);
	/* check if we reached the root */
	if (parent <= 0)
		return (void *)reg;

	ranges = &bus_info;

#if defined(CONFIG_MVEBU_SPL_DIFFRENT_BASE_ADDR) && defined(CONFIG_SPL_BUILD)
	/* Part of Marvell SoC use different internal address in SPL/U-Boot */
	if ((strncmp(fdt_get_name(blob, parent, NULL), "internal-regs", 13) == 0) &&
		(fdtdec_get_int_array_count(blob, parent, "ranges-spl", (u32 *)ranges, 3) == -FDT_ERR_NOTFOUND))
			return NULL;
#endif
	/* if there is no "ranges" property in current node then skip it */
	if (fdtdec_get_int_array_count(blob, parent, "ranges", (u32 *)ranges, 3) == -FDT_ERR_NOTFOUND)
		return fdt_get_regs_base(blob, parent, reg);
	if (reg < ranges->child_bus_address || reg > (ranges->child_bus_address + ranges->size))
		printf("%s register base address not in the ranges\n", fdt_get_name(blob, node, NULL));

	reg = reg + ranges->parent_bus_address - ranges->child_bus_address;
	return fdt_get_regs_base(blob, parent, reg);
}

void *fdt_get_reg_offs_by_compat(enum fdt_compat_id compat_id)
{
	int node;
	const char *compat_str;

	compat_str = fdtdec_get_compatible(compat_id);
	node = fdt_node_offset_by_compatible(gd->fdt_blob, -1, compat_str);
	if (node < 0) {
		error("No %s node found in FDT blob\n", compat_str);
		return 0;
	}
	return fdt_get_regs_offs(gd->fdt_blob, node, "reg");
}
