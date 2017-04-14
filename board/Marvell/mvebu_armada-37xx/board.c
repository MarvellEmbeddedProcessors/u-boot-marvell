/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <phy.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <power/regulator.h>
#ifdef CONFIG_BOARD_CONFIG_EEPROM
#include <mvebu_cfg_eeprom.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* IO expander I2C device */
#define I2C_IO_EXP_ADDR		0x22
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_SATA_OFF	2
#define I2C_IO_REG_0_USB_H_OFF	1

#define PINCTRL_NB_REG_VALUE	0x000173fa
#define PINCTRL_SB_REG_VALUE	0x00007a23

/* Ethernet switch registers */
/* SMI addresses for multi-chip mode */
#define MVEBU_PORT_CTRL_SMI_ADDR(p)	(16 + (p))
#define MVEBU_SW_G2_SMI_ADDR		(28)

/* Multi-chip mode */
#define MVEBU_SW_SMI_DATA_REG		(1)
#define MVEBU_SW_SMI_CMD_REG		(0)
 #define SW_SMI_CMD_REG_ADDR_OFF	0
 #define SW_SMI_CMD_DEV_ADDR_OFF	5
 #define SW_SMI_CMD_SMI_OP_OFF		10
 #define SW_SMI_CMD_SMI_MODE_OFF	12
 #define SW_SMI_CMD_SMI_BUSY_OFF	15

/* Single-chip mode */
/* Switch Port Registers */
#define MVEBU_SW_LINK_CTRL_REG		(1)
#define MVEBU_SW_PORT_CTRL_REG		(4)

/* Global 2 Registers */
#define MVEBU_G2_SMI_PHY_CMD_REG	(24)
#define MVEBU_G2_SMI_PHY_DATA_REG	(25)

int board_early_init_f(void)
{
	const void *blob = gd->fdt_blob;
	const char *bank_name;
	const char *compat = "marvell,armada-3700-pinctl";
	int off, len;
	void __iomem *addr;

	/* FIXME
	 * Temporary WA for setting correct pin control values
	 * until the real pin control driver is awailable.
	 */
	off = fdt_node_offset_by_compatible(blob, -1, compat);
	while (off != -FDT_ERR_NOTFOUND) {
		bank_name = fdt_getprop(blob, off, "bank-name", &len);
		addr = (void __iomem *)fdtdec_get_addr_size_auto_noparent(
				blob, off, "reg", 0, NULL, true);
		if (!strncmp(bank_name, "armada-3700-nb", len))
			writel(PINCTRL_NB_REG_VALUE, addr);
		else if (!strncmp(bank_name, "armada-3700-sb", len))
			writel(PINCTRL_SB_REG_VALUE, addr);

		off = fdt_node_offset_by_compatible(blob, off, compat);
	}

#ifdef CONFIG_BOARD_CONFIG_EEPROM
	cfg_eeprom_init();
#endif

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

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

/* Helper function for accessing switch devices in multi-chip connection mode */
static int mii_multi_chip_mode_write(struct mii_dev *bus, int dev_smi_addr,
				     int smi_addr, int reg, u16 value)
{
	u16 smi_cmd = 0;

	if (bus->write(bus, dev_smi_addr, 0,
		       MVEBU_SW_SMI_DATA_REG, value) != 0) {
		printf("Error writing to the PHY addr=%02x reg=%02x\n",
		       smi_addr, reg);
		return -EFAULT;
	}

	smi_cmd = (1 << SW_SMI_CMD_SMI_BUSY_OFF) |
		  (1 << SW_SMI_CMD_SMI_MODE_OFF) |
		  (1 << SW_SMI_CMD_SMI_OP_OFF) |
		  (smi_addr << SW_SMI_CMD_DEV_ADDR_OFF) |
		  (reg << SW_SMI_CMD_REG_ADDR_OFF);
	if (bus->write(bus, dev_smi_addr, 0,
		       MVEBU_SW_SMI_CMD_REG, smi_cmd) != 0) {
		printf("Error writing to the PHY addr=%02x reg=%02x\n",
		       smi_addr, reg);
		return -EFAULT;
	}

	return 0;
}

/* Bring-up board-specific network stuff */
int board_network_enable(struct mii_dev *bus)
{
	if (!of_machine_is_compatible("marvell,armada-3720-espressobin"))
		return 0;

	/*
	 * FIXME: remove this code once Topaz driver gets available
	 * A3720 Community Board Only
	 * Configure Topaz switch (88E6341)
	 * Set port 0,1,2,3 to forwarding Mode (through Switch Port registers)
	 */
	mii_multi_chip_mode_write(bus, 1, MVEBU_PORT_CTRL_SMI_ADDR(0),
				  MVEBU_SW_PORT_CTRL_REG, 0x7f);
	mii_multi_chip_mode_write(bus, 1, MVEBU_PORT_CTRL_SMI_ADDR(1),
				  MVEBU_SW_PORT_CTRL_REG, 0x7f);
	mii_multi_chip_mode_write(bus, 1, MVEBU_PORT_CTRL_SMI_ADDR(2),
				  MVEBU_SW_PORT_CTRL_REG, 0x7f);
	mii_multi_chip_mode_write(bus, 1, MVEBU_PORT_CTRL_SMI_ADDR(3),
				  MVEBU_SW_PORT_CTRL_REG, 0x7f);

	/* RGMII Delay on Port 0 (CPU port), force link to 1000Mbps */
	mii_multi_chip_mode_write(bus, 1, MVEBU_PORT_CTRL_SMI_ADDR(0),
				  MVEBU_SW_LINK_CTRL_REG, 0xe002);

	/* Power up PHY 1, 2, 3 (through Global 2 registers) */
	mii_multi_chip_mode_write(bus, 1, MVEBU_SW_G2_SMI_ADDR,
				  MVEBU_G2_SMI_PHY_DATA_REG, 0x1140);
	mii_multi_chip_mode_write(bus, 1, MVEBU_SW_G2_SMI_ADDR,
				  MVEBU_G2_SMI_PHY_CMD_REG, 0x9620);
	mii_multi_chip_mode_write(bus, 1, MVEBU_SW_G2_SMI_ADDR,
				  MVEBU_G2_SMI_PHY_CMD_REG, 0x9640);
	mii_multi_chip_mode_write(bus, 1, MVEBU_SW_G2_SMI_ADDR,
				  MVEBU_G2_SMI_PHY_CMD_REG, 0x9660);

	return 0;
}
