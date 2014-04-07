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
#if 0
	char *device, *rev;
	char name[50];
	if (!mvCtrlIsValidSatR())
		printf("       Custom configuration\n");
	device = soc_get_device_name();
	device = soc_get_rev_name();
	printf("SoC:   %s-%s\n", device, rev);
	if (soc_get_cpu_cnt())
		printf("       running %d CPUs\n", mvCtrlGetCpuNum()+1);

	mvCpuNameGet(name);
	printf("CPU:   %s LE",  name);
	if (mvCtrlGetCpuNum())
		printf("       CPU %d\n",  whoAmI());
#endif
	printf("       CPU    @ %d [MHz]\n", soc_cpu_clk_get() / 1000000);
	printf("       L2     @ %d [MHz]\n", soc_l2_clk_get() / 1000000);
	printf("       TClock @ %d [MHz]\n", soc_tclk_get() / 1000000);
	printf("       DDR    @ %d [MHz]\n", soc_ddr_clk_get() / 1000000);
#if 0
	printf("       DDR %dBit Width, %s Memory Access\n", mvCtrlDDRBusWidth(), mvCtrlDDRThruXbar()?"XBAR":"FastPath");
#if defined(CONFIG_ECC_SUPPORT)
	printf("       DDR ECC %s\n", mvCtrlDDRECC()?"Enabled":"Disabled");
#endif
#endif

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



void _enable_caches(void)
{
	/* Nothing to be done */
}
void enable_caches(void) __attribute__((weak, alias("_enable_caches")));
