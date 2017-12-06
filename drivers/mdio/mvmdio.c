/*
 * Copyright (C) 2018 Marvell International Ltd.
 * Written by Ken Ma <make@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <miiphy.h>
#include <phy.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#define MVMDIO_SMI_DATA_SHIFT		0
#define MVMDIO_SMI_PHY_ADDR_SHIFT	16
#define MVMDIO_SMI_PHY_REG_SHIFT	21
#define MVMDIO_SMI_READ_OPERATION	BIT(26)
#define MVMDIO_SMI_WRITE_OPERATION	0
#define MVMDIO_SMI_READ_VALID		BIT(27)
#define MVMDIO_SMI_BUSY			BIT(28)

#define MVMDIO_XSMI_MGNT_REG		0x0
#define MVMDIO_XSMI_PHYADDR_SHIFT	16
#define MVMDIO_XSMI_DEVADDR_SHIFT	21
#define MVMDIO_XSMI_WRITE_OPERATION	(0x5 << 26)
#define MVMDIO_XSMI_READ_OPERATION	(0x7 << 26)
#define MVMDIO_XSMI_READ_VALID		BIT(29)
#define MVMDIO_XSMI_BUSY		BIT(30)
#define MVMDIO_XSMI_ADDR_REG		0x8

#define MVMDIO_TIMEOUT			10000

struct mvmdio_priv {
	void *mdio_base;
};

enum mvmdio_bus_type {
	BUS_TYPE_SMI,
	BUS_TYPE_XSMI
};

/* Wait for the SMI unit to be ready for another operation */
static int mvmdio_smi_wait_ready(struct mii_dev *bus)
{
	u32 timeout = MVMDIO_TIMEOUT;
	struct mvmdio_priv *priv = bus->priv;
	u32 smi_reg;

	/* Wait till the SMI is not busy */
	do {
		/* Read smi register */
		smi_reg = readl(priv->mdio_base);
		if (timeout-- == 0) {
			debug("Error: SMI busy timeout\n");
			return -ETIME;
		}
	} while (smi_reg & MVMDIO_SMI_BUSY);

	return 0;
}

static int mvmdio_smi_read(struct mii_dev *bus, int addr,
			       int devad, int reg)
{
	struct mvmdio_priv *priv = bus->priv;
	u32 val;
	int ret;

	if (devad != MDIO_DEVAD_NONE)
		return -EOPNOTSUPP;

	ret = mvmdio_smi_wait_ready(bus);
	if (ret < 0)
		return ret;

	writel(((addr << MVMDIO_SMI_PHY_ADDR_SHIFT) |
		(reg << MVMDIO_SMI_PHY_REG_SHIFT)  |
		MVMDIO_SMI_READ_OPERATION),
	       priv->mdio_base);

	ret = mvmdio_smi_wait_ready(bus);
	if (ret < 0)
		return ret;

	val = readl(priv->mdio_base);
	if (!(val & MVMDIO_SMI_READ_VALID)) {
		dev_err(bus->parent, "SMI bus read not valid\n");
		return -ENODEV;
	}

	return val & GENMASK(15, 0);
}

static int mvmdio_smi_write(struct mii_dev *bus, int addr, int devad,
				int reg,
				u16 value)
{
	struct mvmdio_priv *priv = bus->priv;
	int ret;

	if (devad != MDIO_DEVAD_NONE)
		return -EOPNOTSUPP;

	ret = mvmdio_smi_wait_ready(bus);
	if (ret < 0)
		return ret;

	writel(((addr << MVMDIO_SMI_PHY_ADDR_SHIFT) |
		(reg << MVMDIO_SMI_PHY_REG_SHIFT)  |
		MVMDIO_SMI_WRITE_OPERATION            |
		(value << MVMDIO_SMI_DATA_SHIFT)),
	       priv->mdio_base);

	return 0;
}

static int mvmdio_xsmi_wait_ready(struct mii_dev *bus)
{
	u32 timeout = MVMDIO_TIMEOUT;
	struct mvmdio_priv *priv = bus->priv;
	u32 xsmi_reg;

	/* Wait till the xSMI is not busy */
	do {
		/* Read xSMI register */
		xsmi_reg = readl(priv->mdio_base);
		if (timeout-- == 0) {
			debug("xSMI busy time-out\n");
			return -ETIME;
		}
	} while (xsmi_reg & MVMDIO_XSMI_BUSY);

	return 0;
}

static int mvmdio_xsmi_read(struct mii_dev *bus, int addr,
				int devad, int reg)
{
	struct mvmdio_priv *priv = bus->priv;
	int ret;

	if (devad == MDIO_DEVAD_NONE)
		return -EOPNOTSUPP;

	ret = mvmdio_xsmi_wait_ready(bus);
	if (ret < 0)
		return ret;

	writel(reg & GENMASK(15, 0), priv->mdio_base + MVMDIO_XSMI_ADDR_REG);
	writel(((addr << MVMDIO_XSMI_PHYADDR_SHIFT) |
		(devad << MVMDIO_XSMI_DEVADDR_SHIFT) |
		MVMDIO_XSMI_READ_OPERATION),
	       priv->mdio_base + MVMDIO_XSMI_MGNT_REG);

	ret = mvmdio_xsmi_wait_ready(bus);
	if (ret < 0)
		return ret;

	if (!(readl(priv->mdio_base + MVMDIO_XSMI_MGNT_REG) &
	      MVMDIO_XSMI_READ_VALID)) {
		dev_err(bus->parent, "XSMI bus read not valid\n");
		return -ENODEV;
	}

	return readl(priv->mdio_base + MVMDIO_XSMI_MGNT_REG) & GENMASK(15, 0);
}

static int mvmdio_xsmi_write(struct mii_dev *bus, int addr, int devad,
				 int reg, u16 value)
{
	struct mvmdio_priv *priv = bus->priv;
	int ret;

	if (devad == MDIO_DEVAD_NONE)
		return -EOPNOTSUPP;

	ret = mvmdio_xsmi_wait_ready(bus);
	if (ret < 0)
		return ret;

	writel(reg & GENMASK(15, 0), priv->mdio_base + MVMDIO_XSMI_ADDR_REG);
	writel(((addr << MVMDIO_XSMI_PHYADDR_SHIFT) |
		(devad << MVMDIO_XSMI_DEVADDR_SHIFT) |
		MVMDIO_XSMI_WRITE_OPERATION | value),
	       priv->mdio_base + MVMDIO_XSMI_MGNT_REG);

	return 0;
}

static int mvmdio_probe(struct udevice *dev)
{
	struct mii_dev **pbus = dev_get_uclass_platdata(dev);
	struct mii_dev *bus = *pbus;
	struct mvmdio_priv *priv;
	enum mvmdio_bus_type type;

	priv = dev_get_priv(dev);
	priv->mdio_base = (void *)dev_get_addr(dev);
	bus->priv = priv;

	type = (enum mvmdio_bus_type)dev_get_driver_data(dev);
	switch (type) {
	case BUS_TYPE_SMI:
		bus->read = mvmdio_smi_read;
		bus->write = mvmdio_smi_write;
		if (!bus->name)
			snprintf(bus->name, MDIO_NAME_LEN,
				 "orion-mdio.%p", priv->mdio_base);
		break;
	case BUS_TYPE_XSMI:
		bus->read = mvmdio_xsmi_read;
		bus->write = mvmdio_xsmi_write;
		if (!bus->name)
			snprintf(bus->name, MDIO_NAME_LEN,
				 "xmdio.%p", priv->mdio_base);
		break;
	}

	return 0;
}

static const struct udevice_id mvmdio_ids[] = {
	{ .compatible = "marvell,orion-mdio", .data = BUS_TYPE_SMI },
	{ .compatible = "marvell,xmdio", .data = BUS_TYPE_XSMI },
	{ }
};

U_BOOT_DRIVER(mvmdio) = {
	.name			= "mvmdio",
	.id			= UCLASS_MDIO,
	.of_match		= mvmdio_ids,
	.probe			= mvmdio_probe,
	.priv_auto_alloc_size	= sizeof(struct mvmdio_priv),
};

