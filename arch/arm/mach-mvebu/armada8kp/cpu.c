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

DECLARE_GLOBAL_DATA_PTR;

#define MVEBU_AR_RFU_BASE		(MVEBU_REGISTER(0x6F0000))
#define MVEBU_RFU_GLOBL_SW_RST		(MVEBU_AR_RFU_BASE + 0x184)

#define MVEBU_MC_MMAP_REG_L(iface, cs)	(0xfe0200ULL + \
						(iface) * 0x10000 + (cs) * 0x8)
#define MVEBU_MC_AREA_LEN_OFFS		16
#define MVEBU_MC_AREA_LEN_MASK		(0x1f << MVEBU_MC_AREA_LEN_OFFS)
#define MVEBU_MC_CS_VALID_MASK		0x1

#define MVEBU_AP_ADDR_RANGE		(0x04000000ULL)
#define MVEBU_AP_BASE_ADDR(ap)		(0xe8000000ULL - \
						(ap) * MVEBU_AP_ADDR_RANGE)
#define MVEBU_MAX_DRAM_IFACE		2
#define MVEBU_MAX_DRAM_IFACE_CS		2

#define CCU_MC_RAR_IF0_REG		(0x000040e0ULL)
#define MC_RAR_ENABLE			1

/*
 * The following table includes all memory regions for Armada 8k Plus.
 */
static struct mm_region mvebu_mem_map[] = {
	/* Armada common memory regions start here */
	{
		/* RAM */
		.phys = 0x0UL,
		.virt = 0x0UL,
#ifdef CONFIG_MVEBU_PALLADIUM
		.size = SZ_512M,
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
	int iface, cs;
	u64 size = 0;
	u32 reg_val, *reg_addr;
		/* Area size is encoded by 5 bit field */
	static u64 area_sz_decode[] = {
		SZ_256M + SZ_128M, SZ_256M + SZ_512M, SZ_1G + SZ_512M,
		3ULL * SZ_1G, 3ULL * SZ_2G, 0, 0, SZ_8M, SZ_16M, SZ_32M,
		SZ_64M, SZ_128M, SZ_256M, SZ_512M, SZ_1G, SZ_2G, SZ_4G,
		SZ_8G, SZ_16G, 2ULL * SZ_16G, 4ULL * SZ_16G, 8ULL * SZ_16G,
		16ULL * SZ_16G, 32ULL * SZ_16G, 64ULL * SZ_16G, 128ULL * SZ_16G,
		0, 0, 0, 0, 0, 0
	};

	for (iface = 0; iface < MVEBU_MAX_DRAM_IFACE; iface++) {
		for (cs = 0; cs < MVEBU_MAX_DRAM_IFACE_CS; cs++) {
			/* DRAM area per AP, DRAM interface and CS */
			reg_addr =
				(u32 *)(MVEBU_MC_MMAP_REG_L(iface, cs) + base);
			reg_val = readl(reg_addr);
			/* Count the area if CS is active */
			if (reg_val & MVEBU_MC_CS_VALID_MASK) {
				reg_val &= MVEBU_MC_AREA_LEN_MASK;
				reg_val >>= MVEBU_MC_AREA_LEN_OFFS;
				debug("%p: DRAM if %d, CS%d: area 0x%x\n",
				      reg_addr, iface, cs, reg_val);
				size += area_sz_decode[reg_val];
			}
		}
	}

	return size;
}

static u32 mvebu_dram_is_in_rar_mode(u32 base)
{
	u32 reg_val, *reg_addr;

	/* Read the "RAR enable" flag for interface-0 only
	 * When RAR is enabled both interfaces have this flag set.
	 */
	reg_addr = (u32 *)(CCU_MC_RAR_IF0_REG + base);
	reg_val = readl(reg_addr);

	return !!(reg_val & MC_RAR_ENABLE);
}
#endif

int mvebu_dram_init(void)
{
#ifdef CONFIG_MVEBU_PALLADIUM
	gd->ram_size = SZ_512M;
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
		printf("\tAP-%d DDR size = %lldMB %s\n", ap,
		       ap_sz[ap] / SZ_1M,
		       dram_mode == 1 ? "interleave mode" : "");
		gd->ram_size += ap_sz[ap];
	}

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
