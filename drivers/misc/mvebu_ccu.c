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
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/ccu.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20’h0} */
#define ADDRESS_SHIFT			(20 - 4)
#define ADDRESS_MASK			(0xFFFFFFF0)
#define CCU_WIN_ALIGNMENT		(0x100000)

/* AP registers */
#define CCU_MAX_WIN_NUM			(8)
#define CCU_WIN_CR_OFFSET(win)		(ccu_info->ccu_base + 0x0 + (0x10 * win))
#define CCU_TARGET_ID_OFFSET		(8)
#define CCU_TARGET_ID_MASK		(0x7F)

#define CCU_WIN_SCR_OFFSET(win)		(ccu_info->ccu_base + 0x4 + (0x10 * win))
#define CCU_WIN_ENA_READ_SECURE		(0x1)
#define CCU_WIN_ENA_WRITE_SECURE	(0x2)

#define CCU_WIN_ALR_OFFSET(win)		(ccu_info->ccu_base + 0x8 + (0x10 * win))
#define CCU_WIN_AHR_OFFSET(win)		(ccu_info->ccu_base + 0xC + (0x10 * win))

#define CCU_WIN_GCR_OFFSET		(ccu_info->ccu_base + 0xD0)
#define CCU_GCR_TARGET_OFFSET		(8)
#define CCU_GCR_TARGET_MASK		(0xF)

DECLARE_GLOBAL_DATA_PTR;

struct ccu_configuration {
	void __iomem *ccu_base;
	u32 max_win;
};
struct ccu_configuration __attribute__((section(".data")))ccu_config;
struct ccu_configuration __attribute__((section(".data")))*ccu_info = &ccu_config;

struct ccu_win {
	u32 base_addr_high;
	u32 base_addr_low;
	u32 win_size_high;
	u32 win_size_low;
	u32 target_id;
};

enum ccu_target_ids {
	IO_0_TID        = 0x00,
	DRAM_0_TID      = 0x03,
	IO_1_TID        = 0x0F,
	CFG_REG_TID     = 0x10,
	RAR_TID         = 0x20,
	SRAM_TID        = 0x40,
	DRAM_1_TID      = 0xC0,
	CCU_MAX_TID,
	INVALID_TID	= 0xFF
};

struct ccu_target_name_map {
	enum ccu_target_ids trgt_id;
	char name[10];
};

struct ccu_target_name_map ccu_target_name_table[] = {
	{IO_0_TID,	"IO-0   "},
	{DRAM_0_TID,	"DRAM-0 "},
	{IO_1_TID,	"IO-1   "},
	{CFG_REG_TID,	"CFG-REG"},
	{RAR_TID,	"RAR    "},
	{SRAM_TID,	"SRAM   "},
	{DRAM_1_TID,	"DRAM-1 "},
	{INVALID_TID,	"INVALID"},
};

static char *ccu_target_name_get(enum ccu_target_ids trgt_id)
{
	int i;

	for (i = 0; i < CCU_MAX_TID; i++)
		if (ccu_target_name_table[i].trgt_id == trgt_id)
			return ccu_target_name_table[i].name;
	return ccu_target_name_get(INVALID_TID);
}

static void ccu_win_check(struct ccu_win *win, u32 win_num)
{
	u64 start_addr, win_size;
	/* check if address is aligned to 1M */
	start_addr = ((u64)win->base_addr_high << 32) + win->base_addr_low;
	if (IS_NOT_ALIGN(start_addr, CCU_WIN_ALIGNMENT)) {
		start_addr = ALIGN_UP(start_addr, CCU_WIN_ALIGNMENT);
		error("Window %d: base address unaligned to 0x%x\n", win_num, CCU_WIN_ALIGNMENT);
		printf("Align up the base address to 0x%llx\n", start_addr);
		win->base_addr_high = (u32)(start_addr >> 32);
		win->base_addr_low = (u32)(start_addr);
	}

	/* size parameter validity check */
	win_size = ((u64)win->win_size_high << 32) + win->win_size_low;
	if (IS_NOT_ALIGN(win_size, CCU_WIN_ALIGNMENT)) {
		win_size = ALIGN_UP(win_size, CCU_WIN_ALIGNMENT);
		error("Window %d: window size unaligned to 0x%x\n", win_num, CCU_WIN_ALIGNMENT);
		printf("Aligning size to 0x%llx\n", win_size);
		win->win_size_high = (u32)(win_size >> 32);
		win->win_size_low = (u32)(win_size);
	}
}

static void ccu_enable_win(struct ccu_win *win, u32 win_id)
{
	u32 ccu_win_reg;
	u32 alr, ahr;
	u64 start_addr, end_addr;

	start_addr = ((u64)win->base_addr_high << 32) + win->base_addr_low;
	end_addr = (start_addr + (((u64)win->win_size_high << 32) + win->win_size_low) - 1);
	alr = (u32)((start_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);

	writel(alr, CCU_WIN_ALR_OFFSET(win_id));
	writel(ahr, CCU_WIN_AHR_OFFSET(win_id));

	ccu_win_reg = WIN_ENABLE_BIT;
	ccu_win_reg |= (win->target_id & CCU_TARGET_ID_MASK) << CCU_TARGET_ID_OFFSET;
	writel(ccu_win_reg, CCU_WIN_CR_OFFSET(win_id));

}

void dump_ccu(void)
{
	u32 win_id, win_cr, alr, ahr;
	u8 target_id;
	u64 start, end;

	/* Dump all AP windows */
	printf("bank  id target   start              end\n");
	printf("----------------------------------------------------\n");
	for (win_id = 0; win_id < ccu_info->max_win; win_id++) {
		win_cr = readl(CCU_WIN_CR_OFFSET(win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> CCU_TARGET_ID_OFFSET) & CCU_TARGET_ID_MASK;
			alr = readl(CCU_WIN_ALR_OFFSET(win_id));
			ahr = readl(CCU_WIN_AHR_OFFSET(win_id));
			start = ((u64)alr << ADDRESS_SHIFT);
			end = (((u64)ahr + 0x10) << ADDRESS_SHIFT);
			printf("ccu   %02x %s  0x%016llx 0x%016llx\n"
				, win_id, ccu_target_name_get(target_id), start, end);
		}
	}
	win_cr = readl(CCU_WIN_GCR_OFFSET);
	target_id = (win_cr >> CCU_GCR_TARGET_OFFSET) & CCU_GCR_TARGET_MASK;
	printf("ccu   GCR %s - all other transactions\n", ccu_target_name_get(target_id));

	return;
}

static bool skip_ccu_window(u32 win_reg)
{
	u8 target_id;

	/* avoid overriding internal register and SRAM windows
	   At SPL stage BootROM open the SRAM window and close it
	   at the end of the SPL stage */
	if (win_reg & WIN_ENABLE_BIT) {
		target_id = (win_reg >> CCU_TARGET_ID_OFFSET) & CCU_TARGET_ID_MASK;
		if (((target_id) == SRAM_TID) || ((target_id) == CFG_REG_TID))
			return true;
	}

	return false;
}

int init_ccu(bool sw_init)
{
	struct ccu_win memory_map[CCU_MAX_WIN_NUM], *win;
	const void *blob = gd->fdt_blob;
	u32 win_id, win_reg;
	u32 node, win_count, array_id;

	debug_enter();
	debug("Initializing CCU Address decoding\n");

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_CCU));
	if (node < 0) {
		error("No CCU address decoding node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the address decoding CCU */
	ccu_info->ccu_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Get the maximum number of CCU windows supported */
	ccu_info->max_win = fdtdec_get_int(blob, node, "max-win", 0);
	if ((ccu_info->max_win == 0) || (ccu_info->max_win > CCU_MAX_WIN_NUM)) {
		ccu_info->max_win = CCU_MAX_WIN_NUM;
		error("failed reading max windows number, set window max size to %d\n", ccu_info->max_win);
	}

	if (sw_init) {
		/* init only the ccu_info structure without updating the ccu windows.
		   The ccu_info is required for the dump_ccu function */
		debug("Done SW CCU Address decoding Initializing\n");
		return 0;
	}

	/* Get the array of the windows and fill the map data */
	win_count = fdtdec_get_int_array_count(blob, node, "windows", (u32 *)memory_map, ccu_info->max_win * 5);
	if (win_count <= 0) {
		debug("no windows configurations found\n");
		return 0;
	}
	win_count = win_count/5; /* every window had 5 variables in FDT:
				    base high, base low, size high, size low, target id) */

#ifndef CONFIG_SPL_BUILD

	/* Set the default target ID to DRAM 0 */
	/* At SPL stage - running from SRAM */
	win_reg = (DRAM_0_TID & CCU_GCR_TARGET_MASK) << CCU_GCR_TARGET_OFFSET;
	writel(win_reg, CCU_WIN_GCR_OFFSET);
#endif

	/* disable AP windows */
	for (win_id = 0; win_id < ccu_info->max_win; win_id++) {
		win_reg = readl(CCU_WIN_CR_OFFSET(win_id));
		if (skip_ccu_window(win_reg))
				continue;

		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, CCU_WIN_CR_OFFSET(win_id));

		win_reg = ~CCU_WIN_ENA_READ_SECURE;
		win_reg |= ~CCU_WIN_ENA_WRITE_SECURE;
		writel(win_reg, CCU_WIN_SCR_OFFSET(win_id));
	}

	for (win_id = 0, array_id = 0, win = memory_map;
		  ((win_id < ccu_info->max_win) && (array_id < win_count)); win_id++) {
		/* win_id is the index of the current ccu window
			array_id is the index of the current FDT window entry */

		win_reg = readl(CCU_WIN_CR_OFFSET(win_id));
		if (skip_ccu_window(win_reg))
				continue;

		ccu_win_check(win, win_id);
		ccu_enable_win(win, win_id);

		win++;
		array_id++;
	}

	if (array_id != win_count)
		error("Set only %d CCU windows. expected %d", array_id, win_count);

	debug("Done CCU Address decoding Initializing\n");
	debug_exit();

	return 0;
}
