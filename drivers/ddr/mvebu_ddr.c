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

#include <common.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>

#ifdef CONFIG_MVEBU_DYNAMIC_SUPPORT
#include <mvHwsDdrMckTraining.h>
#endif

#ifdef CONFIG_MVEBU_STATIC_SUPPORT
static void static_dram_init(struct mvebu_dram_config *config)
{
	mvebu_dram_mac_init(config);
	mvebu_dram_phy_init(config);

	printf("Static DRAM initialization is DONE..\n");
}
#endif

void mvebu_dram_init(const void *blob)
{
	u32 node;
	int sub_node, cs_cnt;
	struct mvebu_dram_config dram_config;
	struct mvebu_dram_config *config = &dram_config;
	u64 size_64;

	debug_enter();

	/* Get DDR MAC node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_DDR_MAC));
	if (node == -1) {
		error("No DDR MAC node found in FDT blob\n");
		return;
	}
	/* Get the base address of the DDR MAC unit */
	config->mac_base = (void *)fdt_get_regs_offs(blob, node, "reg");
	if (config->mac_base == NULL) {
		error("missing DDR MAC base address in DDR MAC node\n");
		return;
	}

	config->cs_count = fdtdec_get_int(blob, node, "cs_count", 0);
	config->bus_width = fdtdec_get_int(blob, node, "bus_width", 0);
	config->size_mb = 0x0;

	/* Get configuration info for each of the DRAM CS's */
	sub_node = fdt_first_subnode(blob, node);
	cs_cnt = 0;
	if (sub_node >= 0) {
		do {
			cs_cnt++;
			size_64 = fdtdec_get_uint64(blob, sub_node, "size", 0x0);
			/* Convert to MB, and accumulate */
			config->size_mb += (size_64 >> 20);
			sub_node = fdt_next_subnode(blob, sub_node);
		} while (sub_node > 0);
		if (cs_cnt < config->cs_count)
			error("Missing configuration for DRAM CS %d to %d.\n", cs_cnt + 1, config->cs_count);
	} else {
		error("No configuration per DRAM CS was found\n");
	}

	/* Get DDR PHY node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_DDR_PHY));
	if (node == -1) {
		error("No DDR PHY node found in FDT blob\n");
		return;
	}
	/* Get the base address of the DDR PHY unit */
	config->phy_base = (void *)fdt_get_regs_offs(blob, node, "reg");
	if (config->phy_base == NULL) {
		error("missing DDR PHY base address in DDR PHY node\n");
		return;
	}

#ifdef CONFIG_MVEBU_STATIC_SUPPORT
	static_dram_init(config);
#elif CONFIG_MVEBU_DYNAMIC_SUPPORT
	dynamic_dram_init(config);
#else
#error "No DRAM configuration selected"
#endif
	debug_exit();
}
