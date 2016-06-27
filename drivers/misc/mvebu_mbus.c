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

/*#define DEBUG*/
#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <malloc.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch/mbus_reg.h>

DECLARE_GLOBAL_DATA_PTR;

#define MBUS_REMAP_SIZE_64		64
#define MBUS_TARGET_DRAM_NUM		0

struct mbus_configuration {
	void __iomem *mbus_base;
	u32 max_win;
	u32 max_remap;
	u32 remap_size;
	u32 internal_win;
};
struct mbus_configuration __attribute__((section(".data")))mbus_config;
struct mbus_configuration __attribute__((section(".data")))*mbus_info = &mbus_config;

struct mbus_fdt_info {
	u32 base;
	u32 size;
	u32 flags;
};

struct mbus_fdt_info __attribute__((section(".data")))fdt_inf_arr[MBUS_WIN_MAP_NUM_MAX];
struct mbus_win __attribute__((section(".data")))memory_map_arr[MBUS_WIN_MAP_NUM_MAX];

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
	u32 base_reg, ctrl_reg, size_reg, win_size, remap_low;

	base_reg = (win->base_addr & MBUS_BR_BASE_MASK);
	win_size = (win->win_size / MBUS_CR_WIN_SIZE_ALIGNMENT) - 1;

	size_reg = (win_size << MBUS_CR_WIN_SIZE_OFFS);
	writel(size_reg, mbus_info->mbus_base + MBUS_WIN_SIZE_REG(win_num));

	ctrl_reg = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_num));
	ctrl_reg &= ~(MBUS_CR_WIN_TARGET_MASK & MBUS_CR_WIN_ATTR_MASK);
	ctrl_reg |= MBUS_CR_WIN_ENABLE;
	ctrl_reg |= win->attribute << MBUS_CR_WIN_ATTR_OFFS;
	ctrl_reg |= win->target << MBUS_CR_WIN_TARGET_OFFS;
	writel(ctrl_reg, mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_num));

	writel(base_reg, mbus_info->mbus_base + MBUS_WIN_BASE_REG(win_num));

	if (win->remapped) {
		remap_low = win->base_addr & MBUS_RLR_REMAP_LOW_MASK;
		writel(remap_low, mbus_info->mbus_base + MBUS_WIN_REMAP_LOW_REG(win_num));

		if (MBUS_REMAP_SIZE_64 == mbus_info->remap_size)
			writel(0x0, mbus_info->mbus_base + MBUS_WIN_REMAP_HIGH_REG(win_num));
	}
	return;
}

/*
 * mbus_win_get
 *
 * This function gets one CPU decode window configuration
 * according to input window number by reading CPU decode
 * window registers.
 *
 * @input:
 *     - win_num: number of CPU decode window
 *
 * @output:
 *     - win: configuration of this CPU decode window
 *
 * @return:  N/A
 */
static void mbus_win_get(struct mbus_win *win, u32 win_num)
{
	u32 base_reg, ctrl_reg, size_reg;

	ctrl_reg = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_num));
	base_reg = readl(mbus_info->mbus_base + MBUS_WIN_BASE_REG(win_num));
	size_reg = readl(mbus_info->mbus_base + MBUS_WIN_SIZE_REG(win_num));

	win->base_addr = (base_reg & MBUS_BR_BASE_MASK) >> MBUS_BR_BASE_OFFS;
	win->win_size = (size_reg & MBUS_CR_WIN_SIZE_MASK) >> MBUS_CR_WIN_SIZE_OFFS;
	win->target = (ctrl_reg & MBUS_CR_WIN_TARGET_MASK) >> MBUS_CR_WIN_TARGET_OFFS;
	win->attribute = (ctrl_reg & MBUS_CR_WIN_ATTR_MASK) >> MBUS_CR_WIN_ATTR_OFFS;
	win->enabled = ctrl_reg & MBUS_CR_WIN_ENABLE;

	return;
}

/*
 * mbus_win_map_build
 *
 * This function builds CPU-DRAM windows mapping
 * by reading CPU-DRAM decode windows registers,
 * so other units like SATA could configure its own
 * DRAM address decode windows correctly.
 *
 * @input: N/A
 *
 * @output:
 *     - win_map: configuration of CPU-DRAM decode window
 *
 * @return:  N/A
 */
void mbus_win_map_build(struct mbus_win_map *win_map)
{
	int win_id;
	struct mbus_win win;

	memset(win_map, 0, sizeof(struct mbus_win_map));
	for (win_id = 0; win_id < mbus_info->max_win; win_id++) {
		mbus_win_get(&win, win_id);

		if ((win.enabled == 0) || (win.target != MBUS_TARGET_DRAM_NUM))
			continue;

		memcpy(&(win_map->mbus_windows[win_map->mbus_win_num]), &win, sizeof(win));
		win_map->mbus_win_num++;
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
			mbus_win_cr = readl(mbus_info->mbus_base + MBUS_WIN_SIZE_REG(win_id));
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

			if (MBUS_REMAP_SIZE_64 == mbus_info->remap_size)
				writel(0x0, mbus_info->mbus_base + MBUS_WIN_REMAP_HIGH_REG(win));
			return 0;
		}
	}

	printf("Couldn't find XBAR window with base address 0x%08x\n", (uint)input);
	return 0;
}

int init_mbus(void)
{
	u32	win_id, mbus_win, start_win_id = 0;
	u32	node, count, index;
	struct mbus_fdt_info *fdt_info = fdt_inf_arr;
	struct mbus_win *memory_map = memory_map_arr, *win;
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
	else if (mbus_info->max_win > MBUS_WIN_MAP_NUM_MAX) {
		error("max windows number is larger than %d\n", MBUS_WIN_MAP_NUM_MAX);
		return -1;
	}

	/* Get the maximum number of MBUS remap windows */
	mbus_info->max_remap = fdtdec_get_int(blob, node, "max-remap", 0);
	if (mbus_info->max_remap == 0)
		error("failed reading max remap windows number\n");

	/* Get the MBUS remap size */
	mbus_info->remap_size = fdtdec_get_int(blob, node, "remap-size", 0);
	if (mbus_info->remap_size == 0)
		error("failed reading MBUS remap size\n");

	/* Get the internal register window number */
	mbus_info->internal_win = fdtdec_get_int(blob, node, "internal-win", 0);
	if (mbus_info->internal_win == 0)
		error("failed reading internal windows number\n");

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

#ifdef CONFIG_MVEBU_MBUS_SKIP_DRAM_WIN
	/* for some Soc, like Armada3700, DRAM window has to be at the beginning,
	     and could not be configured by anyway (keep the default value),
	     In this case, need to skip DRAM window configuration */
	for (start_win_id = 0, win = memory_map; start_win_id < count; start_win_id++, win++)
		if (MBUS_TARGET_DRAM_NUM != win->target)
			break;
#endif
	/* disable all windows */
	for (win_id = start_win_id; win_id < mbus_info->max_win; win_id++) {
		mbus_win = readl(mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_id));
		mbus_win &= ~MBUS_CR_WIN_ENABLE;
		writel(mbus_win, mbus_info->mbus_base + MBUS_WIN_CTRL_REG(win_id));
	}

	/* enable the remapped windows first, the remap windows is at the first 8 windows */
	for (win_id = start_win_id, win = &memory_map[start_win_id]; win_id < mbus_info->max_win; win_id++, win++) {
		if ((win->win_size == 0) || (win->enabled == 0) || (win->remapped == 0))
			continue;

		mbus_win_check(win, win_id);
		debug("set window %d: target %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x, remapped\n",
			win_id, win->target, win->base_addr, win->win_size, win->attribute);

		mbus_win_set(win, win_id);
	}

	/* enable the rest of the windows */
	for (win_id = start_win_id, win = memory_map; win_id < mbus_info->max_win; win_id++, win++) {
		if ((win->win_size == 0) || (win->enabled == 0) || (win->remapped))
			continue;

		mbus_win_check(win, win_id);
		debug("set window %d: target = %d, base = 0x%lx, size = 0x%lx, attribute = 0x%x\n",
			win_id, win->target, win->base_addr, win->win_size, win->attribute);

		mbus_win_set(win, win_id);
	}

	debug("Done MBUS address decoding initializing\n");
	debug_exit();
	return 0;
}
