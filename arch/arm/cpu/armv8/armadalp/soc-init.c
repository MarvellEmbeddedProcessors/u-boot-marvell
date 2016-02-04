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

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <netdev.h>
#include <asm/arch/mbus_reg.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/pinctl.h>
#include <i2c.h>
#include <libfdt.h>

/* IO expander I2C device */
#define I2C_IO_EXP_ADDR	0x22
#define I2C_IO_CFG_REG_0	0x6
#define I2C_IO_DATA_OUT_REG_0	0x2
#define I2C_IO_REG_0_SATA_OFF	2
#define I2C_IO_REG_0_USB_H_OFF	1

/* NB warm reset */
#define MVEBU_NB_WARM_RST_REG	(MVEBU_GPIO_NB_REG_BASE + 0x40)
/* NB warm reset magic number, write it to MVEBU_GPIO_NB_RST_REG triggers warm reset */
#define MVEBU_NB_WARM_RST_MAGIC_NUM	(0x1d1e)

int soc_early_init_f(void)
{
#ifdef CONFIG_MVEBU_PINCTL
	mvebu_pinctl_probe();
#endif

	return 0;
}

int soc_get_rev(void)
{
	/* This should read the soc rev from some register*/
	return 0;
}

int soc_get_id(void)
{
	/* TO-DO, get soc ID from PCIe register */
	/* in ArmadaLP, there is no device ID register, like A38x,
	    it needs to be got from PCIe register, like A370 and AXP */
	u32 id = 0x9991;
	return id;
}

void soc_init(void)
{
	/* Do early SOC specific init here */

	/* now there is no timer/MPP driver,
	  * currently we put all this kind of
	  * configuration here, and will remove
	  * this after official driver is ready
	  */
#ifdef CONFIG_PALLADIUM

#ifdef CONFIG_I2C_MV
	/* 0xD0013830[10] = 1'b0 (select GPIO pins to use for I2C_1) */
	writel((readl(0xd0013830) & ~(1 << 10)), 0xd0013830);
#endif /* CONFIG_I2C_MV */

#endif /* CONFIG_PALLADIUM */
	return;
}

#ifdef CONFIG_MVNETA
/**
 * cpu_eth_init()
 *	invoke mvneta_initialize for each port, which is the initialization
 *	entrance of mvneta driver.
 *
 * Input:
 *	bis - db_info
 *
 * Return:
 *	0 - cool
 */
int cpu_eth_init(bd_t *bis)
{
	/* init neta module */
	if (1 != mvneta_initialize(bis)) {
		error("failed to init mvneta\n");
		return 1;
	}
	/* in ArmadaLP, there is a new register, internal Register Base Address, for GBE to
	    access other internal Register. since GBE is on South bridge, not the same island
	    as CPU, here we set internal reg base value 0xf100000 into it.
	    NETA drvier initialization does not rely on this configuration, so do it after
	    mvneta_initialize() */
	writel(MVEBU_REGS_BASE, MVEBU_ARLP_GBE0_INTERNAL_REG_BASE);
	writel(MVEBU_REGS_BASE, MVEBU_ARLP_GBE1_INTERNAL_REG_BASE);

	return 0;
}
#endif /* CONFIG_MVNETA */

#ifdef CONFIG_I2C_MV
void i2c_clk_enable(void)
{
	/* i2c is enabled by default,
	  * but need this empty routine
	  * to pass compilation.
	*/
	return;
}

#endif /* CONFIG_I2C_MV */

int dram_init(void)
{
#ifdef CONFIG_PALLADIUM
	/* NO DRAM init sequence in Pallaidum, so set static DRAM size of 256MB */
	gd->ram_size = 0x20000000;
#else
	gd->ram_size = 0;

	/* DDR size has been read from dts DDR node in SPL
	 * ddr driver and pass to u-boot. */
	gd->ram_size = (get_info(DRAM_CS0_SIZE) << 20);

	if (gd->ram_size == 0) {
		error("No DRAM banks detected");
		return 1;
	}
#endif

	return 0;
}

void reset_cpu(ulong ignored)
{
	/* write magic number of 0x1d1e to North Bridge Warm Reset register
	   to trigger warm reset */
	writel(MVEBU_NB_WARM_RST_MAGIC_NUM, MVEBU_NB_WARM_RST_REG);
}

#ifdef CONFIG_SCSI_AHCI_PLAT
void board_ahci_power_on(void)
{
/* This I2C IO expander configuration is board specific,
 * and adequate only to Marvell A3700 DB board
 */
#ifdef CONFIG_DEVEL_BOARD
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
#endif /* CONFIG_DEVEL_BOARD */
}
#endif /* CONFIG_SCSI_AHCI_PLAT */

#ifdef CONFIG_USB_XHCI
/* Set USB VBUS signals (via I2C IO expander/GPIO) as output and set output value as enabled */
void board_usb_vbus_init(void)
{
#ifdef CONFIG_DEVEL_BOARD
/* This I2C IO expander configuration is board specific, only to Marvell A3700 DB board.
 * (I2C device at address 0x22, Register 0, BIT 1) */
	int ret_read, ret_write;
	unsigned char cfg_val[1], out_val[1];
	const void *blob = gd->fdt_blob;

	/* Make sure board is supported (currently only A3700-DB is supported) */
	if (fdt_node_check_compatible(blob, 0, "marvell,armada-lp-db0") != 0) {
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

#endif /* CONFIG_DEVEL_BOARD */

}
#endif
