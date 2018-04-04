/*
 * Copyright (C) 2015-2016 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _COMPHY_PRIV_H_
#define _COMPHY_PRIV_H_

#include <dt-bindings/comphy/comphy_data.h>
#include <fdtdec.h>

#if defined(DEBUG)
#define debug_enter()	{ printf("----> Enter %s\n", __func__); }
#define debug_exit()	{ printf("<---- Exit  %s\n", __func__); }
#else
#define debug_enter()
#define debug_exit()
#endif


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
	bool end_point;
};

struct chip_serdes_phy_config {
	struct comphy_mux_data *mux_data;
	int (*ptr_comphy_chip_init)(struct chip_serdes_phy_config *,
				    struct comphy_map *);
	int (*rx_training)(struct chip_serdes_phy_config *, u32);
	void __iomem *comphy_base_addr;
	void __iomem *hpipe3_base_addr;
	u32 comphy_lanes_count;
	u32 comphy_mux_bitcount;
	u32 cp_index;
	struct comphy_map comphy_map_data[MAX_LANE_OPTIONS];
};

/* Register helper functions */
void reg_set(void __iomem *addr, u32 data, u32 mask);
void reg_set_silent(void __iomem *addr, u32 data, u32 mask);
void reg_set16(void __iomem *addr, u16 data, u16 mask);
void reg_set_silent16(void __iomem *addr, u16 data, u16 mask);

/* SoC specific init functions */
#ifdef CONFIG_ARMADA_3700
int comphy_a3700_init(struct chip_serdes_phy_config *ptr_chip_cfg,
		      struct comphy_map *serdes_map);
#else
static inline int comphy_a3700_init(struct chip_serdes_phy_config *ptr_chip_cfg,
				    struct comphy_map *serdes_map)
{
	/*
	 * This function should never be called in this configuration, so
	 * lets return an error here.
	 */
	return -1;
}
#endif

#ifdef CONFIG_ARMADA_8K
int comphy_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg,
		      struct comphy_map *serdes_map);
int comphy_cp110_sfi_rx_training(struct chip_serdes_phy_config *ptr_chip_cfg,
				 u32 lane);

#else
static inline int comphy_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg,
				    struct comphy_map *serdes_map)
{
	/*
	 * This function should never be called in this configuration, so
	 * lets return an error here.
	 */
	return -1;
}

static inline int comphy_cp110_sfi_rx_training(
	struct chip_serdes_phy_config *ptr_chip_cfg,
	u32 lane)
{
	/*
	 * This function should never be called in this configuration, so
	 * lets return an error here.
	 */
	return -1;
}

#endif

void comphy_dedicated_phys_init(void);

/* MUX function */
void comphy_mux_init(struct chip_serdes_phy_config *ptr_chip_cfg,
		     struct comphy_map *comphy_map_data,
		     void __iomem *selector_base);

void comphy_pcie_config_set(u32 comphy_max_count,
			    struct comphy_map *serdes_map);
void comphy_pcie_config_detect(u32 comphy_max_count,
			       struct comphy_map *serdes_map);
void comphy_pcie_unit_general_config(u32 pex_index);

#endif /* _COMPHY_PRIV_H_ */

