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

#define COMMON_PHY_CONFIGURATION1_REG           0x0
#define COMMON_PHY_CONFIGURATION2_REG           0x4
#define COMMON_PHY_CONFIGURATION4_REG           0xC

#define COMMON_PHY_SELECTOR_OFFSET		0xFC

struct comphy_mux_data a38x_comphy_mux_data[] = {
	{4, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX0, 0x1}, {PHY_TYPE_SATA0, 0x2}, {PHY_TYPE_SGMII0, 0x3} } },
	{8, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX0, 0x1}, {PHY_TYPE_PEX0, 0x2}, {PHY_TYPE_SATA0, 0x3},
		{PHY_TYPE_SGMII0, 0x4}, {PHY_TYPE_SGMII1, 0x5}, {PHY_TYPE_USB3_HOST0, 0x6}, {PHY_TYPE_QSGMII, 0x7} } },
	{5, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX1, 0x1}, {PHY_TYPE_PEX0, 0x2}, {PHY_TYPE_SATA1, 0x3},
		{PHY_TYPE_SGMII1, 0x4} } },
	{7, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX3, 0x1}, {PHY_TYPE_PEX0, 0x2}, {PHY_TYPE_SATA3, 0x3},
		{PHY_TYPE_SGMII2, 0x4}, {PHY_TYPE_USB3_HOST0, 0x5}, {PHY_TYPE_USB3_DEVICE, 0x6} } },
	{7, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX1, 0x1}, {PHY_TYPE_SATA1, 0x2}, {PHY_TYPE_SGMII1, 0x3},
		{PHY_TYPE_USB3_HOST0, 0x4}, {PHY_TYPE_USB3_DEVICE, 0x5}, {PHY_TYPE_SATA2, 0x6} } },
	{6, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX2, 0x1}, {PHY_TYPE_SATA2, 0x2}, {PHY_TYPE_SGMII2, 0x3},
		{PHY_TYPE_USB3_HOST1, 0x4}, {PHY_TYPE_USB3_DEVICE, 0x5} } },
};

static int comphy_pcie_power_up(u32 pex_index, void __iomem *comphy_addr, void __iomem *hpipe_addr)
{
	debug_enter();

	comphy_pcie_unit_general_config(pex_index);

	/* power up sequence */
	debug("**** start of PCIe comphy power up sequence ****\n");
	reg_set(comphy_addr + COMMON_PHY_CONFIGURATION1_REG, 0x4471804, 0x3FC7F806);
	reg_set(comphy_addr + COMMON_PHY_CONFIGURATION2_REG, 0x58, 0x5C);
	reg_set(comphy_addr + COMMON_PHY_CONFIGURATION4_REG, 0x1, 0x3);
	reg_set(comphy_addr + COMMON_PHY_CONFIGURATION1_REG, 0x6000, 0x7800);
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG, 0x35, 0x3D);
	reg_set(hpipe_addr + HPIPE_GLOBAL_MISC_CTRL, 0x0, 0xC0);
	reg_set(hpipe_addr + HPIPE_MISC_REG, 0x80, 0x4C0);
	udelay(20);

	/* TODO: Add configuration for 20Mhz */
	/* configuration seq for REF_CLOCK_100MHz */
	debug("**** start of PCIe comphy ref clock configuration ****\n");
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, 0x0, 0x1F);
	reg_set(hpipe_addr + HPIPE_MISC_REG, 0x0, 0x400);
	reg_set(hpipe_addr + HPIPE_GLOBAL_PM_CTRL, 0x1E, 0xFF);
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG, 0x400, 0xC00);
	udelay(20);

	/* PEX - electrical configuration seq */
	debug("**** start of PCIe electrical configuration sequence ****\n");
	reg_set(hpipe_addr + HPIPE_G1_SET_0_REG, 0xB000, 0xF000);
	reg_set(hpipe_addr + HPIPE_G1_SET_1_REG, 0x3C9, 0x3FF);
	reg_set(hpipe_addr + HPIPE_G1_SETTINGS_3_REG, 0xCF, 0xFF);
	reg_set(hpipe_addr + HPIPE_G2_SETTINGS_1_REG, 0x3C9, 0x3FF);
	reg_set(hpipe_addr + HPIPE_G2_SETTINGS_3_REG, 0xAF, 0xFF);
	reg_set(hpipe_addr + HPIPE_G2_SETTINGS_4_REG, 0x300, 0x300);
	reg_set(hpipe_addr + HPIPE_DFE_REG0, 0x8000, 0x8000);
	reg_set(hpipe_addr + HPIPE_PCIE_REG1, 0xD00, 0xF80);
	reg_set(hpipe_addr + HPIPE_PCIE_REG3, 0xAF00, 0xFF00);
	reg_set(hpipe_addr + HPIPE_LANE_CFG4_REG, 0x8, 0x8);
	reg_set(hpipe_addr + HPIPE_VTHIMPCAL_CTRL_REG, 0x3000, 0xFF00);
	udelay(20);

	/* PEX - TX configuration sequence 2 */
	debug("**** start of PCIe TX configuration sequence 2 ****\n");
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG, 0x401, 0x401);
	udelay(20);

	/* PEX - TX configuration sequence 3 */
	debug("**** start of PCIe TX configuration sequence 3 ****\n");
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG, 0x0, 0x401);
	udelay(20000);
	reg_set(hpipe_addr + HPIPE_RX_REG3, 0xDC, 0xFF);
	reg_set(hpipe_addr + HPIPE_RX_REG3, 0x100, 0x100);
	reg_set(hpipe_addr + HPIPE_RX_REG3, 0x0, 0x100);

	/* PEX - TX configuration sequence 1 */
	debug("**** start of PCIe TX configuration sequence 1 ****\n");
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG, 0x0, 0x1);
	udelay(20000);

	debug_exit();
	return readl(hpipe_addr + HPIPE_LANE_STATUS1_REG) & 0x1;
}

int comphy_a38x_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	struct comphy_map *ptr_comphy_map;
	void __iomem *comphy_base_addr, *hpipe3_base_addr;
	u32 comphy_max_count, lane, ret = 0;
	bool is_pex_enabled = false;

	debug_enter();

	/* PHY mux initialize */
	ptr_chip_cfg->mux_data = a38x_comphy_mux_data;
	if (ptr_chip_cfg->comphy_base_addr != 0)
		comphy_mux_init(ptr_chip_cfg, serdes_map, ptr_chip_cfg->comphy_base_addr + COMMON_PHY_SELECTOR_OFFSET);

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;
	hpipe3_base_addr = ptr_chip_cfg->hpipe3_base_addr;
	for (lane = 0, ptr_comphy_map = serdes_map; lane < comphy_max_count; lane++, ptr_comphy_map++) {
		debug("Initialize serdes number %d\n", lane);
		debug("Serdes type = 0x%x\n", ptr_comphy_map->type);
		switch (ptr_comphy_map->type) {
		case PHY_TYPE_UNCONNECTED:
			continue;
		case PHY_TYPE_PEX0:
		case PHY_TYPE_PEX1:
		case PHY_TYPE_PEX2:
		case PHY_TYPE_PEX3:
			is_pex_enabled = true;
			/* TODO: add support for PEX by4 initialization */
			ret = comphy_pcie_power_up(ptr_comphy_map->type - PHY_TYPE_PEX0,
					comphy_base_addr + 0x28 * lane, hpipe3_base_addr + 0x800 * lane);
			break;
		default:
			debug("Unknown SerDes type, skip initialize SerDes %d\n", lane);
			continue;
		}
		if (ret == 0)
			printf("PLL is not locked - Failed to initialize lane %d\n", lane);
	}

	if (is_pex_enabled) {
		/* PEX unit configuration set */
		comphy_pcie_config_set(comphy_max_count, serdes_map);
		comphy_pcie_config_detect(comphy_max_count, serdes_map);
	}

	debug_exit();
	return 0;
}
