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

#define DEBUG
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/comphy_hpipe.h>
#include <asm/arch-mvebu/mvebu.h>

/***************************************************************************************************
  * comphy_pcie_power_up
 ***************************************************************************************************/
static int comphy_pcie_power_up(u32 lane, void __iomem *hpipe_base_addr, void __iomem *comphy_base_addr)
{
	debug_enter();

	debug_exit();

	/* Return the status of the PLL */
	return 1;
}

/***************************************************************************************************
  * comphy_armada_lp_init
 ***************************************************************************************************/
int comphy_armada_lp_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	struct comphy_map *ptr_comphy_map;
	void __iomem *comphy_base_addr, *hpipe_base_addr;
	u32 comphy_max_count, lane, ret = 0;

	debug_enter();

	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;
	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	hpipe_base_addr = ptr_chip_cfg->hpipe3_base_addr;

	for (lane = 0, ptr_comphy_map = serdes_map; lane < comphy_max_count; lane++, ptr_comphy_map++) {
		debug("Initialize serdes number %d\n", lane);
		debug("Serdes type = 0x%x\n", ptr_comphy_map->type);
		switch (ptr_comphy_map->type) {
		case UNCONNECTED:
			continue;
			break;
		case PEX0:
			ret = comphy_pcie_power_up(lane, hpipe_base_addr, comphy_base_addr);
			udelay(20);
			break;
		default:
			debug("Unknown SerDes type, skip initialize SerDes %d\n", lane);
			break;
		}
		if (ret == 0)
			error("PLL is not locked - Failed to initialize lane %d\n", lane);
	}

	debug_exit();
	return 0;
}
