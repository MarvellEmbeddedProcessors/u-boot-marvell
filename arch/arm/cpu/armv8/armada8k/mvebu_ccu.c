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
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/ccu.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20’h0} */
#define ADDRESS_SHIFT			(20 - 4)

/* AP registers */
#define CCU_MAX_WIN_NUM			(8)
#define CCU_WIN_CR_OFFSET(base, win)	(base + 0x0 + (0x10 * win))
#define CCU_TARGET_ID_OFFSET		(8)
#define CCU_TARGET_ID_MASK		(0x7F)

#define CCU_WIN_ALR_OFFSET(base, win)	(base + 0x8 + (0x10 * win))
#define CCU_WIN_AHR_OFFSET(base, win)	(base + 0xC + (0x10 * win))

#define CCU_WIN_GCR_OFFSET(base)	(base + 0xD0)
#define CCU_GCR_TARGET_OFFSET		(8)
#define CCU_GCR_TARGET_MASK		(0xF)

DECLARE_GLOBAL_DATA_PTR;

enum ccu_target_ids {
	IO_0_TID	= 0x00,
	DRAM_0_TID	= 0x03,
	IO_1_TID	= 0x0F,
	CFG_REG_TID	= 0x10,
	RAR_TID		= 0x20,
	SRAM_TID	= 0x40,
	DRAM_1_TID	= 0xC0,
	CCU_MAX_TID,
	INVALID_TID	= 0xFF
};

struct ccu_target_name_map {
	enum ccu_target_ids trgt_id;
	char name[10];
};

struct ccu_target_name_map ccu_target_name_table[] = {
	{IO_0_TID,	"IO-0	"},
	{DRAM_0_TID,	"DRAM-0 "},
	{IO_1_TID,	"IO-1	"},
	{CFG_REG_TID,	"CFG-REG"},
	{RAR_TID,	"RAR	"},
	{SRAM_TID,	"SRAM	"},
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

void dump_ccu(void)
{
	u32 win_id, win_cr, alr, ahr;
	u8 target_id;
	u64 start, end;
	void __iomem *ccu_base;
	const void *blob = gd->fdt_blob;
	u32 node, max_win;

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_CCU));
	if (node < 0) {
		error("No CCU address decoding node found in FDT blob\n");
		return;
	}

	/* Get the base address of the address decoding CCU */
	ccu_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Get the maximum number of CCU windows supported */
	max_win = fdtdec_get_int(blob, node, "max-win", 0);
	if ((max_win == 0) || (max_win > CCU_MAX_WIN_NUM)) {
		max_win = CCU_MAX_WIN_NUM;
		error("failed reading max windows number, set window max size to %d\n", max_win);
	}

	/* Dump all AP windows */
	printf("bank  id target   start		     end\n");
	printf("----------------------------------------------------\n");
	for (win_id = 0; win_id < max_win; win_id++) {
		win_cr = readl(CCU_WIN_CR_OFFSET(ccu_base, win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> CCU_TARGET_ID_OFFSET) & CCU_TARGET_ID_MASK;
			alr = readl(CCU_WIN_ALR_OFFSET(ccu_base, win_id));
			ahr = readl(CCU_WIN_AHR_OFFSET(ccu_base, win_id));
			start = ((u64)alr << ADDRESS_SHIFT);
			end = (((u64)ahr + 0x10) << ADDRESS_SHIFT);
			printf("ccu   %02x %s  0x%016llx 0x%016llx\n"
				, win_id, ccu_target_name_get(target_id), start, end);
		}
	}
	win_cr = readl(CCU_WIN_GCR_OFFSET(ccu_base));
	target_id = (win_cr >> CCU_GCR_TARGET_OFFSET) & CCU_GCR_TARGET_MASK;
	printf("ccu   GCR %s - all other transactions\n", ccu_target_name_get(target_id));

	return;
}
