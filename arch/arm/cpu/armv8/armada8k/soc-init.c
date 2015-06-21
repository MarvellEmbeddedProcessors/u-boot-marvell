/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-armada8k/armada8k.h>

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

struct mvebu_soc_family *soc_init(void)
{
	return &a8k_family_info;
}

int dram_init(void)
{
#ifdef CONFIG_PALLADIUM
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
