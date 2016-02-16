/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
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
#include <miiphy.h>
#include <linux/compat.h>
#include "mvgbe.h"

#if !defined(CONFIG_PHYLIB)
# error Marvell mv_pp2x requires PHYLIB
#endif

/* Some linux -> U-Boot compatibility stuff */
#define netdev_err(dev, fmt, args...)		\
	error(fmt, ##args)
#define netdev_warn(dev, fmt, args...)		\
	printf(fmt, ##args)
#define netdev_info(dev, fmt, args...)		\
	printf(fmt, ##args)

/* GOP SMI Functions  */
#define MVCPN110_GOP_MAC_NUM	4

int mv_pp2x_phylib_init(struct mii_dev *bus, struct eth_device *dev,
			int phyid, int gop_index)
{
	struct phy_device *phydev;

	/* Set phy address of the port */
	phydev = phy_connect(bus, phyid, dev, PHY_INTERFACE_MODE_RGMII);
	if (!phydev) {
		netdev_err(dev, "phy_connect failed dev->name(%s) phyid(%d)\n",
			   dev->name, phyid);
		return 1;
	}

	phy_config(phydev);
	phy_startup(phydev);

	return 0;
}

static int mv_pp2x_initialize_phy(bd_t *bis)
{
	struct mvgbe_device *dmvgbe;
	struct eth_device *dev;
	struct mii_dev *bus;
	int devnum;
	u8 used_ports[MVCPN110_GOP_MAC_NUM] = {0, 0, 1, 1};
	/*0-sgmii, 1-N/A, 2-rgmii0, 3-rgmii1. Only 2 & 3 initialized here */

	for (devnum = 0; devnum < MVCPN110_GOP_MAC_NUM; devnum++) {
		/*skip if port is configured not to use */
		if (used_ports[devnum] == 0)
			continue;

		dmvgbe = malloc(sizeof(struct mvgbe_device));

		if (!dmvgbe) {
			netdev_err(NULL, "Err..(%s) Malloc failed %d\n",
				   __func__, devnum);
			return 1;
		}

		memset(dmvgbe, 0, sizeof(struct mvgbe_device));
		dev = &dmvgbe->dev;

		/* must be less than sizeof(dev->name) */
		sprintf(dev->name, "gop_mac%d", devnum);

		eth_register(dev);

	bus = mdio_get_current_dev();
	if (!bus) {
		netdev_err(dev, "failed to detect MDIO bus\n");
		return 1;
	}
	sprintf(bus->name, dev->name);

#if defined(CONFIG_PHYLIB)
	if (mv_pp2x_phylib_init(bus, dev, (devnum - 2), devnum) != 0) {
		netdev_err(dev, "failed to initialize phy\n");
		return 1;
	}
#elif defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
		miiphy_register(dev->name, bus->read, bus->write);
		/* Set phy address of the port */
		if (miiphy_write(dev->name, MV_PHY_ADR_REQUEST,
				 MV_PHY_ADR_REQUEST,
				 PHY_BASE_ADR + devnum) != 0) {
			netdev_err(dev, "miiphy write failed\n");
			return 1;
		}
#endif
	}
	return 0;
}

int mv_pp2x_initialize(bd_t *bis)
{
	return mv_pp2x_initialize_phy(bis);
}

