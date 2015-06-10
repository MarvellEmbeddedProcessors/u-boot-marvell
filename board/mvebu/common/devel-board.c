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

