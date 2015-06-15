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

#include <config.h>
#include <version.h>
#include <asm/arch/regs-base.h>

.globl lowlevel_init

lowlevel_init:
	/* Move internal registers from 0xD000_0000 to 0xF100_0000 */
	mov    r0, #MVEBU_REGS_BASE
	ldr    r1, _inter_regs_win_reg
	str    r0 ,[r1]
	mov 	pc, lr

.globl _lowlevel_init_val
_lowlevel_init_val:
	.word lowlevel_init

.globl _inter_regs_win_reg
_inter_regs_win_reg:
	.word 0xD0020080

