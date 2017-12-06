 /*
  * Copyright (C) 2018 Marvell International Ltd.
  * Written by Ken Ma <make@marvell.com>
  *
  * SPDX-License-Identifier:	 GPL-2.0
  * https://spdx.org/licenses
  */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass.h>
#include <dm/uclass-internal.h>
#include <miiphy.h>
#include <mdio.h>

DECLARE_GLOBAL_DATA_PTR;

int mdio_mii_bus_get(struct udevice *mdio_dev, struct mii_dev **bus)
{
	*bus = *(struct mii_dev **)dev_get_uclass_platdata(mdio_dev);

	return 0;
}

int mdio_device_get_from_phy(int phy_node, struct udevice **devp)
{
	int mdio_off;

	mdio_off = fdt_parent_offset(gd->fdt_blob, phy_node);
	return uclass_get_device_by_of_offset(UCLASS_MDIO, mdio_off,
					      devp);
}

int mdio_mii_bus_get_from_phy(int phy_node, struct mii_dev **bus)
{
	struct udevice *mdio_dev;
	int ret;

	ret = mdio_device_get_from_phy(phy_node, &mdio_dev);
	if (ret)
		return ret;

	*bus = *(struct mii_dev **)dev_get_uclass_platdata(mdio_dev);

	return 0;
}

int mdio_device_get_from_eth(struct udevice *eth, struct udevice **devp)
{
	int dev_node = dev_of_offset(eth);
	int phy_node;

	phy_node = fdtdec_lookup_phandle(gd->fdt_blob, dev_node, "phy");
	if (phy_node > 0) {
		return mdio_device_get_from_phy(phy_node, devp);
	} else {
		/*
		 * If there is no phy reference under the ethernet fdt node,
		 * it is not an error since the ethernet device may do not use
		 * mode; so in this case, the output mdio device pointer is set
		 * as NULL.
		 */
		*devp = NULL;
		return 0;
	}
}

int mdio_mii_bus_get_from_eth(struct udevice *eth, struct mii_dev **bus)
{
	struct udevice *mdio_dev;
	int ret;

	ret = mdio_device_get_from_eth(eth, &mdio_dev);
	if (ret)
		return ret;

	if (mdio_dev)
		*bus = *(struct mii_dev **)dev_get_uclass_platdata(mdio_dev);
	else
		*bus = NULL;

	return 0;
}

static int mdio_uclass_pre_probe(struct udevice *dev)
{
	struct mii_dev **pbus = dev_get_uclass_platdata(dev);
	struct mii_dev *bus;
	const char *name;

	bus = mdio_alloc();
	if (!bus) {
		printf("Failed to allocate MDIO bus @%p\n",
		       dev_get_addr_ptr(dev));
		return -1;
	}

	name = fdt_getprop(gd->fdt_blob, dev_of_offset(dev),
			   "mdio-name", NULL);
	if (name)
		strncpy(bus->name, name, MDIO_NAME_LEN);
	*pbus = bus;

	return 0;
}

static int mdio_uclass_post_probe(struct udevice *dev)
{
	struct mii_dev **pbus = dev_get_uclass_platdata(dev);

	return mdio_register(*pbus);
}

UCLASS_DRIVER(mdio) = {
	.id		= UCLASS_MDIO,
	.name		= "mdio",
	.pre_probe	= mdio_uclass_pre_probe,
	.post_probe	= mdio_uclass_post_probe,
	.per_device_platdata_auto_alloc_size = sizeof(struct mii_dev *),
};
