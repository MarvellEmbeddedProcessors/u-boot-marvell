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
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/tables.h>
#include <asm/arch-mvebu/thermal.h>
#include <ahci.h>
#include <scsi.h>

#ifdef CONFIG_MVEBU_CCU
#include <asm/arch-mvebu/ccu.h>
#endif
#ifdef CONFIG_MVEBU_RFU
#include <asm/arch-mvebu/rfu.h>
#endif
#ifdef CONFIG_MVEBU_IOB
#include <asm/arch-mvebu/iob.h>
#endif
#ifdef CONFIG_MVEBU_MBUS
#include <asm/arch-mvebu/mbus.h>
#endif

#ifdef CONFIG_SCSI_AHCI_PLAT
void scsi_init(void)
{
	ahci_init();
	/* use mode 1 for scsi_scan, so scsi device info will
	  * be reported (print out) after scan the scsi bus.
	 */
	scsi_scan(1);
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

#if defined(CONFIG_MVEBU_CCU) || defined(CONFIG_MVEBU_RFU) || defined(CONFIG_MVEBU_FLC)
	/* ccu and rfu init support 2 modes:
	   1. regular mode - init both sw structures and configure windows
	   2. sw init - in case that the ccu and rfu were initialized at the SPL,
					init only the ccu and rfu sw structures
	   By default use the regular mode */
	bool sw_init = false;
#endif

	soc_init();

#ifdef CONFIG_MVEBU_FLC
	/* when FLC is enable, the ccu & rfu init already done at the SPL */
	sw_init = true;
#endif

	/* Initialize physical memory map */
#ifdef CONFIG_MVEBU_CCU
	init_ccu(sw_init);
#endif
#ifdef CONFIG_MVEBU_RFU
	init_rfu(sw_init);
#endif
#ifdef CONFIG_MVEBU_IOB
	init_iob();
#endif
#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
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

