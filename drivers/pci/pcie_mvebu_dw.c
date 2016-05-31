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
/*
** Based on:
**   - drivers/pci/pcie_imx.c
**   - drivers/pci/pci_mvebu.c
*/

#include <common.h>
#include <pci.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>
#include "pcie_dw.h"

DECLARE_GLOBAL_DATA_PTR;

#define PCIE_LINK_UP_TIMEOUT_US		(10000)

#define PCIE_GLOBAL_CONTROL		0x8000
#define PCIE_APP_LTSSM_EN		(1 << 2)
#define PCIE_DEVICE_TYPE_OFFSET		(4)
#define PCIE_DEVICE_TYPE_MASK		(0xF)
#define PCIE_DEVICE_TYPE_EP		(0x0) /* Endpoint */
#define PCIE_DEVICE_TYPE_LEP		(0x1) /* Legacy endpoint */
#define PCIE_DEVICE_TYPE_RC		(0x4) /* Root complex */

#define PCIE_GLOBAL_STATUS		0x8008
#define PCIE_GLB_STS_RDLH_LINK_UP	(1 << 1)
#define PCIE_GLB_STS_PHY_LINK_UP	(1 << 9)

#define PCIE_ARCACHE_TRC		0x8050
#define PCIE_AWCACHE_TRC		0x8054
#define ARCACHE_SHAREABLE_CACHEABLE	0x3511
#define AWCACHE_SHAREABLE_CACHEABLE	0x5311

#define LINK_SPEED_GEN_1                0x1
#define LINK_SPEED_GEN_2                0x2
#define LINK_SPEED_GEN_3                0x3

#define MAX_PCIE_BUSES		2

static int mvebu_pcie_link_up(uintptr_t regs_base)
{
	u32 reg;
	int timeout = PCIE_LINK_UP_TIMEOUT_US;
	u32 mask = PCIE_GLB_STS_RDLH_LINK_UP | PCIE_GLB_STS_PHY_LINK_UP;

	while (timeout > 0) {
		reg = readl(regs_base + PCIE_GLOBAL_STATUS);
		if ((reg & mask) == mask)
			return 1;

		udelay(10);
		timeout--;
	}

	return 0;
}

int dw_pcie_link_up(uintptr_t regs_base, u32 cap_speed)
{
	u32 reg;

	/* Disable LTSSM state machine to enable configuration
	 * ans set the device to root complex mode */
	reg = readl(regs_base + PCIE_GLOBAL_CONTROL);
	reg &= ~(PCIE_APP_LTSSM_EN);
	reg &= ~(PCIE_DEVICE_TYPE_MASK << PCIE_DEVICE_TYPE_OFFSET);
	reg |= PCIE_DEVICE_TYPE_RC << PCIE_DEVICE_TYPE_OFFSET;
	writel(reg, regs_base + PCIE_GLOBAL_CONTROL);

	/* Set the PCIe master AXI attributes */
	writel(ARCACHE_SHAREABLE_CACHEABLE, regs_base + PCIE_ARCACHE_TRC);
	writel(AWCACHE_SHAREABLE_CACHEABLE, regs_base + PCIE_AWCACHE_TRC);

	/* DW pre link configurations */
	dw_pcie_configure(regs_base, cap_speed);

	/* Configuration done. Start LTSSM */
	reg = readl(regs_base + PCIE_GLOBAL_CONTROL);
	reg |= PCIE_APP_LTSSM_EN;
	writel(reg, regs_base + PCIE_GLOBAL_CONTROL);

	/* Check that link was established */
	if (!mvebu_pcie_link_up(regs_base))
		return 0;

	/* Link can be established in Gen 1. still need to wait
	   till MAC nagaotiation is completed */
	udelay(100);

	return 1;
}

void pci_init_board(void)
{
	int host_id = -1;
	int first_busno = 0;
	int bus_node_list[MAX_PCIE_BUSES];
	int bus_node, port_node, count;
	u32 cap_speed;
	const void *blob = gd->fdt_blob;
	struct pcie_win mem_win, cfg_win;
	uintptr_t regs_base;
	int err, i;

	count = fdtdec_find_aliases_for_id(blob, "pcie-controller",
			COMPAT_MVEBU_DW_PCIE, bus_node_list, MAX_PCIE_BUSES);

	for (i = 0; i < count; i++) {
		bus_node = bus_node_list[i];

		if (bus_node <= 0)
			continue;

		fdt_for_each_subnode(blob, port_node, bus_node) {
			host_id++;

			if (!fdtdec_get_is_enabled(blob, port_node))
				continue;

			regs_base = (uintptr_t)fdt_get_regs_offs(blob, port_node, "reg");
			if (regs_base == 0) {
				error("Missing registers in PCIe node\n");
				continue;
			}

			if (fdtdec_get_bool(blob, port_node, "endpoint")) {
				dw_pcie_set_endpoint(host_id, regs_base);
				continue;
			}

			cap_speed = fdtdec_get_int(blob, port_node, "force_cap_speed", LINK_SPEED_GEN_3);

			if (cap_speed < LINK_SPEED_GEN_1 || cap_speed > LINK_SPEED_GEN_3) {
				debug("invalid PCIe Gen %d. Forcing to Gen 3\n", cap_speed);
				cap_speed = LINK_SPEED_GEN_3;
			}

			/* Don't register host if link is down */
			if (!dw_pcie_link_up(regs_base, cap_speed)) {
				printf("PCIE-%d: Link down\n", host_id);
				continue;
			}
			printf("PCIE-%d: Link up (Gen%d-x%d, Bus%d)\n", host_id, dw_pcie_get_link_speed(regs_base),
			       dw_pcie_get_link_width(regs_base), first_busno);

			err = fdtdec_get_int_array(blob, port_node, "mem", (u32 *)&mem_win, 2);
			if (err) {
				error("pcie: missing pci memory space in fdt\n");
				continue;
			}

			err = fdtdec_get_int_array(blob, port_node, "cfg", (u32 *)&cfg_win, 2);
			if (err) {
				error("pcie: missing pci configuration space in fdt\n");
				continue;
			}

			/* If all is well register the host */
			first_busno = dw_pcie_init(host_id, regs_base, &mem_win, &cfg_win, first_busno);
			if (first_busno < 0)
				/* Print error message, and try to initialize other hosts */
				printf("Failed to initialize PCIe host %d.\n", host_id);

		}
	}
}
