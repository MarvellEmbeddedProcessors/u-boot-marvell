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

	/* Update sar tables */
	/*if (curr_board->configurable) {
		if (curr_board->config_data->sar_override)
			update_sar_table();
	}*/

	return 0;
}

struct sar_data *board_get_sar(void)
{
	return brd->sar;
}

struct sar_var *board_get_sar_table(void)
{
	if (brd->sar)
		return brd->sar->sar_lookup;
	else
		return NULL;
}
