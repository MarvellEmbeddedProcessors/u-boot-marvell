/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <asm/armv8/mmu.h>
#include <mach/clock.h>
#include <linux/sizes.h>
#include <mach/soc.h>

DECLARE_GLOBAL_DATA_PTR;

/* Armada 7k/8k */
#define MVEBU_RFU_BASE			(MVEBU_REGISTER(0x6f0000))
#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define SOC_DEV_MULTIPLEX_REG		(MVEBU_REGISTER(0x2440208))
#define NAND_FLASH_CLK_CTRL		(MVEBU_REGISTER(0x2440700))
#define RFU_SW_RESET_OFFSET		0
#define NF_CLOCK_SEL_MASK		0x1
#define SOC_MUX_NAND_EN_MASK		0x1
#define CLOCK_1Mhz			1000000

static struct mm_region mvebu_mem_map[] = {
	{
		/* RAM */
		.phys = 0x0UL,
		.virt = 0x0UL,
#ifdef CONFIG_MVEBU_PALLADIUM
		.size = SZ_256M,
#else
		.size = SZ_2G,
#endif
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

#ifdef CONFIG_NAND_PXA3XX
/* Return NAND clock in Hz */
u32 mvebu_get_nand_clock(void)
{
	u32 reg;

	reg = readl(NAND_FLASH_CLK_CTRL);
	if (reg & NF_CLOCK_SEL_MASK)
		return 400 * CLOCK_1Mhz;
	else
		return 250 * CLOCK_1Mhz;
}

/* Select NAND in the device bus multiplexer */
void mvebu_nand_select(void)
{
	setbits_le32(SOC_DEV_MULTIPLEX_REG, SOC_MUX_NAND_EN_MASK);
}
#endif

int mvebu_dram_init(void)
{
#ifdef CONFIG_MVEBU_PALLADIUM
	gd->ram_size = SZ_512M;
#else
	u32 cs;
	gd->ram_size = 0;
	for (cs = 0; cs < 4; cs++)
		if (get_info(DRAM_CS0 + cs))
			gd->ram_size += get_info(DRAM_CS0_SIZE + cs);

	gd->ram_size *= SZ_1M;
	/* if DRAM size == 0, print error message */
	if (gd->ram_size == 0) {
		error("DRAM size not initialized - check DRAM configuration\n");
		printf("\n Using temporary DRAM size of 256MB.\n\n");
		gd->ram_size = SZ_256M;
	}
#endif
	return 0;
}

void mvebu_dram_init_banksize(void)
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
		return;
	}

	gd->bd->bi_dram[0].size = SZ_4G - SZ_1G;
	gd->bd->bi_dram[1].start = SZ_4G;
	gd->bd->bi_dram[1].size = gd->ram_size - SZ_4G + SZ_1G;
}


#if defined(CONFIG_DISPLAY_BOARDINFO)
int print_cpuinfo(void)
{
	soc_print_device_info();
	soc_print_clock_info();
	return 0;
}
#endif
