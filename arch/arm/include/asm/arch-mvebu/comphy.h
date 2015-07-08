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

struct comphy_map {
	enum phy_type type;
	enum phy_speed speed;
};

u32 comphy_init(const void *blob);

#ifdef CONFIG_TARGET_ARMADA_38X
struct comphy_mux_data *get_a38x_comphy_mux_data(void);
#endif
#ifdef CONFIG_TARGET_ARMADA_8K
struct comphy_mux_data *get_ap806_comphy_mux_data(void);
#endif
#ifdef CONFIG_TARGET_CP110
struct comphy_mux_data *get_cp110_comphy_mux_data(void);
#endif

#endif /* _COMPHY_H_ */

