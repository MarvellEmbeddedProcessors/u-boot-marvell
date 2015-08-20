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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <common.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/soc-info.h>
#include <linux/compiler.h>
#include "sar.h"

#define MAX_BOARD_NAME		32

/*
 * mvebu_board_info - Describes board specific features
 *
 * name -	short name of the board
 * id -		a unique board id value
 */
struct mvebu_board_info {
	char *name;
	u32  id;

	/* Configure MPP for different board configurations */
	int (*configure_mpp)(void);
};

struct mvebu_board_info *mvebu_board_info_get(enum fdt_compat_id);

/* Common board API */
int board_get_id(void);

DECLARE_GLOBAL_DATA_PTR;
#define get_board_info()	(struct mvebu_board_info *)(gd->arch.board_family->curr_board)
#define set_board_info(x)	(gd->arch.board_family->curr_board = x)

#endif /* _BOARD_H_ */
