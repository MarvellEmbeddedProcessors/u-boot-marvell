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

#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/mpp.h>

char *mpp_get_desc_table(void)
{
	return soc_get_mpp_desc_table();
}

void set_mpp(int mpp_id, int value)
{
	int reg_offset;
	int field_offset;
	u32 reg, mask;

	if (value > MAX_MPP_OPTS)
		printf("Warning: MPP value %d > max value %d\n", value, MAX_MPP_OPTS);

	/* Calculate register address and bit in register */
	reg_offset   = mpp_id >> (MPP_FIELD_BITS);
	field_offset = (MPP_BIT_CNT) * (mpp_id & MPP_FIELD_MASK);
	mask = (MPP_VAL_MASK << field_offset);

	/* Clip value to bit resolution */
	value &= MPP_VAL_MASK;

	reg = readl(MPP_REGS_BASE + reg_offset);
	reg = (reg & mask) | (value << field_offset);
	writel(reg, MPP_REGS_BASE + reg_offset);
}

u8 get_mpp(int mpp_id)
{
	int reg_offset;
	int field_offset;
	u32 reg, mask;
	u8 value;

	/* Calculate register address and bit in register */
	reg_offset   = mpp_id >> (MPP_FIELD_BITS);
	field_offset = (MPP_BIT_CNT) * (mpp_id & MPP_FIELD_MASK);

	reg = readl(MPP_REGS_BASE + reg_offset);
	val = (reg >> field_offset) & MPP_VAL_MASK;

	if (value > MAX_MPP_OPTS)
		printf("Warning: MPP value %d > max value %d\n", val, MAX_MPP_OPTS);

	return val;
}

void set_mpp_reg(u32 *mpp_reg, int first_reg, int last_reg)
{
	int reg_offset;
	int field_offset;
	int reg, mask;

	while (reg = first_reg; reg < last_reg; reg++; mpp_reg++)
		writel(*mpp_reg, MPP_REGS_BASE + reg);
}

