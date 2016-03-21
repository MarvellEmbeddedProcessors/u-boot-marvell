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

#ifndef __TYPES_H
#define __TYPES_H

typedef char			int_8;
typedef unsigned char		uint_8;
typedef short			int_16;
typedef unsigned short		uint_16;
typedef int			int_32;
typedef unsigned int		uint_32;
typedef long long		int_64;
typedef unsigned long long	uint_64;

typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef long long		s64;
typedef int			s32;
typedef short			s16;
typedef char			s8;
typedef char			byte;

#define	NULL			((void *)0)

/* Errors */
#define NO_ERROR		0x0000
#define ERR_INVALID_ARGUMENT	0x0001
#define ERR_INVALID_COMMAND	0x0002
#define ERR_BAD_CONFIGURATION	0x0003
#define ERR_TIMEOUT		0x0004

#endif /* __TYPES_H */
