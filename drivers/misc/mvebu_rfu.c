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
#include <asm/arch-mvebu/rfu.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {Addr[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)
#define ADDRESS_MASK			(0xFFFFFFF0)
#define RFU_WIN_ALIGNMENT		(0x100000)

DECLARE_GLOBAL_DATA_PTR;

void __iomem *rfu_base;

struct rfu_win {
	u32 base_addr;
	u32 win_size;
	u32 target_id;
};

enum rfu_target_ids {
	BOOTROM_TID	= 0x0,
	STM_TID		= 0x1,
	SPI_TID		= 0x2,
	PCIE_REGS_TID	= 0x3,
	IHBPHY_TID	= 0x4,
	PCIE_PORT_TID	= 0x5,
	RFU_MAX_TID
};

static void rfu_win_check(struct rfu_win *win, u32 win_num)
{
	/* check if address is aligned to 1M */
	if (IS_NOT_ALIGN(win->base_addr, RFU_WIN_ALIGNMENT)) {
		win->base_addr = ALIGN_UP(win->base_addr, RFU_WIN_ALIGNMENT);
		error("Window %d: base address unaligned to 0x%x\n", win_num, RFU_WIN_ALIGNMENT);
		printf("Align up the base address to 0x%x\n", win->base_addr);
	}

	/* size parameter validity check */
	if (IS_NOT_ALIGN(win->win_size, RFU_WIN_ALIGNMENT)) {
		win->win_size = ALIGN_UP(win->win_size, RFU_WIN_ALIGNMENT);
		error("Window %d: window size unaligned to 0x%x\n", win_num, RFU_WIN_ALIGNMENT);
		printf("Aligning size to 0x%x\n", win->win_size);
	}
}

static void *rfu_alr_offset_get(u32 trgt_id)
{
	u32 rfu_alr_offset[RFU_MAX_TID] = {0x0, 0x10, 0x20, 0x208, 0x210, 0x218};

	if (trgt_id < 0 || trgt_id > RFU_MAX_TID) {
		error("target ID is wrong");
		return rfu_base;
	}
	return rfu_base + rfu_alr_offset[trgt_id];
}

static void *rfu_ahr_offset_get(u32 trgt_id)
{
	u32 rfu_ahr_offset[RFU_MAX_TID] = {0x0, 0x18, 0x28, 0x0, 0x214, 0x21c};

	if (trgt_id < 0 || trgt_id > RFU_MAX_TID) {
		error("target ID is wrong");
		return rfu_base;
	}
	return rfu_base + rfu_ahr_offset[trgt_id];
}

static void rfu_enable_win(struct rfu_win *win, u32 trgt_id)
{
	u32 alr, ahr;
	uintptr_t end_addr;

	end_addr = (win->base_addr + win->win_size - 1);
	alr = (u32)((win->base_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	alr |= WIN_ENABLE_BIT;
	writel(alr, rfu_alr_offset_get(trgt_id));

	/* there's no ahr for bootrom and pcie-regs windows */
	if (trgt_id == BOOTROM_TID || trgt_id == PCIE_REGS_TID)
		return;

	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	writel(ahr, rfu_ahr_offset_get(trgt_id));
}

void dump_rfu(void)
{
	u32 trgt_id;
	u32 alr, ahr;
	uintptr_t start, end;
	char *rfu_target_name[RFU_MAX_TID] = {"BootRoom ", "STM      ", "SPI      ",
					"PCIe-reg ", "IHB-Port ", "PCIe-Port"};

	/* Dump all RFU windows */
	printf("bank  target     start              end\n");
	printf("----------------------------------------------------\n");
	for (trgt_id = 0; trgt_id < RFU_MAX_TID; trgt_id++) {
		alr = readl(rfu_alr_offset_get(trgt_id));
		if (alr & WIN_ENABLE_BIT) {
			alr &= ~WIN_ENABLE_BIT;
			ahr = readl(rfu_ahr_offset_get(trgt_id));
			start = (uintptr_t)(alr << ADDRESS_SHIFT);
			end = (uintptr_t)((ahr + 0x10) << ADDRESS_SHIFT);
			printf("rfu   %s  0x%016lx 0x%016lx\n", rfu_target_name[trgt_id], start, end);
		}
	}
	printf("rfu   PIDI-port  - all other IO transactions\n");

	return;
}

int init_rfu(void)
{
	struct rfu_win *memory_map, *win;
	const void *blob = gd->fdt_blob;
	u32 win_id, win_reg, trgt_id;
	u32 node, win_count;

	debug_enter();
	debug("Initializing RFU Address decoding\n");

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_RFU));
	if (node < 0) {
		error("No RFU address decoding node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the address decoding MBUS */
	rfu_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	memory_map = malloc(RFU_MAX_TID * sizeof(struct rfu_win));
	if (memory_map == 0) {
		error("failed allocating struct to init windows configuration\n");
		return -1;
	}

	/* Get the array of the windows and fill the map data */
	win_count = fdtdec_get_int_array_count(blob, node, "windows", (u32 *)memory_map, RFU_MAX_TID * 3);
	if (win_count <= 0) {
		debug("no windows configurations found\n");
		return 0;
	}
	win_count = win_count/3; /* every window had 3 variables in FDT (base, size, target id) */

	/* disable all RFU windows */
	for (trgt_id = 0; trgt_id < RFU_MAX_TID; trgt_id++) {
		win_reg = readl(rfu_alr_offset_get(trgt_id));
		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, rfu_alr_offset_get(trgt_id));
	}

	for (win_id = 0, win = memory_map; win_id < win_count; win_id++, win++) {
		rfu_win_check(win, win_id);
		rfu_enable_win(win, win->target_id);
	}

	free(memory_map);
	debug("Done RFU Address decoding Initializing\n");
	debug_exit();

	return 0;
}
