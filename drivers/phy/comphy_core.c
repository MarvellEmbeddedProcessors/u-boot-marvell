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
/* #define DEBUG */
#include <common.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/comphy.h>

#define COMPHY_MAX_CHIP 4

struct comphy_mux_options {
	enum phy_type type;
	u32 mux_value;
};

struct comphy_mux_data {
	u32 max_lane_values;
	struct comphy_mux_options mux_values[MAX_LANE_OPTIONS];
};

struct comphy_mux_data a38x_comphy_mux_data[] = {
	{4, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {SATA0, 0x2}, {SGMII0, 0x3} } },
	{8, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {PEX0, 0x2}, {SATA0, 0x3},
		{SGMII0, 0x4}, {SGMII1, 0x5}, {USB3_HOST0, 0x6}, {QSGMII, 0x7} } },
	{5, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {PEX0, 0x2}, {SATA1, 0x3}, {SGMII1, 0x4} } },
	{7, {{UNCONNECTED, 0x0}, {PEX3, 0x1}, {PEX0, 0x2}, {SATA3, 0x3}, {SGMII2, 0x4},
		{USB3_HOST0, 0x5}, {USB3_DEVICE, 0x6} } },
	{7, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {SATA1, 0x2}, {SGMII1, 0x3}, {USB3_HOST0, 0x4},
		{USB3_DEVICE, 0x5}, {SATA2, 0x6} } },
	{6, {{UNCONNECTED, 0x0}, {PEX2, 0x1}, {SATA2, 0x2}, {SGMII2, 0x3}, {USB3_HOST1, 0x4},
		{USB3_DEVICE, 0x5} } },
};

struct comphy_mux_data ap806_comphy_mux_data[] = {
	{4, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {SATA0, 0x2}, {SGMII0, 0x3} } },
	{8, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {PEX0, 0x2}, {SATA0, 0x3},
		{SGMII0, 0x4}, {SGMII1, 0x5}, {USB3_HOST0, 0x6}, {QSGMII, 0x7} } },
	{5, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {PEX0, 0x2}, {SATA1, 0x3}, {SGMII1, 0x4} } },
	{7, {{UNCONNECTED, 0x0}, {PEX3, 0x1}, {PEX0, 0x2}, {SATA3, 0x3}, {SGMII2, 0x4},
		{USB3_HOST0, 0x5}, {USB3_DEVICE, 0x6} } },
	{7, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {SATA1, 0x2}, {SGMII1, 0x3}, {USB3_HOST0, 0x4},
		{USB3_DEVICE, 0x5}, {SATA2, 0x6} } },
	{6, {{UNCONNECTED, 0x0}, {PEX2, 0x1}, {SATA2, 0x2}, {SGMII2, 0x3}, {USB3_HOST1, 0x4},
		{USB3_DEVICE, 0x5} } },
};

struct comphy_mux_data cp110_comphy_mux_data[] = {
	{4, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {SATA0, 0x2}, {SGMII0, 0x3} } },
	{8, {{UNCONNECTED, 0x0}, {PEX0, 0x1}, {PEX0, 0x2}, {SATA0, 0x3},
		{SGMII0, 0x4}, {SGMII1, 0x5}, {USB3_HOST0, 0x6}, {QSGMII, 0x7} } },
	{5, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {PEX0, 0x2}, {SATA1, 0x3}, {SGMII1, 0x4} } },
	{7, {{UNCONNECTED, 0x0}, {PEX3, 0x1}, {PEX0, 0x2}, {SATA3, 0x3}, {SGMII2, 0x4},
		{USB3_HOST0, 0x5}, {USB3_DEVICE, 0x6} } },
	{7, {{UNCONNECTED, 0x0}, {PEX1, 0x1}, {SATA1, 0x2}, {SGMII1, 0x3}, {USB3_HOST0, 0x4},
		{USB3_DEVICE, 0x5}, {SATA2, 0x6} } },
	{6, {{UNCONNECTED, 0x0}, {PEX2, 0x1}, {SATA2, 0x2}, {SGMII2, 0x3}, {USB3_HOST1, 0x4},
		{USB3_DEVICE, 0x5} } },
};

struct chip_serdes_phy_config {
	enum fdt_compat_id compat;
	struct comphy_mux_data *mux_data;
	int (*ptr_serdes_phy_power_up)(struct chip_serdes_phy_config *, struct comphy_map *);
	u32 comphy_base_addr;
	u32 hpipe3_base_addr;
	u32 comphy_lanes_count;
	u32 comphy_mux_bitcount;
};

static int comphy_rev_2_1_power_up(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data)
{
	printf("COMPHY power up sequence revision 2.1\n");
	return 0;
}

struct chip_serdes_phy_config chip_config[] = {
#ifdef CONFIG_TARGET_ARMADA_38X
	{.compat = COMPAT_COMPHY_ARMADA_38X,
	 .mux_data = a38x_comphy_mux_data,
	 .ptr_serdes_phy_power_up = comphy_rev_2_1_power_up },
#endif
#ifdef CONFIG_TARGET_ARMADA_8K
	{.compat = COMPAT_COMPHY_AP806,
	 .mux_data = ap806_comphy_mux_data,
	 .ptr_serdes_phy_power_up = comphy_rev_2_1_power_up },
#endif
#ifdef CONFIG_TARGET_CP110
	{.compat = CP110,
	 .mux_data = cp110_comphy_mux_data,
	 .ptr_serdes_phy_power_up = comphy_rev_3_1_power_up },
#endif
	{.compat = COMPAT_UNKNOWN}
};

struct chip_serdes_phy_config *get_chip_config(enum fdt_compat_id compat)
{
	struct chip_serdes_phy_config *ptr_chip_config = chip_config;

	while (ptr_chip_config->compat != COMPAT_UNKNOWN) {
		if (ptr_chip_config->compat == compat)
			return ptr_chip_config;
		ptr_chip_config++;
	}
	return NULL;
}

static char *get_speed_string(enum phy_speed speed)
{
	char *speed_strings[] = {"1.25Gbps", "1.5Gbps", "2.5Gbps", "3.0Gbps",
				"3.25Gbps", "5Gbps", "6Gbps", "6.25Gbps"};
	if (speed < __1_25gbps || speed > __6_25gbps)
		return "invalid";
	return speed_strings[speed];
}

static char *get_type_string(enum phy_type type)
{
	char *type_strings[] = {"unconnected", "PEX0", "PEX1", "PEX2", "PEX3",
				"SATA0", "SATA1", "SATA2", "SATA3", "SGMII0",
				"SGMII1", "SGMII2", "QSGMII", "USB3_HOST0",
				"USB3_HOST1", "USB3_DEVICE", "XAUI", "RXAUI"};
	if (type < UNCONNECTED || type > RXAUI)
		return "invalid";
	return type_strings[type];
}

/* comphy_mux_check_config
 * description: this function passes over the COMPHY lanes and check if the type
 *              is valid for specific lane. If the type is not valid, the function
 *              update the struct and set the type of the lane as UNCONNECTED */
static void comphy_mux_check_config(struct comphy_mux_data *mux_data,
		struct comphy_map *comphy_map_data, int comphy_max_lanes)
{
	struct comphy_mux_options *ptr_mux_opt;
	int lane, opt, valid;
	debug_enter();

	for (lane = 0; lane < comphy_max_lanes; lane++, comphy_map_data++, mux_data++) {
		ptr_mux_opt = mux_data->mux_values;
		for (opt = 0, valid = 0; opt < mux_data->max_lane_values; opt++, ptr_mux_opt++) {
			if (ptr_mux_opt->type == comphy_map_data->type) {
				valid = 1;
				break;
			}
		}
		if (valid == 0) {
			debug("lane number %d, had invalid type %d\n", lane, comphy_map_data->type);
			debug("set lane %d as type %d\n", lane, UNCONNECTED);
			comphy_map_data->type = UNCONNECTED;
		} else {
			debug("lane number %d, has type %d\n", lane, comphy_map_data->type);
		}
	}
	debug_exit();
}

static u32 comphy_mux_get_mux_value(struct comphy_mux_data *mux_data, enum phy_type type, int lane)
{
	struct comphy_mux_options *ptr_mux_opt;
	int opt;
	ptr_mux_opt = mux_data->mux_values;
	for (opt = 0 ; opt < mux_data->max_lane_values; opt++, ptr_mux_opt++)
		if (ptr_mux_opt->type == type)
			return ptr_mux_opt->mux_value;
	return 0;
}

static void comphy_mux_reg_write(struct comphy_mux_data *mux_data,
		struct comphy_map *comphy_map_data, int comphy_max_lanes, u32 base_addr, u32 bitcount)
{
	u32 lane, reg_val;
	u32 value, offset, mask;
	u32 comphy_selector_base = base_addr + 0xfc;

	for (lane = 0; lane < comphy_max_lanes; lane++, comphy_map_data++, mux_data++) {
		offset = lane * bitcount;
		mask = (((1 << bitcount) - 1) << offset);
		value = (comphy_mux_get_mux_value(mux_data, comphy_map_data->type, lane) << offset);
		reg_val = readl(comphy_selector_base);
		debug("Write to address = %#010x, data = %#010x (mask = %#010x) - ", comphy_selector_base, value, mask);
		debug("old value = %#010x ==> ", readl(comphy_selector_base));
		reg_val &= ~mask;
		reg_val |= value;
		writel(reg_val, comphy_selector_base);
		debug("new value %#010x\n", readl(comphy_selector_base));
	}
}

static void comphy_mux_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data)
{
	struct comphy_mux_data *mux_data;
	u32 comphy_base, mux_bitcount;
	u32 comphy_max_lanes;

	debug_enter();

	comphy_max_lanes = ptr_chip_cfg->comphy_lanes_count;
	mux_data = ptr_chip_cfg->mux_data;
	comphy_base = ptr_chip_cfg->comphy_base_addr;
	mux_bitcount = ptr_chip_cfg->comphy_mux_bitcount;

	/* check if the configuration is valid */
	comphy_mux_check_config(mux_data, comphy_map_data, comphy_max_lanes);
	/* Init COMPHY selectors */
	comphy_mux_reg_write(mux_data, comphy_map_data,	comphy_max_lanes, comphy_base, mux_bitcount);

	debug_exit();
}

void comphy_print(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data)
{
	u32 lane;
	char *speed_str, *type_str;

	printf("COMPHY lanes details:\n");
	printf(" | Lane # | Speed     | Type         |\n");
	printf(" -------------------------------------\n");
	for (lane = 0; lane < ptr_chip_cfg->comphy_lanes_count; lane++, comphy_map_data++) {
		speed_str = get_speed_string(comphy_map_data->speed);
		type_str = get_type_string(comphy_map_data->type);
		printf(" |    %-4d| %-10s| %-13s|\n", lane, speed_str, type_str);
	}
	printf(" -------------------------------------\n");
}

u32 comphy_init(const void *blob)
{
	int node, sub_node, comphy_list[COMPHY_MAX_CHIP];
	u32 lane, chip_count, i, max_comphy_count;
	struct comphy_map comphy_map_data[MAX_LANE_OPTIONS];
	struct chip_serdes_phy_config *ptr_chip_cfg;

	debug_enter();
	chip_count = fdtdec_find_aliases_for_id(blob, "comphy",
			COMPAT_MVEBU_COMPHY, comphy_list, COMPHY_MAX_CHIP);

	if (chip_count <= 0)
		return 1;

	printf("COMPHY init sequence\n");
	for (i = 0; i < chip_count ; i++) {
		node = comphy_list[i];
		if (node <= 0)
			continue;

		ptr_chip_cfg = get_chip_config(fdtdec_next_lookup(blob, node, COMPAT_MVEBU_COMPHY));
		if (ptr_chip_cfg == NULL) {
			error("comaptible of comphy-chip is wrong\n");
			continue;
		}
		ptr_chip_cfg->comphy_lanes_count = fdtdec_get_int(blob, node, "max-lanes", 0);
		if (ptr_chip_cfg->comphy_lanes_count <= 0) {
			error("comphy max lanes is wrong, skip PHY%d\n", i);
			continue;
		}
		ptr_chip_cfg->comphy_mux_bitcount = fdtdec_get_int(blob, node, "mux-bitcount", 0);
		if (ptr_chip_cfg->comphy_mux_bitcount <= 0) {
			error("comphy mux bitcount is wrong, skip PHY%d\n", i);
			continue;
		}
		ptr_chip_cfg->comphy_base_addr = (u32)fdt_get_regs_offs(blob, node, "reg-comphy");
		if (ptr_chip_cfg->comphy_base_addr == 0) {
			error("comphy base address is NULL, skip PHY%d\n", i);
			continue;
		}
		ptr_chip_cfg->hpipe3_base_addr = (u32)fdt_get_regs_offs(blob, node, "reg-hpipe3");
		if (ptr_chip_cfg->hpipe3_base_addr == 0) {
			error("comphy hpipe3 address is NULL, skip PHY%d\n", i);
			continue;
		}

		/* GET the count of the SerDes of the spesific chip */
		sub_node = fdt_first_subnode(blob, node);
		if (!sub_node) {
			error("No configuration per PHY found\n");
			continue;
		}
		max_comphy_count = ptr_chip_cfg->comphy_lanes_count;
		lane = 0;
		do {
			comphy_map_data[lane].speed = fdtdec_get_int(blob, sub_node, "phy-speed", INVALID_SPEED);
			comphy_map_data[lane].type = fdtdec_get_int(blob, sub_node, "phy-type", INVALID_TYPE);
			if (comphy_map_data[lane].speed == INVALID_SPEED || comphy_map_data[lane].type == INVALID_TYPE)
				printf("no phy speed or type for lane %d, setting lane as unconnedted\n", lane + 1);
			sub_node = fdt_next_subnode(blob, sub_node);
			lane++;
		} while (sub_node > 0);
		while (lane < max_comphy_count) {
			printf("no phy configuration for lane %d, setting lane as unconnected\n", lane + 1);
			comphy_map_data[lane].type = UNCONNECTED;
			comphy_map_data[lane].speed = INVALID_SPEED;
			lane++;
		}

		/* PHY mux initialize */
		if (ptr_chip_cfg->comphy_base_addr != 0)
			comphy_mux_init(ptr_chip_cfg, comphy_map_data);

		/* PHY print SerDes status */
		comphy_print(ptr_chip_cfg, comphy_map_data);
		/* PHY power UP sequence */
		ptr_chip_cfg->ptr_serdes_phy_power_up(ptr_chip_cfg, comphy_map_data);
	}
	debug_exit();

	return 0;
}
