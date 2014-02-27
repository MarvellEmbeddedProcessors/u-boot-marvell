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

/* Define all SAR variables available for SOC */
/* Assuming their location is equal on all boards */
struct sar_var a8k_sar_lookup[MAX_SAR] = {
	[CPUS_NUM_SAR] = {0, 2, 3,
		{{0x0, "Single CPU", 0},
		 {0x2, "Dual CPU", 0},
		 {0x3, "Quad CPU", VAR_IS_DEFUALT} },
	},
	[FREQ_SAR] = {2, 5, 2,
		{{0x0, "800 / 400 / 400"},
		 {0x5, "1200 / 600 / 600"} },
	},
	[BOOT_SRC_SAR] = {7, 2, 3,
		{{0x0, "NAND boot", 0},
		 {0x2, "SPI boot", VAR_IS_DEFUALT},
		 {0x3, "NOR boot", 0} },
	}
};

/* Define general SAR information */
struct sar_data a8k_sar = {
	.chip_addr    = {0x4c, 0x4d, 0x4e, 0x4f},
	.chip_count   = 4,
	.bit_width    = 5,
	.sar_lookup   = a8k_sar_lookup
};

/* Define all board configuration variables */
/* Assuming their location is equal on all boards */
struct cfg_var a8k_cfg_lookup[MAX_CFG] = {
	[BOARD_ID_CFG] = {0, 0, 3, 4,
		{{ARMADA_8021_DB_ID, "DB-8021", VAR_IS_DEFUALT},
		 {ARMADA_8021_RD_ID, "RD-8021", 0},
		 {ARMADA_8022_DB_ID, "DB-8022", 0},
		 {ARMADA_8022_RD_ID, "RD-8022", 0} },
	},
	[SERDES_0_CFG] = {1, 0, 3, 2,
		{{0x0, "pcie 0", VAR_IS_DEFUALT},
		 {0x1, "sata 0", 0} },
	},
	[SERDES_1_CFG] = {1, 3, 3, 3,
		{{0x0, "pcie 1", 0},
		 {0x1, "sata 1", VAR_IS_DEFUALT},
		 {0x2, "usb3 1", 0} },
	}
};

/* Define general SAR information */
struct cfg_data a8k_cfg = {
	.chip_addr    = 0x55,
	.cfg_lookup   = a8k_cfg_lookup
};



struct mvebu_board_info *a8k_board_lookup[MAX_BOARD_ID] = {
	[ARMADA_8021_DB_ID] = &a8021_db_info,
	[ARMADA_8021_RD_ID] = &a8021_rd_info
};

struct mvebu_board_family a8k_board_family = {
	.default_id = ARMADA_8021_DB_ID,
	.board_cnt = MAX_BOARD_ID,
	.boards_info = a8k_board_lookup,
	.sar = &a8k_sar,
	.cfg = &a8k_cfg,
};

