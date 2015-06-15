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

#ifndef _BOARD_INFO_H_
#define _BOARD_INFO_H_

#include "../common/board.h"
#include "../common/sar.h"

enum a8k_board_types {
	ARMADA_8021_DB_ID = 0,
	ARMADA_8021_RD_ID,
	ARMADA_8022_DB_ID,
	ARMADA_8022_RD_ID,
	MAX_BOARD_ID
};

extern struct mvebu_board_info a8021_db_info;
extern struct mvebu_board_info a8021_rd_info;
extern struct mvebu_board_family a8k_board_family;

#endif /* _BOARD_INFO_H_ */
