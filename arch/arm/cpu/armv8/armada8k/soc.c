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

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-armada8k/armada8k.h>
#include <asm/arch/regs-base.h>
#include <linux/sizes.h>

#define MVEBU_MCKINLEY_REGS_BASE	(MVEBU_REGS_BASE + 0x20000)
#define MVEBU_MC_MEM_ADDR_MAP_REG	(MVEBU_MCKINLEY_REGS_BASE + 0x200)

#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define RFU_SW_RESET_OFFSET		0

int soc_early_init_f(void)
{
	return 0;
}

int soc_get_rev(void)
{
	/* This should read the soc rev from some register*/
	return 0;
}

int soc_get_id(void)
{
	/* This should read the soc id from some register*/
	return CONFIG_ARMADA_8K_SOC_ID;
}

void soc_init(void)
{
	return;
}

int dram_init(void)
{
#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
	gd->ram_size = CONFIG_DDR_OVER_PCI_SIZE;
#elif defined(CONFIG_PALLADIUM)
	gd->ram_size = 0x20000000;
#else
	u32 dram_length, ram_size;

	dram_length = (readl(MVEBU_MC_MEM_ADDR_MAP_REG) >> 16) & 0x1F;
	if (dram_length > 0x3) {
		dram_length -= 0x7;
		ram_size = 8 * SZ_1M;
	} else {
		ram_size = 384 * SZ_1M;
	}

	gd->ram_size = (u64)((u64)ram_size << dram_length);
#endif

	return 0;
}

phys_size_t get_effective_memsize(void)
{
	/* Set Memory size of U-Boot to 1GB only - for relocation only */
	if (gd->ram_size < SZ_1G)
		return gd->ram_size;

	return SZ_1G;
}

void dram_init_banksize(void)
{
	/* Config 2 DRAM banks:
	** Bank 0 - max size 4G - 256M
	** Bank 1 - max size 4G */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	if (gd->ram_size <= SZ_4G) {
		gd->bd->bi_dram[0].size = min(gd->ram_size, (phys_size_t)(SZ_4G - SZ_256M));
		return;
	}

	gd->bd->bi_dram[0].size = SZ_4G - SZ_256M;
	gd->bd->bi_dram[1].start = SZ_4G;
	gd->bd->bi_dram[1].size = gd->ram_size - SZ_4G;
}

void reset_cpu(ulong ignored)
{
	u32 reg;
	reg = readl(RFU_GLOBAL_SW_RST);
	reg &= ~(1 << RFU_SW_RESET_OFFSET);
	writel(reg, RFU_GLOBAL_SW_RST);
}
