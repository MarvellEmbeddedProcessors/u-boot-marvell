/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>

#ifdef CONFIG_MP

__weak u64 arch_get_release_addr(u64 cpu_id)
{
	return 0;
}

__weak void arch_spin_table_reserve_mem(void *fdt)
{
}

static void cpu_update_dt_spin_table(void *blob)
{
	int off;
	__maybe_unused u64 val;
	int addr_cells;
	__maybe_unused fdt32_t *reg;

	off = fdt_path_offset(blob, "/cpus");
	if (off < 0) {
		puts("couldn't find /cpus node\n");
		return;
	}
	of_bus_default_count_cells(blob, off, &addr_cells, NULL);

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);
	while (off != -FDT_ERR_NOTFOUND) {
		reg = (fdt32_t *)fdt_getprop(blob, off, "reg", 0);
		if (reg) {
			val = arch_get_release_addr(
					of_read_number(reg, addr_cells));
			val = cpu_to_fdt64(val);
			fdt_setprop_string(blob, off, "enable-method",
					   "spin-table");
			fdt_setprop(blob, off, "cpu-release-addr",
				    &val, sizeof(val));
		} else {
			puts("Warning: found cpu node without reg property\n");
		}
		off = fdt_node_offset_by_prop_value(blob, off, "device_type",
						    "cpu", 4);
	}

	arch_spin_table_reserve_mem(blob);
}
#endif

int cpu_update_dt(void *fdt)
{
#ifdef CONFIG_MP
	cpu_update_dt_spin_table(fdt);
#endif
	return 0;
}
