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

