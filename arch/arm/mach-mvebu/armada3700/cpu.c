/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <mach/clock.h>
#include <mach/fw_info.h>

DECLARE_GLOBAL_DATA_PTR;

/* Armada 3700 */
#define MVEBU_GPIO_NB_REG_BASE		(MVEBU_REGISTER(0x13800))

#define MVEBU_TEST_PIN_LATCH_N		(MVEBU_GPIO_NB_REG_BASE + 0x8)
#define MVEBU_XTAL_MODE_MASK		BIT(9)
#define MVEBU_XTAL_MODE_OFFS		9
#define MVEBU_XTAL_CLOCK_25MHZ		0x0
#define MVEBU_XTAL_CLOCK_40MHZ		0x1

#define MVEBU_NB_WARM_RST_REG		(MVEBU_GPIO_NB_REG_BASE + 0x40)
#define MVEBU_NB_WARM_RST_MAGIC_NUM	0x1d1e

static struct mm_region mvebu_mem_map[] = {
	{
		/* RAM */
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
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	{
		/* SRAM, MMIO regions */
		.phys = 0xd0000000UL,
		.virt = 0xd0000000UL,
		.size = 0x02000000UL,	/* 32MiB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	},
	{
		/* PCI regions */
		.phys = 0xe8000000UL,
		.virt = 0xe8000000UL,
		.size = 0x02000000UL,	/* 32MiB master PCI space */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
	},
	{
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = mvebu_mem_map;

void reset_cpu(ulong ignored)
{
	/*
	 * Write magic number of 0x1d1e to North Bridge Warm Reset register
	 * to trigger warm reset
	 */
	writel(MVEBU_NB_WARM_RST_MAGIC_NUM, MVEBU_NB_WARM_RST_REG);
}

/*
 * get_ref_clk
 *
 * return: reference clock in MHz (25 or 40)
 */
u32 get_ref_clk(void)
{
	u32 regval;

	regval = (readl(MVEBU_TEST_PIN_LATCH_N) & MVEBU_XTAL_MODE_MASK) >>
		MVEBU_XTAL_MODE_OFFS;

	if (regval == MVEBU_XTAL_CLOCK_25MHZ)
		return 25;
	else
		return 40;
}

#if defined(CONFIG_DISPLAY_BOARDINFO)
int print_cpuinfo(void)
{
	soc_print_clock_info();

	return 0;
}
#endif

static u64 mvebu_dram_scan_ap_sz(void)
{
	struct pt_regs pregs = {0};

	pregs.regs[0] = MV_SIP_DRAM_SIZE;

	smc_call(&pregs);

	if (!pregs.regs[0])
		pr_err("Failed to get ddr size\n");

	return pregs.regs[0];
}

int mvebu_dram_init(void)
{
	gd->ram_size = mvebu_dram_scan_ap_sz();

	if (gd->ram_size == 0) {
		pr_err("DRAM size not initialized - check DRAM configuration\n");
		printf("\n Using temporary DRAM size of 512MB.\n\n");
		gd->ram_size = SZ_512M;
	}

	return 0;
}

int mvebu_dram_init_banksize(void)
{
	/* If ddr size is bellow 2GB there is only one ddr bank used */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	if (gd->ram_size <= SZ_2G) {
		gd->bd->bi_dram[0].size = gd->ram_size;
		return 0;
	}

	/*
	 * If ddr size is above 2GB there is only one case 4GB but the firmware
	 * uses 4 decoding windows for describing it in way reflected below.
	 */
	gd->bd->bi_dram[0].size = SZ_2G;
	gd->bd->bi_dram[1].start = SZ_2G;
	gd->bd->bi_dram[1].size = SZ_1G;
	gd->bd->bi_dram[2].start = SZ_2G + SZ_1G;
	gd->bd->bi_dram[2].size = SZ_256M;
	gd->bd->bi_dram[3].start = 0xe0000000;
	gd->bd->bi_dram[3].size = SZ_128M;

	return 0;
}
