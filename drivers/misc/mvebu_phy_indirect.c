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
#include <asm/arch-mvebu/mvebu_phy_indirect.h>


/* IHB registers */
#define MVEBU_IHB_PHY_BASE(unit_id)			(0xfB000000 + ((uintptr_t)unit_id) * 0x2000000)
#define MVEBU_IHB_PHY_DATA_REG_OFF			0x0
#define MVEBU_IHB_PHY_CMD_REG_OFF			0x4
#define MVEBU_IHB_MAX_UNIT_ID				2

/* IHB Address */
#define MVEBU_IHB_PHY_ADDR_LSB_OFF			0
#define MVEBU_IHB_PHY_ADDR_LSB_MASK			0x3F
#define MVEBU_IHB_PHY_ADDR_MSB_OFF			6
#define MVEBU_IHB_PHY_ADDR_MSB_MASK			(0x3 << MVEBU_IHB_PHY_ADDR_MSB_OFF)

#define MVEBU_IHB_PHY_ADDR_LSB_GET(reg_ofs)	((reg_ofs) & MVEBU_IHB_PHY_ADDR_LSB_MASK)
#define MVEBU_IHB_PHY_ADDR_MSB_GET(reg_ofs)	(((reg_ofs) & MVEBU_IHB_PHY_ADDR_MSB_MASK) >> \
											MVEBU_IHB_PHY_ADDR_MSB_OFF)

/* IHB CMD register fields */
#define	MVEBU_IHB_CMD_OP_OFFS				0
#define	MVEBU_IHB_CMD_OP_MASK				0x1
#define	MVEBU_IHB_CMD_PACKET_LOCAL_OFFS		5
#define	MVEBU_IHB_CMD_DONE_OFFS				6
#define	MVEBU_IHB_CMD_DONE_MASK				(0x3 << MVEBU_IHB_CMD_DONE_OFFS)
#define	MVEBU_IHB_CMD_ADDR_LSB_OFFS			16	/* IHB LSB address bits */
#define	MVEBU_IHB_CMD_ADDR_LSB_MASK			(0x3F << MVEBU_IHB_CMD_ADDR_LSB_OFFS)
#define	MVEBU_IHB_CMD_REGION_OFFS			22	/* IHB access space */
#define	MVEBU_IHB_CMD_REGION_MASK			(0x3 << MVEBU_IHB_CMD_REGION_OFFS)

#define MVEBU_IHB_CMD_GET(cmd, region, reg_ofs) \
				(((cmd << MVEBU_IHB_CMD_OP_OFFS) & MVEBU_IHB_CMD_OP_MASK) | \
				(1 << MVEBU_IHB_CMD_PACKET_LOCAL_OFFS) | \
				((region << MVEBU_IHB_CMD_REGION_OFFS) & MVEBU_IHB_CMD_REGION_MASK) | \
				(MVEBU_IHB_PHY_ADDR_LSB_GET(reg_ofs) << MVEBU_IHB_CMD_ADDR_LSB_OFFS))

/* IHB PHY CTRL register fields */
#define MVEBU_IHB_PHY_CTRL_REG_OFF			0x7
#define MVEBU_IHB_PHY_CTRL_ADDR_MSB_OFFS	27
#define MVEBU_IHB_PHY_CTRL_ADDR_MSB_MASK	(0x3 << 27)
#define MVEBU_IHB_PHY_IF_MODE_OFFS		25
#define MVEBU_IHB_PHY_IF_MODE_MASK		(1 << MVEBU_IHB_PHY_IF_MODE_OFFS)

/* IHB RFU PHY register fields */
#define MVEBU_IHB_RFU_REG_OFF				0x6F010C
#define	MVEBU_IHB_RFU_RESET_OFFS			0
#define	MVEBU_IHB_RFU_RESET_MASK			(1 << MVEBU_IHB_RFU_RESET_OFFS)
#define	MVEBU_IHB_RFU_TCELL_BYPASS_OFFS		10
#define	MVEBU_IHB_RFU_BASE_OFFS				16
#define	MVEBU_IHB_RFU_BASE_MASK				(0xFF << MVEBU_IHB_RFU_BASE_OFFS)

#define	MVEBU_IHB_BASE_HIGH_BYTE_OFF		24

int __attribute__((section(".data"))) ihb_init_done = 0; /* inidicates whether IHB PHY access was initialized */

enum ihb_access_type {
	IHB_WRITE = 0,
	IHB_READ  = 1
};



static int mvebu_ihb_poll_read_done(int reg_ofs, int unit_id)
{
	u32 ihb_cmd_reg = 0;
	u32 timeout = 100;

	do {
		ihb_cmd_reg = readl(MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_CMD_REG_OFF);
	} while (((ihb_cmd_reg & MVEBU_IHB_CMD_DONE_MASK) != MVEBU_IHB_CMD_DONE_MASK) &&
			 (timeout-- > 0));

	if (timeout == 0) {
		error("Read timeout\n");
		return 1;
	}

	return 0;
}

/*  IHB Indirect Access decription:
 *      the access is done using CMD and DATA register.
 *      the address and access type are set in the CMD register.
 *
 *          [0] = 0 -> write
 *                1 -> read
 *      [23:22] = 0x0 -> access IHB Controller space
 *                0x3 -> indicate IHB PHY REG address space,
 *      [21:16] = LSB of the Offset
 *
 *      (MSB are set to IHB_PHY_CTRL register (0x7) bit [28:27]
*/

static int mvebu_ihb_command_set(enum ihb_access_type access_type, int reg_ofs,
		u32 data, int unit_id, enum ihb_region region)
{
	u32 ihb_cmd_reg = 0;
	u32 ihb_phy_ctrl_reg = 0;

	/* Step 1 - set address MSB in IHB_PHY_CTRL (using RMW) */
	/* read IHB_PHY_CTRL */
	ihb_cmd_reg = MVEBU_IHB_CMD_GET(IHB_READ, IHB_CTRL_REGION, MVEBU_IHB_PHY_CTRL_REG_OFF);
	writel(ihb_cmd_reg, MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_CMD_REG_OFF);
	/* poll for read completion */
	if (mvebu_ihb_poll_read_done(reg_ofs, unit_id))
		return 1;

	ihb_phy_ctrl_reg = readl(MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_DATA_REG_OFF);

	/* update msb in MVEBU_IHB_PHY_CTRL */
	ihb_phy_ctrl_reg &= ~MVEBU_IHB_PHY_CTRL_ADDR_MSB_MASK; /* clear msb data */
	ihb_phy_ctrl_reg |= ((MVEBU_IHB_PHY_ADDR_MSB_GET(reg_ofs) << MVEBU_IHB_PHY_CTRL_ADDR_MSB_OFFS) &
						 MVEBU_IHB_PHY_CTRL_ADDR_MSB_MASK);/* set msb data */

    /* write IHB_PHY_CTRL */
	writel(ihb_phy_ctrl_reg, MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_DATA_REG_OFF);	/* set data */
	ihb_cmd_reg = (ihb_cmd_reg & ~MVEBU_IHB_CMD_OP_MASK) | IHB_WRITE;			/* set write command */
	writel(ihb_cmd_reg, MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_CMD_REG_OFF);		/* set commnd */

	/* Step 2 - set IHB command with LSB */
	if (access_type == IHB_WRITE) {
		/* set data in ihb data reg */
		writel(data, MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_DATA_REG_OFF);
	}

	ihb_cmd_reg = MVEBU_IHB_CMD_GET(access_type, region, reg_ofs);
	writel(ihb_cmd_reg, MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_CMD_REG_OFF);		/* set commnd */

	return 0;
}

static int mvebu_ihb_read(int reg_ofs, u32 *val, int unit_id, enum ihb_region
		region)
{
	/* initialize access to ihb phy (incase it wasn't already) */
	if (!ihb_init_done)
		mvebu_phy_indirect_init();

	/* set read command */
	if (mvebu_ihb_command_set(IHB_READ, reg_ofs, 0 /* dummy */, unit_id, region)) {
		error("IHB read: set command failed\n");
		return 1;
	}

	/* poll for read completion */
	if (mvebu_ihb_poll_read_done(reg_ofs, unit_id))
		return 1;

	*val = readl(MVEBU_IHB_PHY_BASE(unit_id) | MVEBU_IHB_PHY_DATA_REG_OFF);

	return 0;
}

static int mvebu_ihb_write(int reg_ofs, u32 data, int unit_id, enum ihb_region region)
{
	/* initialize access to ihb phy (incase it wasn't already) */
	if (!ihb_init_done)
		mvebu_phy_indirect_init();

	/* set write command */
	if (mvebu_ihb_command_set(IHB_WRITE, reg_ofs, data, unit_id, region)) {
		error("IHB write: set command failed\n");
		return 1;
	}

	return 0;
}

int mvebu_phy_indirect_read(enum ihb_region region, enum phy_indirect_unit phy_unit, int unit_id, int reg_ofs, u32 *val)
{
	switch (phy_unit) {
	case INDIRECT_IHB:
		return mvebu_ihb_read(reg_ofs, val, unit_id, region);
	default:
		error("unit %d is not supported\n", phy_unit);
		return 1;
	}
}

int mvebu_phy_indirect_write(enum ihb_region region, enum phy_indirect_unit phy_unit, int unit_id, int reg_ofs, u32 val)
{
	switch (phy_unit) {
	case INDIRECT_IHB:
		return mvebu_ihb_write(reg_ofs, val, unit_id, region);
	default:
		error("unit %d is not supported\n", phy_unit);
		return 1;
	}
}

int mvebu_phy_indirect_init(void)
{
	u8 *regs_base = (u8 *)MVEBU_REGS_BASE;
	u32 reg_data, i;
	u32 ihb_base_high_byte;
	u32 ihb_cmd_reg = 0, val;

	for (i = 0; i < MVEBU_IHB_MAX_UNIT_ID; ++i) {
		ihb_base_high_byte = MVEBU_IHB_PHY_BASE(i) >> MVEBU_IHB_BASE_HIGH_BYTE_OFF;
		reg_data = readl(regs_base + MVEBU_IHB_RFU_REG_OFF);

		/* set configuration data */
		reg_data &= ~MVEBU_IHB_RFU_RESET_MASK; /* un-reset IHB PHY */
		reg_data |= (1 << MVEBU_IHB_RFU_TCELL_BYPASS_OFFS); /* enable TCELL bypass since
											this bridge is not functional */
		reg_data = (ihb_base_high_byte << MVEBU_IHB_RFU_BASE_OFFS) |
					(reg_data & ~MVEBU_IHB_RFU_BASE_MASK); /* set IHB base address */

		/* Configure IHB MAC base address RFU base and Un-reset IHB PHY */
		writel(reg_data, regs_base + MVEBU_IHB_RFU_REG_OFF);

		/* Set IF mode in phy control register */
		ihb_cmd_reg = MVEBU_IHB_CMD_GET(IHB_READ, IHB_CTRL_REGION, MVEBU_IHB_PHY_CTRL_REG_OFF);
		writel(ihb_cmd_reg, MVEBU_IHB_PHY_BASE(i) | MVEBU_IHB_PHY_CMD_REG_OFF);

		val = readl(MVEBU_IHB_PHY_BASE(i) | MVEBU_IHB_PHY_DATA_REG_OFF);
		val = val | MVEBU_IHB_PHY_IF_MODE_MASK;
		writel(val, MVEBU_IHB_PHY_BASE(i) | MVEBU_IHB_PHY_DATA_REG_OFF);

		ihb_cmd_reg = MVEBU_IHB_CMD_GET(IHB_WRITE, IHB_CTRL_REGION, MVEBU_IHB_PHY_CTRL_REG_OFF);
		writel(ihb_cmd_reg, MVEBU_IHB_PHY_BASE(i) | MVEBU_IHB_PHY_CMD_REG_OFF);
	}

	ihb_init_done = 1; /* inidcate that IHB PHY access was initialized */

	return 0;
}
