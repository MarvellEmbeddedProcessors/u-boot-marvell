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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/unit-info.h>
#include <linux/compiler.h>

#define MAX_BOARD_NAME	16

/*
 * mvebu_board_config  - Describes board configuration features
 *
 */
struct mvebu_board_config {
	int x;
};


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
	u16  *unit_mask;
	enum unit_update_mode unit_update_mode;
	struct mvebu_board_config *config;
};

/*
 * mvebu_board_familt - Describes common board data
 *
 * default_id - defualt board id in case board_id can't be read.
 * board_cnt - number of different boards
 * curr_board - pointer to current board's info
 * boards_info - pointer to table containing
 *
 */
struct mvebu_board_family {
	int  default_id;
	int  board_cnt;
	struct mvebu_board_info *curr_board;
	struct mvebu_board_info **boards_info;
};


int common_board_init(struct mvebu_board_family *board_family);

#endif /* _BOARD_H_ */
