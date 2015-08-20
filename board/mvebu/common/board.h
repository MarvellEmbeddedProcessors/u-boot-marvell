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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/soc-info.h>
#include <linux/compiler.h>
#include "sar.h"

#define MAX_BOARD_NAME		32

/*
 * mvebu_board_info - Describes board specific features
 *
 * name -	short name of the board
 * id -		a unique board id value
 * unit_mask -	pointer to table that modifies the
 *		active units for this board (optional)
 * unit_update_mode -	specifies the unit update method (optional). Is either
 *			UNIT_INFO_DISABLE - disable existing units
 *			UNIT_INFO_ENABLE - enable more units
 *			UNIT_INFO_OVERRIDE - override active unit info
 * config -	pointer to board configuration options (optional).
 *		this should be used only for configurable boards like
 *		Development Boards (DB)
 */
struct mvebu_board_info {
	char name[MAX_BOARD_NAME];
	u32  id;

	/* Configure MPP for different board configurations */
	int (*configure_mpp)(void);
};

/*
 * mvebu_board_familt - Describes common board data
 *
 * default_id - defualt board id in case board_id can't be read.
 * board_cnt - number of different boards
 * curr_board - pointer to current board's info
 * boards_info - pointer to table containing
 * sar - General SAR information for all boards (optional)
 *
 */
struct mvebu_board_family {
	int  default_id;
	int  board_cnt;
	struct mvebu_board_info *curr_board;
	struct mvebu_board_info **boards_info;
};

/* APIs required from all board families */
struct mvebu_board_family *board_init_family(void);

/* Common board API */
int board_get_id(void);


DECLARE_GLOBAL_DATA_PTR;
#define get_board_family()	(struct mvebu_board_family *)(gd->arch.board_family)
#define set_board_family(x)	(gd->arch.board_family = x)
#define get_board_info()	(struct mvebu_board_info *)(gd->arch.board_family->curr_board)
#define set_board_info(x)	(gd->arch.board_family->curr_board = x)

#endif /* _BOARD_H_ */
