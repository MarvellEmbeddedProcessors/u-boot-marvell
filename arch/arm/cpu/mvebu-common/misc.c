/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>

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

void enable_caches(void)
{
	/* Nothing to be done */
}

