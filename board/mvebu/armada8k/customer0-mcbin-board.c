/*
 * ***************************************************************************
 * Copyright (C) 2016 Marvell International Ltd.
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
#include <errno.h>
#include <i2c.h>
#include <dt-bindings/gpio/armada-8k-gpio.h>
#include <miiphy.h>
#include <asm/gpio.h>
#include <asm/u-boot.h>

#ifdef CONFIG_USB_XHCI
void board_usb_vbus_init(void)
{
}
#endif /* CONFIG_USB_XHCI */
void mvebu_board_post_eth_init(void)
{
	struct mii_dev *mii = NULL;

	/* Reset 88E1512 1G copper PHY. */
	mii = mdio_get_current_dev();
	if (mii) {
		mii->write(mii, 0x0, 0xffffffff, 0x16, 0x12);
		mii->write(mii, 0x0, 0xffffffff, 0x14, 0x8001);
		udelay(10000);
		mii->write(mii, 0x0, 0xffffffff, 0x16, 0x0);
	}
	/*
	 * Reset the two onboard 10G PHYs. The onboard 1512 1Gbps PHY is reset by
	 * system reset and not a dedicated GPIO.
	 */
	gpio_request(CP1_GPIO(31), "10G_P0_RSTn\n");
	gpio_direction_output(CP1_GPIO(31), 0);
	gpio_request(CP1_GPIO(9), "10G_P0_RSTn\n");
	gpio_direction_output(CP1_GPIO(9), 0);
	udelay(1000);
	gpio_set_value(CP1_GPIO(31), 1);
	gpio_set_value(CP1_GPIO(9), 1);
	udelay(1000);
	/* Set SFP and the two SFP+ TX Disable to 0 (i.e. enable it) */
	gpio_request(CP1_GPIO(24), "SFP_DIS_TX"); /* 2.5Gbps direct to SoC */
	gpio_direction_output(CP1_GPIO(24), 0);
	gpio_request(CP1_GPIO(10), "SFP_PLUS_CP1_DIS_TX");
	gpio_direction_output(CP1_GPIO(10), 0);
	gpio_request(CP1_GPIO(29), "SFP_PLUS_CP0_DIS_TX");
	gpio_direction_output(CP1_GPIO(29), 0);
}
