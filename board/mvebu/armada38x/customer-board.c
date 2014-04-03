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
#include <linux/compiler.h>
#include <asm/arch-mvebu/unit-info.h>
#include "board-info.h"

/* Optionaly define which units to disable */
u16 a38x_customer_unit_disable[MAX_UNIT_ID] = {
	[NAND_UNIT_ID] = id_to_mask(0),
	[SDIO_UNIT_ID] = id_to_mask(0)
};

struct mvebu_board_info a38x_customer_info = {
	.name = "CUSTOMER-BOARD",
	.id = ARMADA_38X_CUSTOMER_ID,
	.unit_mask = a38x_customer_unit_disable,
	.unit_update_mode = UNIT_INFO_DISABLE,
	.mpp_regs = {0x11111111, 0x11111111, 0x55266011, 0x25252051,
		     0x25255555, 0x40042565, 0x55550555, 0x00005550},
	.configurable = 0,
};

