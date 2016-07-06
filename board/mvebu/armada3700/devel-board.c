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
#include <i2c.h>
#include <asm/arch-mvebu/soc.h>
#include "../common/devel-board.h"
#include "../common/sar.h"
#include "../common/cfg_eeprom.h"
#ifdef CONFIG_MVEBU_GPIO
#include <asm/gpio.h>
#endif

/* IO expander I2C device */
#define I2C_IO_EXP_ADDR	0x22
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_SATA_OFF	2
#define I2C_IO_REG_0_USB_H_OFF	1

#if defined(CONFIG_SCSI_AHCI_PLAT) && defined(CONFIG_MVEBU_GPIO)
void board_ahci_power_on(void)
{
	struct fdt_gpio_state gpio;
	int count;
	int node;

	/* Scan device tree sata node */
	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "sata",
			COMPAT_MVEBU_SATA, &node, 1);
	if (count == 0) {
		printf("%s: 'sata' is disabled in Device Tree\n", __func__);
		return;
	}

	fdtdec_decode_gpio(gd->fdt_blob, node, "sata-power", &gpio);
	fdtdec_setup_gpio(&gpio);
	if (fdt_gpio_isvalid(&gpio))
		gpio_direction_output(gpio.gpio, (gpio.flags & FDT_GPIO_ACTIVE_LOW ? 0 : 1));

	return;
}
#endif

#ifdef CONFIG_USB_XHCI
/* Set USB VBUS signals (via I2C IO expander/GPIO) as output and set output value as enabled */
void board_usb_vbus_init(void)
{
/* This I2C IO expander configuration is board specific, only to Marvell A3700 DB board.
 * (I2C device at address 0x22, Register 0, BIT 1) */
	int ret_read, ret_write;
	unsigned char cfg_val[1], out_val[1];
	const void *blob = gd->fdt_blob;

	/* Make sure board is supported (currently only A3700-DB is supported) */
	if (fdt_node_check_compatible(blob, 0, "marvell,armada-3700-db0") != 0) {
		error("Missing USB VBUS power configuration for current board.\n");
		return;
	}

	printf("Enable USB VBUS.\n");

	/* initialize I2C */
	init_func_i2c();

	/* Read configuration (direction) and set VBUS pin as output (reset pin = output) */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char), cfg_val, sizeof(cfg_val));
	cfg_val[0] &= ~(1 << I2C_IO_REG_0_USB_H_OFF);
	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char),
			cfg_val, sizeof(cfg_val));
	if (ret_read || ret_write)
		error("failed to set USB VBUS configuration on I2C IO expander\n");

	/* Read VBUS output value, and disable it */
	ret_read = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	out_val[0] &= ~(1 << I2C_IO_REG_0_USB_H_OFF);

	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	if (ret_read || ret_write)
		error("failed to lower USB VBUS power on I2C IO expander\n");

	/* required delay for configuration to settle - must wait for power on port is disabled
	 * in case VBUS signal was high, required 3 seconds delay to let VBUS signal fully settle down */
	udelay(3000000);

	/* Enable VBUS power: Set output value of VBUS pin as enabled */
	out_val[0] |= (1 << I2C_IO_REG_0_USB_H_OFF);

	ret_write = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_DATA_OUT_REG_0, sizeof(unsigned char), out_val, sizeof(out_val));
	if (ret_write)
		error("failed to raise USB VBUS power on I2C IO expander\n");

	udelay(500000); /* required delay to let output value settle up*/


}
#endif /* CONFIG_USB_XHCI */

/*
 * HW_INFO command is used to set board ID to EEPROM,
 * there will be no EEPROM Board ID if HW_INFO is disabled.
 * And CONFIG_BOARD_CONFIG_EEPROM is selected once CMD_MVEBU_HW_INFO is enabled
 */
#ifdef CMD_MVEBU_HW_INFO
int board_get_id(void)
{
	/* Return board ID in EEPROM for Marvell DB board */
	return cfg_eeprom_get_board_id();
}
#endif

int mvebu_devel_board_init(void)
{
#ifdef CONFIG_MVEBU_SAR
	sar_init();
#endif

	return 0;
}

