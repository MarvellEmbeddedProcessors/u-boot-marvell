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
