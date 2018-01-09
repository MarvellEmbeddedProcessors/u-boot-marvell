/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <power/regulator.h>
#ifdef CONFIG_BOARD_CONFIG_EEPROM
#include <mvebu/cfg_eeprom.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* on Armada3700 rev2 devel-board, IO expander (with I2C address 0x22) bit
 * 14 is used as Serdes Lane 2 muxing, which could be used as SATA PHY or
 * USB3 PHY.
 */
enum COMPHY_LANE2_MUXING {
	COMPHY_LANE2_MUX_USB3,
	COMPHY_LANE2_MUX_SATA
};

/* IO expander I2C device */
#define I2C_IO_EXP_ADDR		0x22
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_SATA_OFF	2
#define I2C_IO_REG_0_USB_H_OFF	1
#define I2C_IO_COMPHY_SATA3_USB_MUX_BIT	14

/*
* For Armada3700 A0 chip, comphy serdes lane 2 could be used as PHY for SATA
* or USB3.
* For Armada3700 rev2 devel-board, pin 14 of IO expander PCA9555 with I2C
* address 0x22 is used as Serdes Lane 2 muxing; the pin needs to be set in
* output mode: high level is for SATA while low level is for USB3;
*/
static int board_comphy_usb3_sata_mux(enum COMPHY_LANE2_MUXING comphy_mux)
{
	int ret;
	u8 buf[8];
	struct udevice *i2c_dev;
	int i2c_byte, i2c_bit_in_byte;

	if (!of_machine_is_compatible("marvell,armada-3720-db-v2") &&
	    !of_machine_is_compatible("marvell,armada-3720-db-v3"))
		return 0;

	ret = i2c_get_chip_for_busnum(0, I2C_IO_EXP_ADDR, 1, &i2c_dev);
	if (ret) {
		printf("Cannot find PCA9555: %d\n", ret);
		return 0;
	}

	ret = dm_i2c_read(i2c_dev, I2C_IO_CFG_REG_0, buf, 2);
	if (ret) {
		printf("Failed to read IO expander value via I2C\n");
		return ret;
	}

	i2c_byte = I2C_IO_COMPHY_SATA3_USB_MUX_BIT / 8;
	i2c_bit_in_byte = I2C_IO_COMPHY_SATA3_USB_MUX_BIT % 8;

	/* Configure IO exander bit 14 of address 0x22 in output mode */
	buf[i2c_byte] &= ~(1 << i2c_bit_in_byte);
	ret = dm_i2c_write(i2c_dev, I2C_IO_CFG_REG_0, buf, 2);
	if (ret) {
		printf("Failed to set IO expander via I2C\n");
		return ret;
	}

	ret = dm_i2c_read(i2c_dev, I2C_IO_DATA_OUT_REG_0, buf, 2);
	if (ret) {
		printf("Failed to read IO expander value via I2C\n");
		return ret;
	}

	/* Configure output level for IO exander bit 14 of address 0x22 */
	if (comphy_mux == COMPHY_LANE2_MUX_SATA)
		buf[i2c_byte] |= (1 << i2c_bit_in_byte);
	else
		buf[i2c_byte] &= ~(1 << i2c_bit_in_byte);

	ret = dm_i2c_write(i2c_dev, I2C_IO_DATA_OUT_REG_0, buf, 2);
	if (ret) {
		printf("Failed to set IO expander via I2C\n");
		return ret;
	}

	return 0;
}

int board_early_init_f(void)
{
#ifdef CONFIG_BOARD_CONFIG_EEPROM
	cfg_eeprom_init();
#endif

#ifdef CONFIG_MVEBU_SYS_INFO
	/*
	 * Call this function to transfer data from address 0x4000000
	 * into a global struct, before code relocation.
	 */
	sys_info_init();
#endif
	return 0;
}

int board_usb2_vbus_init(void)
{
#if defined(CONFIG_DM_REGULATOR)
	struct udevice *regulator;
	int ret;

	/* lower usb vbus  */
	ret = regulator_get_by_platname("usb2-vbus", &regulator);
	if (ret) {
		debug("Cannot get usb2-vbus regulator\n");
		return 0;
	}

	ret = regulator_set_enable(regulator, false);
	if (ret) {
		error("Failed to turn OFF the VBUS regulator\n");
		return ret;
	}
#endif
	return 0;
}

int board_usb3_vbus_init(void)
{
#if defined(CONFIG_DM_REGULATOR)
	struct udevice *regulator;
	int ret;

	/* lower usb vbus  */
	ret = regulator_get_by_platname("usb3-vbus", &regulator);
	if (ret) {
		debug("Cannot get usb3-vbus regulator\n");
		return 0;
	}

	ret = regulator_set_enable(regulator, false);
	if (ret) {
		error("Failed to turn OFF the VBUS regulator\n");
		return ret;
	}
#endif
	return 0;
}

int board_init(void)
{
	board_usb2_vbus_init();

	board_usb3_vbus_init();

	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
#ifdef CONFIG_OF_CONTROL
	printf("U-Boot DT blob at : %p\n", gd->fdt_blob);
#endif

	/* enable serdes lane 2 mux for sata phy */
	board_comphy_usb3_sata_mux(COMPHY_LANE2_MUX_SATA);

	return 0;
}

/* Board specific AHCI / SATA enable code */
int board_ahci_enable(struct udevice *dev)
{
#if defined(CONFIG_DM_REGULATOR)
	int ret;
	struct udevice *regulator;

	ret = device_get_supply_regulator(dev, "power-supply",
					  &regulator);
	if (ret) {
		debug("%s: No sata power supply\n", dev->name);
		return 0;
	}

	ret = regulator_set_enable(regulator, true);
	if (ret) {
		error("Error enabling sata power supply\n");
		return ret;
	}
#endif
	return 0;
}
