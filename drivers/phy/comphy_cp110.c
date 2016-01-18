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

/*#define DEBUG */
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/comphy_hpipe.h>
#include <asm/arch-mvebu/mvebu.h>

#define HPIPE_ADDR(base, lane)		(base + 0x800 * lane)

#define COMMON_SELECTOR_PHY_OFFSET	0x140
#define COMMON_SELECTOR_PIPE_OFFSET	0x144

/* For CP-110 we have 2 Selector registers "PHY Selectors", and " PIPE
   Selectors".
   PIPE selector include USB and PCIe options.
   PHY selector include the Ethernet and SATA options, every Ethernet option has different options,
   for example: serdes lane2 had option Eth_port_0 that include (SGMII0, XAUI0, RXAUI0, KR) */
struct comphy_mux_data cp110_comphy_phy_mux_data[] = {
/* Lane 0 */ {4, {{UNCONNECTED, 0x0}, {SGMII2, 0x1}, {XAUI2, 0x1}, {SATA1, 0x4} } },
/* Lane 1 */ {4, {{UNCONNECTED, 0x0}, {SGMII3, 0x1}, {XAUI3, 0x1}, {SATA1, 0x4} } },
/* Lane 2 */ {6, {{UNCONNECTED, 0x0}, {SGMII0, 0x1}, {XAUI0, 0x1}, {RXAUI0, 0x1}, {KR, 0x1}, {SATA0, 0x4} } },
/* Lane 3 */ {8, {{UNCONNECTED, 0x0}, {SGMII0, 0x1}, {XAUI0, 0x1}, {RXAUI0, 0x1}, {KR, 0x1},
					 {XAUI1, 0x1}, {RXAUI1, 0x1}, {SATA1, 0x4} } },
/* Lane 4 */ {7, {{UNCONNECTED, 0x0}, {SGMII0, 0x1}, {XAUI0, 0x1}, {RXAUI0, 0x1}, {KR, 0x1},
					{SGMII2, 0x1}, {XAUI2, 0x1} } },
/* Lane 5 */ {6, {{UNCONNECTED, 0x0}, {XAUI1, 0x1}, {RXAUI1, 0x1}, {SGMII3, 0x1},
					{XAUI3, 0x1}, {SATA1, 0x4} } },
};

struct comphy_mux_data cp110_comphy_pipe_mux_data[] = {
/* Lane 0 */ {2, {{UNCONNECTED, 0x0}, {PEX0, 0x4} } },
/* Lane 1 */ {4, {{UNCONNECTED, 0x0}, {USB3_HOST0, 0x1}, {USB3_DEVICE, 0x2}, {PEX0, 0x4} } },
/* Lane 2 */ {3, {{UNCONNECTED, 0x0}, {USB3_HOST0, 0x1}, {PEX0, 0x4} } },
/* Lane 3 */ {3, {{UNCONNECTED, 0x0}, {USB3_HOST1, 0x1}, {PEX0, 0x4} } },
/* Lane 4 */ {4, {{UNCONNECTED, 0x0}, {USB3_HOST1, 0x1}, {USB3_DEVICE, 0x2}, {PEX0, 0x4} } },
/* Lane 5 */ {2, {{UNCONNECTED, 0x0}, {PEX1, 0x4} } },
};

static int comphy_pcie_power_up(u32 lane, u32 pcie_by4, void __iomem *hpipe_addr)
{
	debug_enter();
	debug("PCIe power UP sequence\n");
	debug_exit();
	return 0;
}

static int comphy_usb3_power_up(u32 lane, void __iomem *hpipe_addr)
{
	debug_enter();
	debug("USB3 power UP sequence\n");
	debug_exit();
	return 0;
}

static int comphy_sata_power_up(u32 lane, void __iomem *hpipe_addr)
{
	debug_enter();
	debug("SATA power UP sequence\n");
	debug_exit();
	return 0;
}

static void comphy_mux_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	void __iomem *comphy_base_addr;
	struct comphy_map comphy_map_pipe_data[MAX_LANE_OPTIONS];
	struct comphy_map comphy_map_phy_data[MAX_LANE_OPTIONS];
	u32 lane, comphy_max_count;

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;

	/* Copy the SerDes map configuration for PIPE map and PHY map
	   the comphy_mux_init modify the type of the lane if the type is not valid
	   because we have 2 selectores run the comphy_mux_init twice and after
	   that update the original serdes_map */
	for (lane = 0; lane < comphy_max_count; lane++) {
		comphy_map_pipe_data[lane].type = serdes_map[lane].type;
		comphy_map_pipe_data[lane].speed = serdes_map[lane].speed;
		comphy_map_phy_data[lane].type = serdes_map[lane].type;
		comphy_map_phy_data[lane].speed = serdes_map[lane].speed;
	}
	ptr_chip_cfg->mux_data = cp110_comphy_phy_mux_data;
	comphy_mux_init(ptr_chip_cfg, comphy_map_phy_data, comphy_base_addr + COMMON_SELECTOR_PHY_OFFSET);

	ptr_chip_cfg->mux_data = cp110_comphy_pipe_mux_data;
	comphy_mux_init(ptr_chip_cfg, comphy_map_pipe_data, comphy_base_addr + COMMON_SELECTOR_PIPE_OFFSET);
	/* Fix the type after check the PHY and PIPE configuration */
	for (lane = 0; lane < comphy_max_count; lane++)
		if (comphy_map_pipe_data[lane].type == UNCONNECTED && comphy_map_phy_data[lane].type == UNCONNECTED)
			serdes_map[lane].type = UNCONNECTED;
}
int comphy_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	struct comphy_map *ptr_comphy_map;
	void __iomem *hpipe_base_addr;
	u32 comphy_max_count, lane, ret = 0;
	u32 pcie_by4 = 1;

	debug_enter();

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	hpipe_base_addr = ptr_chip_cfg->hpipe3_base_addr;

	comphy_mux_cp110_init(ptr_chip_cfg, serdes_map);

	/* Check if the first 4 lanes configured as By-4 */
	for (lane = 0, ptr_comphy_map = serdes_map; lane < 4; lane++, ptr_comphy_map++) {
		if (ptr_comphy_map->type != PEX0) {
			pcie_by4 = 0;
			break;
		}
	}

	for (lane = 0, ptr_comphy_map = serdes_map; lane < comphy_max_count; lane++, ptr_comphy_map++) {
		debug("Initialize serdes number %d\n", lane);
		debug("Serdes type = 0x%x\n", ptr_comphy_map->type);
		switch (ptr_comphy_map->type) {
		case UNCONNECTED:
			continue;
			break;
		case PEX0:
		case PEX1:
		case PEX2:
		case PEX3:
			ret = comphy_pcie_power_up(lane, pcie_by4, HPIPE_ADDR(hpipe_base_addr, lane));
			break;
		case SATA0:
		case SATA1:
		case SATA2:
		case SATA3:
			ret = comphy_sata_power_up(lane, HPIPE_ADDR(hpipe_base_addr, lane));
			break;
		case USB3_HOST0:
		case USB3_HOST1:
			ret = comphy_usb3_power_up(lane, HPIPE_ADDR(hpipe_base_addr, lane));
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
