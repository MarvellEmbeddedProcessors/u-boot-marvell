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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <linux/compiler.h>
#include "var.h"


struct cfg_var {
	u8 reg_offset;
	u8 start_bit;
	u8 bit_length;
	u8 option_cnt;
	struct var_opts option_desc[MAX_VAR_OPTIONS];
};

struct cfg_data {
	u32	chip_addr;
	struct cfg_var *cfg_lookup;
};

enum cfg_variables {
	BOARD_ID_CFG = 0,
	SERDES_0_CFG,
	SERDES_1_CFG,
	MAX_CFG
};

int  cfg_default_all(void);
int  cfg_default_var(int id);
int  cfg_write_var(int id, int val);
int  cfg_read_var(int id, int *val);
int  cfg_is_available(void);
int  cfg_is_var_active(int id);

struct var_opts *cfg_get_var_opts(int id, int *cnt);

#endif /* _CONFIG_H_ */
