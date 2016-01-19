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
#include <asm/arch-mvebu/sata.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/comphy_hpipe.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>

#define SD_ADDR(base, lane)			(base + 0x1000 * lane)
#define HPIPE_ADDR(base, lane)			(SD_ADDR(base, lane) + 0x800)
#define COMPHY_ADDR(base, lane)			(base + 0x28 * lane)

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

static int comphy_sata_power_up(u32 lane, void __iomem *hpipe_base, void __iomem *comphy_base)
{
	u32 mask, data, ret = 1;
	void __iomem *hpipe_addr = HPIPE_ADDR(hpipe_base, lane);
	void __iomem *sd_ip_addr = SD_ADDR(hpipe_base, lane);
	void __iomem *comphy_addr = COMPHY_ADDR(comphy_base, lane);
	void __iomem *sata_base;

	debug_enter();
	sata_base = fdt_get_reg_offs_by_compat(COMPAT_MVEBU_SATA);
	if (sata_base == 0) {
		debug("SATA address not found in FDT\n");
		return 0;
	}
	debug("SATA address found in FDT %p\n", sata_base);

	debug("stage: MAC configuration - power down comphy\n");
	/* MAC configuration powe down comphy
	   use indirect address for vendor spesific SATA control register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* SATA 0 power down */
	mask = SATA3_CTRL_SATA0_PD_MASK;
	data = 0x1 << SATA3_CTRL_SATA0_PD_OFFSET;
	/* SATA 1 power down */
	mask |= SATA3_CTRL_SATA1_PD_MASK;
	data |= 0x1 << SATA3_CTRL_SATA1_PD_OFFSET;
	/* SATA SSU disable */
	mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
	data |= 0x0 << SATA3_CTRL_SATA1_ENABLE_OFFSET;
	/* SATA port 1 disable */
	mask |= SATA3_CTRL_SATA_SSU_MASK;
	data |= 0x0 << SATA3_CTRL_SATA_SSU_OFFSET;
	reg_set(sata_base + SATA3_VENDOR_DATA, data, mask);

	debug("stage: RFU configurations- hard reset comphy\n");
	/* RFU configurations - hard reset comphy */
	mask = COMMON_PHY_CFG1_PWR_UP_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
	mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
	mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* Set select data  width 40Bit - SATA mode only */
	reg_set(comphy_addr + COMMON_PHY_CFG6_REG,
		0x1 << COMMON_PHY_CFG6_IF_40_SEL_OFFSET, COMMON_PHY_CFG6_IF_40_SEL_MASK);
	/* release from hard reset */
	mask = COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* Wait 1ms - until band gap and ref clock ready */
	mdelay(1);

	debug("stage: Comphy configuration\n");
	/* Start comphy Configuration */
	/* Set reference clock to comes from group 2 - choose 25Mhz */
	reg_set(hpipe_addr + HPIPE_MISC_REG,
		0x1 << HPIPE_MISC_REFCLK_SEL_OFFSET, HPIPE_MISC_REFCLK_SEL_MASK);
	/* Reference frequency select set 1 (for SATA = 25Mhz) */
	mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
	data = 0x1 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
	/* PHY mode select (set SATA = 0x0 */
	mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
	data |= 0x0 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, data, mask);
	/* Set max PHY generation setting - 6Gbps */
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG,
		0x2 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);
	/* Set select data  width 40Bit (SEL_BITS[2:0]) */
	reg_set(hpipe_addr + HPIPE_LOOPBACK_REG,
		0x4 << HPIPE_LOOPBACK_SEL_OFFSET, HPIPE_LOOPBACK_SEL_MASK);

	debug("stage: Analog paramters from ETP(HW)\n");
	/* TODO: Set analog paramters from ETP(HW) - for now use the default datas */

	/* DFE reset sequence */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);
	/* SW reset for interupt logic */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);

	debug("stage: Comphy power up\n");
	/* MAC configuration power up comphy - power up PLL/TX/RX
	   use indirect address for vendor spesific SATA control register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* SATA 0 power up */
	mask = SATA3_CTRL_SATA0_PD_MASK;
	data = 0x0 << SATA3_CTRL_SATA0_PD_OFFSET;
	/* SATA 1 power up */
	mask |= SATA3_CTRL_SATA1_PD_MASK;
	data |= 0x0 << SATA3_CTRL_SATA1_PD_OFFSET;
	/* SATA SSU enable */
	mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
	data |= 0x1 << SATA3_CTRL_SATA1_ENABLE_OFFSET;
	/* SATA port 1 enable */
	mask |= SATA3_CTRL_SATA_SSU_MASK;
	data |= 0x1 << SATA3_CTRL_SATA_SSU_OFFSET;
	reg_set(sata_base + SATA3_VENDOR_DATA, data, mask);

	/* Wait 5ms - Wait for comphy calibration done */
	mdelay(5);

	/* MBUS request size and interface select register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_MBUS_SIZE_SELECT_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* Mbus regret enable */
	reg_set(sata_base + SATA3_VENDOR_DATA, 0x1 << SATA_MBUS_REGRET_EN_OFFSET, SATA_MBUS_REGRET_EN_MASK);

	debug("stage: Check PLL\n");
	data = readl(sd_ip_addr + SD_EXTERNAL_STATUS0_REG);
	/* check the PLL TX */
	if ((data & SD_EXTERNAL_STATUS0_PLL_TX_MASK) == 0) {
		error("SD_EXTERNAL_STATUS0_PLL_TX is 0\n");
		ret = 0;
	}
	/* check the PLL RX */
	if ((data & SD_EXTERNAL_STATUS0_PLL_RX_MASK) == 0) {
		error("SD_EXTERNAL_STATUS0_PLL_RX is 0\n");
		ret = 0;
	}
	/* check the RX init done */
	if ((data & SD_EXTERNAL_STATUS0_RX_INIT_MASK) == 0) {
		error("SD_EXTERNAL_STATUS0_RX_INIT is 0\n");
		ret = 0;
	}

	debug_exit();
	return ret;
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
	void __iomem *comphy_base_addr, *hpipe_base_addr;
	u32 comphy_max_count, lane, ret = 0;
	u32 pcie_by4 = 1;

	debug_enter();

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;
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
			ret = comphy_sata_power_up(lane, hpipe_base_addr, comphy_base_addr);
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
