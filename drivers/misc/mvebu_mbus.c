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


#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <malloc.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/fdt.h>

DECLARE_GLOBAL_DATA_PTR;

struct mbus_configuration {
	void __iomem *mbus_base;
	u32 max_win;
	u32 max_remap;
	u32 internal_win;
};
struct mbus_configuration __attribute__((section(".data")))mbus_config;
struct mbus_configuration __attribute__((section(".data")))*mbus_info = &mbus_config;

struct mbus_fdt_info {
	u32 base;
	u32 size;
	u32 flags;
};

struct mbus_win {
	u32 target;
	uintptr_t base_addr;
	uintptr_t win_size;
	u32 attribute;
	u32 remapped;
	u32 enabled;
};

#define MBUS_WIN_CTRL_REG(win_num)		((win_num < mbus_info->max_remap) ? \
		(win_num * 0x10) : (0x90 + (win_num-8)*0x08))
#define MBUS_CR_WIN_ENABLE			0x1
#define MBUS_CR_WIN_TARGET_OFFS			4
#define MBUS_CR_WIN_TARGET_MASK			(0xf << MBUS_CR_WIN_TARGET_OFFS)
#define MBUS_CR_WIN_ATTR_OFFS			8
#define MBUS_CR_WIN_ATTR_MASK			(0xff << MBUS_CR_WIN_ATTR_OFFS)
#define MBUS_CR_WIN_SIZE_OFFS			16
#define MBUS_CR_WIN_SIZE_MASK			(0xffff << MBUS_CR_WIN_SIZE_OFFS)
#define MBUS_CR_WIN_SIZE_ALIGNMENT		0x10000

#define MBUS_WIN_BASE_REG(win_num)		((win_num < mbus_info->max_remap) ? \
		(0x4 + win_num*0x10) :	(0x94 + (win_num-8)*0x08))
#define MBUS_BR_BASE_OFFS			16
#define MBUS_BR_BASE_MASK			(0xffff <<  MBUS_BR_BASE_OFFS)

#define MBUS_WIN_REMAP_LOW_REG(win_num)		((win_num < mbus_info->max_remap) ? \
		(0x8 + win_num*0x10) : (0))
#define MBUS_RLR_REMAP_LOW_OFFS			16
#define MBUS_RLR_REMAP_LOW_MASK			(0xffff << MBUS_RLR_REMAP_LOW_OFFS)

#define MBUS_WIN_REMAP_HIGH_REG(win_num)	((win_num < mbus_info->max_remap) ? \
		(0xC + win_num*0x10) : (0))
#define MBUS_RHR_REMAP_HIGH_OFFS		0
#define MBUS_RHR_REMAP_HIGH_MASK		(0xffffffff << MBUS_RHR_REMAP_HIGH_OFFS)

#define MBUS_WIN_INTEREG_REG			(0x80)

static void mbus_win_check(struct mbus_win *win, u32 win_num)
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

static void mbus_win_set(struct mbus_win *win, u32 win_num)
{
	u32 base_reg, ctrl_reg, size_to_reg, remap_low;

	base_reg = (win->base_addr & MBUS_BR_BASE_MASK);
	size_to_reg = (win->win_size / MBUS_CR_WIN_SIZE_ALIGNMENT) - 1;

	ctrl_reg = (size_to_reg << MBUS_CR_WIN_SIZE_OFFS);
	ctrl_reg |= MBUS_CR_WIN_ENABLE;
	ctrl_reg |= win->attribute << MBUS_CR_WIN_ATTR_OFFS;
	ctrl_reg |= win->target << MBUS_CR_WIN_TARGET_OFFS;

	writel(ctrl_reg, mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_num));
	writel(base_reg, mbus_info->mbus_base + MBUS_WIN_BASE_REG(win_num));

	if (win->remapped) {
		remap_low = win->base_addr & MBUS_RLR_REMAP_LOW_MASK;
		writel(remap_low, mbus_info->mbus_base + MBUS_WIN_REMAP_LOW_REG(win_num));
		writel(0x0, mbus_info->mbus_base + MBUS_WIN_REMAP_HIGH_REG(win_num));
	}
	return;
}

void dump_mbus(void)
{
	int win_id;
	u32 mbus_win_cr, mbus_win_br;
	u32 target_id, size, attribute;

	printf("bank    id    target    start       end         attribute\n");
	printf("---------------------------------------------------------\n");

	/* Dump all MBUS windows */
	for (win_id = 0; win_id < mbus_info->max_win; win_id++) {
		mbus_win_cr = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_id));
		if (mbus_win_cr & MBUS_CR_WIN_ENABLE) {
			target_id = (mbus_win_cr & MBUS_CR_WIN_TARGET_MASK) >> MBUS_CR_WIN_TARGET_OFFS;
			attribute = (mbus_win_cr & MBUS_CR_WIN_ATTR_MASK) >> MBUS_CR_WIN_ATTR_OFFS;
			size = (mbus_win_cr & MBUS_CR_WIN_SIZE_MASK) >> MBUS_CR_WIN_SIZE_OFFS;
			mbus_win_br = readl(mbus_info->mbus_base + MBUS_WIN_BASE_REG(win_id));
			size = (size + 1) * MBUS_CR_WIN_SIZE_ALIGNMENT;
			printf("mbus    %02d    0x%02x      0x%08x  0x%08x  0x%02x\n",
			       win_id, target_id, mbus_win_br, mbus_win_br + size, attribute);
		}
	}
	mbus_win_br = readl(mbus_info->mbus_base + MBUS_WIN_INTEREG_REG);
	printf("mbus    %02d    0x%02x      0x%08x  0x%08x  0x%02x\n",
	       mbus_info->internal_win, 0xFF, mbus_win_br, mbus_win_br + MVEBU_REGS_SIZE, 0xFF);

	return;
}

int remap_mbus(phys_addr_t input, phys_addr_t output)
{
	int win;
	u32 ctrl, base;

	for (win = 0; win < mbus_info->max_win; win++) {
		ctrl = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win));
		base = readl(mbus_info->mbus_base + MBUS_WIN_BASE_REG(win));

		if ((ctrl & MBUS_CR_WIN_ENABLE) && (base == input)) {
			if (win >= mbus_info->max_remap) {
				printf("Window %d with base addres 0x%08x is not remapable\n",
				       win, (uint)base);
				return 1;
			}
			writel(output & MBUS_RLR_REMAP_LOW_MASK, mbus_info->mbus_base + MBUS_WIN_REMAP_LOW_REG(win));
			writel(0x0, mbus_info->mbus_base + MBUS_WIN_REMAP_HIGH_REG(win));
			return 0;
		}
	}

	printf("Couldn't find XBAR window with base address 0x%08x\n", (uint)input);
	return 0;
}

int init_mbus(void)
{
	u32 win_id, index, mbus_win, count, node;
	struct mbus_fdt_info *fdt_info;
	struct mbus_win *memory_map, *win;
	const void *blob = gd->fdt_blob;

	debug("Initializing MBUS address decoding\n");
	debug_enter();

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_MBUS));
	if (node < 0) {
		error("No MBUS address decoding node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the address decoding MBUS */
	mbus_info->mbus_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Get the maximum number of MBUS windows supported */
	mbus_info->max_win = fdtdec_get_int(blob, node, "max-win", 0);
	if (mbus_info->max_win == 0)
		error("failed reading max windows number\n");

	/* Get the maximum number of MBUS remap windows */
	mbus_info->max_remap = fdtdec_get_int(blob, node, "max-remap", 0);
	if (mbus_info->max_remap == 0)
		error("failed reading max remap windows number\n");

	/* Get the internal register window number */
	mbus_info->internal_win = fdtdec_get_int(blob, node, "internal-win", 0);
	if (mbus_info->internal_win == 0)
		error("failed reading internal windows number\n");

	memory_map = malloc(mbus_info->max_win * sizeof(struct mbus_win));
	if (memory_map == 0)
		error("failed allocating struct to init windows configuration\n");

	fdt_info = malloc(mbus_info->max_win * sizeof(struct mbus_fdt_info));
	if (fdt_info == 0)
		error("failed allocating struct to get windows configuration from FDT\n");

	/* Get the array of the windows and fill the map data */
	count = fdtdec_get_int_array_count(blob, node, "windows", (u32 *)fdt_info, mbus_info->max_win * 3);
	if (count <= 0) {
		debug("no windows configurations found\n");
		return 0;
	}

	count = count/3; /* every window had 3 variables in FDT (base, size, flag) */
	for (index = 0, win = memory_map; index < count; index++, fdt_info++, win++) {
		win->base_addr = fdt_info->base;
		win->win_size = fdt_info->size;
		win->attribute = fdt_info->flags & 0xFF;
		win->target = (fdt_info->flags & 0xF00) >> 8;
		win->remapped = (fdt_info->flags & 0xF000) >> 12;
		win->enabled = 1;
	}
	win->enabled = -1;

	/* disable all windows */
	for (win_id = 0; win_id < mbus_info->max_win; win_id++) {
		mbus_win = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_id));
		mbus_win &= ~MBUS_CR_WIN_ENABLE;
		writel(mbus_win, mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_id));
	}

	/* enable the remapped windows first, the remap windows is at the first 8 windows */
	for (win_id = 0, win = memory_map; win->enabled != -1; win++) {
		if ((win->win_size == 0) || (win->enabled == 0) || (win->remapped == 0))
			continue;

		mbus_win_check(win, win_id);
		debug("set window %d: target %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x, remapped\n",
		      win_id, win->target, win->base_addr, win->win_size, win->attribute);

		mbus_win_set(win, win_id);
		win_id++;
	}

	/* enable the rest of the windows */
	for (win = memory_map; win->enabled != -1; win++) {
		if ((win->win_size == 0) || (win->enabled == 0) || (win->remapped))
			continue;

		mbus_win_check(win, win_id);
		debug("set window %d: target = %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x\n",
		      win_id, win->target, win->base_addr, win->win_size, win->attribute);

		mbus_win_set(win, win_id);
		win_id++;
	}

	debug("Done MBUS address decoding initializing\n");
	debug_exit();
	return 0;
}

