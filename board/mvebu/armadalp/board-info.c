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

struct mvebu_board_info *armadalp_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_LP_DB0_ID] = &db0_armadalp_info,
	[ARMADA_LP_DB1_ID] = &db1_armadalp_info
};
struct mvebu_board_family armadalp_board_family = {
	.default_id = ARMADA_LP_DB0_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = armadalp_board_lookup,
};

