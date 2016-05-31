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

#include <asm/io.h>
#include <common.h>
#include <asm/arch-armada8k/regs-base.h>

#define LLC_CTRL                       0x100
#define LLC_CTRL_EN	                1
#define LLC_EXCLUSIVE_EN		0x100

#define LLC_MNT_CW			0x7bc /* Clean by Way */
#define LLC_MNT_CIW			0x7fc /* Clean-invalidate by Way */
#define LLC_SYNC			0x700

/*
** Get LLC status and mode.
** Returns 1 if LLC is enabled.
*/
int llc_mode_get(int *excl_mode)
{
	u32 val;
	int ret = 0, excl = 0;

	val = readl(MVEBU_LLC_BASE + LLC_CTRL);
	if (val & LLC_CTRL_EN) {
		ret = 1;
		if (val & LLC_EXCLUSIVE_EN)
			excl = 1;
	}
	if (excl_mode)
		*excl_mode = excl;

	return ret;
}

/*
 * flush_l3_cache: cache_v8 only calls a weakly aliased flush_l3_cache
 * in flush_dcache_all(), while invalidate_dcache_all() does not call
 * the appropriate l3 function.
 * While flush_ has "clean" semantics, not "clean-invalidate",
 * we have to make sure no valid LLC contents after this, otherwise
 * when caches are re-enabled by kernel, stale LLC content may eclipse
 * the more up-to-date data written after u-boot disabled caches.
 * Therefore, flush-invalidate all LLC using CIW register.
 */
void flush_l3_cache(void)
{
	if (llc_mode_get(0)) {
		writel(0xffffffff, MVEBU_LLC_BASE + LLC_MNT_CIW);
		writel(1, MVEBU_LLC_BASE + LLC_SYNC);
		asm volatile("dsb sy");
	}
}

