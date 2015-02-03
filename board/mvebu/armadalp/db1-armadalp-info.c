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

struct mvebu_board_config db1_armadalp_config = {
	.sar_cnt = 2,
	.active_sar = {CPUS_NUM_SAR},
	.cfg_cnt = 2,
	.active_cfg = {BOARD_ID_CFG}
};

struct mvebu_board_info db1_armadalp_info = {
	.name = "DB1-MV-ARMADALP",
	.id = ARMADA_LP_DB1_ID,
	.mpp_regs = {0x11111111, 0x00100001},
	.configurable = 1,
	.config_data = &db1_armadalp_config,
};

