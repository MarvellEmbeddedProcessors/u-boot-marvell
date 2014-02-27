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

#ifndef _SAR_H_
#define _SAR_H_

#include <common.h>
#include <asm/arch-mvebu/mvebu.h>
#include <linux/compiler.h>
#include "var.h"

#define MAX_SAR_CHIPS	4

struct sar_var {
	u8 start_bit;
	u8 bit_length;
	u8 option_cnt;
	struct var_opts option_desc[MAX_VAR_OPTIONS];
};

struct sar_data {
	u32	chip_addr[MAX_SAR_CHIPS];
	u8	chip_count;
	u8	bit_width;
	struct sar_var *sar_lookup;
};

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
	MAX_SAR
};

int  sar_default_key(const char *key);
int  sar_defualt_all(void);
int  sar_write_key(const char *key, int val);
int  sar_print_key(const char *key);
void sar_list_keys(void);
int  sar_list_key_opts(const char *key);
int  sar_is_available(void);

#endif /* _SAR_H_ */
