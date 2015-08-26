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

#define ADDRESS_SHIFT			(20)
#define MAX_CCU_WINDOWS			(8)
#define DRAM_0_TID			0x03
#define CCU_WIN_CR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0x0 + (0x10 * win))
#define CCU_TARGET_ID_OFFSET		(8)
#define CCU_TARGET_ID_MASK		(0x7F)
#define CCU_WIN_ALR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0x8 + (0x10 * win))
#define CCU_WIN_AHR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0xC + (0x10 * win))

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
	/* set static dram size 256MB*/
	gd->ram_size = 0x10000000;
#elif defined(CONFIG_PALLADIUM)
	gd->ram_size = 0x20000000;
#else
	u32 alr, ahr;
	u32 target_id, ctrl;
	u32 win;

	for (win = 0; win < MAX_CCU_WINDOWS; win++) {
		ctrl = readl(CCU_WIN_CR_OFFSET(win));
		target_id = (ctrl >> CCU_TARGET_ID_OFFSET) & CCU_TARGET_ID_MASK;

		if (target_id == DRAM_0_TID) {
			alr = readl(CCU_WIN_ALR_OFFSET(win)) << ADDRESS_SHIFT;
			ahr = readl(CCU_WIN_AHR_OFFSET(win)) << ADDRESS_SHIFT;
			gd->ram_size = ahr - alr + 1;
			gd->bd->bi_dram[0].size = gd->ram_size;
			gd->bd->bi_dram[0].start = alr;

			debug("DRAM base 0x%08x size 0x%x\n", alr, (uint)gd->ram_size);
		}
	}
#endif

	return 0;
}
