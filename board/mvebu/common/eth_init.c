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
#include <errno.h>
#include <netdev.h>
#include <spi.h>
#include <miiphy.h>

DECLARE_GLOBAL_DATA_PTR;

/* This routine must 1st register NETA units, prior to
 * external ETH interfaces (i.e PCIe USB2ETH, etc).
 * This requirement must be kept to ensure consistent
 * alignment with mac address environment variables
 * ethaddr, eth1addr, etc, are always saved for NETA units
 */
int board_eth_init(bd_t *bis)
{

/* Initialize MDIO driver, for NETA ETH interface */
#ifdef CONFIG_MVEBU_MDIO
	mvebu_mdio_initialize(gd->fdt_blob);
	miiphy_set_current_dev("mvebu_mdio");
#endif

/* initalize ETH interfaces */
#ifdef CONFIG_MVNETA
	cpu_eth_init(bis);
#endif

#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif

#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif

#ifdef CONFIG_ENC28J60
	printf("init enc28j60:\n");
	enc28j60_initialize(0, 1, 1000, SPI_MODE_0);
#endif

#ifdef CONFIG_MVPP2X
	cpu_eth_init(bis);
#endif

#ifdef CONFIG_EEPRO100
	eepro100_initialize(bis);
#endif
	return 0;
}
