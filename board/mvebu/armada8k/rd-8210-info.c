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
#include <asm/arch-mvebu/unit-info.h>
#include "board-info.h"


struct mvebu_board_config rd_a8021_config = {
	.sar_cnt = 1,
	.active_sar = {CPUS_NUM_SAR},
	.cfg_cnt = 1,
	.active_cfg = {BOARD_ID_CFG}
};

u16 a8021_rd_unit_disable[MAX_UNIT_ID] = {
	[NAND_UNIT_ID] = id_to_mask(0),
	[SDIO_UNIT_ID] = id_to_mask(0)
};

struct mvebu_board_info a8021_rd_info = {
	.name = "RD-MV8021",
	.id = ARMADA_8021_RD_ID,
	.mpp_regs = {0x11111111, 0x00100001},
	.unit_mask = a8021_rd_unit_disable,
	.unit_update_mode = UNIT_INFO_DISABLE,
	.configurable = 1,
	.config_data = &rd_a8021_config,
};

