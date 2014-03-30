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
#include <asm/arch-mvebu/clock.h>

/* Enable each SOC to print its own info */
void __print_soc_info(void)
{
	return;
}

void print_soc_info(void)
	__attribute__((weak, alias("__print_soc_info")));

int print_cpuinfo(void)
{
#if 0
	char *device, *rev;
	char name[50];
	mvBoardIdSet();
	mvBoardNameGet(name);
	printf("Board: %s\n",  name);
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

	print_soc_info();

	return 0;
}

