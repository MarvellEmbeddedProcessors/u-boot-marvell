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
#include <errno.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/mpp.h>
#include <mvebu/mvebu_chip_sar.h>
#include <mvebu/devel-board.h>
#include <mvebu/sar.h>
#include <usb/mvebu_usb.h>

int mvebu_devel_board_init(void)
{
#ifdef CONFIG_MVEBU_MPP_BUS
	char name[8];
	struct sar_val sar;
#endif

#ifdef CONFIG_MVEBU_SAR
	sar_init();
#endif

#ifdef CONFIG_MVEBU_MPP_BUS
	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);

	if (sar.bootsrc.type == BOOTSRC_NAND)
		sprintf(name, "nand");
	else if (sar.bootsrc.type == BOOTSRC_SPI)
		sprintf(name, "spi%d", sar.bootsrc.index);
	else
		sprintf(name, "na");

	if (!mpp_is_bus_enabled(name)) {
		error("Selected boot source (%s) does not match MPP configuration in device tree\n", name);
		printf("HINT: please select proper device tree using fdt_config command\n");
	}

#endif /* CONFIG_MVEBU_MPP_BUS */

#if !defined(CONFIG_PALLADIUM) && defined(CONFIG_USB_XHCI)
	/*
	 * Prepare and switch-off VBUS for all enabled USB3.0 ports.
	 * Also set current limit to the level specified in DT.
	 */
	usb_vbus_toggle(-1, 0);
#endif

	return 0;
}
