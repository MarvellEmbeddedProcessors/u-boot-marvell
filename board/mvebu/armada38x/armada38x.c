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
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch-armada38x/armada38x.h>
#include <linux/compiler.h>
#include "board-info.h"

struct mvebu_board_info *mvebu_board_info_get(enum fdt_compat_id compat_id)
{
	switch (compat_id) {
	case COMPAT_MVEBU_ARMADA_38X_DB:
		return &a38x_db_info;
	case COMPAT_MVEBU_ARMADA_38X_RD:
		return &a38x_rd_info;
	case COMPAT_MVEBU_ARMADA_38X_CUSTOMER:
		return &a38x_customer_info;
	default:
		error("Missing board information for compatible string = %d\n", compat_id);
		return &a38x_db_info;
	}
}
