/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>

#ifdef CONFIG_ARMV8_PSCI

#define ATF_RESERVED_MEM_START	0x4023000
#define ATF_RESERVED_MEM_SIZE	0x10000

static void psci_reserve_mem(void *fdt)
{
#ifndef CONFIG_ARMV8_SECURE_BASE
	int nodeoff;
	int na, ns;
	int root;

	root = fdt_path_offset(fdt, "/");
	if (root < 0)
		return;

	na = fdt_address_cells(fdt, root);
	ns = fdt_size_cells(fdt, root);

	nodeoff = fdt_path_offset(fdt, "/reserved-memory");
	if (nodeoff < 0) {
		nodeoff = fdt_add_subnode(fdt, root, "reserved-memory");
		if (nodeoff < 0)
			return;
	}
	fdt_setprop_u32(fdt, nodeoff, "#address-cells", na);
	fdt_setprop_u32(fdt, nodeoff, "#size-cells", ns);
	fdt_setprop(fdt, nodeoff, "ranges", 0, 0);
	nodeoff = fdt_add_subnode(fdt, nodeoff, "psci-area");
	if (nodeoff < 0)
		return;

	if (is_psci_enabled()) {
		fdt_setprop_u64(fdt, nodeoff, "reg", (unsigned long)__secure_start);
		fdt_appendprop_u64(fdt, nodeoff, "reg",
				   (unsigned long)__secure_end
				   - (unsigned long)__secure_start);
	} else {
		fdt_setprop_u64(fdt, nodeoff, "reg", (unsigned long)ATF_RESERVED_MEM_START);
		fdt_appendprop_u64(fdt, nodeoff, "reg", (unsigned long)ATF_RESERVED_MEM_SIZE);
	}

	fdt_setprop(fdt, nodeoff, "no-map", 0, 0);
#endif
}

static int cpu_update_dt_psci(void *fdt)
{
	int nodeoff;
	int tmp;

	psci_reserve_mem(fdt);

	nodeoff = fdt_path_offset(fdt, "/cpus");
	if (nodeoff < 0) {
		printf("couldn't find /cpus\n");
		return nodeoff;
	}

	/* add 'enable-method = "psci"' to each cpu node */
	for (tmp = fdt_first_subnode(fdt, nodeoff);
	     tmp >= 0;
	     tmp = fdt_next_subnode(fdt, tmp)) {
		const struct fdt_property *prop;
		int len;

		prop = fdt_get_property(fdt, tmp, "device_type", &len);
		if (!prop)
			continue;
		if (len < 4)
			continue;
		if (strcmp(prop->data, "cpu"))
			continue;

		/* Not checking rv here, our approach is to skip over errors in
		 * individual cpu nodes, hopefully some of the nodes are
		 * processed correctly and those will boot
		 */
		fdt_setprop_string(fdt, tmp, "enable-method", "psci");
	}

	/* The PSCI node might be called "/psci" or might be called something
	 * else but contain either of the compatible strings
	 * "arm,psci"/"arm,psci-0.2"
	 */
	nodeoff = fdt_path_offset(fdt, "/psci");
	if (nodeoff >= 0)
		goto init_psci_node;

	nodeoff = fdt_node_offset_by_compatible(fdt, -1, "arm,psci");
	if (nodeoff >= 0)
		goto init_psci_node;

	nodeoff = fdt_node_offset_by_compatible(fdt, -1, "arm,psci-0.2");
	if (nodeoff >= 0)
		goto init_psci2_node;

	nodeoff = fdt_path_offset(fdt, "/");
	if (nodeoff < 0)
		return nodeoff;

	nodeoff = fdt_add_subnode(fdt, nodeoff, "psci");
	if (nodeoff < 0)
		return nodeoff;

init_psci_node:
	tmp = fdt_setprop_string(fdt, nodeoff, "compatible", "arm,psci-0.2");
	if (tmp)
		return tmp;
init_psci2_node:
	tmp = fdt_setprop_string(fdt, nodeoff, "method", "smc");
	if (tmp)
		return tmp;

	return 0;
}
#else

__weak void arch_spin_table_reserve_mem(void *fdt)
{
}

__weak u64 arch_get_release_addr(u64 cpu_id)
{
	return 0;
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
#ifdef CONFIG_ARMV8_PSCI
	cpu_update_dt_psci(fdt);
#else
	cpu_update_dt_spin_table(fdt);
#endif
	return 0;
}
