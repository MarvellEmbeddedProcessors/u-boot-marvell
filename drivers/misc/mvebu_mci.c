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
#include <asm/arch-mvebu/mvebu_mci.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/soc.h>

/* MCI registers */
#define MVEBU_MCI_PHY_BASE(unit_id)			(0xfd000000 + ((uintptr_t)unit_id) * 0x1000000)
#define MVEBU_MCI_PHY_DATA_REG_OFF			0x0
#define MVEBU_MCI_PHY_CMD_REG_OFF			0x4

#define MVEBU_MCI_MAX_UNIT_TO_INIT			3
#define MVEBU_MCI_AXI2HB_REGAXI				0x1c3000
#define MVEBU_MCI_CP_PHY_X2_BASE			MVEBU_CP1_REGS_BASE | MVEBU_MCI_AXI2HB_REGAXI

/* MCI Address */
#define MVEBU_MCI_PHY_ADDR_LSB_OFF			0
#define MVEBU_MCI_PHY_ADDR_LSB_MASK			0x3F
#define MVEBU_MCI_PHY_ADDR_MSB_OFF			6
#define MVEBU_MCI_PHY_ADDR_MSB_MASK			(0x3 << MVEBU_MCI_PHY_ADDR_MSB_OFF)

#define MVEBU_MCI_PHY_ADDR_LSB_GET(reg_ofs)	((reg_ofs) & MVEBU_MCI_PHY_ADDR_LSB_MASK)
#define MVEBU_MCI_PHY_ADDR_MSB_GET(reg_ofs)	(((reg_ofs) & MVEBU_MCI_PHY_ADDR_MSB_MASK) >> \
											MVEBU_MCI_PHY_ADDR_MSB_OFF)

/* MCI CMD register fields */
#define	MVEBU_MCI_CMD_OP_OFFS				0
#define	MVEBU_MCI_CMD_OP_MASK				0x1
#define	MVEBU_MCI_CMD_PACKET_LOCAL_OFFS		5
#define	MVEBU_MCI_CMD_DONE_OFFS				6
#define	MVEBU_MCI_CMD_DONE_MASK				(0x3 << MVEBU_MCI_CMD_DONE_OFFS)
#define	MVEBU_MCI_CMD_ADDR_LSB_OFFS			16	/* MCI LSB address bits */
#define	MVEBU_MCI_CMD_ADDR_LSB_MASK			(0x3F << MVEBU_MCI_CMD_ADDR_LSB_OFFS)
#define	MVEBU_MCI_CMD_REGION_OFFS			22	/* MCI access space */
#define	MVEBU_MCI_CMD_REGION_MASK			(0x3 << MVEBU_MCI_CMD_REGION_OFFS)
#define MVEBU_MCI_CMD_CP_PHY_X4_OFFS			9

/* MCI PHY CTRL register fields */
#define MVEBU_MCI_PHY_CTRL_REG_OFF			0x7
#define MVEBU_MCI_PHY_CTRL_ADDR_MSB_OFFS	27
#define MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK	(0x3 << 27)
#define MVEBU_MCI_PHY_IF_MODE_OFFS		25
#define MVEBU_MCI_PHY_IF_MODE_MASK		(1 << MVEBU_MCI_PHY_IF_MODE_OFFS)

/* MCI RFU PHY register fields */
#define MVEBU_MCI_RFU_REG_OFF				0x6F010C
#define	MVEBU_MCI_RFU_RESET_OFFS			0
#define	MVEBU_MCI_RFU_RESET_MASK			(1 << MVEBU_MCI_RFU_RESET_OFFS)
#define	MVEBU_MCI_RFU_TCELL_BYPASS_OFFS		10
#define	MVEBU_MCI_RFU_BASE_OFFS				16
#define	MVEBU_MCI_RFU_BASE_MASK				(0xFF << MVEBU_MCI_RFU_BASE_OFFS)

#define	MVEBU_MCI_BASE_HIGH_BYTE_OFF		24

int __attribute__((section(".data"))) mci_init_done = 0; /* inidicates whether MCI PHY access was initialized */

enum mci_access_type {
	MCI_WRITE = 0,
	MCI_READ  = 1
};

enum mci_cmd_type {
	MCI_CMD_DEFAULT = 0,
	MCI_CMD_CP1_X4  = 1
};

static unsigned long mvebu_mci_get_base(enum mci_unit phy_unit, int unit_id)
{
	if ((phy_unit == CP_PHY || phy_unit == CP_CTRL) && unit_id == 1)
		return MVEBU_MCI_CP_PHY_X2_BASE;

	return MVEBU_MCI_PHY_BASE(unit_id);
}

static int mvebu_mci_poll_read_done(int reg_ofs, enum mci_unit phy_unit, int unit_id)
{
	u32 mci_cmd_reg = 0;
	u32 timeout = 100;
	unsigned long mci_base = mvebu_mci_get_base(phy_unit, unit_id);

	do {
		mci_cmd_reg = readl(mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);
	} while (((mci_cmd_reg & MVEBU_MCI_CMD_DONE_MASK) != MVEBU_MCI_CMD_DONE_MASK) &&
			 (timeout-- > 0));

	if (timeout == 0) {
		error("Read timeout\n");
		return 1;
	}

	return 0;
}

static unsigned long mvebu_mci_get_cmd(enum mci_cmd_type cmd_type, enum mci_access_type access_type,
				       enum mci_region region, int reg_ofs)
{
	unsigned long cmd;

	/* Build mci indirect cmd */
	cmd = (((access_type << MVEBU_MCI_CMD_OP_OFFS) & MVEBU_MCI_CMD_OP_MASK) |
	      ((region << MVEBU_MCI_CMD_REGION_OFFS) & MVEBU_MCI_CMD_REGION_MASK) |
	      (MVEBU_MCI_PHY_ADDR_LSB_GET(reg_ofs) << MVEBU_MCI_CMD_ADDR_LSB_OFFS));

	/* bit 9 at CP1 MCIx4 cmd should be enabled, otherwise bit 5 */
	if (cmd_type == MCI_CMD_CP1_X4)
		return cmd | (1 << MVEBU_MCI_CMD_CP_PHY_X4_OFFS);
	else
		return cmd | (1 << MVEBU_MCI_CMD_PACKET_LOCAL_OFFS);
}


static unsigned long mvebu_mci_get_cmd_reg(enum mci_unit phy_unit, int unit_id, enum mci_access_type access_type,
					   enum mci_region region, int reg_ofs)
{
	if ((phy_unit == CP_PHY || phy_unit == CP_CTRL) && unit_id == 0)
		return mvebu_mci_get_cmd(MCI_CMD_CP1_X4, access_type, region, reg_ofs);
	else
		return mvebu_mci_get_cmd(MCI_CMD_DEFAULT, access_type, region, reg_ofs);
}

/*  MCI Indirect Access decription:
 *      the access is done using CMD and DATA register.
 *      the address and access type are set in the CMD register.
 *
 *          [0] = 0 -> write
 *                1 -> read
 *      [23:22] = 0x0 -> access MCI Controller space
 *                0x3 -> indicate MCI PHY REG address space,
 *      [21:16] = LSB of the Offset
 *
 *      (MSB are set to MCI_PHY_CTRL register (0x7) bit [28:27]
 */
static int mvebu_mci_command_set(enum mci_access_type access_type, int reg_ofs, u32 data,
				 enum mci_unit phy_unit, int unit_id, enum mci_region region)
{
	u32 mci_cmd_reg = 0;
	u32 mci_phy_ctrl_reg = 0;
	unsigned long mci_base = mvebu_mci_get_base(phy_unit, unit_id);

	/* Step 1 - set address MSB in MCI_PHY_CTRL (using RMW) */
	/* read MCI_PHY_CTRL */
	mci_cmd_reg = mvebu_mci_get_cmd_reg(phy_unit, unit_id, MCI_READ, MCI_CTRL_REGION, MVEBU_MCI_PHY_CTRL_REG_OFF);

	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);
	/* poll for read completion */
	if (mvebu_mci_poll_read_done(reg_ofs, phy_unit, unit_id))
		return 1;

	mci_phy_ctrl_reg = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

	/* update msb in MVEBU_MCI_PHY_CTRL */
	mci_phy_ctrl_reg &= ~MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK; /* clear msb data */
	mci_phy_ctrl_reg |= ((MVEBU_MCI_PHY_ADDR_MSB_GET(reg_ofs) << MVEBU_MCI_PHY_CTRL_ADDR_MSB_OFFS) &
						 MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK);/* set msb data */

	/* write MCI_PHY_CTRL */
	writel(mci_phy_ctrl_reg, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);	/* set data */
	mci_cmd_reg = (mci_cmd_reg & ~MVEBU_MCI_CMD_OP_MASK) | MCI_WRITE;	/* set write command */
	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);		/* set commnd */

	/* Step 2 - set MCI command with LSB */
	if (access_type == MCI_WRITE) {
		/* set data in mci data reg */
		writel(data, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);
	}

	mci_cmd_reg = mvebu_mci_get_cmd_reg(phy_unit, unit_id, access_type, region, reg_ofs);

	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);		/* set commnd */

	return 0;
}

static int mvebu_mci_read(int reg_ofs, u32 *val, enum mci_unit phy_unit, int unit_id, enum mci_region region)
{
	unsigned long mci_base;

	/* initialize access to mci phy (incase it wasn't already) */
	if (!mci_init_done)
		mvebu_mci_phy_init();

	mci_base = mvebu_mci_get_base(phy_unit, unit_id);

	/* set read command */
	if (mvebu_mci_command_set(MCI_READ, reg_ofs, 0 /* dummy */, phy_unit, unit_id, region)) {
		error("MCI read: set command failed\n");
		return 1;
	}

	/* poll for read completion */
	if (mvebu_mci_poll_read_done(reg_ofs, phy_unit, unit_id))
		return 1;

	*val = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

	return 0;
}

static int mvebu_mci_write(int reg_ofs, u32 data, enum mci_unit phy_unit, int unit_id, enum mci_region region)
{
	/* initialize access to mci phy (incase it wasn't already) */
	if (!mci_init_done)
		mvebu_mci_phy_init();

	/* set write command */
	if (mvebu_mci_command_set(MCI_WRITE, reg_ofs, data, phy_unit, unit_id, region)) {
		error("MCI write: set command failed\n");
		return 1;
	}

	return 0;
}

int mvebu_mci_phy_read(enum mci_region region, enum mci_unit phy_unit, int unit_id, int reg_ofs, u32 *val)
{
	switch (phy_unit) {
	case AP_PHY:
	case AP_CTRL:
	case CP_PHY:
	case CP_CTRL:
		return mvebu_mci_read(reg_ofs, val, phy_unit, unit_id, region);
	default:
		error("unit %d is not supported\n", phy_unit);
		return 1;
	}
}

int mvebu_mci_phy_write(enum mci_region region, enum mci_unit phy_unit, int unit_id, int reg_ofs, u32 val)
{
	switch (phy_unit) {
	case AP_PHY:
	case AP_CTRL:
	case CP_PHY:
	case CP_CTRL:
		return mvebu_mci_write(reg_ofs, val, phy_unit, unit_id, region);
	default:
		error("unit %d is not supported\n", phy_unit);
		return 1;
	}
}

int mvebu_mci_phy_init(void)
{
	u8 *regs_base = (u8 *)MVEBU_REGS_BASE;
	u32 reg_data, i;
	u32 mci_base_high_byte;
	u32 mci_cmd_reg = 0, val;
	unsigned long mci_base;
	const void *blob = gd->fdt_blob;

	for (i = 0; i < MVEBU_MCI_MAX_UNIT_TO_INIT; ++i) {
		/* Make sure board is supported (currently only Armada-80x0-DB is supported) */
		if (i == MVEBU_MCI_MAX_UNIT_TO_INIT - 1) {
			if (fdt_node_check_compatible(blob, 0, "marvell,armada-80x0") == 0)
				mci_base = MVEBU_MCI_CP_PHY_X2_BASE;
			else
				break;
		} else {
			mci_base = MVEBU_MCI_PHY_BASE(i);
		}
		mci_base_high_byte = mci_base >> MVEBU_MCI_BASE_HIGH_BYTE_OFF;

		reg_data = readl(regs_base + MVEBU_MCI_RFU_REG_OFF);

		/* set configuration data */
		reg_data &= ~MVEBU_MCI_RFU_RESET_MASK; /* un-reset MCI PHY */
		reg_data |= (1 << MVEBU_MCI_RFU_TCELL_BYPASS_OFFS); /* enable TCELL bypass since
									this bridge is not functional */
		reg_data = (mci_base_high_byte << MVEBU_MCI_RFU_BASE_OFFS) |
					(reg_data & ~MVEBU_MCI_RFU_BASE_MASK); /* set MCI base address */

		/* Configure MCI MAC base address RFU base and Un-reset MCI PHY */
		writel(reg_data, regs_base + MVEBU_MCI_RFU_REG_OFF);

		/* Set IF mode in phy control register */
		mci_cmd_reg = mvebu_mci_get_cmd(MCI_CMD_DEFAULT, MCI_READ, MCI_CTRL_REGION,
						MVEBU_MCI_PHY_CTRL_REG_OFF);
		writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);

		val = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);
		val = val | MVEBU_MCI_PHY_IF_MODE_MASK;
		writel(val, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

		mci_cmd_reg = mvebu_mci_get_cmd(MCI_CMD_DEFAULT, MCI_WRITE, MCI_CTRL_REGION,
						MVEBU_MCI_PHY_CTRL_REG_OFF);
		writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);
	}

	mci_init_done = 1; /* inidcate that MCI PHY access was initialized */

	return 0;
}
