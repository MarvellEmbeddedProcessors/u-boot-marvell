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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mvebu.h>

#define IHB1_X2_START_ADDRESS		(MVEBU_REGS_BASE + 0x6F4238)
#define IHB2_X2_START_ADDRESS		(MVEBU_REGS_BASE + 0x6F4258)
#define IHB_X4_START_ADDRESS		(MVEBU_REGS_BASE + 0x6F4218)

#define IHB_CONFIG_DATA_REG		(0x0)
#define IHB_CONFIG_CMD_REG		(0x4)

#define IHB_X2_VALUE			(0x3B2DF205)
#define IHB_X4_VALUE			(0x452DF205)

enum ihb_type {
	IHB1_X2 = 0,
	IHB2_X2,
	IHB_X4,
};

static int mvebu_ihb_timeout_check(void __iomem *reg, u32 check_val)
{
	u32 reg_data, timeout = 100;
	do {
		reg_data = readl(reg);
	} while (((reg_data & check_val) != check_val) && (timeout-- > 0));

	if (timeout == 0) {
		error("timeout while read from 0x%p - value = 0x%x\n", reg, reg_data);
		return 1;
	}

	return 0;
}

static int mvebu_ihb_run_sequence(void __iomem *base, u32 ihb_value)
{
	u32 reg_data;

	/* Set PCIe mode and enforce - queues separation to avoid dependencies */
	writel(0x80000041, base + IHB_CONFIG_DATA_REG);
	writel(0x00000123, base + IHB_CONFIG_CMD_REG);
	/* Circular packet done */
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT4))
		return 1;

	/* Read check PCIe mode and enforce - internal HB register 0xD: */
	writel(0x000D0121, base + IHB_CONFIG_CMD_REG);
	/* Read data ready */
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT7))
		return 1;
	/* Enforce, PCIe mode accordingly */
	reg_data = readl(base + IHB_CONFIG_DATA_REG);
	if ((reg_data & BIT27) != BIT27) {
		error("BIT 27 is not equal to 1\n");
		return 1;
	}
	if ((reg_data & BIT24) != BIT24) {
		error("BIT 24 is not equal to 1\n");
		return 1;
	}

	/* Prevent crisis mode - divider/timing change to eliminate deadlock
	   scenarios */
	/* Set register in AP */
	writel(ihb_value, base + IHB_CONFIG_DATA_REG);
	writel(0x00250020, base + IHB_CONFIG_CMD_REG);
	/* Command completed */
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT6))
		return 1;

	/* Read check value */
	writel(0x00250021, base + IHB_CONFIG_CMD_REG);
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT7))
		return 1;

	reg_data = readl(base + IHB_CONFIG_CMD_REG);
	if (reg_data != ihb_value) {
		error("data %x not equal to %x\n", reg_data, ihb_value);
		return 1;
	}

	/* Set register in CP */
	writel(ihb_value, base + IHB_CONFIG_DATA_REG);
	writel(0x00250200, base + IHB_CONFIG_CMD_REG);
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT6))
		return 1;

	/* Read check value */
	writel(0x00250200, base + IHB_CONFIG_CMD_REG);
	if (mvebu_ihb_timeout_check(base + IHB_CONFIG_CMD_REG, BIT7))
		return 1;

	reg_data = readl(base + IHB_CONFIG_CMD_REG);
	if (reg_data != ihb_value) {
		error("data %x not equal to %x\n", reg_data, ihb_value);
		return 1;
	}

	return 0;
}

int mvebu_ihb_init(enum ihb_type ihb)
{
	u64 base;
	u32 ihb_val;

	switch (ihb) {
	case IHB1_X2:
		ihb_val = IHB_X2_VALUE;
		base = readl(IHB1_X2_START_ADDRESS);
		break;
	case IHB2_X2:
		ihb_val = IHB_X2_VALUE;
		base = readl(IHB2_X2_START_ADDRESS);
		break;
	case IHB_X4:
		ihb_val = IHB_X4_VALUE;
		base = readl(IHB_X4_START_ADDRESS);
		break;
	default:
		error("IHB type is wrong\n");
		return -1;
	}

	return mvebu_ihb_run_sequence((void __iomem *)base, ihb_val);
}
