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

#ifndef _COMPHY_H_
#define _COMPHY_H_

#include <dt-bindings/comphy/comphy_data.h>
#include <fdtdec.h>

/* COMPHY registers */
#define COMMON_PHY_CFG1_REG			0x0
#define COMMON_PHY_CFG1_PWR_UP_OFFSET		1
#define COMMON_PHY_CFG1_PWR_UP_MASK		(0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET)
#define COMMON_PHY_CFG1_PIPE_SELECT_OFFSET	2
#define COMMON_PHY_CFG1_PIPE_SELECT_MASK	(0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET)
#define COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET	13
#define COMMON_PHY_CFG1_PWR_ON_RESET_MASK	(0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET)
#define COMMON_PHY_CFG1_CORE_RSTN_OFFSET	14
#define COMMON_PHY_CFG1_CORE_RSTN_MASK		(0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET)
#define COMMON_PHY_PHY_MODE_OFFSET		15
#define COMMON_PHY_PHY_MODE_MASK		(0x1 << COMMON_PHY_PHY_MODE_OFFSET)

#define COMMON_PHY_CFG6_REG			0x14
#define COMMON_PHY_CFG6_IF_40_SEL_OFFSET	18
#define COMMON_PHY_CFG6_IF_40_SEL_MASK		(0x1 << COMMON_PHY_CFG6_IF_40_SEL_OFFSET)

#define COMMON_SELECTOR_PHY_OFFSET		0x140
#define COMMON_SELECTOR_PIPE_OFFSET		0x144

#define COMMON_PHY_SD_CTRL1			0x148
#define COMMON_PHY_SD_CTRL1_COMPHY_0_4_PORT_OFFSET	0
#define COMMON_PHY_SD_CTRL1_COMPHY_0_4_PORT_MASK	0xFFFF
#define COMMON_PHY_SD_CTRL1_PCIE_X4_EN_OFFSET	24
#define COMMON_PHY_SD_CTRL1_PCIE_X4_EN_MASK		(0x1 << COMMON_PHY_SD_CTRL1_PCIE_X4_EN_OFFSET)
#define COMMON_PHY_SD_CTRL1_PCIE_X2_EN_OFFSET	25
#define COMMON_PHY_SD_CTRL1_PCIE_X2_EN_MASK		(0x1 << COMMON_PHY_SD_CTRL1_PCIE_X2_EN_OFFSET)
#define COMMON_PHY_SD_CTRL1_RXAUI1_OFFSET		26
#define COMMON_PHY_SD_CTRL1_RXAUI1_MASK			(0x1 << COMMON_PHY_SD_CTRL1_RXAUI1_OFFSET)
#define COMMON_PHY_SD_CTRL1_RXAUI0_OFFSET		27
#define COMMON_PHY_SD_CTRL1_RXAUI0_MASK			(0x1 << COMMON_PHY_SD_CTRL1_RXAUI0_OFFSET)

#define DFX_DEV_GEN_CTRL12			(MVEBU_CP0_REGS_BASE + 0x400280)
#define DFX_DEV_GEN_PCIE_CLK_SRC_OFFSET		7
#define DFX_DEV_GEN_PCIE_CLK_SRC_MASK		(0x3 << DFX_DEV_GEN_PCIE_CLK_SRC_OFFSET)

#define MAX_LANE_OPTIONS			10
#define MAX_UTMI_PHY_COUNT			3

struct comphy_mux_options {
	u32 type;
	u32 mux_value;
};

struct comphy_mux_data {
	u32 max_lane_values;
	struct comphy_mux_options mux_values[MAX_LANE_OPTIONS];
};

struct comphy_map {
	u32 type;
	u32 speed;
	u32 invert;
	bool clk_src;
};

struct chip_serdes_phy_config {
	enum fdt_compat_id compat;
	struct comphy_mux_data *mux_data;
	int (*ptr_comphy_chip_init)(struct chip_serdes_phy_config *, struct comphy_map *);
	void __iomem *comphy_base_addr;
	void __iomem *hpipe3_base_addr;
	u32 comphy_lanes_count;
	u32 comphy_mux_bitcount;
	u32 comphy_index;
};

void reg_set(void __iomem *addr, u32 data, u32 mask);
void reg_set_silent(void __iomem *addr, u32 data, u32 mask);
void reg_set16(void __iomem *addr, u16 data, u16 mask);
void reg_set_silent16(void __iomem *addr, u16 data, u16 mask);
u32 comphy_init(const void *blob);
u32 polling_with_timeout(void __iomem *addr, u32 val, u32 mask, unsigned long usec_timout);
void comphy_dedicated_phys_init(void);

#ifdef CONFIG_TARGET_ARMADA_38X
int comphy_a38x_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data);
#elif defined(CONFIG_TARGET_ARMADA_8K)
int comphy_ap806_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map);
int comphy_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map);
#elif defined(CONFIG_TARGET_ARMADA_3700)
int comphy_a3700_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map);
#endif

/* MUX function */
void comphy_mux_init(struct chip_serdes_phy_config *ptr_chip_cfg,
		struct comphy_map *comphy_map_data, void __iomem *selector_base);

void comphy_pcie_config_set(u32 comphy_max_count, struct comphy_map *serdes_map);
void comphy_pcie_config_detect(u32 comphy_max_count, struct comphy_map *serdes_map);
void comphy_pcie_unit_general_config(u32 pex_index);

#endif /* _COMPHY_H_ */

