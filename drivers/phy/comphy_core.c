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

struct chip_serdes_phy_config chip_config[] = {
#ifdef CONFIG_TARGET_ARMADA_38X
	{.compat = COMPAT_COMPHY_ARMADA_38X,
	 .ptr_comphy_chip_init = comphy_a38x_init },
#endif
#ifdef CONFIG_TARGET_ARMADA_8K
	{.compat = COMPAT_COMPHY_AP806,
	 .ptr_comphy_chip_init = comphy_ap806_init },
	{.compat = COMPAT_COMPHY_CP110,
	 .ptr_comphy_chip_init = comphy_cp110_init },
#endif
#ifdef CONFIG_TARGET_ARMADA_3700
	{.compat = COMPAT_COMPHY_ARMADA_3700,
	 .ptr_comphy_chip_init = comphy_a3700_init },
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

static char *get_speed_string(u32 speed)
{
	char *speed_strings[] = {"1.25 Gbps", "1.5 Gbps", "2.5 Gbps", "3.0 Gbps",
				"3.125 Gbps", "5 Gbps", "6 Gbps", "6.25 Gbps",
				"10.31 Gbps" };
	if (speed < 0 || speed > PHY_SPEED_MAX)
		return "invalid";
	return speed_strings[speed];
}

static char *get_type_string(u32 type)
{
	char *type_strings[] = {"UNCONNECTED", "PEX0", "PEX1", "PEX2", "PEX3",
				"SATA0", "SATA1", "SATA2", "SATA3", "SGMII0",
				"SGMII1", "SGMII2", "SGMII3", "QSGMII", "USB3_HOST0",
				"USB3_HOST1", "USB3_DEVICE", "XAUI0", "XAUI1",
				"XAUI2", "XAUI3", "RXAUI0", "RXAUI1", "KR"};
	if (type < 0 || type > PHY_TYPE_MAX)
		return "invalid";
	return type_strings[type];
}

void reg_set(void __iomem *addr, u32 data, u32 mask)
{
	debug("Write to address = %#010lx, data = %#010x (mask = %#010x) - ", (unsigned long)addr, data, mask);
	debug("old value = %#010x ==> ", readl(addr));
	reg_set_silent(addr, data, mask);
	debug("new value %#010x\n", readl(addr));
}

void reg_set_silent(void __iomem *addr, u32 data, u32 mask)
{
	u32 reg_data;
	reg_data = readl(addr);
	reg_data &= ~mask;
	reg_data |= data;
	writel(reg_data, addr);
}

void reg_set16(void __iomem *addr, u16 data, u16 mask)
{
	debug("Write to address = %#010lx, data = %#06x (mask = %#06x) - ", (unsigned long)addr, data, mask);
	debug("old value = %#06x ==> ", readw(addr));
	reg_set_silent16(addr, data, mask);
	debug("new value %#06x\n", readw(addr));
}

void reg_set_silent16(void __iomem *addr, u16 data, u16 mask)
{
	u16 reg_data;
	reg_data = readw(addr);
	reg_data &= ~mask;
	reg_data |= data;
	writew(reg_data, addr);
}

void comphy_print(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *comphy_map_data)
{
	u32 lane;
	char *speed_str, *type_str;

	for (lane = 0; lane < ptr_chip_cfg->comphy_lanes_count; lane++, comphy_map_data++) {
		if (comphy_map_data->type == PHY_TYPE_UNCONNECTED)
			continue;
		if (comphy_map_data->speed == PHY_SPEED_INVALID) {
			type_str = get_type_string(comphy_map_data->type);
			printf("Comphy-%d: %-13s\n", lane, type_str);
		} else {
			speed_str = get_speed_string(comphy_map_data->speed);
			type_str = get_type_string(comphy_map_data->type);
			printf("Comphy-%d: %-13s %-10s\n", lane, type_str, speed_str);
		}
	}
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

	for (i = 0; i < chip_count ; i++) {
		if (chip_count > 1)
			printf("Comphy chip #%d:\n", i);
		node = comphy_list[i];
		if (node <= 0)
			continue;

		ptr_chip_cfg = get_chip_config(fdtdec_next_lookup(blob, node, COMPAT_MVEBU_COMPHY));
		if (ptr_chip_cfg == NULL) {
			error("compatible of comphy-chip is wrong\n");
			continue;
		}
		ptr_chip_cfg->comphy_lanes_count = fdtdec_get_int(blob, node, "max-lanes", 0);
		if (ptr_chip_cfg->comphy_lanes_count <= 0) {
			error("comphy max lanes is wrong, skip PHY%d\n", i);
			continue;
		}
		ptr_chip_cfg->comphy_mux_bitcount = fdtdec_get_int(blob, node, "mux-bitcount", 0);
		if (ptr_chip_cfg->comphy_mux_bitcount <= 0) {
			error("comphy mux bit count is wrong, skip PHY%d\n", i);
			continue;
		}

		ptr_chip_cfg->comphy_base_addr = fdt_get_regs_offs(blob, node, "reg-comphy");
		if (ptr_chip_cfg->comphy_base_addr == 0) {
			error("comphy base address is NULL, skip PHY%d\n", i);
			continue;
		}
		ptr_chip_cfg->hpipe3_base_addr = fdt_get_regs_offs(blob, node, "reg-hpipe3");
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
			comphy_map_data[lane].speed = fdtdec_get_int(blob, sub_node, "phy-speed", PHY_TYPE_INVALID);
			comphy_map_data[lane].type = fdtdec_get_int(blob, sub_node, "phy-type", PHY_SPEED_INVALID);
			comphy_map_data[lane].invert = fdtdec_get_int(blob, sub_node, "phy-invert",
												PHY_POLARITY_NO_INVERT);
			comphy_map_data[lane].clk_src = fdtdec_get_bool(blob, sub_node, "clk-src");
			if (comphy_map_data[lane].type == PHY_TYPE_INVALID)
				printf("no phy type for lane %d, setting lane as unconnedted\n", lane + 1);
			sub_node = fdt_next_subnode(blob, sub_node);
			lane++;
		} while (sub_node > 0);
		while (lane < max_comphy_count) {
			printf("no phy configuration for lane %d, setting lane as unconnected\n", lane + 1);
			comphy_map_data[lane].type = PHY_TYPE_UNCONNECTED;
			comphy_map_data[lane].speed = PHY_TYPE_INVALID;
			lane++;
		}
		/* Save comphy index for MultiCP devices (A8K) */
		ptr_chip_cfg->comphy_index = i;
		/* PHY power UP sequence */
		ptr_chip_cfg->ptr_comphy_chip_init(ptr_chip_cfg, comphy_map_data);
		/* PHY print SerDes status */
		comphy_print(ptr_chip_cfg, comphy_map_data);
	}

	/* Initialize dedicated PHYs (not muxed SerDes lanes) */
	comphy_dedicated_phys_init();

	debug_exit();

	/*  Return failure in case no comphy unit was set in Device tree */
	if (chip_count <= 0)
		return 1;

	return 0;
}

u32 polling_with_timeout(void __iomem *addr, u32 val, u32 mask, unsigned long
		usec_timout)
{
	u32 data;

	do {
		udelay(1);
		data = readl(addr) & mask;
	} while (data != val  && --usec_timout > 0);

	if (usec_timout == 0)
		return data;
	return 0;
}
