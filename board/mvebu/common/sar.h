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

#ifndef _SAR_H_
#define _SAR_H_

#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <linux/compiler.h>
#include "var.h"

#define MAX_SAR_CHIPS	4
#define MAX_SAR 8

enum sar_variables {
	CPUS_NUM_SAR = 0,
	CPU0_ENDIANES_SAR,
	FREQ_SAR,
	CPU_FREQ_SAR,
	FAB_REQ_SAR,
	BOOT_SRC_SAR,
	BOOT_WIDTH_SAR,
	PEX_MODE_SAR,
	L2_SIZE_SAR,
	DRAM_ECC_SAR,
	DRAM_BUS_WIDTH_SAR,
};

struct sar_var {
	u8 start_bit;
	u8 bit_length;
	u8 option_cnt;
	u8 active;
	bool swap_bit;
	char *desc;
	char *key;
	struct var_opts option_desc[MAX_VAR_OPTIONS];
};

struct sar_data {
	u32	chip_addr[MAX_SAR_CHIPS];
	u8	chip_count;
	u8	bit_width;
	struct sar_var sar_lookup[MAX_SAR];
};

int  sar_default_key(const char *key);
int  sar_defualt_all(void);
int  sar_write_key(const char *key, int val);
int  sar_print_key(const char *key);
void sar_list_keys(void);
int  sar_list_key_opts(const char *key);
int  sar_is_available(void);
void sar_init(void);

#endif /* _SAR_H_ */
