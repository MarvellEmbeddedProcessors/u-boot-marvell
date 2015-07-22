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

#define PCIE_LINK_UP_TIMEOUT_US		(1000000)

#define PCIE_GLOBAL_STATUS		0x8008
#define PCIE_GLB_STS_RDLH_LINK_UP	(1 << 1)
#define PCIE_GLB_STS_PHY_LINK_UP	(1 << 9)

static int mvebu_pcie_link_up(uintptr_t regs_base)
{
	u32 reg, mask;
	int timeout = PCIE_LINK_UP_TIMEOUT_US;

	while (timeout > 0) {
		/* Check for link. */
		reg = readl(regs_base + PCIE_GLOBAL_STATUS);
		mask = PCIE_GLB_STS_RDLH_LINK_UP | PCIE_GLB_STS_PHY_LINK_UP;
		if ((reg & mask) == mask)
			return 1;
		udelay(10);
		timeout--;
	}

	return 0;
}

void pci_init_board(void)
{
	int host_id = -1;
	int first_busno = 0;
	int bus_node, port_node, count;
	const void *blob = gd->fdt_blob;
	struct pcie_win mem_win, cfg_win;
	uintptr_t regs_base;
	int err, link;

	count = fdtdec_find_aliases_for_id(blob, "pcie-controller",
			COMPAT_MVEBU_DW_PCIE, &bus_node, 1);

	if (count <= 0)
		return;

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

		/* Don't register host if link is down */
		if (!dw_pcie_link_up(regs_base))
			continue;

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
			/* Print error message, and try to initialize other
			** hosts. */
			printf("Failed to initialize PCIe host %d.\n", host_id);

		/* Check the link status - for info only */
		link = mvebu_pcie_link_up(regs_base);
		printf("PCIE-%d: Link %s (Bus %d)\n", host_id, (link ? "Up" : "Down"), first_busno);
	}
}

