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
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/clock.h>

#define DEVICE_ID_OFFSET	16
#define DEVICE_REV_OFFSET	8


int __soc_get_rev(void)
{
	u32 rev = readl(MVEBU_DEVICE_REV_REG) >> DEVICE_REV_OFFSET;
	return rev;
}
int soc_get_rev(void) __attribute__((weak, alias("__soc_get_rev")));

int __soc_get_id(void)
{
	u32 id = readl(MVEBU_DEVICE_ID_REG) >> DEVICE_ID_OFFSET;
	return id;
}
int soc_get_id(void) __attribute__((weak, alias("__soc_get_id")));

void print_mv_banner(void)
{
#ifdef CONFIG_SILENT_CONSOLE
	DECLARE_GLOBAL_DATA_PTR;
	gd->flags |= GD_FLG_SILENT;
#endif
	printf("\n");
	printf(" __   __                      _ _\n");
	printf("|  \\/  | __ _ _ ____   _____| | |\n");
	printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
	printf("| |  | | (_| | |   \\ V /  __/ | |\n");
	printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
	printf("         _   _     ____              _\n");
	printf("        | | | |   | __ )  ___   ___ | |_\n");
	printf("        | | | |___|  _ \\ / _ \\ / _ \\| __|\n");
	printf("        | |_| |___| |_) | (_) | (_) | |_\n");
	printf("         \\___/    |____/ \\___/ \\___/ \\__|\n");
	return;
}

/* Enable each SOC to print its own info */
void __print_soc_specific_info(void)
{
	return;
}
void print_soc_specific_info(void) __attribute__((weak, alias("__print_soc_specific_info")));


void mvebu_print_soc_info(void)
{
	soc_print_clock_info();

	print_soc_specific_info();
}

/*
 * We don't use the generic U-BOOT "print_cpuinfo" hook since we want to print
 * the board name first. board name is only available later in init stage
 * and that's when we call "mvebu_print_soc_info"
 */
#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	return 0;
}
#endif

void __reset_cpu(ulong addr)
{
	/* Enable global system reset */
	setbits_le32(MVEBU_RESET_MASK_REG, 0x1);

	/* Trigger reset */
	setbits_le32(MVEBU_SOFT_RESET_REG, 0x1);
}

void reset_cpu(ulong addr) __attribute__((weak, alias("__reset_cpu")));


#ifdef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Nothing to be done */
}
#endif
