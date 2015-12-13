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
#include <fdtdec.h>
#include <miiphy.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-mvebu/fdt.h>

#define MVEBU_SMI_TIMEOUT			10000

/* SMI register fields */
#define	MVEBU_SMI_DATA_OFFS			0	/* Data */
#define	MVEBU_SMI_DATA_MASK			(0xffff << MVEBU_SMI_DATA_OFFS)
#define	MVEBU_SMI_DEV_ADDR_OFFS			16	/* PHY device address */
#define	MVEBU_SMI_REG_ADDR_OFFS			21	/* PHY device reg addr*/
#define	MVEBU_SMI_OPCODE_OFFS			26	/* Write/Read opcode */
#define	MVEBU_SMI_OPCODE_READ			(1 << MVEBU_SMI_OPCODE_OFFS)
#define	MVEBU_SMI_READ_VALID			(1 << 27)	/* Read Valid */
#define	MVEBU_SMI_BUSY				(1 << 28)	/* Busy */

#define	MVEBU_PHY_REG_MASK			0x1f
#define	MVEBU_PHY_ADDR_MASK			0x1f

/* XSMI management register fields */
#define MVEBU_XSMI_DATA_OFFS			0       /* Data */
#define MVEBU_XSMI_DATA_MASK			(0xffff << MVEBU_XSMI_DATA_OFFS)

#define MVEBU_XSMI_PHY_ADDR_OFFS		16      /* PHY device address */
#define MVEBU_XSMI_PHY_ADDR_MASK		(0x1f << MVEBU_XSMI_PHY_ADDR_OFFS)

#define MVEBU_XSMI_DEV_ADDR_OFFS		21      /* PHY device register address */
#define MVEBU_XSMI_DEV_ADDR_MASK		(0x1f << MVEBU_XSMI_DEV_ADDR_OFFS)

#define MVEBU_XSMI_OPCODE_OFFS			26      /* opcode options*/
#define MVEBU_XSMI_OPCODE_MASK			(7 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_WRITE			(1 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_INC_READ		(2 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_READ			(3 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_ADDR_WRITE		(5 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_ADDR_INC_READ		(6 << MVEBU_XSMI_OPCODE_OFFS)
#define MVEBU_XSMI_OPCODE_ADDR_READ		(7 << MVEBU_XSMI_OPCODE_OFFS)

#define MVEBU_XSMI_READ_VALID			(1 << 29)
#define MVEBU_XSMI_BUSY				(1 << 30)

/* XSMI address register */
#define MVEBU_XSMI_REG_ADDR			0x8     /* Offset of the register address */
#define MVEBU_XSMI_REG_ADDR_OFFS		0
#define MVEBU_XSMI_REG_ADDR_MASK		(0xffff << MVEBU_XSMI_REG_ADDR_OFFS)

/* XSMI configuration register */
#define MVEBU_XSMI_CFG_ADDR			0xC
#define MVEBU_XSMI_CFG_DIV_OFFS			0
#define MVEBU_XSMI_CFG_DIV_MASK			(0x3 << MVEBU_XSMI_CFG_DIV_OFFS)

struct mvebu_mdio_base {
	void __iomem *xsmi_base;
	void __iomem *smi_base;
};

struct mvebu_mdio_base mdio_base_addr;

/* SMI functions */
static int mvebu_smi_check_param(int phy_adr, int reg_ofs)
{
	if (phy_adr > MVEBU_PHY_ADDR_MASK) {
		error("Invalid PHY address %d\n", phy_adr);
		return -EFAULT;
	}

	if (reg_ofs > MVEBU_PHY_REG_MASK) {
		error("Invalid register offset %d\n", reg_ofs);
		return -EFAULT;
	}
	return 0;
}

static int mvebu_smi_wait_ready(void *base)
{
	u32 timeout = MVEBU_SMI_TIMEOUT;
	u32 smi_reg;

	/* wait till the SMI is not busy */
	do {
		/* read smi register */
		smi_reg = readl(base);
		if (timeout-- == 0) {
			error("SMI busy timeout\n");
			return -EFAULT;
		}
	} while (smi_reg & MVEBU_SMI_BUSY);

	return 0;
}

static int mvebu_smi_read(struct mii_dev *bus, int phy_adr, int reg_ofs)
{
	struct mvebu_mdio_base *mdio_base = bus->priv;
	void __iomem *base = mdio_base->smi_base;
	u32 smi_reg;
	u32 timeout;

	if (mdio_base->smi_base == NULL) {
		error("SMI base address is NULL\n");
		return -EFAULT;
	}

	/* wait till the SMI is not busy */
	if (mvebu_smi_wait_ready(base) < 0)
		return -EFAULT;

	/* fill the phy address and regiser offset and read opcode */
	smi_reg = (phy_adr << MVEBU_SMI_DEV_ADDR_OFFS)
		| (reg_ofs << MVEBU_SMI_REG_ADDR_OFFS)
		| MVEBU_SMI_OPCODE_READ;

	/* write the smi register */
	writel(smi_reg, base);

	/* wait till read value is ready */
	timeout = MVEBU_SMI_TIMEOUT;
	do {
		/* read smi register */
		smi_reg = readl(base);
		if (timeout-- == 0) {
			error("SMI read ready time-out\n");
			return -EFAULT;
		}
	} while (!(smi_reg & MVEBU_SMI_READ_VALID));

	return readl(base) & MVEBU_SMI_DATA_MASK;
}

static int mvebu_smi_write(struct mii_dev *bus, int phy_adr, int reg_ofs, u16 data)
{
	struct mvebu_mdio_base *mdio_base = bus->priv;
	void __iomem *base = mdio_base->smi_base;
	u32 smi_reg;

	if (mdio_base->smi_base == NULL) {
		error("SMI base address is NULL\n");
		return -EFAULT;
	}

	/* wait till the SMI is not busy */
	if (mvebu_smi_wait_ready(base) < 0)
		return -EFAULT;

	/* fill the phy addr and reg offset and write opcode and data */
	smi_reg = (data << MVEBU_SMI_DATA_OFFS);
	smi_reg |= (phy_adr << MVEBU_SMI_DEV_ADDR_OFFS)
			| (reg_ofs << MVEBU_SMI_REG_ADDR_OFFS);
	smi_reg &= ~MVEBU_SMI_OPCODE_READ;

	/* write the smi register */
	writel(smi_reg, base);

	/* make sure that the write transaction  is over */
	if (mvebu_smi_wait_ready(base) < 0)
		return -EFAULT;

	return 0;
}

/* xSMI functions */
static int mvebu_xsmi_wait_ready(void *base)
{
	u32 timeout = MVEBU_SMI_TIMEOUT;
	u32 xsmi_reg;

	/* wait till the xSMI is not busy */
	do {
		/* read smi register */
		xsmi_reg = readl(base);
		if (timeout-- == 0) {
			error("SMI busy time-out\n");
			return -EFAULT;
		}
	} while (xsmi_reg & MVEBU_XSMI_BUSY);

	return 0;
}

static int mvebu_xsmi_check_param(int phy_adr, int dev_adr, int reg_ofs)
{
	if (phy_adr > (MVEBU_XSMI_PHY_ADDR_MASK >> MVEBU_XSMI_PHY_ADDR_OFFS)) {
		error("Invalid PHY address %d\n", phy_adr);
		return -EFAULT;
	}
	if (dev_adr > (MVEBU_XSMI_DEV_ADDR_MASK >> MVEBU_XSMI_DEV_ADDR_OFFS)) {
		error("Invalid Device address %d\n", dev_adr);
		return -EFAULT;
	}
	if (reg_ofs > (MVEBU_XSMI_REG_ADDR_MASK >> MVEBU_XSMI_REG_ADDR_OFFS)) {
		error("Invalid Reg offset %d\n", reg_ofs);
		return -EFAULT;
	}
	return 0;
}

static int mvebu_xsmi_read(struct mii_dev *bus, int phy_adr, int dev_adr, int reg_adr)
{
	struct mvebu_mdio_base *mdio_base = bus->priv;
	void __iomem *base = mdio_base->xsmi_base;
	u32 xsmi_reg;
	u32 timeout;

	if (mdio_base->xsmi_base == NULL) {
		error("XSMI base address is NULL\n");
		return -EFAULT;
	}

	/* wait till the SMI is not busy */
	if (mvebu_xsmi_wait_ready(base) < 0)
		return -EFAULT;

	/* fill the register offset */
	xsmi_reg = (reg_adr << MVEBU_XSMI_REG_ADDR_OFFS);
	writel(xsmi_reg, base + MVEBU_XSMI_REG_ADDR);

	/* fill the phy address and device address and read opcode */
	xsmi_reg = (phy_adr << MVEBU_XSMI_PHY_ADDR_OFFS)
		| (dev_adr << MVEBU_XSMI_DEV_ADDR_OFFS)
		| MVEBU_XSMI_OPCODE_ADDR_READ;

	/* write the smi register */
	writel(xsmi_reg, base);

	/*wait till read value is ready */
	timeout = MVEBU_SMI_TIMEOUT;
	do {
		/* read smi register */
		xsmi_reg = readl(base);
		if (timeout-- == 0) {
			error("SMI read ready time-out\n");
			return -EFAULT;
		}
	} while (!(xsmi_reg & MVEBU_XSMI_READ_VALID));

	return readl(base) & MVEBU_XSMI_DATA_MASK;

}

static int mvebu_xsmi_write(struct mii_dev *bus, int phy_adr, int dev_adr, int reg_adr, u16 data)
{
	struct mvebu_mdio_base *mdio_base = bus->priv;
	void __iomem *base = mdio_base->xsmi_base;
	u32 xsmi_reg;

	if (mdio_base->xsmi_base == NULL) {
		error("XSMI base address is NULL\n");
		return -EFAULT;
	}

	/* wait till the xSMI is not busy */
	if (mvebu_xsmi_wait_ready(base) < 0)
		return -EFAULT;

	/* fill the register offset */
	xsmi_reg = (reg_adr << MVEBU_XSMI_REG_ADDR_OFFS);
	writel(xsmi_reg, base + MVEBU_XSMI_REG_ADDR);

	/* fill the phy address and device address and write opcode */
	xsmi_reg = (data << MVEBU_XSMI_DATA_OFFS);
	xsmi_reg |= (phy_adr << MVEBU_XSMI_PHY_ADDR_OFFS) | (dev_adr << MVEBU_XSMI_DEV_ADDR_OFFS);
	xsmi_reg &= ~MVEBU_XSMI_OPCODE_ADDR_WRITE;

	/* write the xsmi register */
	writel(xsmi_reg, base);

	/* wait till the SMI is not busy */
	if (mvebu_xsmi_wait_ready(base) < 0)
		return -EFAULT;

	return 0;
}

int mvebu_mdio_read(struct mii_dev *bus, int phy_adr, int dev_adr, int reg_ofs)
{
	if (dev_adr == -1) {
		/* check parameters */
		if (mvebu_smi_check_param(phy_adr, reg_ofs) < 0)
			return -EFAULT;
		return mvebu_smi_read(bus, phy_adr, reg_ofs);
	} else {
		/* check parameters */
		if (mvebu_xsmi_check_param(phy_adr, dev_adr, reg_ofs) < 0)
			return -EFAULT;
		return mvebu_xsmi_read(bus, phy_adr, dev_adr, reg_ofs);
	}
}

int mvebu_mdio_write(struct mii_dev *bus, int phy_adr, int dev_adr, int reg_ofs, u16 val)
{
	if (dev_adr == -1) {
		/* check parameters */
		if (mvebu_smi_check_param(phy_adr, reg_ofs) < 0)
			return -EFAULT;
		return mvebu_smi_write(bus, phy_adr, reg_ofs, val);
	} else {
		/* check parameters */
		if (mvebu_xsmi_check_param(phy_adr, dev_adr, reg_ofs) < 0)
			return -EFAULT;
		return mvebu_xsmi_write(bus, phy_adr, dev_adr, reg_ofs, val);
	}
}

int mvebu_mdio_initialize(const void *blob)
{
	struct mii_dev *bus;
	u32 node;
	struct mvebu_mdio_base *mdio_base = &mdio_base_addr;

	/* Get the MDIO node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_MDIO));
	if (node < 0) {
		error("No MDIO node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the address MDIO */
	mdio_base->xsmi_base = (void *)fdt_get_regs_offs(blob, node, "reg_xsmi");
	if (mdio_base->xsmi_base == NULL)
		debug("No XSMI base address found\n");

	mdio_base->smi_base = (void *)fdt_get_regs_offs(blob, node, "reg_smi");
	if (mdio_base->smi_base == NULL)
		debug("No SMI base address found\n");

	bus = mdio_alloc();
	if (!bus) {
		error("Failed to allocate MVEBU MDIO bus");
		return -1;
	}

	bus->read = mvebu_mdio_read;
	bus->write = mvebu_mdio_write;
	bus->reset = NULL;
	/* use given name or generate its own unique name */
	snprintf(bus->name, MDIO_NAME_LEN, "mvebu_mdio");
	bus->priv = mdio_base;
	if (mdio_register(bus) < 0) {
		error("failed to register MDIO bus\n");
		return -1;
	}

	return 0;
}
