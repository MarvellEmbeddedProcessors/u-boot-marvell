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

/* IO expander I2C device */
#define I2C_IO_EXP_ADDR	0x22
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_SATA_OFF	2
#define I2C_IO_REG_0_USB_H_OFF	1

#ifdef CONFIG_SCSI_AHCI_PLAT
void board_ahci_power_on(void)
{
/* This I2C IO expander configuration is board specific,
 * and adequate only to Marvell A3700 DB board
 */
	int ret;
	unsigned char buffer[1];

	/* Enable power of SATA by set IO expander via I2C,
	 * to set corresponding bit to output mode to enable the power for SATA.
	 */
	ret = i2c_read(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char), buffer, sizeof(buffer));
	if (ret)
		error("failed to read IO expander value via I2C\n");

	buffer[0] &= ~(1 << I2C_IO_REG_0_SATA_OFF);
	ret = i2c_write(I2C_IO_EXP_ADDR, I2C_IO_CFG_REG_0, sizeof(unsigned char), buffer, sizeof(buffer));
	if (ret)
		error("failed to set IO expander via I2C\n");
}
#endif /* CONFIG_SCSI_AHCI_PLAT */

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

int mvebu_devel_board_init(void)
{
#ifdef CONFIG_MVEBU_SAR
	sar_init();
#endif

	return 0;
}

