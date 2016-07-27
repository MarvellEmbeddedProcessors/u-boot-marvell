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
#include <usb/mvebu_usb.h>
#endif

/* board_usb_vbus_init: to be implemented for special usage of VBUS (i.e. marvell
** IO-Expander) */
#ifdef CONFIG_USB_XHCI
void __board_usb_vbus_init(void)
{
	/* this function is not a must, if have the definition of "gpio-vbus" node,
	   can get the same functionality in usb_vbus_toggle() */
}
void board_usb_vbus_init(void) __attribute__((weak, alias("__board_usb_vbus_init")));

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

/* usb_vbus_gpio_toggle: toggle GPIO VBUS in USB node to the status we want.
	node: the usb node which the VBUS belongs to.
	status: 0 - set VBUS to low;  1 - set VBUS to high. */
static void usb_vbus_gpio_toggle(int node, bool status)
{
	struct fdt_gpio_state gpio;
	int delay = 0;

	if (!fdtdec_decode_gpio(gd->fdt_blob, node, "gpio-vbus", &gpio)) {
		fdtdec_setup_gpio(&gpio);
		if (fdt_gpio_isvalid(&gpio))
			gpio_direction_output(gpio.gpio, status ? (gpio.flags & FDT_GPIO_ACTIVE_LOW ? 0 : 1) :
									(gpio.flags & FDT_GPIO_ACTIVE_LOW ? 1 : 0));

		delay = fdtdec_get_int(gd->fdt_blob, node, status ? "vbus-enable-delay" : "vbus-disable-delay", 0);
		if (delay)
			mdelay(delay);
	}
}

/* usb_vbus_toggle: toggle VBUS to the status we want
	index: the usb index which this VBUS belongs to, if index = -1, toggle all the usb3 VBUS in Device Tree.
	status: 0 - set VBUS to low;  1 - set VBUS to high. */
int usb_vbus_toggle(int index, bool status)
{
	int i;
	int count = board_usb_get_enabled_port_count();

	if (count <= 0 || index < -1 || index >= count)
		return -1;

	if (index == -1) {
		for (i = 0; i < count; i++)
			usb_vbus_gpio_toggle(node_list[i], status);
	} else {
		usb_vbus_gpio_toggle(node_list[index], status);
	}

	return 0;
}

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
	if (usb_vbus_toggle(index, 1)) {
		error("could not enable VBUS for usb index %d!\n", index);
		return -ENODEV;
	}

	debug("mvebu-xhci: init hccr %lx and hcor %lx hc_length %ld\n",
	      (uintptr_t)*hccr, (uintptr_t)*hcor,
		(uintptr_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void xhci_hcd_stop(int index)
{
	usb_vbus_toggle(index, 0);
}
#endif

