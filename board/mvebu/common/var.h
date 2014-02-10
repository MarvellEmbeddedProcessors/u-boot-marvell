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

#ifndef _VAR_H_
#define _VAR_H_

#include <common.h>
#include <linux/compiler.h>

#define INVALID_KEY	0xFF
#define MAX_VAR_OPTIONS	10

#define VAR_IS_DEFUALT	0x1
#define VAR_IS_LAST	0x2

struct var_opts {
	u8 value;
	char *desc;
	u8 flags;
};

struct var_desc {
	char *key;
	char *description;
};


#endif /* _VAR_H_ */
