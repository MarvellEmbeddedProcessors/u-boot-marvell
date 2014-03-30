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
#include "sar.h"
#include "config.h"

#define MAX_BOARD_NAME		16

/*
 * mvebu_board_config  - Describes board configuration features
 *
 * sar_cnt - number of suported SAR options
 * supported_sar - array of supported SAR variables
 * sar_override - table that overrides the default sar table
 */
struct mvebu_board_config {
	int sar_cnt;
	enum sar_variables active_sar[MAX_SAR];
	struct sar_var *sar_override;
	int cfg_cnt;
	enum cfg_variables active_cfg[MAX_SAR];
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
	bool configurable;
	struct mvebu_board_config *config_data;
};

/*
 * mvebu_board_familt - Describes common board data
 *
 * default_id - defualt board id in case board_id can't be read.
 * board_cnt - number of different boards
 * curr_board - pointer to current board's info
 * boards_info - pointer to table containing
 * sar - General SAR information for all boards (optional)
 *
 */
struct mvebu_board_family {
	int  default_id;
	int  board_cnt;
	struct mvebu_board_info *curr_board;
	struct mvebu_board_info **boards_info;
	struct sar_data *sar;
	struct cfg_data *cfg;
};

/* APIs required from all board families */
struct mvebu_board_family *board_init_family(void);

/* Common board API */
int board_get_id(void);


DECLARE_GLOBAL_DATA_PTR;
#define get_board_family()	(struct mvebu_board_family *)(gd->arch.board_family)
#define set_board_family(x)	(gd->arch.board_family = x)
#define get_board_info()	(struct mvebu_board_info *)(gd->arch.board_family->curr_board)
#define set_board_info(x)	(gd->arch.board_family->curr_board = x)

#endif /* _BOARD_H_ */
