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

/* #define DEBUG*/

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch-armada8k/armada8k.h>
#include <linux/compiler.h>
#include "board-info.h"

struct mvebu_board_info *mvebu_board_info_get(enum fdt_compat_id compat_id)
{
	switch (compat_id) {
#ifdef CONFIG_APN_806_DB
	case COMPAT_MVEBU_APN_806_DB:
		return &a8021_db_info;
#endif
	case COMPAT_MVEBU_APN_806_CUSTOMER:
	default:
		error("Missing board information for compatible string = %d\n", compat_id);
#ifdef CONFIG_APN_806_DB
		return &a8021_db_info;
#else
		return NULL;
#endif
	}
}

