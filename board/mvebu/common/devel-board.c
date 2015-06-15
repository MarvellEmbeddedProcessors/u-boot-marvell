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

#include <common.h>
#include <errno.h>
#include <asm/arch-mvebu/soc.h>
#include "devel-board.h"

#include "sar.h"

int mvebu_devel_board_init(struct mvebu_board_family *brd_fam)
{
	struct mvebu_board_info *brd = brd_fam->curr_board;
	int cfg_id, idx;

	/* Update SAR and CFG for board */
	if (brd->configurable) {
		struct mvebu_board_config *brd_cfg = brd->config_data;

		/* Deactivate all CFG entries */
		for (cfg_id = 0; cfg_id < MAX_CFG; cfg_id++)
			brd_fam->cfg->cfg_lookup[cfg_id].active = 0;
		/* Activate board entries */
		for (idx = 0; idx < brd_cfg->sar_cnt; idx++)
			brd_fam->cfg->cfg_lookup[brd_cfg->active_sar[idx]].active = 1;
		/* Initialize sample at reset structs for the SatR command */
		sar_init();
	}

	/* Update MPP configurations */
	if (brd->configure_mpp)
		(*brd->configure_mpp)();

	return 0;
}

struct cfg_data *board_get_cfg(void)
{
	struct mvebu_board_family *brd_fam = get_board_family();

	if (brd_fam->curr_board->configurable &&
	    brd_fam->curr_board->config_data->cfg_cnt)
		return brd_fam->cfg;
	else
		return NULL;
}

struct cfg_var *board_get_cfg_table(void)
{
	struct mvebu_board_family *brd_fam = get_board_family();

	if (brd_fam->curr_board->configurable &&
	    brd_fam->curr_board->config_data->cfg_cnt)
		return brd_fam->cfg->cfg_lookup;
	else
		return NULL;
}

