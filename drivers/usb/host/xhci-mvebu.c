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
#include <asm/arch-mvebu/soc.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>

#ifdef CONFIG_USB_XHCI
#include "usb.h"
#include "xhci.h"
#endif

#ifdef CONFIG_USB_XHCI
int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	int node_list[CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS], node;
	int i, count;
	unsigned long usb3_reg_base;

	/* in dts file, go through all the 'usb3' nodes.
	 */
	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "usb3",
			COMPAT_MVEBU_USB3, node_list, CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS);
	if (count == 0) {
		error("could not find usb3 node in FDT, initialization skipped!\n");
		return -ENXIO;
	}
	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		/* fetch 'reg' propertiy from 'usb3' node */
		usb3_reg_base = (unsigned long)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
		if (usb3_reg_base == FDT_ADDR_T_NONE) {
			error("could not find reg in usb3 node, initialization skipped!\n");
			return -ENXIO;
		}

		*hccr = (struct xhci_hccr *)usb3_reg_base;
		*hcor = (struct xhci_hcor *)((unsigned long) *hccr
					+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

		debug("mvebu-xhci: init hccr %lx and hcor %lx hc_length %ld\n",
		      (uintptr_t)*hccr, (uintptr_t)*hcor,
			(uintptr_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));
		return 0;
	}

	return -ENXIO;
}

void xhci_hcd_stop(int index)
{
	return;
}
#endif

