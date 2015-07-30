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
/* #define DEBUG_REG */

#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <malloc.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/iob.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)
#define ADDRESS_MASK			(0xFFFFFFF0)
#define IOB_WIN_ALIGNMENT		(0x100000)

/* IOB registers */
#define IOB_MAX_WIN_NUM			(24)

#define IOB_WIN_CR_OFFSET(win)		(iob_info->iob_base + 0x0 + (0x20 * win))
#define IOB_TARGET_ID_OFFSET		(8)
#define IOB_TARGET_ID_MASK		(0xF)

#define IOB_WIN_SCR_OFFSET(win)		(iob_info->iob_base + 0x4 + (0x20 * win))
#define IOB_WIN_ENA_CTRL_WRITE_SECURE	(0x1)
#define IOB_WIN_ENA_CTRL_READ_SECURE	(0x2)
#define IOB_WIN_ENA_WRITE_SECURE	(0x4)
#define IOB_WIN_ENA_READ_SECURE		(0x8)

#define IOB_WIN_ALR_OFFSET(win)		(iob_info->iob_base + 0x8 + (0x20 * win))
#define IOB_WIN_AHR_OFFSET(win)		(iob_info->iob_base + 0xC + (0x20 * win))

DECLARE_GLOBAL_DATA_PTR;

struct iob_configuration {
	void __iomem *iob_base;
	u32 max_win;
};
struct iob_configuration __attribute__((section(".data")))iob_config;
struct iob_configuration __attribute__((section(".data")))*iob_info = &iob_config;

struct iob_win {
	u32 base_addr_high;
	u32 base_addr_low;
	u32 win_size_high;
	u32 win_size_low;
	u32 target_id;
};

enum target_ids_iob {
	INTERNAL_TID    = 0x0,
	IHB0_TID        = 0x1,
	PEX1_TID        = 0x2,
	PEX2_TID        = 0x3,
	PEX0_TID        = 0x4,
	NAND_TID        = 0x5,
	RUNIT_TID       = 0x6,
	IHB1_TID        = 0x7,
	IOB_MAX_TID
};

static void iob_win_check(struct iob_win *win, u32 win_num)
{
	u64 base_addr, win_size;
	/* check if address is aligned to the size */
	base_addr = ((u64)win->base_addr_high << 32) + win->base_addr_low;
	if (IS_NOT_ALIGN(base_addr, IOB_WIN_ALIGNMENT)) {
		base_addr = ALIGN_UP(base_addr, IOB_WIN_ALIGNMENT);
		error("Window %d: base address unaligned to 0x%x\n", win_num, IOB_WIN_ALIGNMENT);
		printf("Align up the base address to 0x%llx\n", base_addr);
		win->base_addr_high = (u32)(base_addr >> 32);
		win->base_addr_low = (u32)(base_addr);
	}

	/* size parameter validity check */
	win_size = ((u64)win->win_size_high << 32) + win->win_size_low;
	if (IS_NOT_ALIGN(win_size, IOB_WIN_ALIGNMENT)) {
		win_size = ALIGN_UP(win_size, IOB_WIN_ALIGNMENT);
		error("Window %d: window size unaligned to 0x%x\n", win_num, IOB_WIN_ALIGNMENT);
		printf("Aligning size to 0x%llx\n", win_size);
		win->win_size_high = (u32)(win_size >> 32);
		win->win_size_low = (u32)(win_size);
	}
}

static void iob_enable_win(struct iob_win *win, u32 win_id)
{
	u32 iob_win_reg;
	u32 alr, ahr;
	u64 start_addr, end_addr;

	iob_win_reg = WIN_ENABLE_BIT;
	iob_win_reg |= (win->target_id & IOB_TARGET_ID_MASK) << IOB_TARGET_ID_OFFSET;
	writel(iob_win_reg, IOB_WIN_CR_OFFSET(win_id));

	start_addr = ((u64)win->base_addr_high << 32) + win->base_addr_low;
	end_addr = (start_addr + (((u64)win->win_size_high << 32) + win->win_size_low) - 1);
	alr = (u32)((start_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);

	writel(alr, IOB_WIN_ALR_OFFSET(win_id));
	writel(ahr, IOB_WIN_AHR_OFFSET(win_id));
}

void dump_iob(void)
{
	u32 win_id, win_cr, alr, ahr;
	u8 target_id;
	u64 start, end;
	char *iob_target_name[IOB_MAX_TID] = {"CONFIG", "IHB0 ", "PEX1 ", "PEX2 ",
					      "PEX0 ", "NAND ", "RUNIT", "IHB1 "};

	/* Dump all IOB windows */
	printf("bank  id target  start              end\n");
	printf("----------------------------------------------------\n");
	for (win_id = 0; win_id < iob_info->max_win; win_id++) {
		win_cr = readl(IOB_WIN_CR_OFFSET(win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> IOB_TARGET_ID_OFFSET) & IOB_TARGET_ID_MASK;
			alr = readl(IOB_WIN_ALR_OFFSET(win_id));
			ahr = readl(IOB_WIN_AHR_OFFSET(win_id));
			start = ((u64)alr << ADDRESS_SHIFT);
			end = (((u64)ahr + 0x10) << ADDRESS_SHIFT);
			printf("iob   %02d %s   0x%016llx 0x%016llx\n"
					, win_id, iob_target_name[target_id], start, end);
		}
	}

	return;
}

int init_iob(void)
{
	struct iob_win *memory_map, *win;
	const void *blob = gd->fdt_blob;
	u32 win_id, win_reg;
	u32 node, win_count;

	debug_enter();
	debug("Initializing IOB Address decoding\n");

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_IOB));
	if (node < 0) {
		error("No IOB address decoding node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the address decoding MBUS */
	iob_info->iob_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Get the maximum number of iob windows supported */
	iob_info->max_win = fdtdec_get_int(blob, node, "max-win", 0);
	if (iob_info->max_win == 0) {
		iob_info->max_win = IOB_MAX_WIN_NUM;
		error("failed reading max windows number\n");
	}

	memory_map = malloc(iob_info->max_win * sizeof(struct iob_win));
	if (memory_map == 0) {
		error("failed allocating struct to init windows configuration\n");
		return -1;
	}

	/* Get the array of the windows and fill the map data */
	win_count = fdtdec_get_int_array_count(blob, node, "windows", (u32 *)memory_map, iob_info->max_win * 5);
	if (win_count <= 0) {
		debug("no windows configurations found\n");
		return 0;
	}
	win_count = win_count/5; /* every window had 5 variables in FDT:
				    base high, base low, size high, size low, target id) */

	/* disable all IOB windows, start from win_id = 1 because can't disable internal register window */
	for (win_id = 1; win_id < iob_info->max_win; win_id++) {
		win_reg = readl(IOB_WIN_CR_OFFSET(win_id));
		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, IOB_WIN_CR_OFFSET(win_id));

		win_reg = ~IOB_WIN_ENA_CTRL_WRITE_SECURE;
		win_reg |= ~IOB_WIN_ENA_CTRL_READ_SECURE;
		win_reg |= ~IOB_WIN_ENA_WRITE_SECURE;
		win_reg |= ~IOB_WIN_ENA_READ_SECURE;
		writel(win_reg, IOB_WIN_SCR_OFFSET(win_id));
	}

	for (win_id = 1, win = memory_map; win_id < win_count + 1; win_id++, win++) {
		iob_win_check(win, win_id);
		iob_enable_win(win, win_id);
	}

	debug("Done IOB Address decoding Initializing\n");
	debug_exit();

	return 0;
}
