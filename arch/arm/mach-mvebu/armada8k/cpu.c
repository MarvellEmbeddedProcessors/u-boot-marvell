/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <linux/libfdt.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/arch/cpu.h>
#include <linux/sizes.h>
#include <asm/arch/soc.h>
#include <asm/armv8/mmu.h>
#include <mach/clock.h>
#include <mach/soc.h>
#include <mach/fw_info.h>

DECLARE_GLOBAL_DATA_PTR;

/* Armada 7k/8k */
#define MVEBU_RFU_BASE			(MVEBU_REGISTER(0x6f0000))
#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define RFU_SW_RESET_OFFSET		0

/* Firmware related definition used for SMC calls */
#define MV_SIP_DRAM_SIZE		0x82000010

static struct mm_region mvebu_mem_map[] = {
	/* Armada 80x0 memory regions include the CP1 (slave) units */
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
		/* RAM 66MB-2GB */
		.phys = ATF_REGION_END,
		.virt = ATF_REGION_END,
		.size = SZ_2G,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	{
		/* MMIO regions */
		.phys = SOC_REGS_PHY_BASE,
		.virt = SOC_REGS_PHY_BASE,
		.size = SZ_1G,

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
	u32 reg;

	reg = readl(RFU_GLOBAL_SW_RST);
	reg &= ~(1 << RFU_SW_RESET_OFFSET);
	writel(reg, RFU_GLOBAL_SW_RST);
}

#if defined(CONFIG_DISPLAY_BOARDINFO)
int print_cpuinfo(void)
{
	soc_print_device_info();
	soc_print_clock_info();
	soc_print_system_cache_info();

	return 0;
}
#endif

static u64 mvebu_dram_scan_ap_sz(void)
{
	struct pt_regs pregs = {0};

	pregs.regs[0] = MV_SIP_DRAM_SIZE;
	pregs.regs[1] = (unsigned long)SOC_REGS_PHY_BASE;

	smc_call(&pregs);

	if (!pregs.regs[0])
		pr_err("Failed to get ddr size\n");

	return pregs.regs[0];
}

int mvebu_dram_init(void)
{
	gd->ram_size = mvebu_dram_scan_ap_sz();

	/* if DRAM size == 0, print error message */
	if (gd->ram_size == 0) {
		pr_err("DRAM size not initialized - check DRAM configuration\n");
		printf("\n Using temporary DRAM size of 256MB.\n\n");
		gd->ram_size = SZ_256M;
	}

	return 0;
}

int mvebu_dram_init_banksize(void)
{
	/*
	 * Config 2 DRAM banks:
	 * Bank 0 - max size 4G - 1G
	 * Bank 1 - ram size - 4G + 1G
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	if (gd->ram_size <= SZ_4G - SZ_1G) {
		gd->bd->bi_dram[0].size = min(gd->ram_size,
					      (phys_size_t)(SZ_4G - SZ_1G));
		return 0;
	}

	gd->bd->bi_dram[0].size = SZ_4G - SZ_1G;
	gd->bd->bi_dram[1].start = SZ_4G;
	gd->bd->bi_dram[1].size = gd->ram_size - SZ_4G + SZ_1G;

	return 0;
}
