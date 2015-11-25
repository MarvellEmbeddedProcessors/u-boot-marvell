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

#include <common.h>
#include <asm/arch-mvebu/spl.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/io_addr_dec.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/arch-mvebu/mbus.h>
#include <asm/arch-mvebu/a3700_gpio.h>
#include <asm/arch-mvebu/mvebu_misc.h>

void (*ptr_uboot_start)(void);

void early_spl_init(void)
{
#ifdef CONFIG_MVEBU_SPL_A3700_GPIO
	mvebu_a3700_gpio();
#endif

	/* Clock should be enabeld before initialize the I/O units */
#ifdef CONFIG_MVEBU_A3700_CLOCK
	init_a3700_clock();
#endif

#ifdef CONFIG_MVEBU_A3700_MISC_INIT
	misc_init_cci400();
#endif
}

void late_spl_init(void)
{
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
#ifdef CONFIG_MVEBU_SPL_DDR_SUPPORT
	mvebu_dram_init(gd->fdt_blob);
#endif

#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
#endif

#ifdef CONFIG_MVEBU_A3700_IO_ADDR_DEC
	init_a3700_io_addr_dec();
#endif
}

void soc_spl_jump_uboot(void)
{
	debug("SPL processing done. Jumping to u-boot\n\n");
	ptr_uboot_start = 0;
	/* Jump from SPL to u-boot start address */
	ptr_uboot_start();
}
