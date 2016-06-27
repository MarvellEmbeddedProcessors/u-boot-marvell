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
#include <asm/arch-mvebu/comphy.h>
#include <netdev.h>
#include <asm/arch/mbus_reg.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/pinctl.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/io_addr_dec.h>
#include <libfdt.h>
#include <asm/arch/boot_mode.h>
#include <fdt_support.h>
#include <asm/arch/pm.h>
#include <asm/arch/avs.h>

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
	/* in Armada3700, there is no device ID register, like A38x,
	    it needs to be got from PCIe register, like A370 and AXP */
	u32 id = 0x9991;
	return id;
}

void soc_init(void)
{
	/* Do early SOC specific init here */
#ifdef CONFIG_MVEBU_A3700_IO_ADDR_DEC
	init_io_addr_dec();
#endif

#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif

/*
 * The AVS should be initialized before PM since the PM will decide
 * to invoke DVS either by AVS, I2C or GPIO.
 */
#ifdef CONFIG_MVEBU_A3700_AVS
	init_avs();
#endif

#ifdef CONFIG_MVEBU_A3700_PM
	init_pm();
#endif
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
	/* in Armada3700, there is a new register, internal Register Base Address, for GBE to
	    access other internal Register. since GBE is on South bridge, not the same island
	    as CPU, here we set internal reg base value 0xf100000 into it.
	    NETA drvier initialization does not rely on this configuration, so do it after
	    mvneta_initialize() */
	writel(MVEBU_REGS_BASE, MVEBU_A3700_GBE0_INTERNAL_REG_BASE);
	writel(MVEBU_REGS_BASE, MVEBU_A3700_GBE1_INTERNAL_REG_BASE);

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
	gd->ram_size = 0;

	/* DDR size has been passed to u-boot from ATF. */
	gd->ram_size = (get_info(DRAM_CS0_SIZE) << 20);

	if (gd->ram_size == 0) {
		error("No DRAM banks detected");
		return 1;
	}

	return 0;
}

void reset_cpu(ulong ignored)
{
	/* write magic number of 0x1d1e to North Bridge Warm Reset register
	   to trigger warm reset */
	writel(MVEBU_NB_WARM_RST_MAGIC_NUM, MVEBU_NB_WARM_RST_REG);
}

/************************************************************************
   Function:  mvebu_is_in_recovery_mode

   The function checks if the system currently boots into recovery mode.
   The recovery mode is intended to bring up bricked board using UART
   port as the boot device. This mode is either trigered by escape
   sequence or by reset sample jumpers.

   Return - 1 if recovery mode is active or 0 otherwise
************************************************************************/
bool mvebu_is_in_recovery_mode(void)
{
	u32 regval;

	/* Check if we are in UART recovery boot mode - currently selected by botstrap */
	regval = (readl(MVEBU_TEST_PIN_LATCH_N) & MVEBU_BOOTMODE_MASK) >> MVEBU_BOOTMODE_OFFS;
	return regval == MVEBU_BOOTMODE_UART;
}

#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void)
{
#ifdef CONFIG_MULTI_DT_FILE
	uint8_t *fdt_blob;
	fdt_blob = cfg_eeprom_get_fdt();
	set_working_fdt_addr(fdt_blob);
#endif

#ifdef CONFIG_BOARD_CONFIG_EEPROM
	/* reset the validation_counter */
	cfg_eeprom_finish();
#endif


#ifdef CONFIG_MVEBU_BOOTMODE_SWITCH_SUPPORT
	/* here we switch back to original mode mode by
	 * writing I2C chip 4c address 0.
	 */
	uchar boot_mode_set = get_info(BOOT_MODE);

	/* Check if oringinal mode is invalid, if it is invalid and set it to auto scan mode */
	if (boot_mode_set < BOOT_MODE_AUTO_SCAN || boot_mode_set >= BOOT_MODE_MAX)
		mvebu_boot_mode_set(BOOT_MODE_AUTO_SCAN);
	else
		mvebu_boot_mode_set(boot_mode_set);
#endif

	return 0;
}
#endif
