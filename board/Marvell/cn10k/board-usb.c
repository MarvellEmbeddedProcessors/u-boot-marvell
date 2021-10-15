// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2021 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <string.h>
#include <errno.h>
#include <dm.h>
#include <linux/delay.h>
#include <usb.h>
#include <asm/gpio.h>

extern int fdt_get_bdk_usb_power_gpio(int port_no);

/**
 * CN106XX A0 silicon is unable to control the USB power GPIO signal from
 * the XHCI block.  This provides a workaround where the GPIO pin is manually
 * toggled.
 */
int board_xhci_power_set(struct usb_device *udev, bool enable)
{
	struct udevice *dev = udev->dev;
	struct gpio_desc desc;
	char str_gpio[32];
	int gpio_pin = fdt_get_bdk_usb_power_gpio(dev->seq);
	int ret;

	pr_debug("%s(%d, %sable)\n", __func__, gpio_pin, enable ? "en" : "dis");
	if (gpio_pin < 0) {
		printf("%s: No GPIO pin found for port %d\n",
		       __func__, dev->seq);
		return -1;
	}

	/* Look up the GPIO pin */
	snprintf(str_gpio, sizeof(str_gpio), "gpio0A%d", gpio_pin);
	ret = dm_gpio_lookup_name(str_gpio, &desc);
	if (ret) {
		printf("%s: GPIO '%s' not found\n", __func__, str_gpio);
		return -1;
	}
	/* Request it */
	ret = dm_gpio_request(&desc, "mrvl_xhci_vbus_pwr");
	if (ret && ret != -EBUSY) {
		printf("%s: Requesting GPIO pin %s failed\n", __func__, str_gpio);
		return -1;
	}
	/* Set it */
	dm_gpio_set_dir_flags(&desc, GPIOD_IS_OUT);
	dm_gpio_set_value(&desc, enable ? 1 : 0);
	/* Free it */
	dm_gpio_free(NULL, &desc);
	udelay(10);

	return 0;
}
