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
#include <asm/gpio.h>
#include <asm/arch-mvebu/fdt.h>

#ifdef CONFIG_USB_XHCI
#include "usb.h"
#include "xhci.h"
#endif

/* board_usb_vbus_init: to be implemented for special usage of VBUS (i.e. marvell
** IO-Expander) */
#ifdef CONFIG_USB_XHCI
void __board_usb_vbus_init(void)
{
	printf("%s is weak function, need to implement it if need to change VBUS\n", __func__);
}
void board_usb_vbus_init(void) __attribute__((weak, alias("__board_usb_vbus_init")));

/* usb_vbus_init: generic device tree dependent routine for VBUS handling */
static void usb_vbus_init(int node)
{
#ifdef CONFIG_MVEBU_GPIO
	struct fdt_gpio_state gpio;
	fdtdec_decode_gpio(gd->fdt_blob, node, "gpio-vbus", &gpio);
	fdtdec_setup_gpio(&gpio);
	if (fdt_gpio_isvalid(&gpio))
		gpio_direction_output(gpio.gpio, (gpio.flags & FDT_GPIO_ACTIVE_LOW ? 0 : 1));
#endif
}

/* Device tree global data scanned at 1st init for usb3 nodes */
int node_list[CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS];

#ifdef CONFIG_OF_CONTROL
/* Parse and save enabled device tree usb3 nodes, and return enabled node count */
int board_usb_get_enabled_port_count(void)
{
	static int count = -1;

	/* Scan the device tree once only */
	if (count < 0) {
		/* Scan device tree usb3 nodes once, and save relevant nodes in static node_list */
		count = fdtdec_find_aliases_for_id(gd->fdt_blob, "usb3",
				COMPAT_MVEBU_USB3, node_list, CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS);
	}

	if (count == 0)
		printf("%s: 'usb3' is disabled in Device Tree\n", __func__);

	/* Return enabled port count */
	return count;
}
#endif

bool vbus_initialized = 0;
int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	int node;
	unsigned long usb3_reg_base;

	/* Enable USB VBUS for all ports at once, using I2C io-expander */
	if (!vbus_initialized) {
		/* TODO: need to be updated according to Device tree */
		board_usb_vbus_init();
		vbus_initialized = 1; /* mark I2C USB VBUS cycle completed */
	}

	/* node_list: Enabled DT nodes were initialized in usb_device_tree_init(),
	 * so it's valid to use node_list[index] to fetch its registers */
	node = node_list[index];

	/* fetch 'reg' property from 'usb3' node */
	usb3_reg_base = (unsigned long)fdt_get_regs_offs(gd->fdt_blob, node, "reg");

	if (usb3_reg_base == FDT_ADDR_T_NONE) {
		error("could not find reg property in usb3 node, initialization skipped!\n");
		return -ENXIO;
	}

	*hccr = (struct xhci_hccr *)usb3_reg_base;
	*hcor = (struct xhci_hcor *)((unsigned long) *hccr
					+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	/* Enable USB VBUS per port (only via GPIO):
	** enable VBUS using GPIO, and got information from USB node in
	** device tree */
	usb_vbus_init(node);

	debug("mvebu-xhci: init hccr %lx and hcor %lx hc_length %ld\n",
	      (uintptr_t)*hccr, (uintptr_t)*hcor,
		(uintptr_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void xhci_hcd_stop(int index)
{
	return;
}
#endif

