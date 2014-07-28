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

#include <common.h>
#include <asm/io.h>

#define GTC_CNTCR		0x0
#define GTC_ENABLE_BIT		0x1

/* Currently timer is not needed for ARMv8 */
int timer_init(void)
{
	u32 gtc_cntcr = readl(MVEBU_GENERIC_TIMER_BASE + GTC_CNTCR);

	/* Check if earlier SW enabled the generic timer */
	if ((gtc_cntcr & GTC_ENABLE_BIT) == 0x0) {
		gtc_cntcr |= GTC_ENABLE_BIT;
		writel(gtc_cntcr, MVEBU_GENERIC_TIMER_BASE + GTC_CNTCR);
	}

	return 0;
}
