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
#include <i2c.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/mpp.h>
#include <mvebu_chip_sar.h>
#include "../common/devel-board.h"
#include "../common/sar.h"

/* IO expander I2C device - to be relocated to IO expander code */
#define I2C_IO_EXP_ADDR	0x21
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_USB_H0_OFF	0
#define I2C_IO_REG_0_USB_H1_OFF	1
#define I2C_IO_REG_0_USB_H0_CL	4
#define I2C_IO_REG_0_USB_H1_CL	5

void board_usb_current_limit_init(void)
{
#ifdef CONFIG_PALLADIUM
	debug("USB CURRENT LIMIT change using IO-Expander is not supported in Palladium\n");
#else /* CONFIG_PALLADIUM */
/* Set USB Current Limit signals (via I2C IO expander/GPIO) as output and set
** output value as enabled */
/* This I2C IO expander configuration is board specific, only to DB-7040 board.
 * (USB3_Host0 current limit: I2C device at address 0x21, Register 0, BIT 5)
 * (USB3_Host1 current limit: I2C device at address 0x21, Register 0, BIT 6) */
	int ret_read, ret_write , both_limit_pins;
	unsigned char cfg_val[1], out_val[1];
	const void *blob = gd->fdt_blob;

	/* Make sure board is supported (currently only Armada-70x0-DB is supported) */
	if (fdt_node_check_compatible(blob, 0, "marvell,armada-70x0-db") != 0) {
		debug("Missing USB VBUS power configuration for current board.\n");
		return;
	}

	/* initialize I2C */
	init_func_i2c();

	both_limit_pins = (1 << I2C_IO_REG_0_USB_H0_CL) | (1 << I2C_IO_REG_0_USB_H1_CL);

	/* Read configuration (direction) and set Current limit pin as output (reset pin = output) */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char), cfg_val, sizeof(cfg_val));
	cfg_val[0] &= ~both_limit_pins;
	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char),
			cfg_val, sizeof(cfg_val));
	if (ret_read || ret_write) {
		error("failed to set USB Current Limit configuration on I2C IO expander\n");
		return;
	}

	/* Read Current limit output value, and enable it */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	out_val[0] |= both_limit_pins;
	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	if (ret_write) {
		error("failed to raise USB Current limit power on I2C IO expander\n");
		return;
	}

	mdelay(500);/* required delay to let output value settle */

#endif /* !CONFIG_PALLADIUM */
}

void board_usb_vbus_early_init(void)
{
#ifdef CONFIG_PALLADIUM
	debug("VBUS change using IO-Expander is not supported in Palladium\n");
#else /* CONFIG_PALLADIUM */
/* Set USB VBUS signals (via I2C IO expander/GPIO) as output and set output value as disabled */
/* This I2C IO expander configuration is board specific, only to DB-7040 board.
 * (USB3_Host0 Vbus: I2C device at address 0x21, Register 0, BIT 0)
 * (USB3_Host0 Vbus: I2C device at address 0x21, Register 0, BIT 1) */
	int ret_read, ret_write , both_vbus_pins;
	unsigned char cfg_val[1], out_val[1];
	const void *blob = gd->fdt_blob;

	/* Make sure board is supported (currently only Armada-70x0-DB is supported) */
	if (fdt_node_check_compatible(blob, 0, "marvell,armada-70x0-db") != 0) {
		debug("Missing USB VBUS power configuration for current board.\n");
		return;
	}

	/* initialize I2C */
	init_func_i2c();

	both_vbus_pins = (1 << I2C_IO_REG_0_USB_H0_OFF) | (1 << I2C_IO_REG_0_USB_H1_OFF);

	/* Read configuration (direction) and set VBUS pin as output (reset pin = output) */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char), cfg_val, sizeof(cfg_val));
	cfg_val[0] &= ~both_vbus_pins;
	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char),
			cfg_val, sizeof(cfg_val));
	if (ret_read || ret_write) {
		error("failed to set USB VBUS configuration on I2C IO expander\n");
		return;
	}

	/* Read VBUS output value, and disable it */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	out_val[0] &= ~both_vbus_pins;

	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	if (ret_read || ret_write) {
		error("failed to lower USB VBUS power on I2C IO expander\n");
		return;
	}

	mdelay(500); /* required delay to let output value settle */
#endif /* !CONFIG_PALLADIUM */
}

#ifdef CONFIG_USB_XHCI
void board_usb_vbus_init(void)
{
#ifdef CONFIG_PALLADIUM
	debug("VBUS change using IO-Expander is not supported in Palladium\n");
#else /* CONFIG_PALLADIUM */
/* Set USB VBUS signals (via I2C IO expander/GPIO) as output and set output value as enabled */
/* This I2C IO expander configuration is board specific, only to DB-7040 board.
 * (USB3_Host0 Vbus: I2C device at address 0x21, Register 0, BIT 0)
 * (USB3_Host0 Vbus: I2C device at address 0x21, Register 0, BIT 1) */
	int ret_write , both_vbus_pins;
	unsigned char out_val[1];

	both_vbus_pins = (1 << I2C_IO_REG_0_USB_H0_OFF) | (1 << I2C_IO_REG_0_USB_H1_OFF);
	/* Read VBUS output value, and disable it */
	i2c_read(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	out_val[0] &= ~both_vbus_pins;

	/* Enable VBUS power: Set output value of VBUS pin as enabled */
	out_val[0] |= both_vbus_pins;
	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	if (ret_write) {
		error("failed to raise USB VBUS power on I2C IO expander\n");
		return;
	}

	mdelay(500);/* required delay to let output value settle */
#endif /* !CONFIG_PALLADIUM */
}
#endif /* CONFIG_USB_XHCI */

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

	/* Set USB Current Limit signals as output and set output value as enabled */
	board_usb_current_limit_init();
	/* Set USB VBUS signals as output and set output value as disabled */
	board_usb_vbus_early_init();

	return 0;
}

