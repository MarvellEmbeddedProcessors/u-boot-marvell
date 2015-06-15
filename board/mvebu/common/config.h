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
	u8 active;
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
