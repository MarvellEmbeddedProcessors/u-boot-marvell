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
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/adec.h>

DECLARE_GLOBAL_DATA_PTR;

static void adec_win_check(struct adec_win *win, u32 win_num)
{
	/* check if address is aligned to the size */
	if (IS_NOT_ALIGN(win->base_addr, win->win_size)) {
		win->base_addr = ALIGN_UP(win->base_addr, win->win_size);
		error("Window %d: base address unaligned to window size\n", win_num);
		printf("Align up the base address to 0x%lx\n", win->base_addr);
	}
	/* size parameter validity check */
	if (IS_NOT_ALIGN(win->win_size, CR_WIN_SIZE_ALIGNMENT)) {
		win->win_size = ALIGN_UP(win->win_size, CR_WIN_SIZE_ALIGNMENT);
		error("Window %d: window size unaligned to 0x%x\n", win_num, CR_WIN_SIZE_ALIGNMENT);
		printf("Aligning size to 0x%lx\n", win->win_size);
	}
}

static void adec_enable_rfu_win(struct adec_win *win, u32 win_id)
{
	u32 alr, ahr;
	uintptr_t end_addr;

	end_addr = (win->base_addr + win->win_size - 1);
	alr = (u32)((win->base_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	alr |= WIN_ENABLE_BIT;
	writel(alr, RFU_WIN_ALR_OFFSET(win_id));

	/* there's no ahr for bootrom window */
	if (win_id == BOOTROM_RFU_WINDOW_NUM)
		return;

	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	writel(ahr, RFU_WIN_AHR_OFFSET(win_id));
}

static void adec_enable_iob_win(struct adec_win *win, u32 win_id)
{
	u32 iob_win_reg;
	u32 alr, ahr;
	uintptr_t end_addr;

	iob_win_reg = WIN_ENABLE_BIT;
	iob_win_reg |= (win->target_id & IOB_TARGET_ID_MASK) << IOB_TARGET_ID_OFFSET;
	writel(iob_win_reg, IOB_WIN_CR_OFFSET(win_id));

	end_addr = (win->base_addr + win->win_size - 1);
	alr = (u32)((win->base_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);

	writel(alr, IOB_WIN_ALR_OFFSET(win_id));
	writel(ahr, IOB_WIN_AHR_OFFSET(win_id));
}

static void adec_enable_ap_win(struct adec_win *win, u32 win_id)
{
	u32 ap_win_reg;
	u32 alr, ahr;
	uintptr_t end_addr;

	ap_win_reg = WIN_ENABLE_BIT;
	ap_win_reg |= (win->target_id & AP_TARGET_ID_MASK) << AP_TARGET_ID_OFFSET;
	writel(ap_win_reg, AP_WIN_CR_OFFSET(win_id));

	end_addr = (win->base_addr + win->win_size - 1);
	alr = (u32)((win->base_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	ahr = (u32)((end_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);

	writel(alr, AP_WIN_ALR_OFFSET(win_id));
	writel(ahr, AP_WIN_AHR_OFFSET(win_id));
}

void adec_dump(void)
{
	u32 win_id;
	u32 win_cr;
	u32 alr, ahr;
	u8 target_id;
	uintptr_t start, end;

	/* Dump all AP windows */
	printf("id target  start              end\n");
	printf("----------------------------------------------\n");
	for (win_id = 0; win_id < MAX_AP_WINDOWS; win_id++) {
		win_cr = readl(AP_WIN_CR_OFFSET(win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> AP_TARGET_ID_OFFSET) & AP_TARGET_ID_MASK;
			alr = readl(AP_WIN_ALR_OFFSET(win_id));
			ahr = readl(AP_WIN_AHR_OFFSET(win_id));
			start = (uintptr_t)(alr << ADDRESS_SHIFT);
			end = (uintptr_t)((ahr + 0x10) << ADDRESS_SHIFT);
			printf("%02d %02d      0x%016lx 0x%016lx\n", win_id, target_id, start, end);
		}
	}
	/* Dump all RFU windows */
	for (win_id = 0; win_id < MAX_RFU_WINDOWS; win_id++) {
		alr = readl(RFU_WIN_ALR_OFFSET(win_id));
		if (alr & WIN_ENABLE_BIT) {
			alr = readl(RFU_WIN_ALR_OFFSET(win_id));
			ahr = readl(RFU_WIN_AHR_OFFSET(win_id));
			start = (uintptr_t)(alr << ADDRESS_SHIFT);
			end = (uintptr_t)((ahr + 0x10) << ADDRESS_SHIFT);
			printf("%02d         0x%016lx 0x%016lx\n", win_id, start, end);
		}
	}
	/* Dump all IOB windows */
	for (win_id = 0; win_id < MAX_IOB_WINDOWS; win_id++) {
		win_cr = readl(IOB_WIN_CR_OFFSET(win_id));
		if (win_cr & WIN_ENABLE_BIT) {
			target_id = (win_cr >> IOB_TARGET_ID_OFFSET) & IOB_TARGET_ID_MASK;
			alr = readl(IOB_WIN_ALR_OFFSET(win_id));
			ahr = readl(IOB_WIN_AHR_OFFSET(win_id));
			start = (uintptr_t)(alr << ADDRESS_SHIFT);
			end = (uintptr_t)((ahr + 0x10) << ADDRESS_SHIFT);
			printf("%02d %02d      0x%016lx 0x%016lx\n", win_id, target_id, start, end);
		}
	}
	printf("\nnote: unmapped addresses will go to GCR\n");
	return;
}

int adec_init(struct adec_win *windows)
{
	u32 win_id;
	u32 win_reg, target_id;

	debug_enter();
	debug("Initializing CCU Address decoding\n");

	/* disable all RFU windows */
	for (win_id = 0; win_id < MAX_RFU_WINDOWS; win_id++) {
		win_reg = readl(RFU_WIN_ALR_OFFSET(win_id));
		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, RFU_WIN_ALR_OFFSET(win_id));
	}

	/* disable all AP windows */
	for (win_id = 0; win_id < MAX_AP_WINDOWS; win_id++) {
		win_reg = readl(AP_WIN_CR_OFFSET(win_id));
		target_id = (win_reg >> AP_TARGET_ID_OFFSET) & AP_TARGET_ID_MASK;
		/* disable all the windows except DRAM and CFG_SPACE windows */
		if (target_id == DRAM_0_TID || target_id == DRAM_1_TID || target_id == CFG_REG_TID)
			continue;
		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, AP_WIN_CR_OFFSET(win_id));

		win_reg = ~AP_WIN_ENA_READ_SECURE;
		win_reg |= ~AP_WIN_ENA_WRITE_SECURE;
		writel(win_reg, AP_WIN_SCR_OFFSET(win_id));
	}

	/* disable all IOB windows, start from win_id = 1 because can't disable internal register window */
	for (win_id = 1; win_id < MAX_IOB_WINDOWS; win_id++) {
		win_reg = readl(IOB_WIN_CR_OFFSET(win_id));
		win_reg &= ~WIN_ENABLE_BIT;
		writel(win_reg, IOB_WIN_CR_OFFSET(win_id));

		win_reg = ~IOB_WIN_ENA_CTRL_WRITE_SECURE;
		win_reg |= ~IOB_WIN_ENA_CTRL_READ_SECURE;
		win_reg |= ~IOB_WIN_ENA_WRITE_SECURE;
		win_reg |= ~IOB_WIN_ENA_READ_SECURE;
		writel(win_reg, IOB_WIN_SCR_OFFSET(win_id));
	}

	for (win_id = 0; windows[win_id].target_id != INVALID_TID; win_id++) {
		if (windows[win_id].enabled) {
			adec_win_check(&windows[win_id], win_id);
			switch (windows[win_id].win_type) {
			case ADEC_IOB_WIN:
				adec_enable_iob_win(&windows[win_id], win_id);
				break;
			case ADEC_AP_WIN:
				adec_enable_ap_win(&windows[win_id], win_id);
				break;
			case ADEC_RFU_WIN:
				adec_enable_rfu_win(&windows[win_id], win_id);
				break;
			default:
				error("Window %d: Bad window type %d\n", win_id, windows[win_id].win_type);
			}
		}
	}

	debug("Done CCU Address decoding Initializing\n");
	debug_exit();

	return 0;
}

int dram_init(void)
{
#ifdef CONFIG_PALLADIUM
	gd->ram_size = 0x20000000;
#else
	u32 alr, ahr;
	u32 target_id, ctrl;
	u32 win;

	for (win = 0; win < MAX_AP_WINDOWS; win++) {
		ctrl = readl(AP_WIN_CR_OFFSET(win));
		target_id = (ctrl >> AP_TARGET_ID_OFFSET) & AP_TARGET_ID_MASK;

		if (target_id == DRAM_0_TID) {
			alr = readl(AP_WIN_ALR_OFFSET(win)) << ADDRESS_SHIFT;
			ahr = readl(AP_WIN_AHR_OFFSET(win)) << ADDRESS_SHIFT;
			gd->ram_size = ahr - alr + 1;
			gd->bd->bi_dram[0].size = gd->ram_size;
			gd->bd->bi_dram[0].start = alr;

			debug("DRAM base 0x%08x size 0x%x\n", alr, (uint)gd->ram_size);
		}
	}
#endif

	return 0;
}
