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
#include <errno.h>
#include <netdev.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/driver_interface.h>

int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif

#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif

#ifdef CONFIG_EEPRO100
	eepro100_initialize(bis);
#endif
	return 0;
}
