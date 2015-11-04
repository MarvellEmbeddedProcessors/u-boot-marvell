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
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/io.h>

void init_a3700_sata_addr_dec(struct mbus_win_map *win_map)
{
	struct mbus_win *win;
	int win_id;
	u32 control_value = 0;
	u32 base_value = 0;

	/* fabric decode window configuration for SATA,
	     this configuration is not part of SATA unit (not in SATA's regs range),
	     and default value of fabric decode windows for other units works well,
	     SATA is the only unit needs reconfig.
	     So there is no driver for fabric decode window configuration */

	/* disable all windows first */
	writel(0, MVEBU_ARLP_SATA_DEC_WIN_CTRL(0));
	writel(0, MVEBU_ARLP_SATA_DEC_WIN_CTRL(1));
	writel(0, MVEBU_ARLP_SATA_DEC_WIN_CTRL(2));

	/* configure SATA decode windows to DRAM, according to CPU-DRAM
	  * decode window configurations */
	for (win_id = 0, win = &win_map->mbus_windows[win_id];
	      win_id < win_map->mbus_win_num; win_id++, win++) {
		/* set size */
		control_value |= win->win_size << MVEBU_ARLP_SATA_DEC_WIN_CTRL_SIZE_OFF;
		/* set attr */
		control_value |= win->attribute << MVEBU_ARLP_SATA_DEC_WIN_CTRL_ATTR_OFF;
		/* set target */
		control_value |= win->target << MVEBU_ARLP_SATA_DEC_WIN_CTRL_TARGET_OFF;
		/* set enable */
		control_value |= win->enabled << MVEBU_ARLP_SATA_DEC_WIN_CTRL_EN_OFF;
		/* set base */
		base_value |= win->base_addr << MVEBU_ARLP_SATA_DEC_WIN_BASE_OFF;

		writel(base_value, MVEBU_ARLP_SATA_DEC_WIN_BASE(win_id));
		writel(control_value, MVEBU_ARLP_SATA_DEC_WIN_CTRL(win_id));
	}
}

int init_a3700_io_addr_dec(void)
{
	int	rval = 0;
	struct mbus_win_map win_map;

	debug_enter();

	/* Add units configuration code here */
	/* fetch CPU-DRAM window mapping information by reading
	  * from CPU-DRAM decode windows (only the enabled ones) */
	mbus_win_map_build(&win_map);

	/* sata unit addr dec configuration */
	init_a3700_sata_addr_dec(&win_map);

	debug_exit();
	return rval;
}

