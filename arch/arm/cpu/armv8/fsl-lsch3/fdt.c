/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>
#include "mp.h"

#ifdef CONFIG_MP
u64 arch_get_release_addr(u64 cpu_id)
{
	u64 val;

	val = (u64)get_spin_tbl_addr();
	val += id_to_core(cpu_id) * SPIN_TABLE_ELEM_SIZE;

	return val;
}

void arch_spin_table_reserve_mem(void *fdt)
{
	size_t *boot_code_size = &(__secondary_boot_code_size);

	fdt_add_mem_rsv(fdt, (uintptr_t)&secondary_boot_code,
			*boot_code_size);
}

static void ft_fixup_cpu(void *blob)
{
}

#endif

void ft_cpu_setup(void *blob, bd_t *bd)
{
#ifdef CONFIG_MP
	ft_fixup_cpu(blob);
#endif
}
