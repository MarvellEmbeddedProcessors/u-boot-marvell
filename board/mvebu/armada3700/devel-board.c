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

#if defined(CONFIG_MVEBU_COMPHY_SUPPORT) && !defined(CONFIG_A3700_Z_SUPPORT)
#include <dt-bindings/comphy/comphy_data.h>
#include <dt-bindings/gpio/armada-3700-gpio.h>

/* on Armada3700 rev2 devel-board, IO expander (with I2C address 0x22) bit
 * 14 is used as Serdes Lane 2 muxing, which could be used as SATA PHY or
 * USB3 PHY.
 */
#define COMPHY_SATA_USB3_MUX_GPIO ARMADA_3700_GPIO(PERIPHERAL_0, 14)

enum COMPHY_LANE2_MUXING {
	COMPHY_LANE2_MUX_USB3,
	COMPHY_LANE2_MUX_SATA,
};
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

#if defined(CONFIG_MVEBU_COMPHY_SUPPORT) && !defined(CONFIG_A3700_Z_SUPPORT)
void board_comphy_usb3_sata_mux(void)
{
	int count;
	int node;
	int sub_node;
	int phy_type;
	enum COMPHY_LANE2_MUXING comphy_mux;

	/* Scan device tree comphy node */
	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "comphy",
			COMPAT_MVEBU_COMPHY, &node, 1);
	if (count == 0) {
		error("%s: 'comphy' is disabled in Device Tree\n", __func__);
		return;
	}
	/* go to Serdes Lane 2 sub_node */
	sub_node = fdt_subnode_offset(gd->fdt_blob, node, "phy2");
	if (!sub_node) {
		debug("%s: No configuration for PHY2 found\n", __func__);
		return;
	}
	phy_type = fdtdec_get_int(gd->fdt_blob, sub_node, "phy-type", PHY_TYPE_INVALID);

	if (phy_type == PHY_TYPE_USB3_HOST0) {
		comphy_mux = COMPHY_LANE2_MUX_USB3;
	} else if (phy_type == PHY_TYPE_SATA0) {
		comphy_mux = COMPHY_LANE2_MUX_SATA;
	} else if (phy_type == PHY_TYPE_INVALID) {
		debug("%s: PHY2 is not used\n", __func__);
		return;
	} else {
		error("%s: phy type (%d) for lane 2 is invalid\n", __func__, phy_type);
		return;
	};

	gpio_request(COMPHY_SATA_USB3_MUX_GPIO, "sata_usb3_mux");
	gpio_direction_output(COMPHY_SATA_USB3_MUX_GPIO, comphy_mux);

	return;
}
#endif

int mvebu_devel_board_init(void)
{
#ifdef CONFIG_MVEBU_SAR
	sar_init();
#endif

#if defined(CONFIG_MVEBU_COMPHY_SUPPORT) && !defined(CONFIG_A3700_Z_SUPPORT)
	board_comphy_usb3_sata_mux();
#endif
	return 0;
}

