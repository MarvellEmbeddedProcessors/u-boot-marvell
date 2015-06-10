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

#define SATR_EEPROM0_ADDR	0x22

/* Define all board configuration variables */
/* Assuming their location is equal on all boards */
struct cfg_var armadalp_cfg_lookup[MAX_CFG] = {
	[BOARD_ID_CFG] = {0, 0, 3, 4, 0,
		{{ARMADA_LP_DB0_ID, "DB-9990", VAR_IS_DEFUALT},
		 {ARMADA_LP_DB1_ID, "DB-9991", 0} },
	},
	[SERDES_0_CFG] = {1, 0, 3, 2, 0,
		{{0x0, "pcie 0", VAR_IS_DEFUALT},
		 {0x1, "sata 0", 0} },
	},
	[SERDES_1_CFG] = {1, 3, 3, 3, 0,
		{{0x0, "pcie 1", 0},
		 {0x1, "sata 1", VAR_IS_DEFUALT},
		 {0x2, "usb3 1", 0} },
	}
};

/* Define general SAR information */
struct cfg_data armadalp_cfg = {
	.chip_addr    = 0x55,
	.cfg_lookup   = armadalp_cfg_lookup
};



struct mvebu_board_info *armadalp_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_LP_DB0_ID] = &db0_armadalp_info,
	[ARMADA_LP_DB1_ID] = &db1_armadalp_info
};
struct mvebu_board_family armadalp_board_family = {
	.default_id = ARMADA_LP_DB0_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = armadalp_board_lookup,
	.cfg = &armadalp_cfg,
};

