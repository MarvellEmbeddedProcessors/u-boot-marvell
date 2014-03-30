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

#ifndef _BOARD_INFO_H_
#define _BOARD_INFO_H_

#include "../common/board.h"
#include "../common/sar.h"

enum a38x_board_types {
	ARMADA_8021_DB_ID = 0,
	ARMADA_8021_RD_ID,
	ARMADA_8022_DB_ID,
	ARMADA_8022_RD_ID,
	MAX_BOARD_ID
};

extern struct mvebu_board_info a38x_db_info;
extern struct mvebu_board_info a38x_rd_info;
extern struct mvebu_board_family a38x_board_family;

#endif /* _BOARD_INFO_H_ */
