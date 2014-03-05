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

/* #define DEBUG*/

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/soc.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

struct mvebu_board_family *brd;

int board_get_id(void)
{
	/* Stub to implement generic board id read */
	return 1;
}
/* this function is weak so non standard boards can iplement their own */
/*void board_get_id(void) __attribute__((weak, alias("__board_get_id")));*/

int common_board_init(struct mvebu_board_family *board_family)
{
	int board_id = board_get_id();
	struct mvebu_board_info *curr_board;
	u16 *unit_mask = soc_get_unit_mask_table();
	int sar_id, cfg_id, idx;

	brd = board_family;

	if ((board_id < 0) || (board_id > brd->board_cnt)) {
		printf("ERROR: %s: unidentified board id %d. Using default %d\n",
		       __func__, board_id, brd->default_id);
		board_id = brd->default_id;
	}

	brd->curr_board = brd->boards_info[board_id];
	curr_board = brd->curr_board;

	printf("Board: %s\n", curr_board->name);

	/* Update active units list for board */
	if (curr_board->unit_mask)
		update_unit_info(unit_mask, curr_board->unit_mask,
				 curr_board->unit_update_mode);

	/* Update SAR and CFG for board */
	if (curr_board->configurable) {
		struct mvebu_board_config *brd_cfg = curr_board->config_data;

		/* Deactivate all SAR entries */
		for (sar_id = 0; sar_id < MAX_SAR; sar_id++)
			brd->sar->sar_lookup[sar_id].active = 0;

		/* Activate board entries */
		for (idx = 0; idx < brd_cfg->sar_cnt; idx++)
			brd->sar->sar_lookup[brd_cfg->active_sar[idx]].active = 1;

		/* Deactivate all CFG entries */
		for (cfg_id = 0; cfg_id < MAX_CFG; cfg_id++)
			brd->sar->sar_lookup[cfg_id].active = 0;

		/* Activate board entries */
		for (idx = 0; idx < brd_cfg->cfg_cnt; idx++)
			brd->sar->sar_lookup[brd_cfg->active_cfg[idx]].active = 1;
	}

	return 0;
}

struct sar_data *board_get_sar(void)
{
	if (brd->curr_board->configurable &&
	    brd->curr_board->config_data->sar_cnt)
		return brd->sar;
	else
		return NULL;
}

struct sar_var *board_get_sar_table(void)
{
	if (brd->curr_board->configurable &&
	    brd->curr_board->config_data->sar_cnt)
		return brd->sar->sar_lookup;
	else
		return NULL;
}

struct cfg_data *board_get_cfg(void)
{
	if (brd->curr_board->configurable &&
	    brd->curr_board->config_data->cfg_cnt)
		return brd->cfg;
	else
		return NULL;
}

struct cfg_var *board_get_cfg_table(void)
{
	if (brd->curr_board->configurable &&
	    brd->curr_board->config_data->cfg_cnt)
		return brd->cfg->cfg_lookup;
	else
		return NULL;
}
