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

#include <config.h>
#include <common.h>
#include <errno.h>
#include <i2c.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/thermal.h>
#include <asm/arch-mvebu/mvebu_phy_indirect.h>
#include <ahci.h>
#include <scsi.h>
#include <mvebu_chip_sar.h>

#ifdef CONFIG_MVEBU_CCU
#include <asm/arch-mvebu/ccu.h>
#endif
#ifdef CONFIG_MVEBU_MBUS
#include <asm/arch-mvebu/mbus.h>
#endif

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SYS_I2C)
/* General I2C initialization wrapper */
int init_func_i2c(void)
{
#ifdef CONFIG_SYS_I2C
	i2c_init_all();
#else
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
	return 0;
}
#endif

#ifdef CONFIG_SCSI_AHCI_PLAT
void __board_ahci_power_on(void)
{
	return;
}
void board_ahci_power_on(void) __attribute__((weak, alias("__board_ahci_power_on")));

void board_ahci_init(void)
{
	/* enable the AHCI power */
	board_ahci_power_on();

	ahci_init();
	/* use mode 1 for scsi_scan, so scsi device info will
	  * be reported (print out) after scan the scsi bus.
	 */
}
#endif

/* Weak function for SoC who need specific thermal sensor */
u32 __mvebu_thermal_sensor_probe(void)
{
	return 0;
}
u32 mvebu_thermal_sensor_probe(void) __attribute__((weak, alias("__mvebu_thermal_sensor_probe")));

/* Weak function for boards who need specific init seqeunce */
int __soc_late_init(void)
{
	return 0;
}
int soc_late_init(void) __attribute__((weak, alias("__soc_late_init")));

int mvebu_soc_init()
{
	int ret;

	soc_init();

#ifdef CONFIG_MVEBU_CHIP_SAR
	/* Sample at reset register init */
	mvebu_sar_init(gd->fdt_blob);
#endif

	mvebu_thermal_sensor_probe();

	/* Soc specific init */
	ret = soc_late_init();
	if (ret)
		error("SOC late init failed");

#ifdef CPU_RELEASE_ADDR
	*(unsigned long *)CPU_RELEASE_ADDR = 0;
#endif

	return 0;
}

