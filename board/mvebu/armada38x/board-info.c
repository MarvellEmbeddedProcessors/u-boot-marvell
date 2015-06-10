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

#define SATR_EEPROM0_ADDR	0x22

/* Define all board configuration variables */
/* Assuming their location is equal on all boards */
struct cfg_var a38x_cfg_lookup[MAX_CFG] = {
	[BOARD_ID_CFG] = {0, 0, 3, 4, 0,
		{{ARMADA_38X_DB_ID, "DB-88F6820-BP", VAR_IS_DEFUALT},
		 {ARMADA_38X_RD_ID, "DB-88F6820-RD", 0} }
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
struct cfg_data a38x_cfg = {
	.chip_addr    = 0x55,
	.cfg_lookup   = a38x_cfg_lookup
};

struct mvebu_board_info *a38x_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_38X_DB_ID] = &a38x_db_info,
	[ARMADA_38X_RD_ID] = &a38x_rd_info,
	[ARMADA_38X_CUSTOMER_ID] = &a38x_customer_info
};

struct mvebu_board_family a38x_board_family = {
	.default_id = ARMADA_38X_DB_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = a38x_board_lookup,
	.cfg = &a38x_cfg,
};

