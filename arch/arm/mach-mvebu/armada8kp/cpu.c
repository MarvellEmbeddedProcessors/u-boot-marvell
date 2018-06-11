/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/sizes.h>
#include <asm/io.h>
#include <asm/armv8/mmu.h>
#include <mach/clock.h>
#include <mach/fw_info.h>

DECLARE_GLOBAL_DATA_PTR;

#define MVEBU_AR_RFU_BASE		(MVEBU_REGISTER(0x6F0000))
#define MVEBU_RFU_GLOBL_SW_RST		(MVEBU_AR_RFU_BASE + 0x184)

#define MVEBU_AP_ADDR_RANGE		(0x04000000ULL)
#define MVEBU_AP_BASE_ADDR(ap)		(0xe8000000ULL - \
						(ap) * MVEBU_AP_ADDR_RANGE)

#define CCU_MC_RAR_IF0_REG		(0x000040e0ULL)
#define MC_RAR_ENABLE			1

/* Firmware related definition used for SMC calls */
#define MV_SIP_DRAM_SIZE		0x82000010

/*
 * The following table includes all memory regions for Armada 8k Plus.
 */
static struct mm_region mvebu_mem_map[] = {
	/* Armada common memory regions start here */
	{
		/* RAM 0-64MB */
		.phys = 0x0UL,
		.virt = 0x0UL,
		.size = ATF_REGION_START,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	/* ATF and TEE region 0x4000000-0x5400000 not mapped */
	{
		/* RAM */
		.phys = ATF_REGION_END,
		.virt = ATF_REGION_END,
#ifdef CONFIG_MVEBU_PALLADIUM
		.size = SZ_2G,
#else
		.size = SZ_4G - SZ_1G,
#endif
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	{
		/* SRAM, MMIO regions - AP810 region */
		.phys = 0xdc000000UL,
		.virt = 0xdc000000UL,
		.size = 0x24000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	},
	{
		/* SRAM, MMIO regions - AP810 CPx configurations */
		.phys = 0x8100000000UL,
		.virt = 0x8100000000UL,
		.size = 0x7f00000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	},
	{
		0,
	}
};

struct mm_region *mem_map = mvebu_mem_map;

void enable_caches(void)
{
	icache_enable();
	dcache_enable();
}

void reset_cpu(ulong ignored)
{
	writel(0x0, MVEBU_RFU_GLOBL_SW_RST);
}

#ifndef CONFIG_MVEBU_PALLADIUM
static u64 mvebu_dram_scan_ap_sz(u32 base)
{
	struct pt_regs pregs = {0};

	pregs.regs[0] = MV_SIP_DRAM_SIZE;
	pregs.regs[1] = (unsigned long)base;

	smc_call(&pregs);

	if (!pregs.regs[0])
		error("Failed to get ddr size\n");

	return pregs.regs[0];
}

static u32 mvebu_dram_is_in_rar_mode(u32 base)
{
	u32 reg_val, *reg_addr;

	/* Read the "RAR enable" flag for interface-0 only
	 * When RAR is enabled both interfaces have this flag set.
	 */
	reg_addr = (u32 *)(CCU_MC_RAR_IF0_REG + base);
	reg_val = readl(reg_addr);

	return reg_val & MC_RAR_ENABLE;
}
#endif

int mvebu_dram_init(void)
{
#ifdef CONFIG_MVEBU_PALLADIUM
	gd->ram_size = SZ_2G;
#else
	gd->ram_size = SZ_2G;
#endif
	return 0;
}

void mvebu_dram_init_banksize(void)
{
#ifdef CONFIG_MVEBU_PALLADIUM
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = gd->ram_size;
#else
	int idx, ap;
	int ap_num, cp_num;
	u64 ap_sz[] = {0, 0, 0, 0};

	soc_get_ap_cp_num(&ap_num, &cp_num);

	gd->ram_size = 0;
	for (ap = 0; ap < ap_num; ap++) {
		u32 dram_mode =
			mvebu_dram_is_in_rar_mode(MVEBU_AP_BASE_ADDR(ap));

		ap_sz[ap] = mvebu_dram_scan_ap_sz(MVEBU_AP_BASE_ADDR(ap));
		printf("\tAP-%d DDR - %3lld GiB %s\n", ap,
		       ap_sz[ap] / SZ_1G,
		       dram_mode == 1 ? "- interleave mode" : "");
		gd->ram_size += ap_sz[ap];
	}

	/* If total DRAM size == 0, print error message and try to
	 * continue with 256MB */
	if (gd->ram_size == 0) {
		error("DRAM size not initialized - check DRAM configuration\n");
		printf("\n Using temporary DRAM size of 256MB.\n\n");
		gd->ram_size = SZ_256M;
	}

	printf("\tTotal mem - ");

	for (idx = 0; idx < CONFIG_NR_DRAM_BANKS; idx++)
		gd->bd->bi_dram[idx].size = 0;

	/* Only remap internal registers if the memory connected to AP0
	 * is greater than 3G, so there is a region overlapping and it is
	 * smaller than 255G, when there is still a 1G space for remapping
	 * exists in the AP0 DRAM address range.
	 */
	idx = 0;
	gd->bd->bi_dram[idx].start = 0;
	if ((ap_sz[idx] > (SZ_4G - SZ_1G)) &&
	    (ap_sz[idx] < (8 * SZ_32G - SZ_1G))) {
		/* Bypass internal registers region between
		 * 3G and 4G boundaries
		 */
		gd->bd->bi_dram[idx].size = SZ_4G - SZ_1G;
		idx++;
		gd->bd->bi_dram[idx].start = SZ_4G;
		gd->bd->bi_dram[idx].size = ap_sz[0] - SZ_4G + SZ_1G;
	} else {
		gd->bd->bi_dram[idx].size = ap_sz[0];
	}

	idx++;
	if (ap_num == 2) {
		/* Dual AP only */
		gd->bd->bi_dram[idx].size = ap_sz[1];
		gd->bd->bi_dram[idx].start = 256ULL * SZ_1G;
		return;
	}

	/* Quad AP support */
	if (ap_num > 2) {
		gd->bd->bi_dram[idx].start = 128ULL * SZ_1G;
		idx++;
		gd->bd->bi_dram[idx].start = 256ULL * SZ_1G;
		gd->bd->bi_dram[idx].size = ap_sz[2];
		idx++;
		gd->bd->bi_dram[idx].start = 384ULL * SZ_1G;
		gd->bd->bi_dram[idx].size = ap_sz[3];
	}
#endif
}

#if defined(CONFIG_DISPLAY_BOARDINFO)
int print_cpuinfo(void)
{
	soc_print_clock_info();
	soc_print_system_cache_info();

	return 0;
}
#endif
