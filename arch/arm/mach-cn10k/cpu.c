// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <asm/arch/board.h>

DECLARE_GLOBAL_DATA_PTR;

#define CN10K_MEM_MAP_USED 4

/* +1 is end of list which needs to be empty */
#define CN10K_MEM_MAP_MAX (CN10K_MEM_MAP_USED + CONFIG_NR_DRAM_BANKS + 3)

static struct mm_region cn10k_mem_map[CN10K_MEM_MAP_MAX] = {
	{
		.virt = 0x800000000000UL,
		.phys = 0x800000000000UL,
		.size = 0x40000000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	}, {
		.virt = 0x840000000000UL,
		.phys = 0x840000000000UL,
		.size = 0x40000000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	}, {
		.virt = 0x880000000000UL,
		.phys = 0x880000000000UL,
		.size = 0x40000000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	}, {
		.virt = 0x8c0000000000UL,
		.phys = 0x8c0000000000UL,
		.size = 0x40000000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	}
};

struct mm_region *mem_map = cn10k_mem_map;

#define SHFW_REGION	0x3000000UL
void mem_map_fill(u64 rvu_addr, u64 rvu_size)
{
	int banks = CN10K_MEM_MAP_USED;
	u32 dram_start = CONFIG_SYS_TEXT_BASE;

	for (int i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		cn10k_mem_map[banks].virt = dram_start;
		cn10k_mem_map[banks].phys = dram_start;
		cn10k_mem_map[banks].size = gd->ram_size;
		cn10k_mem_map[banks].attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
					    PTE_BLOCK_NON_SHARE;
		banks = banks + 1;
	}
	cn10k_mem_map[banks].virt = dram_start - SHFW_REGION;
	cn10k_mem_map[banks].phys = dram_start - SHFW_REGION;
	cn10k_mem_map[banks].size = SHFW_REGION;
	cn10k_mem_map[banks].attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
				    PTE_BLOCK_NON_SHARE;
	banks++;
	cn10k_mem_map[banks].virt = rvu_addr;
	cn10k_mem_map[banks].phys = rvu_addr;
	cn10k_mem_map[banks].size = rvu_size;
	cn10k_mem_map[banks].attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
				    PTE_BLOCK_NON_SHARE;
}

u64 get_page_table_size(void)
{
	return 0x80000;
}

void reset_cpu(ulong addr)
{
}
