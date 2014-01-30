/*
 * (C) Copyright 2013
 * Yehuda Yitschak <yehuday@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	debug( "Start Initializing Armada8021-pxp board");
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
