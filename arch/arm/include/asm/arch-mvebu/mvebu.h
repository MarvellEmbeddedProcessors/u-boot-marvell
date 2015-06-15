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

#ifndef _MVEBU_H_
#define _MVEBU_H_

/* Bit definitions */
#define NO_BIT		0x00000000
#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10		0x00000400
#define BIT11		0x00000800
#define BIT12		0x00001000
#define BIT13		0x00002000
#define BIT14		0x00004000
#define BIT15		0x00008000
#define BIT16		0x00010000
#define BIT17		0x00020000
#define BIT18		0x00040000
#define BIT19		0x00080000
#define BIT20		0x00100000
#define BIT21		0x00200000
#define BIT22		0x00400000
#define BIT23		0x00800000
#define BIT24		0x01000000
#define BIT25		0x02000000
#define BIT26		0x04000000
#define BIT27		0x08000000
#define BIT28		0x10000000
#define BIT29		0x20000000
#define BIT30		0x40000000
#define BIT31		0x80000000

/* Use this functions only when printf is allowed */
#if defined(CONFIG_MVEBU_DEBUG_FUNC_IN_OUT) || defined(DEBUG)
#define debug_enter()	printf("----> Enter %s\n", __func__);
#define debug_exit()  printf("<---- Exit  %s\n", __func__);
#else
#define debug_enter()
#define debug_exit()
#endif

/* Macro for testing alignment. Positive if number is NOT aligned */
#define IS_NOT_ALIGN(number, align)	((number) & ((align) - 1))

/* Macro for alignment up. For example, ALIGN_UP(0x0330, 0x20) = 0x0340 */
#define ALIGN_UP(number, align) (((number) & ((align) - 1)) ? \
		(((number) + (align)) & ~((align)-1)) : (number))

#endif	/* MVEBU_H */
