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
#include <linux/compiler.h>
#include "board-info.h"

struct mvebu_board_info *a8k_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_8021_DB_ID] = &a8021_db_info,
	[ARMADA_8021_RD_ID] = &a8021_rd_info
};

struct mvebu_board_family a8k_board_family = {
	.default_id = ARMADA_8021_DB_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = a8k_board_lookup,
};

