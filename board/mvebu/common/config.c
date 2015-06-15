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

#define DEBUG

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <linux/compiler.h>
#include "devel-board.h"
#include "config.h"

DECLARE_GLOBAL_DATA_PTR;

#define I2C_DUMMY_BASE ((int)0x91000000)

/* I2C interface commands */
static int i2c_write_dummy(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	uintptr_t reg = (uintptr_t)(I2C_DUMMY_BASE) + addr;
	writeb(*buffer, reg);
	return 0;
}

static int i2c_read_dummy(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	uintptr_t reg = (uintptr_t)(I2C_DUMMY_BASE) + addr;
	(*buffer) = (uchar)readb(reg);
	return 0;
}

int cfg_is_var_active(int id)
{
	struct cfg_var *cfg_table = board_get_cfg_table();
	return cfg_table[id].active;
}

struct var_opts *cfg_get_var_opts(int id, int *cnt)
{
	struct cfg_var *cfg_table = board_get_cfg_table();

	(*cnt) = cfg_table[id].option_cnt;

	return cfg_table[id].option_desc;
}

struct cfg_var *cfg_id_to_var(int id)
{
	struct cfg_var *cfg_table = board_get_cfg_table();
	cfg_table += id;
	return cfg_table;
}

int cfg_is_available(void)
{
	if (board_get_cfg_table() == NULL)
		return 0;
	else
		return 1;
}

int cfg_write_var(int id, int val)
{
	struct cfg_data *cfg = board_get_cfg();
	struct cfg_var *var = cfg_id_to_var(id);
	uchar reg;
	u32 var_mask = (1 << var->bit_length) - 1;

	if (i2c_read_dummy(cfg->chip_addr, var->reg_offset, 1, &reg, 1))
		return -1;

	/* Update the bitfield inside the register */
	val &= var_mask;
	reg &= ~(var_mask << var->start_bit);
	reg |= (val << var->start_bit);

	/* Write the full cfg register back to i2c */
	if (i2c_write_dummy(cfg->chip_addr, var->reg_offset, 1, &reg, 1))
		return -1;

	return 0;
}

int cfg_read_var(int id, int *val)
{
	struct cfg_data *cfg = board_get_cfg();
	struct cfg_var *var = cfg_id_to_var(id);
	uchar reg;
	u32 var_mask = (1 << var->bit_length) - 1;

	if (i2c_read_dummy(cfg->chip_addr, var->reg_offset, 1, &reg, 1))
		return -1;

	(*val) = (reg >> var->start_bit) & var_mask;

	debug("var offet = %d len = %d val = 0x%x\n", var->start_bit, var->bit_length, (*val));

	return 0;
}

int cfg_default_var(int id)
{
	struct cfg_var	*var = cfg_id_to_var(id);
	struct var_opts *opts;
	struct var_opts *dflt =	NULL;
	int i;

	opts = var->option_desc;
	for (i = 0; i < var->option_cnt; i++, opts++) {
		if (opts->flags & VAR_IS_DEFUALT)
			dflt = opts;
	}

	if (dflt == NULL) {
		printf("Error: Failed to find default option");
		return 1;
	}

	if (cfg_write_var(id, dflt->value)) {
		printf("Error: Failed to write default value");
		return 1;
	}

	debug("Wrote default value 0x%x = %s\n", dflt->value, dflt->desc);
	return 0;
}

int cfg_default_all(void)
{
	int id;
	int ret = 0;

	for (id = 0; id < MAX_CFG; id++) {
		if (cfg_is_var_active(id))
			ret |= cfg_default_var(id);
	}

	return ret;
}


