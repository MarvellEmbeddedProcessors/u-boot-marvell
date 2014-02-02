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
#include <asm/arch-mvebu/adec.h>

void __iomem *ap_adec_base;

static void adec_win_to_range(uintptr_t base_addr, uintptr_t win_size,
			u32 *alr, u32 *ahr)
{
	uintptr_t end = (base_addr + win_size - 1);

	(*alr) = (u32)((base_addr >> ADDRESS_SHIFT) & ADDRESS_MASK);
	(*ahr) = (u32)((end >> ADDRESS_SHIFT) & ADDRESS_MASK);
}

static void adec_enable_ap_win(struct adec_win *win, int win_id)
{
	u32 ccu_win_cr;
	u32 alr, ahr;

	ccu_win_cr = WIN_ENABLE_BIT;
	ccu_win_cr |= (win->target_id & TARGET_ID_MASK) << TARGET_ID_OFFSET;
	ccu_win_cr |= (win->rar_enable & RAR_EN_MASK) << RAR_EN_OFFSET;

	writel(ccu_win_cr, ap_adec_base + CCU_WIN_CR_OFFSET(win_id));

	adec_win_to_range(win->base_addr, win->win_size, &alr, &ahr);

	writel(alr, ap_adec_base + CCU_WIN_ALR_OFFSET(win_id));
	writel(ahr, ap_adec_base + CCU_WIN_AHR_OFFSET(win_id));
}

void adec_dump(void)
{
	int win_id;
	u32 ccu_win_cr;
	u32 alr, ahr;
	u8 target_id;
	uintptr_t start, end;

	printf("id target  start              end\n");
	printf("----------------------------------------------\n");

	/* Dump all AP windows */
	for (win_id = 0; win_id < MAX_AP_WINDOWS; win_id++) {
		ccu_win_cr = readl(ap_adec_base + CCU_WIN_CR_OFFSET(win_id));
		if (ccu_win_cr & WIN_ENABLE_BIT) {
			target_id = (ccu_win_cr >> TARGET_ID_OFFSET) & TARGET_ID_MASK;
			alr = readl(ap_adec_base + CCU_WIN_ALR_OFFSET(win_id));
			ahr = readl(ap_adec_base + CCU_WIN_AHR_OFFSET(win_id));
			start = (uintptr_t)(alr << ADDRESS_SHIFT);
			end = (uintptr_t)((ahr + 0x10) << ADDRESS_SHIFT);
			printf("%02d %02d      0x%016lx 0x%016lx\n", win_id, target_id, start, end);
		}
	}

	return;
}

int adec_ap_init(struct adec_win *windows, void __iomem *ap_base)
{
	int win_id;

	debug("Initialzing AP ADEC unit\n");
	ap_adec_base = ap_base;

	for (win_id = 0; win_id < MAX_AP_WINDOWS; win_id++) {
		if (windows->target_id == INVALID_TID)
			break;

		adec_enable_ap_win(windows, win_id);
		windows++;
	}

	debug("Done AP ADEC init\n");

	return 0;
}
