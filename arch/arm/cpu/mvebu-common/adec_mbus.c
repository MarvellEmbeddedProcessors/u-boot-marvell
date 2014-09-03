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

/* #define DEBUG */
/* #define DEBUG_REG */

#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/adec.h>

void __iomem *mbus_adec_base;

DECLARE_GLOBAL_DATA_PTR;

static void adec_win_check(struct adec_win *win, u32 win_num)
{
	/* check if address is aligned to the size */
	if (IS_NOT_ALIGN(win->base_addr, win->win_size)) {
		win->base_addr = ALIGN_UP(win->base_addr, win->win_size);
		error("Window %d: base address unaligned to size\n", win_num);
		printf("Align up base address to 0x%lx\n", win->base_addr);
	}
	/* size parameter validity check */
	if (IS_NOT_ALIGN(win->win_size, MBUS_CR_WIN_SIZE_ALIGNMENT)) {
		win->win_size = ALIGN_UP(win->win_size, MBUS_CR_WIN_SIZE_ALIGNMENT);
		error("Window %d: window size unaligned to 0x%x\n", win_num, MBUS_CR_WIN_SIZE_ALIGNMENT);
		printf("Align up the size to 0x%lx\n", win->win_size);
	}
}

static void adec_win_set(struct adec_win *win, u32 win_num)
{
	u32 base_reg, ctrl_reg, size_to_reg, remap_low;

	base_reg = (win->base_addr & MBUS_BR_BASE_MASK);
	size_to_reg = (win->win_size / MBUS_CR_WIN_SIZE_ALIGNMENT) - 1;

	ctrl_reg = (size_to_reg << MBUS_CR_WIN_SIZE_OFFS);
	ctrl_reg |= MBUS_CR_WIN_ENABLE;
	ctrl_reg |= win->attribute << MBUS_CR_WIN_ATTR_OFFS;
	ctrl_reg |= win->target << MBUS_CR_WIN_TARGET_OFFS;

	writel(ctrl_reg, mbus_adec_base + MBUS_WIN_CTRL_REG(win_num));
	writel(base_reg, mbus_adec_base + MBUS_WIN_BASE_REG(win_num));

	if (win->remapped) {
		remap_low = win->base_addr & MBUS_RLR_REMAP_LOW_MASK;
		writel(remap_low, mbus_adec_base + MBUS_WIN_REMAP_LOW_REG(win_num));
		writel(0x0, mbus_adec_base + MBUS_WIN_REMAP_HIGH_REG(win_num));
	}
	return;
}

void adec_dump(void)
{
	int win_id;
	u32 mbus_win_cr, mbus_win_br;
	u32 target_id, size, attribute;

	printf("id    target    start       end         attribute\n");
	printf("-------------------------------------------------\n");

	/* Dump all AP windows */
	for (win_id = 0; win_id < MAX_MBUS_WINS; win_id++) {
		mbus_win_cr = readl(mbus_adec_base + MBUS_WIN_CTRL_REG(win_id));
		if (mbus_win_cr & MBUS_CR_WIN_ENABLE) {
			target_id = (mbus_win_cr & MBUS_CR_WIN_TARGET_MASK) >> MBUS_CR_WIN_TARGET_OFFS;
			attribute = (mbus_win_cr & MBUS_CR_WIN_ATTR_MASK) >> MBUS_CR_WIN_ATTR_OFFS;
			size = (mbus_win_cr & MBUS_CR_WIN_SIZE_MASK) >> MBUS_CR_WIN_SIZE_OFFS;
			mbus_win_br = readl(mbus_adec_base + MBUS_WIN_BASE_REG(win_id));
			size = (size + 1) * MBUS_CR_WIN_SIZE_ALIGNMENT;
			printf("%02d    0x%02x      0x%08x  0x%08x  0x%02x\n",
			       win_id, target_id, mbus_win_br, mbus_win_br + size, attribute);
		}
	}
	mbus_win_br = readl(mbus_adec_base + MBUS_WIN_INTEREG_REG);
	printf("%02d    0x%02x      0x%08x  0x%08x  0x%02x\n",
	       INTERNAL_REG_WIN_NUM, INVALID_TARGET_ID, mbus_win_br, mbus_win_br + MVEBU_REGS_SIZE, 0xFF);

	return;
}

int adec_remap(phys_addr_t input, phys_addr_t output)
{
	int win;
	u32 ctrl, base;

	for (win = 0; win < MAX_MBUS_WINS; win++) {
		ctrl = readl(mbus_adec_base + MBUS_WIN_CTRL_REG(win));
		base = readl(mbus_adec_base + MBUS_WIN_BASE_REG(win));

		if ((ctrl & MBUS_CR_WIN_ENABLE) && (base == input)) {
			if (win >= MAX_MBUS_REMAP_WINS) {
				printf("Window %d with base addres 0x%08x is not remapable\n",
				       win, (uint)base);
				return 1;
			}
			writel(output & MBUS_RLR_REMAP_LOW_MASK, mbus_adec_base + MBUS_WIN_REMAP_LOW_REG(win));
			writel(0x0, mbus_adec_base + MBUS_WIN_REMAP_HIGH_REG(win));
			return 0;
		}
	}

	printf("Couldn't find XBAR window with base address 0x%08x\n", (uint)input);
	return 0;
}

int adec_init(struct adec_win *windows)
{
	u32 win_id, index, mbus_win;

	debug("Initializing MBUS address decoding\n");
	mbus_adec_base = (void *)MVEBU_ADEC_BASE;

	/* disable all windows */
	for (win_id = 0; win_id < MAX_MBUS_WINS; win_id++) {
		mbus_win = readl(mbus_adec_base + MBUS_WIN_CTRL_REG(win_id));
		mbus_win &= ~MBUS_CR_WIN_ENABLE;
		writel(mbus_win, mbus_adec_base + MBUS_WIN_CTRL_REG(win_id));
	}

	/* enable the remapped windows first, the remap windows is at the first 8 windows */
	for (index = 0, win_id = 0; windows[index].enabled != TBL_TERM; index++) {
		if ((windows[index].win_size == 0) ||
		    (windows[index].enabled == 0) ||
		    (windows[index].remapped == 0))
			continue;

		adec_win_check(&windows[index], win_id);
		debug("set window %d: target %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x, remapped\n",
		      win_id, windows[index].target, windows[index].base_addr,
			windows[index].win_size, windows[index].attribute);

		adec_win_set(&windows[index], win_id);
		win_id++;
	}
	/* enable the rest of the windows */
	for (index = 0; windows[index].enabled != TBL_TERM; index++) {
		if ((windows[index].win_size == 0) ||
		    (windows[index].enabled == 0) ||
		    (windows[index].remapped))
			continue;

		adec_win_check(&windows[index], win_id);
		debug("set window %d: target = %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x\n",
		      win_id, windows[index].target, windows[index].base_addr,
		      windows[index].win_size, windows[index].attribute);

		adec_win_set(&windows[index], win_id);
		win_id++;
	}

	debug("Done MBUS address decoding initializing\n");

	return 0;
}

int dram_init(void)
{
	int cs;
	u32 ctrl, size, base;

	gd->ram_size = 0;
	/*
	* We assume the DDR training code has configured
	* The SDRAM adec windows so we pull our info from there
	*/

	for (cs = 0; cs < CONFIG_NR_DRAM_BANKS; cs++) {
		ctrl = readl(MBUS_SDRAM_CTRL_REG(cs));
		if (ctrl & MBUS_CR_WIN_ENABLE) {
			base = readl(MBUS_SDRAM_BASE_REG(cs));
			size = (ctrl & MBUS_SDRAM_SIZE_MASK) + MBUS_SDRAM_SIZE_ALIGN;
			gd->bd->bi_dram[cs].start = base;
			gd->bd->bi_dram[cs].size = size;

			gd->ram_size += size;

			debug("DRAM bank %d base 0x%08x size 0x%x\n", cs, base, size);
		}
	}

	if (gd->ram_size == 0) {
		error("No DRAM banks detected");
		return 1;
	}

	return 0;
}

