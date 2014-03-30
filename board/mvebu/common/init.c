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

/* #define DEBUG */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/soc.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

struct mvebu_board_family *board_family;

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	return 0;
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
/* Do very basic stuff like board and soc detection */
int board_early_init_f(void)
{
	return 0;
}
#endif

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	/* Boot progress. */
#if 0
	/* Initialize special environment variables. */
	misc_init_r_env();

	/* CPU Init (part of it depends on env varisables. */
	mv_cpu_init();

	/* USB init (according to usbMode).
	** XOR init
	*/
	// ACS-TODO: Why do we need this?

	/* Boot Progress. */

	/* Print of MPP configuration and modules. */
	mvBoardMppModuleTypePrint();
	mvBoardOtherModuleTypePrint();

	/* Boot Progress. */

	/* Phy Initialization. */
	/* Init the PHY or Switch of the board */
	mvebu_board_phy_switch_init();
#endif
	return 0;
}
#endif


int mvebu_io_init(void)
{
	return 0;
}

int mvebu_board_init(void)
{
	debug("Initializing board\n");

	/*board_family = board_init_family();*/

	return 0;
}


int board_init(void)
{
	mvebu_soc_init();

	mvebu_board_init();

	mvebu_io_init();
	
	return 0;
}

int dram_init(void)
{

	gd->ram_size = 0x20000000;
	return 0;
}

/*
 * Board specific reset that is system reset.
 */
void reset_cpu(ulong addr)
{
}

