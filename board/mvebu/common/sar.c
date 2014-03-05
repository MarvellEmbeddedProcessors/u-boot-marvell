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

#define DEBUG

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <linux/compiler.h>
#include "board.h"
#include "sar.h"

DECLARE_GLOBAL_DATA_PTR;

struct var_desc sar_desc_table[MAX_SAR] = {
	[CPUS_NUM_SAR] =	{ "cpusnum", "Number of active CPUs"},
	[CPU0_ENDIANES_SAR] =	{ "cpuend", "CPU endianes mode"},
	[FREQ_SAR] =		{ "freq", "SOC frequency configiuration (CPU/ Fabric/ DDR)"},
	[CPU_FREQ_SAR] =	{ "cpufreq", "CPU Frequency"},
	[FAB_REQ_SAR] =		{ "fabfreq", "Fabric (L2) frequency"},
	[BOOT_SRC_SAR] =	{ "bootsrc", "Boot device source"},
	[BOOT_WIDTH_SAR] =	{ "bootwidth", "Width of boot device read"},
	[PEX_MODE_SAR] =	{ "pexmode", "Pex configuration"},
	[L2_SIZE_SAR] =		{ "l2size", "Size of L2 cache"},
	[DRAM_ECC_SAR] =	{ "dramecc", "Enables DRAM ECC"},
	[DRAM_BUS_WIDTH_SAR] =	{ "dramwidth", "DRAM bus width"}
};

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

static int sar_read_reg(u32 *reg)
{
	uchar byte = 0;
	int ret, chip;
	u32 sar_reg = 0;
	struct sar_data *sar = board_get_sar();
	int reg_width = sar->bit_width;
	u8  reg_mask = (1 << reg_width) - 1;

	for (chip = 0; chip  < sar->chip_count; chip++) {
		/*ret = i2c_read(var->chip_addr, var->reg_offset, 1, &byte, 1);*/
		ret = i2c_read_dummy(sar->chip_addr[chip], 0, 1, &byte, 1);
		if (ret) {
			printf("Error: %s: Failed reading from chip 0x%x\n",
			       __func__, sar->chip_addr[chip]);
			return -1;
		}
		sar_reg |= (byte & reg_mask) << (chip * reg_width);
	}
	debug("read: sar register = 0x%08x\n", sar_reg);
	*reg = sar_reg;

	return 0;
}

int sar_write_reg(u32 sar_reg)
{
	uchar byte = 0;
	int ret, chip;
	struct sar_data *sar = board_get_sar();
	int reg_width = sar->bit_width;
	u8  reg_mask = (1 << reg_width) - 1;

	for (chip = 0; chip  < sar->chip_count; chip++) {
		/*ret = i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)*/
		byte = (sar_reg >> (chip * reg_width)) & reg_mask;
		ret = i2c_write_dummy(sar->chip_addr[chip], 0, 1, &byte, 1);
		if (ret) {
			printf("Error: %s: Failed writing to chip 0x%x\n",
			       __func__, sar->chip_addr[chip]);
			return -1;
		}
	}
	debug("write: sar register = 0x%08x\n", sar_reg);

	return 0;
}


int sar_read_var(struct sar_var *var, int *val)
{
	u32 sar_reg;
	u32 var_mask = (1 << var->bit_length) - 1;

	if (sar_read_reg(&sar_reg))
		return -1;

	(*val) = (sar_reg >> var->start_bit) & var_mask;

	debug("var offet = %d len = %d val = 0x%x\n", var->start_bit, var->bit_length, (*val));

	return 0;
}

int sar_write_var(struct sar_var *var, int val)
{
	u32 sar_reg;
	u32 var_mask = (1 << var->bit_length) - 1;

	if (sar_read_reg(&sar_reg))
		return -1;

	/* Update the bitfield inside the sar register */
	val &= var_mask;
	sar_reg &= ~(var_mask << var->start_bit);
	sar_reg |= (val << var->start_bit);

	/* Write the full sar register back to i2c */
	if (sar_write_reg(sar_reg))
		return -1;

	return 0;
}

static int sar_default_var(struct sar_var *var)
{
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

	if (sar_write_var(var, dflt->value)) {
		printf("Error: Failed to write default value");
		return 1;
	}

	debug("Wrote default value 0x%x = %s\n", dflt->value, dflt->desc);
	return 0;
}

int sar_get_key_id(const char *key)
{
	int id;

	for (id = 0; id < MAX_SAR; id++) {
		if (strcmp(key, sar_desc_table[id].key) == 0)
			return id;
	}
	return -1;
}

int sar_is_var_active(int id)
{
	struct sar_var *sar_table = board_get_sar_table();
	return sar_table[id].active;
}

struct var_opts *sar_get_var_opts(int id, int *cnt)
{
	struct sar_var *sar_table = board_get_sar_table();

	(*cnt) = sar_table[id].option_cnt;

	return sar_table[id].option_desc;
}

int sar_validate_key(const char *key)
{
	int id = sar_get_key_id(key);

	if (id  == -1) {
		printf("Satr: Error: Unknown key \"%s\"\n", key);
		return -1;
	}
	if (sar_is_var_active(id) == 0) {
		printf("Satr: Error: Key \"%s\" is inactive on this board\n", key);
		return -1;
	}
	return id;
}

struct sar_var *sar_id_to_var(int id)
{
	struct sar_var *sar_table = board_get_sar_table();
	sar_table += id;
	return sar_table;
}


/* Interface to SatR command */
int sar_is_available(void)
{
	if (board_get_sar_table() == NULL)
		return 0;
	else
		return 1;
}

void sar_print_var(int id, bool print_opts)
{
	int cnt;
	struct var_opts *opts;

	printf("%-10s %s\n", sar_desc_table[id].key,
	       sar_desc_table[id].description);

	if (print_opts) {
		opts = sar_get_var_opts(id, &cnt);
		while (cnt--) {
			printf("\t0x%-2x %s ", opts->value, opts->desc);
			if (opts->flags & VAR_IS_DEFUALT)
				printf("[Default]");
			printf("\n");
			opts++;
		}
	}
}


void sar_list_keys(void)
{
	int id;

	printf("\n");
	for (id = 0; id < MAX_SAR; id++) {
		if (sar_is_var_active(id))
			sar_print_var(id, 0);
	}
	printf("\n");
}

int sar_list_key_opts(const char *key)
{
	int id = sar_validate_key(key);

	if (id == -1)
		return -EINVAL;

	printf("\n");
	sar_print_var(id, 1);
	printf("\n");

	return 0;
}


int  sar_print_key(const char *key)
{
	int id = sar_validate_key(key);
	struct sar_var *var;
	struct var_opts *opts;
	char *desc = NULL;
	int val, ret, cnt;

	if (id == -1)
		return -EINVAL;

	var = sar_id_to_var(id);
	ret = sar_read_var(var, &val);
	if (ret)
		return ret;

	opts = sar_get_var_opts(id, &cnt);
	while (cnt--) {
		if (opts->value == val)
			desc = opts->desc;
		opts++;
	}

	if (desc == NULL)
		printf("%s = 0x%x  ERROR: UNKNOWN OPTION\n", key, val);
	else
		printf("%s = 0x%x  %s\n", key, val, desc);

	return 0;
}

int  sar_write_key(const char *key, int val)
{
	int id = sar_validate_key(key);
	struct sar_var *var;
	struct var_opts *opts;
	char *desc = NULL;
	int cnt;

	if (id == -1)
		return -EINVAL;

	var = sar_id_to_var(id);
	opts = sar_get_var_opts(id, &cnt);
	while (cnt--) {
		if (opts->value == val)
			desc = opts->desc;
		opts++;
	}

	if (desc == NULL) {
		printf("ERROR: value 0x%x not supported for key %s\n", val, key);
		printf("use \"SatR list %s\" to print supported values\n", key);
	}

	if (sar_write_var(var, val))
		return -1;

	/* Display the updated variable */
	sar_print_key(key);

	return 0;
}

int sar_defualt_all(void)
{
	struct sar_var *var;
	int id;
	int ret = 0;

	for (id = 0; id < MAX_SAR; id++) {
		var = sar_id_to_var(id);
		if (sar_is_var_active(id)) {
			ret |= sar_default_var(var);
			sar_print_key(sar_desc_table[id].key);
		}
	}

	return ret;
}

int  sar_default_key(const char *key)
{
	int id = sar_validate_key(key);
	struct sar_var *var;
	int ret;

	if (id == -1)
		return -EINVAL;

	var = sar_id_to_var(id);
	ret = sar_default_var(var);
	if (ret)
		return ret;

	/* Display the updated variable */
	sar_print_key(key);

	return 0;
}

