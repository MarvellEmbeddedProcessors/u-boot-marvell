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

#include <fdtdec.h>

#define MAX_LANE_OPTIONS 10

enum phy_speed {
	__1_25gbps,
	__1_5gbps,
	__2_5gbps,
	__3gbps,
	__3_125gbps,
	__5gbps,
	__6gbps,
	__6_25gbps,
	MAX_SERDES_SPEED,
	INVALID_SPEED = 0xff
};

enum phy_type {
	UNCONNECTED,
	PEX0,
	PEX1,
	PEX2,
	PEX3,
	SATA0,
	SATA1,
	SATA2,
	SATA3,
	SGMII0,
	SGMII1,
	SGMII2,
	QSGMII,
	USB3_HOST0,
	USB3_HOST1,
	USB3_DEVICE,
	XAUI,
	RXAUI,
	MAX_PHY_TYPE,
	INVALID_TYPE = 0xff
};

struct comphy_mux_options {
	enum phy_type type;
	u32 mux_value;
};

struct comphy_mux_data {
	u32 max_lane_values;
	struct comphy_mux_options mux_values[MAX_LANE_OPTIONS];
};

struct comphy_map {
	enum phy_type type;
	enum phy_speed speed;
};

struct chip_serdes_phy_config {
	enum fdt_compat_id compat;
	struct comphy_mux_data *mux_data;
	int (*ptr_comphy_chip_init)(struct chip_serdes_phy_config *, struct comphy_map *);
	void __iomem *comphy_base_addr;
	void __iomem *hpipe3_base_addr;
	u32 comphy_lanes_count;
	u32 comphy_mux_bitcount;
};

void reg_set(void __iomem *addr, u32 data, u32 mask);
void reg_set_silent(void __iomem *addr, u32 data, u32 mask);
u32 comphy_init(const void *blob);

#ifdef CONFIG_TARGET_ARMADA_38X
int comphy_a38x_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data);
#elif defined(CONFIG_TARGET_ARMADA_8K)
int comphy_ap806_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map);
#endif



void comphy_pcie_config_set(u32 comphy_max_count, struct comphy_map *serdes_map);
void comphy_pcie_config_detect(u32 comphy_max_count, struct comphy_map *serdes_map);
void comphy_pcie_unit_general_config(u32 pex_index);

#endif /* _COMPHY_H_ */

