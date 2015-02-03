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
#include <linux/compiler.h>
#include <asm/arch-mvebu/unit-info.h>
#include "board-info.h"

/* Optionaly define which units to disable */
u16 armadalp_customer_unit_disable[MAX_UNIT_ID] = {
	[NAND_UNIT_ID] = id_to_mask(0),
	[SDIO_UNIT_ID] = id_to_mask(0)
};

struct mvebu_board_info armadalp_customer_info = {
	.name = "CUSTOMER-BOARD0-MV-ARMADA-LP",
	.id = ARMADA_LP_CUSTOMER0_ID,
	.unit_mask = armadalp_customer_unit_disable,
	.unit_update_mode = UNIT_INFO_DISABLE,
	.mpp_regs = {0x11111111, 0x00100001},
	.configurable = 0,
};

