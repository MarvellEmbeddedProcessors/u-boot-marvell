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

#include <common.h>
#include <linux/compiler.h>
#include <asm/arch-mvebu/unit-info.h>
#include "board-info.h"

enum a8k_board_types {
	ARMADA_8021_DB_ID,
	ARMADA_8021_RD_ID,
	ARMADA_8022_DB_ID,
	ARMADA_8022_RD_ID,
	MAX_BOARD_ID
};

u16 a8021_rd_unit_disable[MAX_UNIT_ID] = {
	[NAND_UNIT_ID] = id_to_mask(0),
	[SDIO_UNIT_ID] = id_to_mask(0)
};

struct mvebu_board_info a8021_db_info = {
	.name = "DB-MV8021",
	.id = ARMADA_8021_DB_ID,
};

struct mvebu_board_info a8021_rd_info = {
	.name = "RD-MV8021",
	.id = ARMADA_8021_RD_ID,
	.unit_mask = a8021_rd_unit_disable,
	.unit_update_mode = UNIT_INFO_DISABLE,
};

struct mvebu_board_info *a8k_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_8021_DB_ID] = &a8021_db_info,
	[ARMADA_8021_RD_ID] = &a8021_rd_info
};

struct mvebu_board_family a8k_board_family = {
	.default_id = ARMADA_8021_DB_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = a8k_board_lookup,
};

