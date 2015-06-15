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
