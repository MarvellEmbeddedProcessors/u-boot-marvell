/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <dm.h>
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mvebu_mci.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/soc.h>

/* MCI registers */
#define MVEBU_MCI_PHY_DATA_REG_OFF	0x0
#define MVEBU_MCI_PHY_CMD_REG_OFF	0x4
#define MVEBU_MCI_PHY_BASE_OFF		0X8
#define MVEBU_MCI_MAX_UNIT_TO_INIT	3

/* MCI Address */
#define MVEBU_MCI_PHY_ADDR_LSB_OFF	0
#define MVEBU_MCI_PHY_ADDR_LSB_MASK	0x3F
#define MVEBU_MCI_PHY_ADDR_MSB_OFF	6
#define MVEBU_MCI_PHY_ADDR_MSB_MASK	(0x3 << MVEBU_MCI_PHY_ADDR_MSB_OFF)

#define MVEBU_MCI_PHY_ADDR_LSB_GET(reg_ofs)	((reg_ofs) & \
						 MVEBU_MCI_PHY_ADDR_LSB_MASK)
#define MVEBU_MCI_PHY_ADDR_MSB_GET(reg_ofs)	(((reg_ofs) & \
						 MVEBU_MCI_PHY_ADDR_MSB_MASK) \
						 >> MVEBU_MCI_PHY_ADDR_MSB_OFF)

/* MCI CMD register fields */
#define	MVEBU_MCI_CMD_OP_OFFS		0
#define	MVEBU_MCI_CMD_OP_MASK		0x1
#define	MVEBU_MCI_CMD_PACKET_LOCAL_OFFS	5
#define	MVEBU_MCI_CMD_DONE_OFFS		6
#define	MVEBU_MCI_CMD_DONE_MASK		(0x3 << MVEBU_MCI_CMD_DONE_OFFS)
#define	MVEBU_MCI_CMD_ADDR_LSB_OFFS	16	/* MCI LSB address bits */
#define	MVEBU_MCI_CMD_ADDR_LSB_MASK	(0x3F << MVEBU_MCI_CMD_ADDR_LSB_OFFS)
#define	MVEBU_MCI_CMD_REGION_OFFS	22	/* MCI access space */
#define	MVEBU_MCI_CMD_REGION_MASK	(0x3 << MVEBU_MCI_CMD_REGION_OFFS)
#define MVEBU_MCI_CMD_CP_PHY_X4_OFFS	9

/* MCI PHY CTRL register fields */
#define MVEBU_MCI_PHY_CTRL_REG_OFF	0x7
#define MVEBU_MCI_PHY_CTRL_ADDR_MSB_OFFS 27
#define MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK (0x3 << 27)
#define MVEBU_MCI_PHY_IF_MODE_OFFS	25
#define MVEBU_MCI_PHY_IF_MODE_MASK	(1 << MVEBU_MCI_PHY_IF_MODE_OFFS)

/* MCI RFU PHY register fields */
#define	MVEBU_MCI_RFU_RESET_OFFS	0
#define	MVEBU_MCI_RFU_RESET_MASK	(1 << MVEBU_MCI_RFU_RESET_OFFS)
#define	MVEBU_MCI_RFU_TCELL_BYPASS_OFFS	10
#define	MVEBU_MCI_RFU_BASE_OFFS		16
#define	MVEBU_MCI_RFU_BASE_MASK		(0xFF << MVEBU_MCI_RFU_BASE_OFFS)

#define	MVEBU_MCI_BASE_HIGH_BYTE_OFF	24

enum mci_access_type {
	MCI_WRITE = 0,
	MCI_READ  = 1
};

enum mci_cmd_type {
	MCI_CMD_DEFAULT = 0,
	MCI_CMD_CP1_X4  = 1
};

/* describe which phy unit supported */
enum mci_phy_unit {
	NONE,
	AP,
	CP,
	/* This value indicate that the current MCI unit can read
	 * both sides of the lane (cp and ap)
	 */
	AP_AND_CP
};

struct probe_mci_info {
	unsigned long mci_base;
	unsigned int unit_id;
	enum mci_phy_unit mci_phy_unit;
};

static struct probe_mci_info *mci_probed_info[MVEBU_MCI_MAX_UNIT_TO_INIT];

static int check_phy_unit_type(enum mci_unit phy_unit,
			       enum mci_phy_unit mci_phy_unit)
{
	if (mci_phy_unit == AP_AND_CP && phy_unit != MCI_MAX)
		return -EINVAL;

	if (mci_phy_unit == CP && (phy_unit == CP_CTRL || phy_unit == CP_PHY))
		return -EINVAL;

	if (mci_phy_unit == AP && (phy_unit == AP_CTRL || phy_unit == AP_PHY))
		return -EINVAL;

	return 0;
}

static unsigned long mvebu_mci_get_base(enum mci_unit phy_unit, int unit_id)
{
	int i;
	for (i = 0; i < MVEBU_MCI_MAX_UNIT_TO_INIT; i++) {
		if (mci_probed_info[i] &&
		    mci_probed_info[i]->unit_id == unit_id &&
		    check_phy_unit_type(phy_unit,
					mci_probed_info[i]->mci_phy_unit))
			return mci_probed_info[i]->mci_base;
	}
	return 0;
}

static int mvebu_mci_poll_read_done(int reg_ofs, enum mci_unit phy_unit,
				    int unit_id)
{
	u32 mci_cmd_reg = 0;
	u32 timeout = 100;
	unsigned long mci_base = mvebu_mci_get_base(phy_unit, unit_id);
	if (mci_base == 0)
		return -EFAULT;

	do {
		mci_cmd_reg = readl(mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);
	} while (((mci_cmd_reg & MVEBU_MCI_CMD_DONE_MASK) !=
		   MVEBU_MCI_CMD_DONE_MASK) && (timeout-- > 0));

	if (timeout == 0) {
		error("Read timeout\n");
		return -ETIME;
	}

	return 0;
}

static unsigned long mvebu_mci_get_cmd(enum mci_cmd_type cmd_type,
				       enum mci_access_type access_type,
				       enum mci_region region, int reg_ofs)
{
	unsigned long cmd;

	/* Build mci indirect cmd */
	cmd = (((access_type << MVEBU_MCI_CMD_OP_OFFS) &
	      MVEBU_MCI_CMD_OP_MASK) | ((region << MVEBU_MCI_CMD_REGION_OFFS) &
	      MVEBU_MCI_CMD_REGION_MASK) |
	      (MVEBU_MCI_PHY_ADDR_LSB_GET(reg_ofs) <<
	      MVEBU_MCI_CMD_ADDR_LSB_OFFS));

	/* bit 9 at CP1 MCIx4 cmd should be enabled, otherwise bit 5 */
	if (cmd_type == MCI_CMD_CP1_X4)
		return cmd | (1 << MVEBU_MCI_CMD_CP_PHY_X4_OFFS);
	else
		return cmd | (1 << MVEBU_MCI_CMD_PACKET_LOCAL_OFFS);
}


static unsigned long mvebu_mci_get_cmd_reg(enum mci_unit phy_unit, int unit_id,
					   enum mci_access_type access_type,
					   enum mci_region region, int reg_ofs)
{
	if ((phy_unit == CP_PHY || phy_unit == CP_CTRL) && unit_id == 0)
		return mvebu_mci_get_cmd(MCI_CMD_CP1_X4, access_type, region,
					 reg_ofs);
	else
		return mvebu_mci_get_cmd(MCI_CMD_DEFAULT, access_type, region,
					 reg_ofs);
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
static int mvebu_mci_command_set(enum mci_access_type access_type, int reg_ofs,
				 u32 data, enum mci_unit phy_unit, int unit_id,
				 enum mci_region region)
{
	u32 mci_cmd_reg = 0;
	u32 mci_phy_ctrl_reg = 0;
	int ret;
	unsigned long mci_base = mvebu_mci_get_base(phy_unit, unit_id);

	if (mci_base == 0)
		return -EFAULT;

	/* Step 1 - set address MSB in MCI_PHY_CTRL (using RMW) */
	/* read MCI_PHY_CTRL */
	mci_cmd_reg = mvebu_mci_get_cmd_reg(phy_unit, unit_id, MCI_READ,
					    MCI_CTRL_REGION,
					    MVEBU_MCI_PHY_CTRL_REG_OFF);

	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);
	/* poll for read completion */
	ret = mvebu_mci_poll_read_done(reg_ofs, phy_unit, unit_id);
	if (ret)
		return ret;

	mci_phy_ctrl_reg = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

	/* update msb in MVEBU_MCI_PHY_CTRL */
	/* clear msb data */
	mci_phy_ctrl_reg &= ~MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK;
	/* set msb data */
	mci_phy_ctrl_reg |= ((MVEBU_MCI_PHY_ADDR_MSB_GET(reg_ofs) <<
			      MVEBU_MCI_PHY_CTRL_ADDR_MSB_OFFS) &
			      MVEBU_MCI_PHY_CTRL_ADDR_MSB_MASK);

	/* write MCI_PHY_CTRL */
	/* set data */
	writel(mci_phy_ctrl_reg, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);
	/* set write command */
	mci_cmd_reg = (mci_cmd_reg & ~MVEBU_MCI_CMD_OP_MASK) | MCI_WRITE;
	/* set commnd */
	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);

	/* Step 2 - set MCI command with LSB */
	if (access_type == MCI_WRITE) {
		/* set data in mci data reg */
		writel(data, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);
	}

	mci_cmd_reg = mvebu_mci_get_cmd_reg(phy_unit, unit_id, access_type,
					    region, reg_ofs);
	/* set commnd */
	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);

	return 0;
}

int mvebu_mci_phy_read(enum mci_region region, enum mci_unit phy_unit,
		       int unit_id, int reg_ofs, u32 *val)
{
	unsigned long mci_base = mvebu_mci_get_base(phy_unit, unit_id);
	int ret;

	if (mci_base == 0)
		return -EFAULT;

	/* set read command */
	ret = mvebu_mci_command_set(MCI_READ, reg_ofs, 0 /* dummy */, phy_unit,
				  unit_id, region);
	if (ret) {
		error("MCI read: set command failed\n");
		return ret;
	}

	/* poll for read completion */
	ret = mvebu_mci_poll_read_done(reg_ofs, phy_unit, unit_id);
	if (ret)
		return ret;

	*val = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

	return 0;
}

int mvebu_mci_phy_write(enum mci_region region, enum mci_unit phy_unit,
			int unit_id, int reg_ofs, u32 val)
{
	int ret;

	/* set write command */
	ret = mvebu_mci_command_set(MCI_WRITE, reg_ofs, val, phy_unit,
				  unit_id, region);
	if (ret) {
		error("MCI write: set command failed\n");
		return ret;
	}

	return 0;
}

int mvebu_mci_phy_init(unsigned long mci_base)
{
	u32 mci_cmd_reg = 0, val;

	/* Set IF mode in phy control register */
	mci_cmd_reg = mvebu_mci_get_cmd(MCI_CMD_DEFAULT,
					MCI_READ, MCI_CTRL_REGION,
					MVEBU_MCI_PHY_CTRL_REG_OFF);
	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);

	val = readl(mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);
	val = val | MVEBU_MCI_PHY_IF_MODE_MASK;
	writel(val, mci_base | MVEBU_MCI_PHY_DATA_REG_OFF);

	mci_cmd_reg = mvebu_mci_get_cmd(MCI_CMD_DEFAULT, MCI_WRITE,
					MCI_CTRL_REGION,
					MVEBU_MCI_PHY_CTRL_REG_OFF);
	writel(mci_cmd_reg, mci_base | MVEBU_MCI_PHY_CMD_REG_OFF);

	return 0;
}

static int mci_probe(struct udevice *dev)
{
	void *blob = (void *)gd->fdt_blob;
	int node = dev_of_offset(dev);
	unsigned long mci_base;
	enum mci_phy_unit mci_phy_unit;
	int unit_id;
	struct probe_mci_info *mci_probe_info;
	int i;

	void __iomem *mci_base_reg = (void *)dev_get_addr_index(dev, 0);
	if (IS_ERR(mci_base_reg))
		return PTR_ERR(mci_base_reg);

	if (fdtdec_get_bool(blob, node, "indirect_base_reg"))
		mci_base = readl(mci_base_reg) << MVEBU_MCI_PHY_BASE_OFF;
	else
		mci_base = (unsigned long)mci_base_reg;

	if (fdtdec_get_bool(blob, node, "phy_unit_ap"))
		mci_phy_unit = AP;

	if (fdtdec_get_bool(blob, node, "phy_unit_cp"))
		mci_phy_unit = mci_phy_unit == NONE ? CP : AP_AND_CP;

	if (mci_phy_unit == NONE)
		return -EINVAL;

	unit_id = fdtdec_get_int(blob, node, "unit_id", -1);
	if (unit_id == -1)
		return -EINVAL;

	mci_probe_info = malloc(sizeof(struct probe_mci_info));
	if (!mci_probe_info) {
		printf("%s: No memory for mci_probe\n", __func__);
		return -ENOMEM;
	}
	mci_probe_info->mci_base = mci_base;
	mci_probe_info->unit_id = unit_id;
	mci_probe_info->mci_phy_unit = mci_phy_unit;

	for (i = 0; i < MVEBU_MCI_MAX_UNIT_TO_INIT; i++) {
		if (mci_probed_info[i])
			continue;

		mci_probed_info[i] = mci_probe_info;
		break;
	}

	mvebu_mci_phy_init(mci_base);
	return 0;
}

static const struct udevice_id mci_ids[] = {
	{ .compatible = "marvell,mvebu-mci" },
	{ }
};

U_BOOT_DRIVER(mvebu_mci) = {
	.name	= "mvebu-mci",
	.id	= UCLASS_MISC,
	.of_match = mci_ids,
	.probe	= mci_probe,
};
