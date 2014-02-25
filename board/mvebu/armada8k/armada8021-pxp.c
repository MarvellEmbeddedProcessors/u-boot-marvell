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

/* #define DEBUG*/

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch-armada8k/armada8k.h>
#include <linux/compiler.h>
#include "board-info.h"

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	debug("Start Armada8021-pxp board init\n");

	a8k_init();

	common_board_init(&a8k_board_family);

	/* a8k specific board init goes here */

	return 0;
}

int dram_init(void)
{
	/*
	 * Clear spin table so that secondary processors
	 * observe the correct value after waken up from wfe.
	 */
	*(unsigned long *)CPU_RELEASE_ADDR = 0;

	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

int timer_init(void)
{
	return 0;
}

/*
 * Board specific reset that is system reset.
 */
void reset_cpu(ulong addr)
{
}

/*
 * Board specific ethernet initialization routine.
 */
int board_eth_init(bd_t *bis)
{
	return 0;
}
