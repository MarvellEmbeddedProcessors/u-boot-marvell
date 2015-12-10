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

/* #define DEBUG */
#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>

DECLARE_GLOBAL_DATA_PTR;

struct dec_win_config {
	void __iomem *dec_win_base;
	u32 max_win;
	u32 max_remap;
	u32 remap_size;
	u32 win_offset;
};

struct io_dec_fdt_info {
	u32 base;
	u32 size;
	u32 flags;
};

/* There are up to 14 IO unit which need address deocode in Armada-3700 */
#define IO_UNIT_NUM_MAX		(14)
struct io_dec_fdt_info __attribute__((section(".data")))io_dec_fdt_arr[IO_UNIT_NUM_MAX];

#define MVEBU_DEC_WIN_CTRL_REG(base, win, off)	(MVEBU_REGS_BASE + base + (win * off))
#define MVEBU_DEC_WIN_BASE_REG(base, win, off)	(MVEBU_REGS_BASE + base + (win * off) + 0x4)
#define MVEBU_DEC_WIN_REMAP_REG(base, win, off)	(MVEBU_REGS_BASE + base + (win * off) + 0x8)

#define MVEBU_DEC_WIN_CTRL_SIZE_OFF	(16)
#define MVEBU_DEC_WIN_CTRL_ATTR_OFF	(8)
#define MVEBU_DEC_WIN_CTRL_TARGET_OFF	(4)
#define MVEBU_DEC_WIN_CTRL_EN_OFF	(0)
#define MVEBU_DEC_WIN_BASE_OFF		(16)

/* set io decode window */
int set_io_addr_dec(struct mbus_win_map *win_map, struct dec_win_config *dec_win)
{
	struct mbus_win *win;
	int id;
	u32 ctrl = 0;
	u32 base = 0;

	/* disable all windows first */
	for (id = 0; id < dec_win->max_win; id++)
		writel(0, MVEBU_DEC_WIN_CTRL_REG(dec_win->dec_win_base, id, dec_win->win_offset));

	/* configure eMMC decode windows for DRAM, according to CPU-DRAM
	 * decode window configurations
	 */
	for (id = 0, win = &win_map->mbus_windows[id]; id < win_map->mbus_win_num; id++, win++) {
		/* set size */
		ctrl = win->win_size << MVEBU_DEC_WIN_CTRL_SIZE_OFF;
		/* set attr */
		ctrl |= win->attribute << MVEBU_DEC_WIN_CTRL_ATTR_OFF;
		/* set target */
		ctrl |= win->target << MVEBU_DEC_WIN_CTRL_TARGET_OFF;
		/* set base */
		base = win->base_addr << MVEBU_DEC_WIN_BASE_OFF;

		/* set base address*/
		writel(base, MVEBU_DEC_WIN_BASE_REG(dec_win->dec_win_base, id, dec_win->win_offset));
		/* set remap window, some unit does not have remap window */
		if (id < dec_win->max_remap)
			writel(base, MVEBU_DEC_WIN_REMAP_REG(dec_win->dec_win_base, id, dec_win->win_offset));
		/* set control register */
		writel(ctrl, MVEBU_DEC_WIN_CTRL_REG(dec_win->dec_win_base, id, dec_win->win_offset));
		/* enable the address decode window at last to make it effective */
		ctrl |= win->enabled << MVEBU_DEC_WIN_CTRL_EN_OFF;
		writel(ctrl, MVEBU_DEC_WIN_CTRL_REG(dec_win->dec_win_base, id, dec_win->win_offset));

		debug("set_io_addr_dec %d result: ctrl(0x%x) base(0x%x)",
		      id, readl(MVEBU_DEC_WIN_CTRL_REG(dec_win->dec_win_base, id, dec_win->win_offset)),
		      readl(MVEBU_DEC_WIN_BASE_REG(dec_win->dec_win_base, id, dec_win->win_offset)));
		if (id < dec_win->max_remap)
			debug(" remap(%x)\n",
			      readl(MVEBU_DEC_WIN_REMAP_REG(dec_win->dec_win_base, id, dec_win->win_offset)));
		else
			debug("\n");
	}
	return 0;
}

int init_io_addr_dec(void)
{
	int ret = 0;
	u32 node;
	u32 count;
	int index;
	struct mbus_win_map win_map;
	struct dec_win_config dec_win;
	const void *blob = gd->fdt_blob;
	struct io_dec_fdt_info *fdt_info = io_dec_fdt_arr;

	debug("Initializing MBUS IO address decode windows\n");
	debug_enter();

	/* fetch CPU-DRAM window mapping information by reading
	 * CPU-DRAM decode windows (only the enabled ones)
	 */
	mbus_win_map_build(&win_map);
	for (index = 0; index < win_map.mbus_win_num; index++)
		debug("MBUS DRAM mapping %d base(0x%llx) size(0x%llx) target(%d) attr(%d)\n",
		      index, (u64)win_map.mbus_windows[index].base_addr, (u64)win_map.mbus_windows[index].win_size,
		      win_map.mbus_windows[index].target, win_map.mbus_windows[index].attribute);

	/* Get I/O address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_MBUS_IO_DEC));
	if (node < 0) {
		error("No I/O address decoding node found in FDT blob\n");
		return -1;
	}

	/* Get the array of the windows and fill the map data */
	count = fdtdec_get_int_array_count(blob, node, "unit_io_decode_info", (u32 *)fdt_info, IO_UNIT_NUM_MAX * 3);
	if (count <= 0) {
		debug("no windows configurations found\n");
		return 0;
	}

	/* each window has 3 variables in FDT (base, size, flags)
	 * base: base address of IO decode window
	 * size: size of IO decode window register in unit of byte
	 * flags: information about this IO decode window which is combined by IO_ATTR
	 *     IO_ATTR(max_win, max_remap, remap_size, win_offset) (((max_win) << 24) | ((max_remap) << 16) |
	 *            ((remap_size) << 8) | (win_offset))
	 *            max_win: how many decode window that this unit has
	 *            max_remap: the decode window number including remapping that this unit has
	 *            remap_size: remap window size in unit of bits, normally should be 32 or 64
	 *            win_offset: the offset between continuous decode windows with the same unit, typically 0x10
	 *
	 * Example in FDT: <0xcb00 0x30 IO_ATTR(3, 0, 32, 0x10)>
	 */
	count = count / 3;
	for (index = 0; index < count; index++, fdt_info++) {
		dec_win.dec_win_base = (void *)((u64)fdt_info->base);
		dec_win.max_win = (fdt_info->flags >> 24) & 0xFF;
		dec_win.max_remap = (fdt_info->flags >> 16) & 0xFF;
		dec_win.remap_size = (fdt_info->flags >> 8) & 0xFF;
		dec_win.win_offset = fdt_info->flags & 0xFF;

		/* set I/O address decode window */
		ret = set_io_addr_dec(&win_map, &dec_win);
		if (ret) {
			error("failed to set io address decode\n");
			break;
		}
		debug("set io decode window successfully, base(0x%x) size(0x%x)",
		      fdt_info->base, fdt_info->size);
		debug(" max_win(%d) max_remap(%d) remap_size(%d) win_offset(%d)\n",
		      dec_win.max_win, dec_win.max_remap, dec_win.remap_size, dec_win.win_offset);
	}

	debug_exit();
	return ret;
}
